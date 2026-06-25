#ifndef B1SteppingAction_h
#define B1SteppingAction_h 1

#include "G4Step.hh"
#include "G4UserSteppingAction.hh"

class RootManager;
class SteppingAction : public G4UserSteppingAction {
public:
  explicit SteppingAction(RootManager * rootManager);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step * step) override;

private:
  RootManager * fRootManager = nullptr;
};

#endif
