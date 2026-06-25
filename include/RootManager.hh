#ifndef RootManager_hh
#define RootManager_hh

#include <string>

#include "TFile.h"
#include "TTree.h"

#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4UImessenger.hh"
#include "G4VProcess.hh"
#include "MCEventInfo.hh"
#include "MCPMTData.hh"
#include "MCPrimaryData.hh"
#include "MCTrackData.hh"

class G4UIdirectory;
class G4UIcmdWithAnInteger;

class RootManager : public G4UImessenger {
public:
  // Bit flags for /ROOT/savetrackopt — combine with bitwise OR.
  enum TrackSaveFlag : G4int {
    kSaveNone     = 0,
    kSaveHeavy    = 1 << 0,  // muons, hadrons, ions (not e±, not photons)
    kSaveElec     = 1 << 1,  // e± not produced by EM-shower processes
    kSaveEMShower = 1 << 2,  // e± from compt/phot/conv/annihil/Ioni/Brem
    kSaveOptical  = 1 << 3,  // optical photons
  };

  // Bit flags for /ROOT/savestepopt — same bit positions as TrackSaveFlag.
  enum StepSaveFlag : G4int {
    kStepNone     = 0,
    kStepHeavy    = 1 << 0,  // record steps for heavy tracks (muon/hadron/ion)
    kStepElec     = 1 << 1,  // record steps for non-shower e±
    kStepEMShower = 1 << 2,  // record steps for EM-shower e±
    kStepOptical  = 1 << 3,  // record steps for optical photons
  };

  // Bit flags for water region tagging (stored in MCTrack::fRegionMask).
  enum MuonRegion : G4int {
    kRegNone    = 0,
    kRegCeiling = 1 << 0,  // water above the inner room (top)
    kRegWallPX  = 1 << 1,  // +X side wall water
    kRegWallMX  = 1 << 2,  // -X side wall water
    kRegWallPY  = 1 << 3,  // +Y side wall water
    kRegWallMY  = 1 << 4,  // -Y side wall water
    kRegDoor    = 1 << 5,  // door water volume
  };

  RootManager();
  virtual ~RootManager();

  virtual void SetNewValue(G4UIcommand *, G4String);

  virtual void BeginOfRun(const G4Run *);
  virtual void EndOfRun(const G4Run *);
  virtual void BeginOfEvent(const G4Event *);
  virtual void EndOfEvent(const G4Event *);
  virtual void RecordTrack(const G4Track *);
  virtual void RecordStep(const G4Step *, const G4VProcess *);
  void AddRegion(G4int trackId, G4int regionBit);

  void SetRootFilename(const char * fname) { fRootFilename = fname; }
  void OpenRootFile();
  void CloseRootFile();

  void Booking();

private:
  G4UIdirectory * fROOTDir;

  G4UIcmdWithAnInteger * fTrackSaveOptCmd;
  G4UIcmdWithAnInteger * fStepSaveOptCmd;
  G4UIcmdWithAnInteger * fHitPhotonSaveCmd;

  G4int fTrackSaveFlag;
  G4int fStepSaveOption;
  G4int fHitPhotonSave;

  G4int fPMTHitCollId;

  MCPrimaryData * fPrimaryData;
  MCTrackData * fTrackData;
  MCPMTData * fPMTData;
  MCEventInfo * fEventInfo;

  std::string fRootFilename;
  TFile * fRootFile;
  TTree * fRunTree;
  TTree * fEventTree;
};

#endif
