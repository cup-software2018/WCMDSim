# WCMDSim

Geant4-based cosmic muon simulation for the AMoRE WCMD detector.

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install
make -j$(nproc)
make install
```

## Setup

```bash
source /path/to/install/setup_wcmdsim.sh
```

## Run

```bash
# Interactive (opens Geant4 UI)
./WCMDSim

# Batch with default files (muon_input.root → muon_output.root)
./WCMDSim run.mac

# Batch with explicit input/output
./WCMDSim run.mac input.root output.root
```

## Output options (`run.mac`)

### Track saving (`/ROOT/savetrackopt`)

Bitmask controlling which particle types are recorded as MCTrack objects.

| Bit | Value | Particle type |
|-----|-------|---------------|
| 0 | 1 | Muons, hadrons, ions |
| 1 | 2 | e± (not from EM shower) |
| 2 | 4 | e± from EM shower (compt/phot/conv/annihil/Ioni/Brem) |
| 3 | 8 | Optical photons |

```
/ROOT/savetrackopt 1    # muons only (default)
/ROOT/savetrackopt 7    # muons + all e±
/ROOT/savetrackopt 15   # everything
```

### Step saving (`/ROOT/savestepopt`)

Bitmask for saving step-level info inside each MCTrack. Same bit positions as `savetrackopt`.

| Bit | Value | Step type |
|-----|-------|-----------|
| 0 | 1 | Muons, hadrons, ions |
| 1 | 2 | e± (not from EM shower) |
| 2 | 4 | e± from EM shower |
| 3 | 8 | Optical photons (WARNING: enormous output) |

**Key rule**: a step is saved only when **both** conditions hold:
- the track type bit is set in `savetrackopt` → MCTrack object exists
- the track type bit is set in `savestepopt`

Effective step set = `savetrackopt & savestepopt`.
Setting a bit in `savestepopt` that is not set in `savetrackopt` has no effect.

```
/ROOT/savestepopt 0     # no steps at all
/ROOT/savestepopt 1     # muon/hadron steps only (default)
/ROOT/savestepopt 3     # muon + non-shower e± steps  (requires savetrackopt >= 3)
/ROOT/savestepopt 7     # muon + all e± steps          (requires savetrackopt >= 7)
```

Example — muon tracks + muon steps only:
```
/ROOT/savetrackopt 1
/ROOT/savestepopt  1
```

Example — muon tracks + all e± tracks, but steps only for muons:
```
/ROOT/savetrackopt 7
/ROOT/savestepopt  1
```

### PMT hit detail (`/ROOT/savehitphoton`)

```
/ROOT/savehitphoton 0   # PMT hit counts only (default)
/ROOT/savehitphoton 1   # individual photon hit info per PMT (time, KE, ...)
```

## Output data classes (`libWCMDSimData`)

The standalone ROOT library can be loaded without Geant4:

```cpp
// In a ROOT macro or script
gSystem->Load("libWCMDSimData");

TFile * f = TFile::Open("muon_output.root");
TTree * tree = f->Get<TTree>("event");

MCTrackData * tracks = nullptr;
tree->SetBranchAddress("MCTrackData", &tracks);
```