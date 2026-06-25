#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "RootManager.hh"
#include "RunAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(RootManager * rootManager)
  : fRootManager(rootManager)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run * run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  fRootManager->BeginOfRun(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run * run)
{
  G4cout << "### Run " << run->GetRunID() << " end." << G4endl;

  fRootManager->EndOfRun(run);
}
