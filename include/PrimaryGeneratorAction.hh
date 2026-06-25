#ifndef B1PrimaryGeneratorAction_h
#define B1PrimaryGeneratorAction_h 1

#include <vector>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4String.hh"
#include "G4Types.hh"

class G4Event;
class TH3D;

/// Generates cosmic muons from ROOT input distributions.
///
/// Input ROOT file must contain:
///   h_flux : TH3D — x: kinetic energy [GeV], y: zenith theta [deg], z: azimuth phi [deg]
///
/// Each event:
///   1. Sample (energy, theta, phi) from h_flux
///   2. Sample a random foot point (x0, y0) uniformly on z=0 disk of radius R
///   3. Back-trace along the muon direction to the hemisphere surface -> start position

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    explicit PrimaryGeneratorAction(const G4String& inputFile = "muon_input.root");
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event*) override;

    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  private:
    void BuildCDF();
    void SampleTH3D(G4double & energy, G4double & theta, G4double & phi) const;

    G4ParticleGun *       fParticleGun = nullptr;
    TH3D *                fHistFlux    = nullptr;  // x: energy [GeV], y: theta [deg], z: phi [deg]

    std::vector<G4double> fCDF;     // normalised cumulative bin sums, size = nx*ny*nz + 1
    G4int                 fNBinsY  = 0;
    G4int                 fNBinsZ  = 0;

    G4double fWorldRadius = 0.;
};

#endif
