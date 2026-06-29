#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "G4OpticalPhysics.hh"
#include "G4RunManagerFactory.hh"
#include "PMTFastSimPhysics.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"
#include "Shielding.hh"

#include <chrono>

int main(int argc, char ** argv)
{
  // Usage:
  //   ./WCMDSim                                interactive, default files
  //   ./WCMDSim run.mac                        batch, default files
  //   ./WCMDSim input.root                     interactive, specified input
  //   ./WCMDSim run.mac input.root             batch, specified input
  //   ./WCMDSim run.mac input.root output.root batch, specified input and output

  G4String macroFile;
  G4String inputFile = "muon_input.root";
  G4String outputFile = "muon_output.root";

  int rootArgCount = 0;
  for (int i = 1; i < argc; i++) {
    G4String arg = argv[i];
    if (arg.size() > 5 && arg.substr(arg.size() - 5) == ".root") {
      if (rootArgCount++ == 0) inputFile = arg;
      else outputFile = arg;
    }
    else {
      macroFile = arg;
    }
  }

  G4UIExecutive * ui = macroFile.empty() ? new G4UIExecutive(argc, argv) : nullptr;

  // Seed from high-resolution clock (microsecond precision) — unique across parallel jobs.
  // CLHEP requires seed in [1, 900000000].
  G4long seed = std::chrono::high_resolution_clock::now().time_since_epoch().count() % 900000000L;
  G4Random::setTheSeed(seed);
  G4cout << "Random seed: " << seed << G4endl;

  G4SteppingVerbose::UseBestUnit(4);

  auto * runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

  runManager->SetUserInitialization(new DetectorConstruction());

  auto * physicsList = new Shielding;
  physicsList->RegisterPhysics(new PMTFastSimPhysics()); // must precede G4OpticalPhysics
  physicsList->RegisterPhysics(new G4OpticalPhysics());
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new ActionInitialization(inputFile, outputFile));

  auto * UImanager = G4UImanager::GetUIpointer();

  if (ui) {
    auto * visManager = new G4VisExecutive;
    visManager->Initialize();
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
    delete visManager;
  }
  else {
    UImanager->ApplyCommand("/control/execute " + macroFile);
  }

  delete runManager;
}
