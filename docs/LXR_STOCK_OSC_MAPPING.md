# LXR-K2 stock engine OSC mapping — stage 1, exact legacy PAR_* values

**Implementation scope:** expose eight existing stock firmware parameters per OSC button in two 4-knob pages. All original DSP processing and all original `PAR_*` enum indices and kit byte storage remain untouched. No new synth engine or modulation generator is implemented by the UI remapping.

## Prioritization rules

1. OSC Page 1 contains the four controls most useful for immediate sound sculpting; Page 2 contains another four character controls.
2. Prefer existing `PAR_*` controls rather than synthetic macros in the first revision, to guarantee identical behavior and preserve old kits.
3. The OSC editor may temporarily repeat controls that still exist on a legacy AEG/FM/CLICK page; subsequent reconciliation will remove duplication once new supplemental processors are designed.
4. This is **stock LXR**, not a new classic drum emulation. These eight controls should make sense for each of the four native DSP structures.
5. Any printed shorthand label should correspond to the stock parameter's **actual semantics**. For example, pitch envelope amount is not a generic “Punch” knob, and Snare's noise frequency is not its noise/tonal mix.

## Drum 1, 2, 3 (same structure, each independently editable)

For each drum voice index N=1,2,3:

| Knob | OSC Page 1 (priority) | Existing parameter | Page 2 (character) | Existing parameter |
|---|---|---|---|---|
| 1 | Coarse Tune | `PAR_COARSEN` | Main Wave | `PAR_OSC_WAVE_DRUMN` |
| 2 | Amp Decay | `PAR_VELODN` | Pitch EG Decay | `PAR_MOD_EGN` |
| 3 | Pitch EG Amount | `PAR_MODAMNTN` | FM Amount | `PAR_FMAMNTN` |
| 4 | Transient Volume | `PAR_TRANSN_VOL` | FM Frequency | `PAR_FM_FREQN` |

The label `PAR_COARSEN` means `PAR_COARSE1`, `PAR_COARSE2` or `PAR_COARSE3` (and similarly for other fields). This uses **no new parameters**. Existing pitch slope, amp slope, transient pitch/wave, filter, PWM and FM mode remain reachable in legacy pages.

## Snare (trigger position 4, DSP voice 4)

| Knob | OSC Page 1 | Existing parameter | OSC Page 2 | Existing parameter |
|---|---|---|---|---|
| 1 | Oscillator Tune | `PAR_COARSE4` | Oscillator Wave | `PAR_OSC_WAVE_SNARE` |
| 2 | Amp Decay | `PAR_VELOD4` | Pitch EG Decay | `PAR_MOD_EG4` |
| 3 | Tone/Noise Mix | `PAR_MIX1` | Repeat | `PAR_REPEAT4` |
| 4 | Noise Frequency | `PAR_NOISE_FREQ1` | Transient Volume | `PAR_TRANS4_VOL` |

`PAR_MIX1` sets tonal/noise balance; `PAR_NOISE_FREQ1` is noise oscillator/tuning behavior, not mix.

## Cymbal/Clap (trigger position 5, DSP voice 5)

| Knob | OSC Page 1 | Existing parameter | OSC Page 2 | Existing parameter |
|---|---|---|---|---|
| 1 | Main Tune | `PAR_COARSE5` | Main Wave | `PAR_WAVE1_CYM` |
| 2 | Amp Decay | `PAR_VELOD5` | Mod Osc 1 Frequency | `PAR_MOD_OSC_F1_CYM` |
| 3 | Mod Osc 1 Gain | `PAR_MOD_OSC_GAIN1_CYM` | Mod Osc 2 Frequency | `PAR_MOD_OSC_F2_CYM` |
| 4 | Mod Osc 2 Gain | `PAR_MOD_OSC_GAIN2_CYM` | Transient Volume | `PAR_TRANS5_VOL` |

This remains a **single shared Cymbal/Clap voice**, not two simultaneous independent DSP voices. Separate clap and cymbal engines are planned for the future registry.

## Closed Hi-Hat (trigger position 6) and Open Hi-Hat (trigger position 7)

| Knob | OSC Page 1 | Existing parameter | OSC Page 2 | Existing parameter |
|---|---|---|---|---|
| 1 | Main Tune | `PAR_COARSE6` | Main Wave | `PAR_WAVE1_HH` |
| 2 | **Closed/Open Amp Decay** | `PAR_VELOD6_CLOSED` or `PAR_VELOD6_OPEN` | Mod Osc 1 Frequency | `PAR_MOD_OSC_F1` |
| 3 | Mod Osc 1 Gain | `PAR_MOD_OSC_GAIN1` | Mod Osc 2 Frequency | `PAR_MOD_OSC_F2` |
| 4 | Mod Osc 2 Gain | `PAR_MOD_OSC_GAIN2` | Transient Volume | `PAR_TRANS6_VOL` |

**Important:** Only the closed/open decay is separately stored in the original firmware. Other parameters on both hat OSC editors point to the same source indices. This is the **correct behavior for a UI-only legacy compatibility stage**. The planned K2 patch format may later store two independent hat parameter profiles and engine IDs while maintaining choke/retrigger behavior.

## Labels and menu implementation

Existing `Core/Inc/menu/menu.h` text IDs are used for all fields. For this first implementation, the OSC screen displays the *existing stock abbreviations* (COA, DEC, MOD, etc.) rather than adding a new string table. The mapping has eight top text IDs plus eight bottom PAR_* IDs in the existing `Page` structure at `Core/Inc/menu/menuPages.h` for each voice position. The existing `menu_switchSubPage` toggle supports a second row of four OSC values.

The final engine-aware UI needs dedicated human-readable labels (three-character display constraints), a switchable engine registry, per-engine ranges/defaults, stable mod targets and versioned save storage. Until those are implemented, call this a **legacy LXR OSC priority view** only.

## Regression checklist

- [ ] Desktop and Eurorack cross-compile successfully.
- [ ] OSC toggles between two rows of four on every track.
- [ ] All 8 OSC labels map to the correct original PAR_* IDs.
- [ ] Each drum voice independently updates its own indexes.
- [ ] Snare noise mix and frequency are distinct.
- [ ] Hat closed/open decay varies independently; the other six shared source controls remain in sync.
- [ ] Loading a factory kit changes no parameter value until an encoder is moved.
- [ ] Saving/reloading a legacy kit preserves every original byte including hidden parameters.
- [ ] Legacy AEG/MOD/FM/CLICK/FILTER/MIX and desktop LFO pages remain operational.
- [ ] Original per-voice oscillator/FX output and trigger behavior is unchanged.
- [ ] Separate MOD 2-slot, 2-page feature is not misrepresented as implemented; it remains planned.
