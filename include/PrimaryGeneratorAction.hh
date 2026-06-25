#ifndef B1PrimaryGeneratorAction_h
#define B1PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4String.hh"
#include "G4Types.hh"

class G4Event;
class TH1D;
class TH2D;
class TFile;

/// Generates cosmic muons from ROOT input distributions.
///
/// Input ROOT file must contain:
///   hEnergy  : kinetic energy [GeV]             (TH1D)
///   hAngular : zenith theta [deg] vs azimuth phi [deg]  (TH2D, x=theta, y=phi)
///
/// Each event:
///   1. Sample (theta, phi) from hAngular and energy from hEnergy
///   2. Sample a random foot point (x0, y0) uniformly on z=0 disk of radius R
///   3. Back-trace along the muon direction to the hemisphere surface -> start position
///   This ensures muons with the same (theta,phi) can hit any part of the detector array.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    explicit PrimaryGeneratorAction(const G4String& inputFile = "muon_input.root");
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event*) override;

    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  private:
    static G4double SampleTH1D(const TH1D* h);
    static void     SampleTH2D(const TH2D* h, G4double& x, G4double& y);

    G4ParticleGun* fParticleGun  = nullptr;
    TH1D*          fHistEnergy   = nullptr;  // energy [GeV]
    TH2D*          fHistAngular  = nullptr;  // x: theta [deg], y: phi [deg]

    G4double fWorldRadius = 0.;
};

#endif
