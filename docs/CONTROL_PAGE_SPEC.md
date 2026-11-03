# LXR-K2 — Front panel and eight-control OSC editor (design v0.2)

**Status:** Design decision confirmed 2026-11-03. No DSP, UI or preset implementation has been changed yet. This is a proposed mapping to refine during engine development.

## The owner's UI decision

The selected voice exposes all eight existing sound-page/section buttons. **OSC** owns the instrument-specific engine editor: **two pages, four parameters per page, eight total**. Page 1 contains the four most important expressive controls for that particular engine. Page 2 contains four additional, less essential sound-character controls. All engines can have a different eight-control vocabulary, with deliberate consistency only when it helps musicians.

The **other seven section buttons remain available** and initially expose **one page of four parameters each**. Four parameters is a v1 presentation limit, not a hard architectural maximum. The full instrument editor therefore has 8 + 7×4 = **36 visible controls** before system settings.

The existing source maps eight select buttons to eight subpage slots in `Core/Inc/menu/menuPages.h`; each `Page` descriptor already holds eight parameter positions (two displayed rows of four). `menu_switchSubPage(buttonNr, 1)` already supports toggling half-pages on repeat selection. The proposed change is an adaptation, not a rewrite from scratch.

## Proposed button assignment

| Select button | Sound-page role | Controls, v1 | Design notes |
|---|---|---|---|
| **OSC** | Selected synthesis engine | **8 (2×4)** | Engine-specific; Page 1 priority musical controls, Page 2 secondary sound-shaping controls |
| **AEG** | Additional amplitude contour | **4** | Attack, Hold, Tail, Curve (proposal); neutral/bypass defaults, not a second redundant engine Decay |
| **MOD** | One flexible modulation assignment | **4** | Source, Target, Depth, Slew; explore MIDI velocity/accent/aux inputs and target-specific mapping |
| **FM** | Optional synthesis layer | **4** | Mode, Ratio, Depth, Feedback; a parallel/secondary-oscillator layer where applicable, bypass for an unneeded classic or PCM engine |
| **TRANS** | Optional transient/noise overlay | **4** | Type, Amount, Tone, Length; preserve natural clicks embedded in classic engines |
| **FILTER** | Additional post-engine filter | **4** | Cutoff, Resonance, Type, Drive; bypass default to preserve engine-authentic voicing |
| **LFO** | Performance modulation oscillator | **4** | Rate, Wave, Destination, Amount; detailed sync/retrigger might later receive a second page |
| **MIX** | Per-voice channel | **4** | Level, Pan, Drive, Output (hardware mapping differs between desktop/Eurorack) |

These are **draft roles** for AEG through MIX. The user has approved the number of controls and the OSC placement, *not* these exact parameter names or optional layers. The physical TRANS button corresponds to the current transient section; exact panel lettering may be shorter.

## Proposed navigation

- Select a drum, snare, clap/cym or hat articulation using the existing voice/track selection.
- Press **OSC** to enter its Page 1 of four highest-priority engine-specific controls.
- Press **OSC** again to toggle to Page 2. Press again to return to Page 1. A small page indicator should distinguish `OSC 1/2` vs `OSC 2/2`.
- Press any of the **other seven buttons** to enter its one-page (four-control) editor.
- Return to OSC at Page 1 by default when changing voice or engine, while allowing retained page state on a same-voice quick return if straightforward.
- The selected engine's **identity** should remain visible in editor context without using one of its eight knobs.
- Engine selection should be entered with a separate voice-level command, and the four knobs or data encoder should browse/confirm engine variants. **Exact button gesture is pending hardware review.** Desktop's `SHIFT + OSC` is a candidate: SHIFT+section is currently unimplemented in its voice-select path; Eurorack has a different compile-gated SHIFT behavior and must not be assumed to accept that gesture.
- The existing menu system, FX, desktop sequencer and Eurorack MOD matrix modes should remain operational and not be consumed by this editor redesign.

## Core engine examples (OSC only)

| Engine | OSC Page 1 — priority | OSC Page 2 — secondary |
|---|---|---|
| LXR Drum | Tune, Decay, Sweep, Punch | FM Amount, Ratio, Transient, Tone |
| 909 Kick | Tune, Decay, Punch, Attack | Body, Pitch Curve, Osc Color, Click Tone |
| 808 Kick | Tune, Decay, Tone, Attack | Sub, Pitch Bend, Harmonics, Click Tone |
| 909 Snare | Tune, Decay, Snappy, Tone | Noise Color, Body, Rattle, Attack |
| 808 Clap | Decay, Tone, Scatter, Attack | Noise Color, Tail, Flutter, Stereo |
| 606 Closed Hat | Decay, Tone, Metal, Attack | Tuning, Noise Blend, Sizzle, Choke Shape |
| K2 Phase | Tune, Decay, Ratio, Index | Feedback, Fold, Motion, Tone |

These parameters are deliberately engine-specific and **not yet locked**. Engine page 1 should contain what users change most often in a performance. Avoid including generic mixer Level/Output in an engine's eight when those are consistently accessible on MIX. Likewise, any extra processing page must augment rather than duplicate or unexpectedly override the core oscillator model.

## Layering/compatibility semantics to specify before coding

- **OSC** controls the **core renderer** that creates the primary sound.
- **AEG/MOD/FM/TRANS/FILTER/LFO/MIX** configure reusable processing or modulation modules. A module can be unavailable or safely bypassed for an engine that cannot support it efficiently or musically. A bypass state must not silently consume CPU or alter a factory sound.
- Default supplemental layers must be neutral when selecting a new engine so that a classic 808/909/606 engine sounds correct without setting seven extra pages.
- AEG is a *post-engine contour*, if implemented, not a separate copy of the engine's natural decay. Define neutral behavior, retrigger interaction and choke semantics carefully.
- MOD and LFO routing should share a stable semantic target registry: a per-engine control slot (OSC 1–8) or a supplemental-page control. Existing MIDI/NRPN indexes and Eurorack 18-slot CV matrix are **not** renumbered.
- FM's four controls are placeholders for a **secondary synthesis layer**, not a requirement that every model use FM (e.g., sample-based 707).
- TRANS is an additional optional exciter, not a second copy of the algorithm's essential click/noise model.
- The FILTER page is a post-engine optional filter, independent from internal filter shaping the engine sound.
- MIX is an output channel stage, not part of the eight engine controls; respect desktop vs Eurorack audio-routing differences.
- Four parameters per supplementary page is the v1 presentation limit. Extend per-section page descriptors later through a versioned expansion mechanism without changing the OSC 8-control contract.

## Storage model and backward compatibility

The existing stock parameter array and kit file format use positional IDs. Preserve all legacy indexed parameters and the original kit as imported. Do not renumber existing `PAR_*`, MIDI CC, modulation targets or parameter-order arrays.

The future K2 kit extension will need to persist: per-play-position engine ID + eight core parameters; supplemental-page parameters and bypass states; external modulation destinations; and independent open/closed hat engine profiles. Design a versioned extension or sidecar associated with a kit, preserve legacy kit data and allow rollback. Factory kits imported into K2 must sound the same until the user actively selects a new engine or supplemental processing.

## Engineering implications

- The existing `Page` structure supports eight slots; existing `menu_switchSubPage` can toggle 1st/2nd row. New dynamic engine descriptors must provide labels, ranges, defaults and conversion to DSP state instead of assuming `PAR_*` indexes.
- The OSC page is **dynamic by selected engine**; AEG through MIX initially use mostly common descriptors, with per-engine applicability/bypass.
- Input handling currently reads the active static `menuPages` table from multiple code paths, so a complete implementation must redirect reads in `menu.c`, `IO/adcPots.c`, parameter type/value conversion and menu repaint functions rather than just changing one table.
- Each device has seven playable triggers but only six synth voices; closed/open hi-hats share the underlying voice. Persist articulation-specific OSC engine settings without promising simultaneous independent rendering or changing choking behavior initially.
- CMake baseline builds have passed previously, but no new interface feature has been compiled or hardware tested.

## Next design questions

1. Choose exact AEG through MIX parameters and which sections may have engine-specific variations.
2. Decide engine selection gesture on each hardware target.
3. Define default state and layering order for FM, TRANS, FILTER and AEG.
4. Specify envelope independence, smoothing, modulation targets and MIDI/CV control mappings.
5. Begin a UI-only OSC prototype with the unmodified LXR renderer and use factory-kit audio comparisons to prove legacy compatibility.
