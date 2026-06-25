#include "ActionInitialization.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization(const G4String & inputFile,
                                           const G4String & rootFilename)
  : fInputFile(inputFile),
    fRootFilename(rootFilename)
{
  fRootManager = new RootManager();
  fRootManager->SetRootFilename(fRootFilename);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const { SetUserAction(new RunAction(fRootManager)); }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
  fRootManager->OpenRootFile();
  fRootManager->Booking();

  SetUserAction(new PrimaryGeneratorAction(fInputFile));

  auto * runAction = new RunAction(fRootManager);
  SetUserAction(runAction);

  auto * eventAction = new EventAction(fRootManager);
  SetUserAction(eventAction);
  SetUserAction(new TrackingAction(fRootManager));
  SetUserAction(new SteppingAction(fRootManager));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
