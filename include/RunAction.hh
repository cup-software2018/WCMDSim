#ifndef B1RunAction_h
#define B1RunAction_h 1

#include "G4Accumulable.hh"
#include "G4UserRunAction.hh"

class G4Run;
class RootManager;
class RunAction : public G4UserRunAction {
public:
  explicit RunAction(RootManager * rootManager);
  ~RunAction() override = default;

  void BeginOfRunAction(const G4Run * run) override;
  void EndOfRunAction(const G4Run * run) override;

private:
  RootManager * fRootManager = nullptr;
};

#endif
