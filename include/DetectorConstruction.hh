#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSurface;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction() = default;
  ~DetectorConstruction() override = default;

  G4VPhysicalVolume * Construct() override;

  G4LogicalVolume * GetScoringVolume() const { return fScoringVolume; }

private:
  void DefineMaterials();
  void DefineOpticalSurfaces();

  // optical surfaces
  G4OpticalSurface * fTyvekSurface        = nullptr;
  G4OpticalSurface * fPhotocathodeSurface = nullptr;
  G4OpticalSurface * fWaterPMTSurface     = nullptr;

  G4LogicalVolume * fScoringVolume = nullptr;
};

#endif
