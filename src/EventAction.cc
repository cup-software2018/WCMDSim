#include "EventAction.hh"
#include "G4Event.hh"
#include "RootManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RootManager * rootManager)
  : fRootManager(rootManager)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event * event) { fRootManager->BeginOfEvent(event); }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event * event) { fRootManager->EndOfEvent(event); }
