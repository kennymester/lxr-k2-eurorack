# LXR-K2 — Multi-engine firmware design (proposal v0.2)

Status: **DESIGN PROPOSAL ONLY** — no audio DSP, preset format, or UI code changed. Review with the project owner before implementation. Prepared 2026-11-03 for both LXR-K2 desktop and Eurorack.

## Product intent

Turn the LXR-02 into a streamlined multi-engine percussion synthesizer. Each engine exposes exactly **eight core controls under OSC**, split into Page 1 (four highest-priority musical controls) and Page 2 (four secondary controls). The two-slot modulation UI has **MOD routing and LFO motion on separate physical buttons on desktop**, and **two toggled MOD screens on Eurorack**. The routing view is Destination 1, Depth 1, Destination 2, Depth 2; motion is Type 1, Rate 1, Type 2, Rate 2. AEG, FM, CLICK, FILTER and MIX have one page of four controls each for now (36 distinct sound/channel controls per track). Four knobs align with four LCD columns. The engine picker sits outside the OSC eight controls. See [control/page specification](CONTROL_PAGE_SPEC.md).

Engine families:
1. **LXR** — curated, simplified access to the stock sound architecture without eliminating its original DSP code.
2. **Classic** — original implementations inspired by the musical behavior of Roland TR-909, TR-808, TR-707, TR-606 percussion. These are not ROM dumps, trademarked product repackagings, or clones of proprietary software.
3. **Experimental** — wholly original algorithmic percussion: PM/FM, feedback, wavefolding, resonators, chaotic modulation, noise and spectral textures. The target is the aesthetic range of modern digital percussion, not copying another manufacturer's firmware or presets.

## Hard interface requirements

**Design decisions confirmed by owner:** eight engine-specific OSC parameters per voice, priority controls on OSC Page 1; exactly two local modulation routes per track configured through desktop MOD+LFO buttons or Eurorack's two MOD screens; AEG/FM/CLICK/FILTER/MIX with four controls each. Desktop has an eighth physical sound-section button (LFO) not present on Eurorack; it shows two type/rate pairs directly, with no third modulation slot. Clap/cymbal retains one shared MOD/LFO configuration when switching between its engines. Specific supplemental-processing controls are provisional. See [CONTROL_PAGE_SPEC.md](CONTROL_PAGE_SPEC.md) for proposed button roles and navigation.


- **OSC button:** Page 1 (Core) shows four controls most important to each engine's musical character; Page 2 (Shape) shows its four less essential controls. Every engine defines its own ordered eight controls.
- Only one four-column row is active at any time; knob/encoder N always edits the value under screen column N.
- Page selection must be efficient with existing controls; desktop four encoders versus Eurorack four endless pots and a separate DATA encoder require distinct input handlers but the **same parameter layout**.
- Dedicated per-voice **ENGINE** selection is outside eight sound parameters; it is kit-scoped, displays engine family/variant, and must be accessible without consuming a sound-page slot.
- **Hardware-specific physical buttons:** Eurorack has seven sound sections (**OSC, AEG, MOD, FM, CLICK, FILTER, MIX**); Desktop has those seven plus physical **LFO**. OSC gets 2×4 engine controls. Desktop MOD has 1×4 routing controls and desktop LFO has 1×4 motion controls; Eurorack MOD toggles between those two four-control screens on a second press. Each of AEG/FM/CLICK/FILTER/MIX gets 1×4 initially. The two mod slots are not replicated for clap/cymbal engine changes; no extra independent modulator. Preserve separate Eurorack CV MOD operating mode, MIDI, kits, FX, and desktop sequencer. Exact engine-selection gesture remains open.
- Mixer-level volume, pan and output routing belong under **MIX** (or a deliberate platform-specific utility screen), outside OSC's eight core controls. Decimation, MIDI channel, sequencer functions, global FX and the Eurorack modulation matrix must remain accessible from utility/mixer/configuration/performance views; none may silently disappear.
- Let **each engine** define its own eight meaningful OSC parameters, ordered by priority (Page 1 before Page 2), with readable labels, units, sensible curves and musical defaults. AEG/FM/CLICK/FILTER/MIX are reusable additional sections; MOD implements two slot-based generators. Supplemental processing must default to neutral/bypass, not alter classic-engine character unexpectedly.
- Do not multiplex one knob into a secret extra function on OSC to evade the engine-specific eight-control promise. Additional shared processing belongs in clearly marked section buttons.

## Hardware voice inventory: critical distinction

The original firmware exposes **seven note/trigger positions** using **six synthesis engines**:
- Tracks/trigger positions **1–3**: three independently assignable drum engines.
- Position **4**: snare instrument, selectable snare-family algorithms.
- Position **5**: **one combined Clap/Cymbal instrument**, not independent simultaneous clap and cymbal voices. Engine selection switches that slot between clap, cymbal and related percussion types. **Both engine choices share the same saved MOD/LFO slot values**, so switching between them never resets or creates alternate modulation settings. Preserve unsupported destination selections as inactive rather than remapping automatically.
- Positions **6 (closed hat) and 7 (open hat)**: two playable triggers that share **one** HiHatVoice renderer and normally choke/retrigger each other.

K2 design target: store **separate engine ID and eight-parameter profile** for closed and open hi-hats (independent selection/editing), while preserving the existing shared hat voice and closed/open choke semantics at first. On a trigger, choose the corresponding articulation profile, then render on the shared hi-hat voice. Do not promise simultaneous, overlapping independent open/closed engines without explicitly allocating additional voices and measuring resource usage. Preserve the ability for a closed hat to cut off an open hat.

All three drum tracks may select different engine variants; there must be no hardcoded assignment of Drum 1 to kick only, Drum 2 to tom only, etc.

## Engine selection and patch persistence

Proposed model:
```c
typedef struct {
    uint16_t engine_id;        // persistent, stable registry ID
    uint8_t param[8];         // normalized 0..127, per-engine descriptor controls units/defaults
} K2EnginePatch;

K2EnginePatch k2_voice_patch[7];  // 7 playable trigger positions, 6 underlying synthesis voices
```

This is an architectural sketch, **not production C**. Preserve existing source/headers until parameter mapping and storage have been audited; use meaningful IDs per engine family+type with never-reused values. Decouple voice number, patch slot, and render instance to avoid hat/clap confusion.

Rendering contract for each algorithm family:
- `init`, `loadPatch`, `setParameter`, `noteOn/accent`, `renderBlock`, `reset`.
- Reuse oscillator/envelope/filter components where feasible; implement new algorithms in separate DSP modules, with no malloc, blocking file I/O, and no floating-point transcendental calls in unbounded per-sample loops.
- If an engine must be swapped mid-tail, define deterministic fade/choke/crossfade; no reuse of incompatible DSP state without reset.
- Parameter changes and modulated values require smoothing where discontinuities could cause clicks.

## Preset compatibility: strict safety gates

Current `Core/Inc/Parameters.h` uses positional PAR_* values and `Preset/presetManager.c` saves `END_OF_SOUND_PARAMETERS` raw indexed bytes. That is a **binary compatibility constraint**, not an extensible engine-patch schema.

- Do **not** insert new PAR_* enum members into the middle of the legacy index space, delete the old fields, renumber MIDI CCs, or repurpose existing sound parameters globally.
- An imported stock kit uses the original DSP parameter state and starts with the LXR engine selected. The first load must be sonically unchanged. Curated eight LXR controls edit eight chosen parameters/macros; other historical parameter values remain intact in the underlying kit data until explicitly reset.
- For native K2 patches, design a **versioned extension/sidecar** with magic, schema version, stable per-trigger engine ID, all eight parameter values, integrity/CRC checks and explicit association to a kit. Select the exact file format only after examining the existing naming and save lifecycle. Unknown/invalid records must safely fall back to stock LXR behavior, not zero memory.
- Define how switching engines handles old settings: each engine has its own reset defaults and optional previous values; do not accidentally interpret a 909 filter param as an FM ratio or lose an existing kit on save.
- Test RAM kit morph, temporary autosaves, desktop project/pattern changes, direct SD load/save, and Eurorack CV save behavior before shipping.
- Kit data produced by custom firmware must never be claimed fully compatible with stock firmware; keep backups and a migration/rollback path.

## Modulation compatibility

- Reconcile the old `modTargets` indexes with eight virtual sound controls without changing old IDs.
- New K2 per-engine modulation destinations should have semantic IDs: `voice.position + engine.controlSlot`; mapping to DSP properties is engine-specific.
- Desktop: check MIDI CC/NRPN automation and step parameter locks, LFO and velocity modulation.
- Eurorack: check 18-slot `ModMatrix_Processor.c`, five physical CV sources, accents and 1V/oct tracking; patch-switching must not leave stale modulation pointers.
- Audio-rate modulation is a separate optional phase. Initial target: bounded block-rate control updates with smoothing.

## Performance limits

The source indicates STM32F765VG (216 MHz), audio output block size 64, approximate configured actual sample rate 44,140 Hz, and 16 KiB of tightly constrained ITCM. Record flash, ITCM, DTCM and peak callback cycles across all six voices and FX at maximum load. Every classic algorithm needs an explicit CPU envelope; PCM-based engines need a licensed asset format, sample storage plan, decoding/looping, and no SD reads in the audio interrupt.

## Rollout / implementation

1. Freeze verified stock build and copy representative factory/user kits for regression.
2. Add a centralized engine descriptor registry and an eight-control **UI-only LXR view** backed by original parameters: no synthesis change.
3. Add engine selector to voice editor/kit state without consuming one of the eight sound slots, with careful per-trigger mapping.
4. Version K2 patch data and implement full round-trip loading/saving/migration; keep stock .img update and bootloader regions untouched.
5. Implement original new DSP engines incrementally, beginning with 909 and 808, then 606. Treat 707 as a PCM content and licensing dependency before scheduling full fidelity.
6. Develop original experimental algorithms.
7. Port and hardware-regress changes independently on desktop and Eurorack, including hat choking, pitch CV, MIDI, CPU and kit serialization.
8. Require modified-firmware startup labeling consistent with NOTICE.md before distributing any changed binary.

## Items to confirm with owner

- Is engine selection a dedicated voice-level editor action, or an action reached by holding SHIFT while the voice is selected? **Recommendation:** voice-level ENGINE picker outside the sound pages.
- Are mixer, performance, FX, modulation and MIDI/CV settings allowed to retain their present separate screens? **Recommendation:** yes — eight applies only to per-engine sound design.
- Should the hat open and closed articulations support **independent engine choice** despite sharing one voice and mutual choking? **Recommendation:** yes, with shared rendering in v1.
- Should classic 707 be faithful PCM playback, or an original synthesis approximation with a 707-style character? Rights/space/testing differ greatly.
