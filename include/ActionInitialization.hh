#ifndef B1ActionInitialization_h
#define B1ActionInitialization_h 1

#include "G4String.hh"
#include "G4VUserActionInitialization.hh"
#include "RootManager.hh"

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization {
public:
  explicit ActionInitialization(const G4String & inputFile = "muon_input.root",
                                const G4String & rootFilename = "muon_output.root");
  ~ActionInitialization() override = default;

  void BuildForMaster() const override;
  void Build() const override;

private:
  G4String fInputFile;
  G4String fRootFilename;
  RootManager * fRootManager;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
