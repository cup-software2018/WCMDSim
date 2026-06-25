#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "RootManager.hh"
#include "TrackingAction.hh"
#include "Trajectory.hh"

TrackingAction::TrackingAction(RootManager * r)
  : fRootManager(r)
{
}

void TrackingAction::PreUserTrackingAction(const G4Track * aTrack)
{
  // Trajectory must be set before any early return — ProcessOneTrack accesses it after Pre.
  fpTrackingManager->SetTrajectory(new Trajectory(aTrack));

  // Delta electrons from muon ionization in air have no detector relevance — kill immediately.
  const G4VProcess * proc = aTrack->GetCreatorProcess();
  const G4LogicalVolume * lv = aTrack->GetLogicalVolumeAtVertex();
  if (proc && proc->GetProcessName() == "muIoni"
      && aTrack->GetParticleDefinition()->GetParticleName() == "e-"
      && lv && lv->GetMaterial()->GetName() == "G4_AIR") {
    const_cast<G4Track *>(aTrack)->SetTrackStatus(fKillTrackAndSecondaries);
  }
}

void TrackingAction::PostUserTrackingAction(const G4Track * aTrack)
{
  Trajectory * trajectory = (Trajectory *)fpTrackingManager->GimmeTrajectory();
  if (trajectory) trajectory->SetDrawTrajectory(true);

  fRootManager->RecordTrack(aTrack);
}