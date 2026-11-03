# LXR-K2 control layout and LXR engine migration — v0.3

**Design approved:** OSC has two pages of four engine-specific parameters, ordered by performance importance. MOD has two pages of four parameters with **two modulation routes per playable track**. All other available sound-section buttons have one page of four controls initially. No engine DSP changes are approved by this document.

## Exact hardware distinction

- **Eurorack LXR DRUM:** seven sound-section buttons: **OSC, AEG, MOD, FM, CLICK, FILTER, MIX**. It has **no dedicated LFO sound-section button**. The module also has a separate MOD operating-mode button for the CV modulation matrix: do not conflate these two buttons.
- **Desktop LXR-02:** eight sound-section buttons: **OSC, AEG, MOD, FM, CLICK, FILTER, LFO, MIX**. Desktop LFO is a real physical button.
- **Unified K2 interface:** same OSC and MOD layouts on both targets. The additional desktop LFO button is proposed as a **direct shortcut to MOD Page 2**, rather than a new third modulation slot. Exact key handling must be regression-tested against existing desktop interaction. Four controls across each of AEG, FM, CLICK, FILTER and MIX.
- Per voice-position, this is **OSC 8 + MOD 8 + (AEG, FM, CLICK, FILTER, MIX × 4) = 36 distinct visible controls**. The desktop shortcut does not increase the count.
- Do not repurpose or hide Eurorack's independent **CV MOD mode**, which contains 18 slots in the existing firmware; the two new local MOD slots do not replace its matrix.

## Exact MOD page requirement

| Section | Knob 1 | Knob 2 | Knob 3 | Knob 4 |
|---|---|---|---|---|
| **MOD Page 1 (Routing)** | Destination 1 | Depth 1 | Destination 2 | Depth 2 |
| **MOD Page 2 (Motion)** | Type 1 | Rate 1 | Type 2 | Rate 2 |

Each track has exactly **two local modulation slots**, each a tuple of {destination, depth, type, rate}; no third hidden local slot. Two slots per **seven playable positions** means 14 logical slot configurations, though only six voice engines render audio. Open/closed hats have separate saved modulation configurations but share one hat DSP renderer; when their trigger arrives, use that articulation's settings with existing choke behavior. No simultaneous independent hat voices are implied.

**Type** is proposed as a generator shape/behavior: OFF, SINE, TRIANGLE, SAW, SQUARE, SAMPLE & HOLD or SMOOTH RANDOM. **Rate** adjusts speed in Hz (with synchronized divisions a future/secondary option). OFF produces no modulation; a depth of zero bypasses output. All waveforms must be bounded and deterministic. When switching a type, define deterministic phase/retrigger and click-free destination changes.

Destinations should be selected from engine-aware semantic targets (OSC controls 1–8 and eligible AEG/FM/CLICK/FILTER/MIX controls). Use stable persistent IDs. Depth is bipolar where the target permits it; at zero the source contributes no change. For audio-level parameters, clamp, smooth and apply modulation to a working copy of DSP state, *not* destructive writes to the underlying stored patch values.

The original code does **not** already implement two arbitrary independent mod generators per track. This requires future DSP/modulation work, memory budgeting, parameter storage and kit migration, not simply rearranging two menu rows. Existing single per-voice LFO/velocity/NRPN data and Eurorack CV modulation must remain compatible until a verified migration exists.

## Sound-section architecture

| Button | Controls | Proposed purpose |
|---|---:|---|
| **OSC** | **8**, 2 pages | Eight **engine-specific** core controls, four most important on Page 1 and four secondary on Page 2 |
| AEG | 4 | Additional envelope contour: Attack, Hold, Tail, Curve (provisional) |
| **MOD** | **8**, 2 pages | Two destinations/depths; two types/rates as defined above |
| FM | 4 | Optional secondary synthesis/oscillator module (provisional, engine bypass when not useful) |
| CLICK | 4 | Optional transient/noise overlay (provisional) |
| FILTER | 4 | Optional post-engine filter/saturation (provisional) |
| MIX | 4 | Voice Level, Pan, Drive, Output (platform-dependent routing) |
| Desktop-only physical LFO | 0 new controls | Proposed shortcut to MOD Page 2, without a third local modulation source |

All supplemental pages must have sensible bypass/default states. Their exact four parameter names and DSP ordering are not yet approved.

## OSC navigation

- Select one of the seven playable track positions.
- First press **OSC**: Core Page 1; repeat OSC: Core Page 2; repeat: Page 1.
- Press another sound-section button to go to its first page. Repeat **MOD** to alternate its two pages.
- The desktop-only LFO button, if enabled as shortcut, opens **MOD Page 2** directly.
- Only four controls at a time; labels and datatypes derive from the chosen engine descriptor.
- A separate Engine picker does not consume any of the eight OSC controls; exact hardware shortcut remains to be decided.

## Stock LXR mapping: *UI-only first stage*

Implement the eight-control OSC editor by **direct mapping to existing PAR_* indices**. This does not change synthesis, reindex parameter IDs, remove original sound controls, or overwrite saved data. See [LXR stock voice mapping](LXR_STOCK_OSC_MAPPING.md) for precise mappings across all seven playable positions.

Stage 1 may temporarily show the same legacy parameter on OSC and its historical AEG/MOD/FM/CLICK page: this is deliberate while proving legacy behavior. Stage 2 will reconcile shared/supplemental pages without eliminating any current parameter access or making duplicated knobs misleading.

For imported legacy kits, preserve the byte-for-byte underlying parameter state and source DSP graph. **The LXR engine is the original engine running unmodified**; OSC is simply a prioritized view of eight existing controls until the new engine registry/preset extension is introduced.

## Safety and compatibility

- Static menu records use `Page` with eight `top*` names and eight `bot*` PAR_* indices; these already support two four-control views.
- Desktop has 8 section positions, Eurorack has 7; do **not** assume one shared index constant and blindly repurpose index 6 as a desktop LFO.
- Do not insert new `PAR_*` values into the middle of `Core/Inc/Parameters.h`; the current `presetManager.c` serializes positional bytes and stock MIDI CC/NRPN depends on index identity.
- Desktop's menu and physical ADC driver and Eurorack's CV/MOD mode must be audited together before implementing dynamic engine descriptors or two mod generators.
- All future firmware builds must report ITCM, flash, timing and hardware validation. Current specification changes are documentation only.
