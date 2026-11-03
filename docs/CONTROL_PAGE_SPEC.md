# LXR-K2 control layout and LXR engine migration — v0.4

**Design approved:** OSC has two pages of four engine-specific parameters, ordered by performance importance. Each track has **exactly two local modulation slots**. Desktop places routing (two destinations and depths) on physical **MOD** and their types/rates on physical **LFO**. Eurorack uses **MOD first press for routing, second press for types/rates**. All other available sound sections have one page of four controls initially. The shared clap/cymbal voice retains the same two MOD/LFO slots when switching engines. No modulation DSP changes are implemented by this document.

## Exact hardware distinction

- **Eurorack LXR DRUM:** seven sound-section buttons: **OSC, AEG, MOD, FM, CLICK, FILTER, MIX**. It has **no dedicated LFO sound-section button**. The module also has a separate MOD operating-mode button for the CV modulation matrix: do not conflate these two buttons.
- **Desktop LXR-02:** eight sound-section buttons: **OSC, AEG, MOD, FM, CLICK, FILTER, LFO, MIX**. Desktop LFO is a real physical button.
- **Unified K2 logical layout with device-specific physical access:** Desktop **MOD** is always Destination 1, Depth 1, Destination 2, Depth 2, while desktop **LFO** is always Type 1, Rate 1, Type 2, Rate 2. They are two views of the same two modulation slots, **not** an LFO shortcut to an additional hidden MOD page. Eurorack physical **MOD** toggles those two views via a second press/double tap. Four controls each remain on AEG, FM, CLICK, FILTER and MIX.
- Per voice-position, this is **OSC 8 + 2 shared modulation slots × 4 fields (8) + (AEG, FM, CLICK, FILTER, MIX × 4) = 36 distinct visible controls**. Desktop distributes the eight modulation fields across MOD and LFO; Eurorack displays them on two MOD pages.
- Do not repurpose or hide Eurorack's independent **CV MOD mode**, which contains 18 slots in the existing firmware; the two new local MOD slots do not replace its matrix.

## Exact modulation control requirement

| Physical section | Knob 1 | Knob 2 | Knob 3 | Knob 4 |
|---|---|---|---|---|
| **Desktop MOD / Eurorack MOD Page 1 (Routing)** | Destination 1 | Depth 1 | Destination 2 | Depth 2 |
| **Desktop LFO / Eurorack MOD Page 2 (Motion)** | Type 1 | Rate 1 | Type 2 | Rate 2 |

Each track has exactly **two local modulation slots**, each a tuple of {destination, depth, type, rate}; no third hidden local slot. The seven playable positions require **14 logical slot configurations**, though there are six render voices. **Clap and cymbal are different engines/edits for the same fifth render voice**: they use **the same two modulation slot values** and must display identical MOD and LFO parameters when switching between those engines. Changing cymbal to clap (or back) must not reset or replace the slot data. The uniqueness check operates on the one shared clap/cymbal pair, so a destination selected for the clap is unavailable to the second slot when the cymbal is edited, and vice versa. If the destination is unsupported by a newly selected engine, preserve its stored target and depth but show it as inactive; do not silently retarget it. Open/closed hats have separate planned modulation configurations but share one renderer and choke behavior; do not imply simultaneous independent playback.

**Type** is proposed as a generator shape/behavior: OFF, SINE, TRIANGLE, SAW, SQUARE, SAMPLE & HOLD or SMOOTH RANDOM. **Rate** adjusts speed in Hz (with synchronized divisions a future/secondary option). OFF produces no modulation; a depth of zero bypasses output. All waveforms must be bounded and deterministic. When switching a type, define deterministic phase/retrigger and click-free destination changes.

**Track-local destination exclusivity:** the two local modulation slots on the same playable track must never select the same non-OFF destination. Selecting a target in Slot 1 removes that target from Slot 2's available choices, and vice versa. If the user attempts a duplicate through controls that bypass the picker (MIDI, import, API), the update is rejected and the existing two destinations remain unchanged. OFF/None may appear in both slots. Distinct tracks may independently use the same destination *name* (e.g., each track's filter cutoff); exclusivity is scoped to one track's two slots, not to all voices or the Eurorack's separate 18-slot CV matrix. Destination identity is based on stable target IDs, not displayed labels. Destinations should be selected from engine-aware semantic targets (OSC controls 1–8 and eligible AEG/FM/CLICK/FILTER/MIX controls). Use stable persistent IDs. Depth is bipolar where the target permits it; at zero the source contributes no change. For audio-level parameters, clamp, smooth and apply modulation to a working copy of DSP state, *not* destructive writes to the underlying stored patch values.

The original code does **not** already implement two arbitrary independent mod generators per track. This requires future DSP/modulation work, memory budgeting, parameter storage and kit migration, not simply rearranging two menu rows. Existing single per-voice LFO/velocity/NRPN data and Eurorack CV modulation must remain compatible until a verified migration exists.

## Sound-section architecture

| Button | Controls | Proposed purpose |
|---|---:|---|
| **OSC** | **8**, 2 pages | Eight **engine-specific** core controls, four most important on Page 1 and four secondary on Page 2 |
| AEG | 4 | Additional envelope contour: Attack, Hold, Tail, Curve (provisional) |
| **MOD** | **4 desktop / 8 Eurorack** | Desktop: Destination 1, Depth 1, Destination 2, Depth 2. Eurorack: same routing on Page 1; Type 1, Rate 1, Type 2, Rate 2 on Page 2 |
| FM | 4 | Optional secondary synthesis/oscillator module (provisional, engine bypass when not useful) |
| CLICK | 4 | Optional transient/noise overlay (provisional) |
| FILTER | 4 | Optional post-engine filter/saturation (provisional) |
| MIX | 4 | Voice Level, Pan, Drive, Output (platform-dependent routing) |
| **LFO (desktop only)** | **4** | Type 1, Rate 1, Type 2, Rate 2 for the same two MOD slots; no third modulation source |

All supplemental pages must have sensible bypass/default states. Their exact four parameter names and DSP ordering are not yet approved.

## OSC navigation

- Select one of the seven playable track positions.
- First press **OSC**: Core Page 1; repeat OSC: Core Page 2; repeat: Page 1.
- **Desktop:** Press MOD for the routing/depth controls; press the separate physical LFO button for type/rate controls. Each opens a single four-control screen; neither is a nested page of the other.
- **Eurorack:** Press MOD to open routing/depth; press it again (the second tap/double-tap gesture) to display type/rate; another tap returns to routing. Implement against the existing second-press page toggling and validate button timing in hardware; do not interfere with the separate CV MOD operating mode.
- **Clap/cymbal:** switching the fifth track's active instrument/editor never changes the displayed MOD/LFO values or creates per-engine copies of these settings.
- Only four controls at a time; labels and datatypes derive from the chosen engine descriptor.
- A separate Engine picker does not consume any of the eight OSC controls; exact hardware shortcut remains to be decided.

## Stock LXR mapping: *UI-only first stage*

Implement the eight-control OSC editor by **direct mapping to existing PAR_* indices**. This does not change synthesis, reindex parameter IDs, remove original sound controls, or overwrite saved data. See [LXR stock voice mapping](LXR_STOCK_OSC_MAPPING.md) for precise mappings across all seven playable positions.

Stage 1 may temporarily show the same legacy parameter on OSC and its historical AEG/MOD/FM/CLICK page: this is deliberate while proving legacy behavior. Stage 2 will reconcile shared/supplemental pages without eliminating any current parameter access or making duplicated knobs misleading.

For imported legacy kits, preserve the byte-for-byte underlying parameter state and source DSP graph. **The LXR engine is the original engine running unmodified**; OSC is simply a prioritized view of eight existing controls until the new engine registry/preset extension is introduced.

## Safety and compatibility

- Static menu records use `Page` with eight `top*` names and eight `bot*` PAR_* indices; these already support two four-control views.
- Desktop has 8 section positions, Eurorack has 7. On desktop the physical LFO section is intentionally part of the MOD/LFO two-slot pair. On Eurorack the existing section layout lacks LFO. Do **not** blindly share static subpage indexes or confuse Eurorack sound-section MOD with its separate CV MOD operating mode.
- Do not insert new `PAR_*` values into the middle of `Core/Inc/Parameters.h`; the current `presetManager.c` serializes positional bytes and stock MIDI CC/NRPN depends on index identity.
- Desktop's menu and physical ADC driver and Eurorack's CV/MOD mode must be audited together before implementing dynamic engine descriptors or two mod generators.
- The same track-local uniqueness guard must be enforced in the menu picker, MIDI/programmatic changes, and patch import, with clear feedback for rejected duplicates. For a malformed saved patch containing duplicate non-OFF targets, keep Slot 1 and set Slot 2's destination to OFF on load while retaining its depth/type/rate values; report that the patch was repaired and do not write it back until the user saves.
- All future firmware builds must report ITCM, flash, timing and hardware validation. Current specification changes are documentation only.
