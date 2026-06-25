#include <iomanip>
#include <iostream>

#include "G4HCofThisEvent.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryVertex.hh"
#include "G4SDManager.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"
#include "G4VPhysicalVolume.hh"
#include "MCPMT.hh"
#include "MCPhotonHit.hh"
#include "MCPrimary.hh"
#include "MCTrack.hh"
#include "PMTHit.hh"
#include "RootManager.hh"

using namespace std;
using namespace CLHEP;

RootManager::RootManager()
  : G4UImessenger(),
    fPMTHitCollId(-1),
    fPrimaryData(nullptr),
    fTrackData(nullptr),
    fPMTData(nullptr),
    fEventInfo(nullptr),
    fRootFile(nullptr)
{
  fTrackSaveFlag = kSaveNone;
  fStepSaveOption = 0;
  fHitPhotonSave = 0;

  fROOTDir = new G4UIdirectory("/ROOT/");

  fTrackSaveOptCmd = new G4UIcmdWithAnInteger("/ROOT/savetrackopt", this);
  fStepSaveOptCmd = new G4UIcmdWithAnInteger("/ROOT/savestepopt", this);
  fHitPhotonSaveCmd = new G4UIcmdWithAnInteger("/ROOT/savehitphoton", this);

  G4cout << "RootManager::RootManager() created" << G4endl;
}

RootManager::~RootManager()
{
  delete fTrackSaveOptCmd;
  delete fStepSaveOptCmd;
  delete fHitPhotonSaveCmd;

  G4cout << "RootManager::RootManager() destroyed" << G4endl;
}

void RootManager::SetNewValue(G4UIcommand * command, G4String newValues)
{
  if (command == fTrackSaveOptCmd) fTrackSaveFlag = G4UIcmdWithAnInteger::GetNewIntValue(newValues);
  else if (command == fStepSaveOptCmd)
    fStepSaveOption = G4UIcmdWithAnInteger::GetNewIntValue(newValues);
  else if (command == fHitPhotonSaveCmd)
    fHitPhotonSave = G4UIcmdWithAnInteger::GetNewIntValue(newValues);
}

void RootManager::BeginOfRun(const G4Run * aRun)
{
  OpenRootFile();
  Booking();

  G4int runId = aRun->GetRunID();
  G4int nEventToBeProcessed = aRun->GetNumberOfEventToBeProcessed();

  G4cout << G4endl;
  G4cout << "++++++++++++++++++ Run Initialized ++++++++++++++++++" << G4endl;
  G4cout << "   RunID              : " << runId << G4endl;
  G4cout << "   NEventToBeProcessed: " << nEventToBeProcessed << G4endl;
  G4cout << "++++++++++++++++++ Run Initialized ++++++++++++++++++" << G4endl;
  G4cout << G4endl;
}

void RootManager::EndOfRun(const G4Run * aRun)
{
  G4int runId = aRun->GetRunID();
  G4int nEventProcessed = aRun->GetNumberOfEvent();

  CloseRootFile();

  delete fEventInfo;
  delete fPrimaryData;
  delete fTrackData;
  delete fPMTData;

  G4cout << G4endl;
  G4cout << "++++++++++++++++++  Run Finalized  ++++++++++++++++++" << G4endl;
  G4cout << "   RunID          : " << runId << G4endl;
  G4cout << "   NEventProcessed: " << nEventProcessed << G4endl;
  G4cout << "++++++++++++++++++  Run Finalized  ++++++++++++++++++" << G4endl;
  G4cout << G4endl;
}

void RootManager::BeginOfEvent(const G4Event *)
{
  fPrimaryData->Clear();
  fTrackData->Clear();
  fPMTData->Clear();

  G4SDManager * SDman = G4SDManager::GetSDMpointer();
  if (fPMTHitCollId < 0) { fPMTHitCollId = SDman->GetCollectionID("pmtHitCollection"); }
}

void RootManager::EndOfEvent(const G4Event * anEvent)
{
  G4int eventId = anEvent->GetEventID() + 1; // event number starts from 1

  fEventInfo->SetEventNumber(eventId);

  // primary vertex
  G4int npvx = anEvent->GetNumberOfPrimaryVertex();
  for (int i = 0; i < npvx; i++) {
    G4PrimaryVertex * pvx = anEvent->GetPrimaryVertex(i);
    double x0 = pvx->GetX0();
    double y0 = pvx->GetY0();
    double z0 = pvx->GetZ0();
    double t0 = pvx->GetT0();

    int nptl = pvx->GetNumberOfParticle();
    for (int j = 0; j < nptl; j++) {
      G4PrimaryParticle * ptl = pvx->GetPrimary(j);

      MCPrimary * prim = fPrimaryData->Add();
      prim->SetT0(t0);
      prim->SetVertex(x0, y0, z0);
      prim->SetMomentum(ptl->GetPx(), ptl->GetPy(), ptl->GetPz());
      prim->SetKineticEnergy(ptl->GetKineticEnergy());
      prim->SetTrackId(ptl->GetTrackID());
      prim->SetParticleName(ptl->GetG4code()->GetParticleName().c_str());
    }
  }

  // PMT Hits
  G4HCofThisEvent * hits = anEvent->GetHCofThisEvent();
  PMTHitsCollection * pmtHC = nullptr;

  if (hits && (fPMTHitCollId >= 0)) { pmtHC = (PMTHitsCollection *)(hits->GetHC(fPMTHitCollId)); }

  if (pmtHC) {
    G4int nPMT = pmtHC->entries();

    for (int i = 0; i < nPMT; i++) {
      const PMTHit * pmt = (*pmtHC)[i];
      int pmtId = pmt->GetPMTId();

      MCPMT * mcpmt = fPMTData->Add();
      mcpmt->SetId(pmtId);

      int nph = pmt->GetNHit();
      if (fHitPhotonSave) {
        for (int j = 0; j < nph; j++) {
          MCPhotonHit * ph = pmt->GetHit(j);
          mcpmt->AddHit(ph);
        }
        mcpmt->Sort();
      }
    }
    fPMTData->Sort();
  }

  fEventTree->Fill();

  if (eventId > 0 && eventId % 100 == 0)
    G4cout << std::setw(12) << eventId << " events processed ..." << G4endl;
}

void RootManager::RecordTrack(const G4Track * gtrack)
{
  if (!fTrackSaveFlag) return;

  const G4ParticleDefinition * pdef = gtrack->GetParticleDefinition();
  const G4String & pname = pdef->GetParticleName();
  const G4VProcess * proc = gtrack->GetCreatorProcess();
  const G4String procName = proc ? proc->GetProcessName() : "";

  if (pname == "opticalphoton") {
    if (!(fTrackSaveFlag & kSaveOptical)) return;
  }
  else if (pname == "e-" || pname == "e+") {
    bool isEMShower = procName == "compt" || procName == "phot" || procName == "conv" ||
                      procName == "annihil" || G4StrUtil::contains(procName, "Ioni") ||
                      G4StrUtil::contains(procName, "Brem");
    if (!(fTrackSaveFlag & (isEMShower ? kSaveEMShower : kSaveElec))) return;
  }
  else {
    if (!(fTrackSaveFlag & kSaveHeavy)) return;
  }

  MCTrack * mtrack = fTrackData->FindTrack(gtrack->GetTrackID());
  if (!mtrack) {
    mtrack = fTrackData->Add();
    mtrack->SetParticleName(pname.data());
    mtrack->SetPDGCode(pdef->GetPDGEncoding());
    mtrack->SetTrackId(gtrack->GetTrackID());
    mtrack->SetParentId(gtrack->GetParentID());
    mtrack->SetVertex(gtrack->GetVertexPosition().x(), gtrack->GetVertexPosition().y(),
                      gtrack->GetVertexPosition().z());
    mtrack->SetMomentumDir(gtrack->GetVertexMomentumDirection().x(),
                           gtrack->GetVertexMomentumDirection().y(),
                           gtrack->GetVertexMomentumDirection().z());
    mtrack->SetKineticEnergy(gtrack->GetVertexKineticEnergy());
    mtrack->SetGlobalTime(gtrack->GetGlobalTime());
    mtrack->SetLocalTime(gtrack->GetLocalTime());
    if (proc) mtrack->SetProcessName(procName.data());
  }
}

void RootManager::RecordStep(const G4Step * aStep, const G4VProcess * proc)
{
  if (!proc || !fTrackSaveFlag || !fStepSaveOption) return;

  G4Track * track = aStep->GetTrack();
  const G4String & pname = track->GetParticleDefinition()->GetParticleName();

  // Classify this track into a StepSaveFlag bit.
  G4int stepBit = kStepNone;
  if (pname == "opticalphoton") { stepBit = kStepOptical; }
  else if (pname == "e-" || pname == "e+") {
    const G4VProcess * creator = track->GetCreatorProcess();
    const G4String creatorName = creator ? creator->GetProcessName() : "";
    bool isEMShower = creatorName == "compt" || creatorName == "phot" || creatorName == "conv" ||
                      creatorName == "annihil" || G4StrUtil::contains(creatorName, "Ioni") ||
                      G4StrUtil::contains(creatorName, "Brem");
    stepBit = isEMShower ? kStepEMShower : kStepElec;
  }
  else {
    stepBit = kStepHeavy;
  }

  if (!(fStepSaveOption & stepBit)) return;

  G4StepPoint * postStepPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume * volume = postStepPoint->GetPhysicalVolume();
  if (!volume) return;

  MCTrack * mcTrack = fTrackData->FindTrack(track->GetTrackID());
  if (!mcTrack) return;

  MCStep * mcStep = mcTrack->AddStep();
  mcStep->SetStepLength(aStep->GetStepLength());
  mcStep->SetEnergyDeposit(aStep->GetTotalEnergyDeposit());
  mcStep->SetEnergyDepositNonIonizing(aStep->GetNonIonizingEnergyDeposit());
  mcStep->SetKineticEnergy(postStepPoint->GetKineticEnergy());
  mcStep->SetGlobalTime(postStepPoint->GetGlobalTime());
  mcStep->SetLocalTime(postStepPoint->GetLocalTime());
  G4ThreeVector pos = postStepPoint->GetPosition();
  mcStep->SetStepPoint(pos.x(), pos.y(), pos.z());
  mcStep->SetVolumeName(volume->GetName().data());
  mcStep->SetProcessName(proc->GetProcessName().data());
}

void RootManager::OpenRootFile()
{
  fRootFile = new TFile(fRootFilename.c_str(), "recreate");
  G4cout << "RootManager::OpenRootFile(): output file " << fRootFilename << " opened ..." << G4endl;
}

void RootManager::CloseRootFile()
{
  fRootFile->Write();
  fRootFile->Close();
  delete fRootFile;
  fRootFile = NULL;
}

void RootManager::AddRegion(G4int trackId, G4int regionBit)
{
  MCTrack * track = fTrackData->FindTrack(trackId);
  if (track) track->AddRegion(regionBit);
}

void RootManager::Booking()
{
  fEventInfo = new MCEventInfo();
  fPrimaryData = new MCPrimaryData();
  fTrackData = new MCTrackData();
  fPMTData = new MCPMTData();

  fEventTree = new TTree("Event", "Event");
  fEventTree->Branch("MCEventInfo", &fEventInfo);
  fEventTree->Branch("MCPrimaryData", &fPrimaryData);
  fEventTree->Branch("MCTrackData", &fTrackData);
  fEventTree->Branch("MCPMTData", &fPMTData);
}
