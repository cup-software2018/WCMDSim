#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"

class RootManager;
class TrackingAction : public G4UserTrackingAction {

public:
  explicit TrackingAction(RootManager * rootManager);
  virtual ~TrackingAction() = default;

  virtual void PreUserTrackingAction(const G4Track * track);
  virtual void PostUserTrackingAction(const G4Track * track);

private:
  RootManager * fRootManager;
};

#endif