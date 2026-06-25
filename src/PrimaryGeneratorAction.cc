#include <cmath>

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

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

  fHistEnergy = static_cast<TH1D *>(f->Get("h_dJdE"));
  fHistAngular = static_cast<TH2D *>(f->Get("h_dJdtdp"));

  if (!fHistEnergy || !fHistAngular) {
    G4Exception("PrimaryGeneratorAction", "ROOT002", JustWarning,
                "Missing histogram(s): expected hEnergy (TH1D), hAngular (TH2D)"
                " -- using fallback: 1 GeV mu- straight down");
    fHistEnergy = nullptr;
    fHistAngular = nullptr;
  }
  else {
    fHistEnergy ->SetDirectory(nullptr);
    fHistAngular->SetDirectory(nullptr);
  }

  f->Close();
  delete f;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fHistEnergy;
  delete fHistAngular;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double PrimaryGeneratorAction::SampleTH1D(const TH1D * h)
{
  G4double r = G4UniformRand() * h->Integral();
  G4double cumul = 0.;
  for (G4int i = 1; i <= h->GetNbinsX(); i++) {
    cumul += h->GetBinContent(i);
    if (r <= cumul) {
      G4double lo = h->GetBinLowEdge(i);
      return lo + G4UniformRand() * h->GetBinWidth(i);
    }
  }
  return h->GetBinLowEdge(h->GetNbinsX() + 1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SampleTH2D(const TH2D * h, G4double & valX, G4double & valY)
{
  G4double r = G4UniformRand() * h->Integral();
  G4double cumul = 0.;
  for (G4int ix = 1; ix <= h->GetNbinsX(); ix++) {
    for (G4int iy = 1; iy <= h->GetNbinsY(); iy++) {
      cumul += h->GetBinContent(ix, iy);
      if (r <= cumul) {
        valX = h->GetXaxis()->GetBinLowEdge(ix) + G4UniformRand() * h->GetXaxis()->GetBinWidth(ix);
        valY = h->GetYaxis()->GetBinLowEdge(iy) + G4UniformRand() * h->GetYaxis()->GetBinWidth(iy);
        return;
      }
    }
  }
  valX = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX() + 1);
  valY = h->GetYaxis()->GetBinLowEdge(h->GetNbinsY() + 1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event * anEvent)
{
  // Fallback when no ROOT input file is available (e.g. geometry-only visualisation)
  if (!fHistEnergy || !fHistAngular) {
    fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, fWorldRadius * 0.9));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1));
    fParticleGun->SetParticleEnergy(1. * GeV);
    fParticleGun->GeneratePrimaryVertex(anEvent);
    return;
  }

  G4double energy = SampleTH1D(fHistEnergy) * GeV;

  G4double thetaDeg, phiDeg;
  SampleTH2D(fHistAngular, thetaDeg, phiDeg);

  G4double theta = thetaDeg * deg;
  G4double phi = phiDeg * deg;
  G4double sinTheta = std::sin(theta);
  G4double cosTheta = std::cos(theta);
  G4double sinPhi = std::sin(phi);
  G4double cosPhi = std::cos(phi);

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
