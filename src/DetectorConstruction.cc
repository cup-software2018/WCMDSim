#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MultiUnion.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4Orb.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "PMTLogicalVolume.hh"
#include "PMTSD.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  auto nist = G4NistManager::Instance();
  auto water = nist->FindOrBuildMaterial("G4_WATER");
  auto glass = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  // remaining materials (air, steel, vacuum, aluminum) need no optical properties
  nist->FindOrBuildMaterial("G4_AIR");
  nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  nist->FindOrBuildMaterial("G4_Galactic");
  nist->FindOrBuildMaterial("G4_Al");

  // ---- water optical properties ----
  G4double energyAbsRay[25] = {1.550 * eV, 1.600 * eV, 1.653 * eV, 1.710 * eV, 1.771 * eV,
                               1.837 * eV, 1.907 * eV, 1.984 * eV, 2.066 * eV, 2.156 * eV,
                               2.254 * eV, 2.362 * eV, 2.480 * eV, 2.610 * eV, 2.755 * eV,
                               2.917 * eV, 3.100 * eV, 3.306 * eV, 3.542 * eV, 3.815 * eV,
                               4.133 * eV, 4.508 * eV, 4.959 * eV, 5.510 * eV, 6.199 * eV};

  G4double absLength[25] = {
      500.09 * mm,   417.08 * mm,   385.07 * mm,   629.23 * mm,   1668.89 * mm,
      2414.59 * mm,  3134.09 * mm,  3585.16 * mm,  4372.93 * mm,  12912.37 * mm,
      23169.57 * mm, 33544.46 * mm, 44642.86 * mm, 46428.90 * mm, 41527.91 * mm,
      30352.94 * mm, 19503.39 * mm, 9284.15 * mm,  4542.28 * mm,  2498.96 * mm,
      1551.81 * mm,  968.61 * mm,   615.54 * mm,   377.30 * mm,   148.20 * mm};

  G4double rayleighLength[25] = {
      2631578.95 * mm, 2303867.40 * mm, 1994818.65 * mm, 1718579.23 * mm, 1475221.24 * mm,
      1268421.05 * mm, 1077586.21 * mm, 904050.63 * mm,  762807.02 * mm,  642538.07 * mm,
      543325.18 * mm,  456871.35 * mm,  384615.38 * mm,  316296.88 * mm,  255100.00 * mm,
      197864.66 * mm,  148629.31 * mm,  107644.84 * mm,  77894.74 * mm,   55887.85 * mm,
      39393.14 * mm,   26945.24 * mm,   17867.87 * mm,   11196.32 * mm,   6712.96 * mm};

  G4double energyRindex[23] = {1.550 * eV, 1.653 * eV, 1.771 * eV, 1.837 * eV, 1.907 * eV,
                               1.984 * eV, 2.066 * eV, 2.156 * eV, 2.254 * eV, 2.362 * eV,
                               2.480 * eV, 2.610 * eV, 2.755 * eV, 2.917 * eV, 3.100 * eV,
                               3.306 * eV, 3.542 * eV, 3.815 * eV, 4.133 * eV, 4.508 * eV,
                               4.959 * eV, 5.510 * eV, 6.199 * eV};

  G4double rIndex[23] = {1.329, 1.330, 1.331, 1.331, 1.331, 1.332, 1.332, 1.333,
                         1.333, 1.334, 1.335, 1.336, 1.337, 1.338, 1.339, 1.341,
                         1.343, 1.346, 1.349, 1.354, 1.362, 1.373, 1.396};

  auto waterMPT = new G4MaterialPropertiesTable();
  waterMPT->AddProperty("ABSLENGTH", energyAbsRay, absLength, 25);
  waterMPT->AddProperty("RAYLEIGH", energyAbsRay, rayleighLength, 25);
  waterMPT->AddProperty("RINDEX", energyRindex, rIndex, 23);
  water->SetMaterialPropertiesTable(waterMPT);

  // ---- glass optical properties ----
  G4double energyRindexGlass[7] = {1.550 * eV, 1.771 * eV, 2.066 * eV, 2.480 * eV,
                                   3.100 * eV, 4.133 * eV, 6.199 * eV};

  G4double rIndexGlass[7] = {1.46623, 1.46809, 1.47092, 1.47552, 1.48364, 1.49973, 1.53464};

  G4double energyAbsGlass[7] = {1.550 * eV, 1.610 * eV, 2.066 * eV, 2.480 * eV,
                                3.757 * eV, 4.133 * eV, 6.199 * eV};

  G4double absLengthGlass[7] = {0.1e-3 * mm, 0.5e3 * mm,  1.0e3 * mm, 2.0e3 * mm,
                                1.0e3 * mm,  0.1e-3 * mm, 0.1e-3 * mm};

  auto glassMPT = new G4MaterialPropertiesTable();
  glassMPT->AddProperty("RINDEX", energyRindexGlass, rIndexGlass, 7);
  glassMPT->AddProperty("ABSLENGTH", energyAbsGlass, absLengthGlass, 7);
  glass->SetMaterialPropertiesTable(glassMPT);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineOpticalSurfaces()
{
  // ---- Tyvek reflector ----
  fTyvekSurface = new G4OpticalSurface("TyvekSurface");
  fTyvekSurface->SetType(dielectric_metal);
  fTyvekSurface->SetFinish(ground);
  fTyvekSurface->SetModel(unified);

  G4double photonEnergy[9] = {1.550 * eV, 1.771 * eV, 2.066 * eV, 2.480 * eV, 3.100 * eV,
                              3.542 * eV, 4.133 * eV, 4.959 * eV, 6.199 * eV};

  G4double tyvekReflectivity[9] = {0.92, 0.93, 0.94, 0.95, 0.96, 0.96, 0.95, 0.94, 0.94};

  // All specular constants default to 0 → Lambertian fraction = 1 (pure diffuse reflection).
  auto tyvekMPT = new G4MaterialPropertiesTable();
  tyvekMPT->AddProperty("REFLECTIVITY", photonEnergy, tyvekReflectivity, 9);
  fTyvekSurface->SetMaterialPropertiesTable(tyvekMPT);

  // ---- photocathode (thin-film model) ----
  fPhotocathodeSurface = new G4OpticalSurface("Photocathode_opsurf");
  fPhotocathodeSurface->SetType(dielectric_metal);
  fPhotocathodeSurface->SetFinish(polished);
  fPhotocathodeSurface->SetModel(glisur);

  G4double energyEff[56] = {
      1.8788 * eV, 1.8871 * eV, 1.8969 * eV, 1.9071 * eV, 1.9189 * eV, 1.9291 * eV, 1.9411 * eV,
      1.9518 * eV, 1.9623 * eV, 1.9748 * eV, 1.9894 * eV, 2.0026 * eV, 2.0176 * eV, 2.0345 * eV,
      2.0520 * eV, 2.0737 * eV, 2.0957 * eV, 2.1183 * eV, 2.1480 * eV, 2.1828 * eV, 2.2231 * eV,
      2.2724 * eV, 2.3208 * eV, 2.3692 * eV, 2.4116 * eV, 2.4697 * eV, 2.5637 * eV, 2.6680 * eV,
      2.7848 * eV, 2.9090 * eV, 3.0658 * eV, 3.2354 * eV, 3.4201 * eV, 3.6273 * eV, 3.7661 * eV,
      3.8671 * eV, 3.9309 * eV, 3.9814 * eV, 4.0266 * eV, 4.0796 * eV, 4.0945 * eV, 4.1258 * eV,
      4.1423 * eV, 4.1660 * eV, 4.1899 * eV, 4.2070 * eV, 4.2314 * eV, 4.2473 * eV, 4.2649 * eV,
      4.2900 * eV, 4.3079 * eV, 4.3334 * eV, 4.3502 * eV, 4.3686 * eV, 4.3949 * eV, 4.4121 * eV};

  G4double pmtEff[56] = {0.00011, 0.00014, 0.00020, 0.00027, 0.00036, 0.00049, 0.00067, 0.00091,
                         0.00121, 0.00164, 0.00226, 0.00307, 0.00413, 0.00566, 0.00755, 0.01033,
                         0.01389, 0.01854, 0.02494, 0.03299, 0.04291, 0.05628, 0.07258, 0.09601,
                         0.12701, 0.16379, 0.18918, 0.21666, 0.24814, 0.27471, 0.29399, 0.30157,
                         0.29902, 0.27471, 0.22037, 0.16379, 0.12071, 0.08972, 0.06501, 0.04750,
                         0.03531, 0.02580, 0.01901, 0.01378, 0.01015, 0.00742, 0.00542, 0.00396,
                         0.00292, 0.00212, 0.00156, 0.00113, 0.00083, 0.00061, 0.00044, 0.00033};

  G4double energyRI[23] = {1.5498 * eV, 1.8233 * eV, 1.8644 * eV, 1.9074 * eV, 1.9524 * eV,
                           1.9997 * eV, 2.0493 * eV, 2.1014 * eV, 2.1562 * eV, 2.2140 * eV,
                           2.2749 * eV, 2.3392 * eV, 2.4074 * eV, 2.4796 * eV, 2.5563 * eV,
                           2.6379 * eV, 2.7248 * eV, 2.8177 * eV, 2.9172 * eV, 3.0240 * eV,
                           3.1387 * eV, 3.2626 * eV, 6.1990 * eV};

  G4double pmtRI[23] = {3.00, 2.96, 2.95, 2.95, 2.95, 2.96, 2.98, 3.01, 3.06, 3.12, 3.20, 3.26,
                        3.09, 3.00, 3.00, 3.00, 2.87, 2.70, 2.61, 2.38, 2.18, 1.92, 1.00};

  G4double pmtKI[23] = {0.10, 0.33, 0.34, 0.34, 0.35, 0.37, 0.38, 0.42, 0.46, 0.53, 0.63, 0.86,
                        1.05, 1.06, 1.11, 1.34, 1.44, 1.50, 1.53, 1.71, 1.69, 1.69, 3.00};

  G4double energyConst[2] = {1.0 * eV, 7.0 * eV};
  G4double pmtRefl[2] = {0.0, 0.0};
  G4double pmtThk[2] = {26.0 * nm, 26.0 * nm};

  auto pcMPT = new G4MaterialPropertiesTable();
  pcMPT->AddProperty("EFFICIENCY", energyEff, pmtEff, 56);
  pcMPT->AddProperty("RINDEX", energyRI, pmtRI, 23);
  pcMPT->AddProperty("KINDEX", energyRI, pmtKI, 23, true);
  pcMPT->AddProperty("REFLECTIVITY", energyConst, pmtRefl, 2);
  pcMPT->AddProperty("THICKNESS", energyConst, pmtThk, 2, true);
  fPhotocathodeSurface->SetMaterialPropertiesTable(pcMPT);

  // ---- transparent water↔PMT border (overrides Tyvek skin at PMT hole faces) ----
  fWaterPMTSurface = new G4OpticalSurface("WaterPMTSurf");
  fWaterPMTSurface->SetType(dielectric_dielectric);
  fWaterPMTSurface->SetFinish(polished);
  fWaterPMTSurface->SetModel(glisur);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume * DetectorConstruction::Construct()
{
  DefineMaterials();
  DefineOpticalSurfaces();

  auto air = G4Material::GetMaterial("G4_AIR");
  auto steel = G4Material::GetMaterial("G4_STAINLESS-STEEL");
  auto water = G4Material::GetMaterial("G4_WATER");
  auto vacuum = G4Material::GetMaterial("G4_Galactic");
  auto glass = G4Material::GetMaterial("G4_Pyrex_Glass");
  auto aluminum = G4Material::GetMaterial("G4_Al");

  // ---- World : hemisphere (flat at z=0, dome in +Z) ----
  auto worldOrb = new G4Orb("World_orb", 10000. * mm);
  auto worldHalf = new G4Box("World_half", 20000. * mm, 20000. * mm, 5000. * mm);
  auto solidWorld = new G4IntersectionSolid("World", worldOrb, worldHalf, nullptr,
                                            G4ThreeVector(0., 0., 5000. * mm));
  auto logicWorld = new G4LogicalVolume(solidWorld, air, "World");
  auto physWorld =
      new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, true);

  // Flat floor at z=0; tank base at z=4000 mm → tank centre z=5571 mm.
  const G4ThreeVector tankPos(0., 0., 5571. * mm);

  // ---- primitive solids (half-sizes, mm) ----
  // Z is vertical.  Door on +Y face, at -X corner.
  auto sOuter = new G4Box("s_TankOuter", 3205. * mm, 3205. * mm, 1571. * mm);
  auto sCav = new G4Box("s_TankCav", 3197. * mm, 3197. * mm, 1563. * mm);
  auto sDoor = new G4Box("s_DoorBox", 877.5 * mm, 350. * mm, 1571. * mm);
  auto sRoomO = new G4Box("s_RoomOuter", 2513. * mm, 2513. * mm, 1083. * mm);
  auto sRoomI = new G4Box("s_RoomInner", 2505. * mm, 2505. * mm, 1075. * mm);
  auto sDoorI = new G4Box("s_DoorInner", 869.5 * mm, 342. * mm, 1563. * mm);

  // Water half-height: cavity base at world Z=4008 mm, top determined by PMT gap below.
  // waterZHalf fixed at 1433 mm; waterZOffset computed after PMT LV is built.
  const G4double waterZHalf = 1433. * mm;
  auto sWaterCav = new G4Box("s_WaterCav", 3197. * mm, 3197. * mm, waterZHalf);
  auto sDoorWaterS = new G4Box("s_DoorWaterS", 869.5 * mm, 342. * mm, waterZHalf);

  // ---- key positions ----
  // All "T" positions are in TankAir (≡ tank-centre) local frame.
  const G4ThreeVector doorPosT(-2327.5 * mm, 2855. * mm, 0. * mm);      // door, tank frame
  const G4ThreeVector roomPosT(0. * mm, 0. * mm, -480. * mm);           // room, tank frame
  const G4ThreeVector doorPosW(-2327.5 * mm, 2855. * mm, 5571. * mm);   // door, world
  const G4ThreeVector roomPosW(0. * mm, 0. * mm, 5091. * mm);           // room, world
  const G4ThreeVector doorPosRoom(-2327.5 * mm, 2855. * mm, 480. * mm); // door rel. to room

  // ---- 1) tank steel shell ----
  auto sShell1 = new G4SubtractionSolid("s_Shell1", sOuter, sCav);
  auto sShell = new G4SubtractionSolid("s_Shell", sShell1, sDoor, nullptr, doorPosT);
  auto lvShell = new G4LogicalVolume(sShell, steel, "TankSteel");
  new G4PVPlacement(nullptr, tankPos, lvShell, "TankSteel", logicWorld, false, 0, true);

  // ---- 2) water solid built after pmtLog (waterZOffset depends on PMT parameters) ----

  // ---- 3) inner room liner ----
  auto sRoomShell1 = new G4SubtractionSolid("s_RoomShell1", sRoomO, sRoomI);
  auto sRoomShell = new G4SubtractionSolid("s_RoomShell", sRoomShell1, sDoor, nullptr, doorPosRoom);
  auto lvRoomSteel = new G4LogicalVolume(sRoomShell, steel, "RoomSteel");

  // ---- 4) room interior ----
  auto lvRoomAir = new G4LogicalVolume(sRoomI, air, "RoomAir");

  // ---- 5) door steel shell ----
  auto sDoorShell = new G4SubtractionSolid("s_DoorShell", sDoor, sDoorI);
  auto lvDoorSteel = new G4LogicalVolume(sDoorShell, steel, "DoorSteel");
  // DoorSteel extends 8 mm outside sCav (through the wall), so it stays in world.
  new G4PVPlacement(nullptr, doorPosW, lvDoorSteel, "DoorSteel", logicWorld, false, 0, true);

  // ---- PMT logical volume ----
  auto sdMan = G4SDManager::GetSDMpointer();
  auto pmtSD = new PMTSD("/WCMD/PMTSD");
  sdMan->AddNewDetector(pmtSD);

  // ExteriorMat = water so the photocathode-region boundary is water→glass (no air gap).
  auto pmtLog = new PMT10inchLogicalVolume("PMT", water, glass, fPhotocathodeSurface, vacuum,
                                           aluminum, nullptr, pmtSD);

  // PMT bounding cylinder parameters
  auto pmtTubs = (G4Tubs *)pmtLog->GetSolid();
  const G4double pmtHH = pmtTubs->GetZHalfLength(); // 165 mm
  const G4double pmtR = pmtTubs->GetOuterRadius();  // 126.5 mm
  const G4double zEq = pmtLog->GetZEquator();       // ≈ 68.3 mm

  // Depth of photocathode below the water surface (= below the equator plane).
  // After rotateX(180°): PMT local +Z → world -Z, so photocathode apex is
  // pmtHH - zEq ≈ 96.7 mm below the water surface.
  const G4double photocathodeDepth = pmtHH - zEq;
  const G4double holeHH = photocathodeDepth; // exact: hole bottom = PMT cylinder bottom
  const G4double holeR = pmtR + 0.1 * mm;    // 0.1 mm tolerance

  // ---- 2) water solid ----
  // Place water so PMT base (top of rotated bounding cylinder) is 1 mm below tank ceiling.
  // Condition: waterZOffset + waterZHalf + zEq + pmtHH = cavHH - pmtTopGap
  const G4double cavHH = 1563. * mm; // sCav Z half
  const G4double pmtTopGap = 1. * mm;
  const G4double waterZOffset = cavHH - pmtTopGap - waterZHalf - zEq - pmtHH;

  const G4ThreeVector roomPosWaterT = roomPosT - G4ThreeVector(0, 0, waterZOffset);
  const G4ThreeVector doorPosWaterT = doorPosT - G4ThreeVector(0, 0, waterZOffset);

  auto sWater1 = new G4SubtractionSolid("s_Water1", sWaterCav, sRoomO, nullptr, roomPosWaterT);
  auto sWater = new G4SubtractionSolid("s_Water", sWater1, sDoor, nullptr, doorPosWaterT);

  // ---- PMT hole union (48 cylinders) for subtraction from TankWater ----
  // Hole centres are at the top face of the WaterCav solid (z = +waterZHalf).
  auto pmtHoleTube = new G4Tubs("PMThole", 0., holeR, holeHH, 0., CLHEP::twopi);
  auto allHoles = new G4MultiUnion("AllPMTHoles");
  G4RotationMatrix noRot;

  // 48 PMT (x,y) positions in tank/TankAir frame (mm)
  G4double pmtXY[48][2] = {
      {2850, -2850},  {1900, -2850},  {950, -2850},  {0, -2850},   {-950, -2850}, {-1900, -2850},
      {-2850, -2850}, {2850, -1900},  {1900, -1900}, {950, -1900}, {0, -1900},    {-950, -1900},
      {-1900, -1900}, {-2850, -1900}, {2850, -950},  {1900, -950}, {950, -950},   {0, -950},
      {-950, -950},   {-1900, -950},  {-2850, -950}, {2850, 0},    {1900, 0},     {950, 0},
      {0, 0},         {-950, 0},      {-1900, 0},    {-2850, 0},   {2850, 950},   {1900, 950},
      {950, 950},     {0, 950},       {-950, 950},   {-1900, 950}, {-2850, 950},  {2850, 1900},
      {1900, 1900},   {950, 1900},    {0, 1900},     {-950, 1900}, {-1900, 1900}, {-2850, 1900},
      {2850, 2850},   {1900, 2850},   {950, 2850},   {0, 2850},    {-950, 2850},  {-2328, 2850}};

  for (int i = 0; i < 48; i++) {
    // Hole centre in WaterCav solid local frame: (pmtX, pmtY, +waterZHalf)
    // The XY offsets of WaterCav and TankAir are identical (both centred at tankPos XY).
    G4Transform3D tr(noRot, G4ThreeVector(pmtXY[i][0] * mm, pmtXY[i][1] * mm, waterZHalf));
    allHoles->AddNode(*pmtHoleTube, tr);
  }
  allHoles->Voxelize();

  // Final water solid with PMT holes
  auto sWaterFinal = new G4SubtractionSolid("s_WaterFinal", sWater, allHoles);
  auto lvWater = new G4LogicalVolume(sWaterFinal, water, "TankWater");
  fScoringVolume = lvWater;

  // ---- DoorWater solid with hole for PMT #47 ----
  // PMT #47 position in DoorWater solid local = pmt_world_XY - door_world_XY
  G4double dw_lx = pmtXY[47][0] * mm - doorPosT.x(); // -2328 - (-2327.5) = -0.5 mm
  G4double dw_ly = pmtXY[47][1] * mm - doorPosT.y(); //  2850 -   2855    = -5.0 mm
  auto doorPMTHole = new G4Tubs("DoorPMThole", 0., holeR, holeHH, 0., CLHEP::twopi);
  auto sDoorWaterFinal = new G4SubtractionSolid("s_DoorWaterFinal", sDoorWaterS, doorPMTHole,
                                                nullptr, G4ThreeVector(dw_lx, dw_ly, waterZHalf));
  auto lvDoorWater = new G4LogicalVolume(sDoorWaterFinal, water, "DoorWater");

  // ---- TankAir : full cavity (sCav box), air — common mother for water + PMTs ----
  // sCav is reused as the TankAir solid (also used in sShell subtraction above).
  auto lvTankAir = new G4LogicalVolume(sCav, air, "TankAir");
  new G4PVPlacement(nullptr, tankPos, lvTankAir, "TankAir", logicWorld, false, 0, true);

  // ---- placements inside TankAir ----
  // Water sits at (0, 0, waterZOffset) so its top face is at the air–water interface.
  auto physWater = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, waterZOffset), lvWater,
                                     "TankWater", lvTankAir, false, 0, true);

  // Room liner and interior
  new G4PVPlacement(nullptr, roomPosT, lvRoomSteel, "RoomSteel", lvTankAir, false, 0, true);
  new G4PVPlacement(nullptr, roomPosT, lvRoomAir, "RoomAir", lvTankAir, false, 0, true);

  // Door water column (fits exactly within TankAir XY bounds)
  new G4PVPlacement(nullptr, G4ThreeVector(doorPosT.x(), doorPosT.y(), waterZOffset), lvDoorWater,
                    "DoorWater", lvTankAir, false, 0, true);

  // ---- PMT placements inside TankAir ----
  // After rotateX(180°), PMT local +Z → world -Z (photocathode faces down).
  // Place PMT centre so the equator (PMT local +zEq → world -zEq from centre)
  // lands exactly at the water surface (TankAir local z = waterZOffset + waterZHalf).
  auto rotPMT = new G4RotationMatrix();
  rotPMT->rotateX(180. * deg);

  const G4double pmtCentreZ = waterZOffset + waterZHalf + zEq; // in TankAir local

  char PMTname[64];
  for (int i = 0; i < 48; i++) {
    sprintf(PMTname, "PMTPhys%d", i);
    auto physPMT =
        new G4PVPlacement(rotPMT, G4ThreeVector(pmtXY[i][0] * mm, pmtXY[i][1] * mm, pmtCentreZ),
                          pmtLog, PMTname, lvTankAir, false, i, true);
    new G4LogicalBorderSurface("WaterPMT_" + std::to_string(i), physWater, physPMT,
                               fWaterPMTSurface);
  }

  // ---- Tyvek reflector skin ----
  // Water volumes: XY walls, bottom, and top (non-PMT area).
  new G4LogicalSkinSurface("TyvekSkinMain", lvWater, fTyvekSurface);
  new G4LogicalSkinSurface("TyvekSkinDoor", lvDoorWater, fTyvekSurface);
  // TankAir: steel walls and ceiling above the water line.
  new G4LogicalSkinSurface("TyvekSkinAir", lvTankAir, fTyvekSurface);

  // ---- visualisation ----
  auto vaSteel = new G4VisAttributes(G4Colour(0.68, 0.72, 0.77, 0.30));
  auto vaWater = new G4VisAttributes(G4Colour(0.18, 0.62, 0.84, 0.35));
  auto vaDoorS = new G4VisAttributes(G4Colour(0.79, 0.59, 0.18, 0.45));
  auto vaRoom = new G4VisAttributes(G4Colour(0.20, 0.27, 0.35, 0.20));
  auto vaAir = new G4VisAttributes(G4Colour(0.90, 0.90, 0.90, 0.05));
  vaSteel->SetForceSolid(true);
  vaWater->SetForceSolid(true);
  vaDoorS->SetForceSolid(true);
  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
  lvTankAir->SetVisAttributes(vaAir);
  lvShell->SetVisAttributes(vaSteel);
  lvWater->SetVisAttributes(vaWater);
  lvRoomSteel->SetVisAttributes(vaSteel);
  lvDoorSteel->SetVisAttributes(vaDoorS);
  lvDoorWater->SetVisAttributes(vaWater);
  lvRoomAir->SetVisAttributes(vaRoom);

  return physWorld;
}
