#include "G4SteppingManager.hh"
#include "RootManager.hh"
#include "SteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(RootManager * rootManager)
  : fRootManager(rootManager)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step * step)
{
  G4SteppingManager * steppingManager = fpSteppingManager;
  G4VProcess * proc = steppingManager->GetfCurrentProcess();

  fRootManager->RecordStep(step, proc);
}
