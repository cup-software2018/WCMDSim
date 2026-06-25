#ifndef B1SteppingAction_h
#define B1SteppingAction_h 1

#include "G4Step.hh"
#include "G4UserSteppingAction.hh"
#include "DetectorConstruction.hh"

class RootManager;
class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction(RootManager * rootManager, const DetectorConstruction * detCon);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step * step) override;

private:
  RootManager *                              fRootManager = nullptr;
  const DetectorConstruction *               fDetCon      = nullptr;
};

#endif
