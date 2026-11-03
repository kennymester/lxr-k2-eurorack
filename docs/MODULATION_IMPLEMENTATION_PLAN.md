# LXR-K2: two-slot modulation engine — implementation plan v0.1

Status: design/engineering plan. **No new DSP, UI MOD/LFO controls, parameter persistence, or migration have been implemented or hardware tested.**

## Approved contract (same logical slots, different buttons)

Two independent local modulation assignments per playable track position (7 positions, so 14 logical slot records):

| Control | Desktop LXR-02 | Eurorack LXR |
|---|---|---|
| Destination 1 | MOD knob 1 | MOD routing Page 1 knob 1 |
| Depth 1 | MOD knob 2 | MOD routing Page 1 knob 2 |
| Destination 2 | MOD knob 3 | MOD routing Page 1 knob 3 |
| Depth 2 | MOD knob 4 | MOD routing Page 1 knob 4 |
| Type 1 | LFO knob 1 | MOD motion Page 2 knob 1 |
| Rate 1 | LFO knob 2 | MOD motion Page 2 knob 2 |
| Type 2 | LFO knob 3 | MOD motion Page 2 knob 3 |
| Rate 2 | LFO knob 4 | MOD motion Page 2 knob 4 |

Desktop MOD and LFO are **separate one-page button screens**. Eurorack has no physical LFO section button: first press MOD opens routing, second press/double tap selects motion, subsequent press returns to routing. Use current voice-mode page toggle semantics wherever possible without affecting the separate Eurorack **SELECT_MODE_MOD** CV-matrix mode.

**Clap/cymbal (track 5)** is one synthesis voice with two possible instrument engines and **one shared MOD/LFO configuration**. Both instrument editors display identical mod source type, rate, destination and depth, and the settings never change when the engine switches. If a destination is unsupported by the selected engine, keep the saved destination/depth intact, clearly indicate inactive, and restore effectiveness when an engine supporting it is selected. The engine-specific sound pages (OSC) may differ. Do not allocate an extra 14th/15th modulation slot for clap vs cymbal mode.

**Hi-hat positions 6 and 7** retain separate planned local modulation configurations while sharing one audio renderer and mutual choke; activating the other articulation loads/uses its settings safely. That is different from the clap/cymbal rule and must be tested.

## Legacy implementation facts, traced from source

- Each of the six render voices currently embeds **one** `Lfo` state: `DrumVoice` ×3, `Snare`, `CymbalVoice`, `HiHat`.
- `Core/Inc/Soundengine/DSPAudio/lfo.h` includes native SINE, TRI, SAW_UP, SAW_DOWN, REC, NOISE, EXP_UP, EXP_DOWN waveform IDs. Avoid changing native IDs. `LFO_MAX_F=200` Hz is a defined ceiling, not a K2 modulation-rate promise.
- `Core/Src/Soundengine/DSPAudio/modulationNode.c` directly updates/restores original parameter state and **enumerates only the six existing Lfo.modTarget pointers**, plus six velocity modulation nodes, in `modNode_originalValueChanged()` and `modNode_resetTargets()`.
- `modNode_setDestination()` calls `modNode_resetTargets()`; adding new destinations without changing this logic can clobber the original value of a jointly modulated target.
- On Eurorack `Core/Src/Soundengine/ModMatrix_Processor.c` exposes a separate CV matrix with 18 slots and currently sources single per-voice LFO states. **Do not reduce or replace those 18 slots.**
- Stock `Core/Inc/menu/menuPages.h` holds positional PAR_ IDs for section pages; `Core/Src/Preset/presetManager.c` serializes legacy sound parameter bytes in that order. Do not insert new IDs between existing enum values or destructively rewrite factory kits.
- The stock LXR OSC eight-control priority view is already implemented as a separate draft PR #3. Preserve its exact mappings while developing new MOD/LFO behavior.

## Proposed data model (sketch only)

```c
typedef struct {
    uint16_t target_id;  // stable engine-aware semantic destination ID, NOT PAR_* index reinterpreted
    int8_t depth;        // suggest -63..+63 around neutral zero, final scaling TBD
    uint8_t type;        // stable waveform enum with OFF plus native shapes
    uint8_t rate;        // stored 0..127, mapped to a meaningful Hz/sync law
} K2ModSlotConfig;

K2ModSlotConfig mod[7][2];  // track 5 is the common clap/cymbal, positions 6 & 7 separate hats
```

This shape is conceptual. It is not built, does not change native .kit files, and does not specify the final persistence encoding. Type and rate values must have stable, well-documented units and defaults. Do not let a numeric `0` target inadvertently point to an actual DSP parameter.

## Runtime composition requirements

1. For each of six render voices, evaluate up to two active local modulation sources per audio block, using track-selected settings. Only one hat articulation is active in the shared renderer; no new simultaneous hat polyphony is implied.
2. Preserve original parameter base values in the current patch or parameter view. Compute modulation as **base + contribution_1 + contribution_2 + existing CV/velocity/automation**, with documented scaling and clamping, rather than repeatedly changing stored `activeParameters` and attempting to infer the old value later.
3. Two slots must support the same target without fighting: define deterministic summing and clipping; an inactive engine target must have no DSP side effects but must retain settings.
4. Apply per-target smoothing at block/control rate to avoid clicks, zipper noise and unintended transient alteration; real-time code must avoid heap allocations, blocking SD I/O and unbounded loops.
5. Keep existing MIDI, velocity modulation and 18 Eurorack CV-matrix routes working. The native `modNode_resetTargets` and `modNode_originalValueChanged` code requires an audited refactor or wrapper to track all active K2 routes without stale pointers.
6. Decide trigger/phase behavior explicitly (default may be phase reset on a note, or free-running) before implementation. Leave rate waveform details provisional until tested.
7. Engine change should not mutate or reset the clap/cymbal shared route data. New engines must expose a capability table of valid destinations; invalid targets are displayed as inactive, preserved and not silently rebound.
8. A new K2 kit format or sidecar must persist all 14 slot configurations and version info, with fallback to the untouched stock LXR sound when absent. Preserve old stock MIDI addresses and preset indices.

## Target-specific UI and edit guards

- Desktop: currently section index 2 is MOD, index 6 is LFO. Populate their existing separate 4-column screens from the **same two-slot model**. No desktop MOD second page and no physical third slot.
- Eurorack: section index 2 is MOD; existing `Page` supports two rows of four. MOD Page 1/2 toggle can reuse section button second-press behavior; confirm rapid double tap and press timing in hardware. Separate Eurorack CV MOD mode is an independent path and must remain functional.
- Other sections (AEG, FM, CLICK, FILTER, MIX) initially display four controls each. Stock OSC has eight and remains unchanged.
- In both platforms the label for a source's Type/Rate in the editor should make slot 1 and slot 2 association obvious.

## Incremental implementation PR sequence

1. **State + tests**: add a standalone two-slot `K2ModSlotConfig` model and pure parameter conversion/default helpers with no audio-engine wiring, no changes to original kit file format. Test slot count (14), clap/cym shared indexing, invalid targets and OFF defaults.
2. **UI**: bind desktop MOD and LFO to the shared model; bind Eurorack MOD two-row editor to it without modifying Eurorack CV MOD. Keep DSP bypassed during UI-only validation; visibly indicate inactive outputs instead of pretending modulation is active.
3. **DSP**: integrate two bounded modulation generators per render voice with safe target summing, no stale state or destructive parameter changes; measure worst-case cycles and flash/ITCM pressure.
4. **Persistence**: versioned K2 kit extension, opt-in migration, full round-trip and stock-kit rollback tests. Do not ship unsigned or hardware-untested images as releases.
5. **Hardware regression**: 7 trigger positions, 6 render voices, shared cymbal/hat states, MIDI transport, desktop clock/sequence, Eurorack CV and 18-slot matrix, factory kit exact-audio comparisons, all FX/output routing.

## Acceptance checklist

- [ ] Desktop MOD shows Dest 1 / Depth 1 / Dest 2 / Depth 2; LFO shows Type 1 / Rate 1 / Type 2 / Rate 2.
- [ ] Eurorack MOD second tap toggles to Type/Rate and another returns to Dest/Depth; CV MOD mode remains intact.
- [ ] Two independent modulation assignments operate on all three drum tracks and the snare.
- [ ] Clap/cym MOD/LFO settings are identical and unchanged across changing engines and editing views.
- [ ] Open/closed hat configurations are distinct, retrigger/choke works as intended, no simultaneous-hat claim.
- [ ] Both modulators can address the same valid target deterministically; OFF or zero depth is neutral.
- [ ] Any engine switch preserves unsupported destination choices without remapping/modulating the wrong DSP field.
- [ ] Existing factory kits, parameter indices, MIDI CC, velocity modulation and Eurorack 18-slot CV routing remain compatible.
- [ ] Clean desktop/Eurorack CI builds, measured CPU/memory, and physical device tests are documented before firmware release.
