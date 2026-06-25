#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSurface;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  // Bounds of the inner room in TankWater-local coordinates.
  // Used by SteppingAction to classify which wall/ceiling a muon is traversing.
  struct WaterRegionBounds {
    double roomHalfX = 0;  // half-width of inner room in X [mm]
    double roomHalfY = 0;  // half-width of inner room in Y [mm]
    double roomTopZ  = 0;  // Z of room top face in TankWater local frame [mm]
  };

  DetectorConstruction() = default;
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume * Construct() override;

  G4LogicalVolume *       GetScoringVolume()      const { return fScoringVolume; }
  const WaterRegionBounds & GetWaterRegionBounds() const { return fWaterRegionBounds; }

private:
  void DefineMaterials();
  void DefineOpticalSurfaces();

  // optical surfaces
  G4OpticalSurface * fTyvekSurface        = nullptr;
  G4OpticalSurface * fPhotocathodeSurface = nullptr;
  G4OpticalSurface * fWaterPMTSurface     = nullptr;

  G4LogicalVolume *  fScoringVolume      = nullptr;
  WaterRegionBounds  fWaterRegionBounds  = {};
};

#endif
