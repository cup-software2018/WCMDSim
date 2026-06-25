#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"

class RootManager;
class EventAction : public G4UserEventAction {
public:
  explicit EventAction(RootManager * rootManager);
  ~EventAction() override = default;

  void BeginOfEventAction(const G4Event * event) override;
  void EndOfEventAction(const G4Event * event) override;

private:
  RootManager * fRootManager = nullptr;
};

#endif
