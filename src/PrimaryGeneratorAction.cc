#include <algorithm>
#include <cmath>

#include "TFile.h"
#include "TH3D.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "PrimaryGeneratorAction.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4String & inputFile)
  : fWorldRadius(10. * m)
{
  fParticleGun = new G4ParticleGun(1);
  fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("mu-"));

  auto * f = TFile::Open(inputFile);
  if (!f || f->IsZombie()) {
    G4Exception(
        "PrimaryGeneratorAction", "ROOT001", JustWarning,
        ("Cannot open input file: " + inputFile + " -- using fallback: 1 GeV mu- straight down")
            .c_str());
    delete f;
    return;
  }

  fHistFlux = static_cast<TH3D *>(f->Get("h_dJdEdTdP"));

  if (!fHistFlux) {
    G4Exception("PrimaryGeneratorAction", "ROOT002", JustWarning,
                "Missing histogram: expected h_dJdEdTdP (TH3D, x=energy[GeV], y=theta[deg], z=phi[deg])"
                " -- using fallback: 1 GeV mu- straight down");
  }
  else {
    fHistFlux->SetDirectory(nullptr);
    BuildCDF();
  }

  f->Close();
  delete f;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fHistFlux;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::BuildCDF()
{
  const G4int nx = fHistFlux->GetNbinsX();
  const G4int ny = fHistFlux->GetNbinsY();
  const G4int nz = fHistFlux->GetNbinsZ();

  fNBinsY = ny;
  fNBinsZ = nz;

  const G4int nbins = nx * ny * nz;
  fCDF.resize(nbins + 1);
  fCDF[0] = 0.;

  G4int k = 0;
  for (G4int ix = 1; ix <= nx; ix++)
    for (G4int iy = 1; iy <= ny; iy++)
      for (G4int iz = 1; iz <= nz; iz++, ++k)
        fCDF[k + 1] = fCDF[k] + fHistFlux->GetBinContent(ix, iy, iz);

  const G4double total = fCDF[nbins];
  for (auto & v : fCDF) v /= total;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SampleTH3D(G4double & energy, G4double & theta, G4double & phi) const
{
  const G4double r = G4UniformRand();

  // Binary search: find k such that fCDF[k] <= r < fCDF[k+1]
  G4int k = (G4int)(std::upper_bound(fCDF.begin(), fCDF.end(), r) - fCDF.begin()) - 1;
  const G4int nbins = (G4int)fCDF.size() - 1;
  if (k < 0)      k = 0;
  if (k >= nbins) k = nbins - 1;

  const G4int ix = k / (fNBinsY * fNBinsZ) + 1;
  const G4int iy = (k % (fNBinsY * fNBinsZ)) / fNBinsZ + 1;
  const G4int iz = k % fNBinsZ + 1;

  energy = fHistFlux->GetXaxis()->GetBinLowEdge(ix) + G4UniformRand() * fHistFlux->GetXaxis()->GetBinWidth(ix);
  theta  = fHistFlux->GetYaxis()->GetBinLowEdge(iy) + G4UniformRand() * fHistFlux->GetYaxis()->GetBinWidth(iy);
  phi    = fHistFlux->GetZaxis()->GetBinLowEdge(iz) + G4UniformRand() * fHistFlux->GetZaxis()->GetBinWidth(iz);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event * anEvent)
{
  // Fallback when no ROOT input file is available (e.g. geometry-only visualisation)
  if (!fHistFlux) {
    fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, fWorldRadius * 0.9));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1));
    fParticleGun->SetParticleEnergy(1. * GeV);
    fParticleGun->GeneratePrimaryVertex(anEvent);
    return;
  }

  G4double energyGeV, thetaDeg, phiDeg;
  SampleTH3D(energyGeV, thetaDeg, phiDeg);

  G4double energy   = energyGeV * GeV;
  G4double theta    = thetaDeg * deg;
  G4double phi      = phiDeg * deg;
  G4double sinTheta = std::sin(theta);
  G4double cosTheta = std::cos(theta);
  G4double sinPhi   = std::sin(phi);
  G4double cosPhi   = std::cos(phi);

  // Muon direction: downward at (theta, phi)
  G4ThreeVector dir(-sinTheta * cosPhi, -sinTheta * sinPhi, -cosTheta);

  // Sample foot point (x0, y0) uniformly on the world base disk (radius = fWorldRadius).
  G4double r     = fWorldRadius * std::sqrt(G4UniformRand());
  G4double alpha = 2. * M_PI * G4UniformRand();
  G4double x0    = r * std::cos(alpha);
  G4double y0    = r * std::sin(alpha);

  // Back-trace from (x0,y0,0) along -dir to the hemisphere (radius fWorldRadius).
  // t^2 + 2t*sinTheta*(x0*cosPhi + y0*sinPhi) + (x0^2+y0^2-R_world^2) = 0
  G4double R = fWorldRadius;
  G4double B = sinTheta * (x0 * cosPhi + y0 * sinPhi);
  G4double C = x0 * x0 + y0 * y0 - R * R;
  G4double t = -B + std::sqrt(B * B - C);

  G4ThreeVector pos(x0 + t * sinTheta * cosPhi, y0 + t * sinTheta * sinPhi, t * cosTheta);

  fParticleGun->SetParticlePosition(pos);
  fParticleGun->SetParticleMomentumDirection(dir);
  fParticleGun->SetParticleEnergy(energy);
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
