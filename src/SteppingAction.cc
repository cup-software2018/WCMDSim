#include <cmath>

#include "G4SteppingManager.hh"
#include "G4Track.hh"

#include "RootManager.hh"
#include "SteppingAction.hh"

SteppingAction::SteppingAction(RootManager * rootManager, const DetectorConstruction * detCon)
  : fRootManager(rootManager),
    fDetCon(detCon)
{
}

void SteppingAction::UserSteppingAction(const G4Step * step)
{
  G4SteppingManager * steppingManager = fpSteppingManager;
  G4VProcess * proc = steppingManager->GetfCurrentProcess();

  fRootManager->RecordStep(step, proc);

  // Water region tagging — muons only
  const G4Track * track = step->GetTrack();
  if (std::abs(track->GetParticleDefinition()->GetPDGEncoding()) != 13) return;

  const G4String & volName =
    step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

  G4int regionBit = RootManager::kRegNone;

  if (volName == "DoorWater") {
    regionBit = RootManager::kRegDoor;
  }
  else if (volName == "TankWater") {
    const auto & bounds = fDetCon->GetWaterRegionBounds();
    const G4TouchableHandle & touch = step->GetPreStepPoint()->GetTouchableHandle();
    const G4ThreeVector localPos =
      touch->GetHistory()->GetTopTransform().TransformPoint(
        step->GetPreStepPoint()->GetPosition());

    const double x = localPos.x();
    const double y = localPos.y();
    const double z = localPos.z();

    if (z > bounds.roomTopZ) {
      regionBit = RootManager::kRegCeiling;
    }
    else {
      // Face strips only — corners (both outside X and Y) are left untagged.
      // This keeps all four wall regions at equal area (inner-room face × water thickness).
      const bool outsideX = x > bounds.roomHalfX || x < -bounds.roomHalfX;
      const bool outsideY = y > bounds.roomHalfY || y < -bounds.roomHalfY;
      if (outsideX && !outsideY)
        regionBit = (x > 0) ? RootManager::kRegWallPX : RootManager::kRegWallMX;
      else if (!outsideX && outsideY)
        regionBit = (y > 0) ? RootManager::kRegWallPY : RootManager::kRegWallMY;
    }
  }

  if (regionBit != RootManager::kRegNone)
    fRootManager->AddRegion(track->GetTrackID(), regionBit);
}
