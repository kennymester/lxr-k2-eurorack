# LXR-K2 engine catalog — exploratory design matrix

**Proposal, not implemented.** Every engine defines **exactly eight core synthesis controls on OSC**, four on each OSC page, with the most important four on Page 1 and four secondary controls on Page 2. MOD has two pages for exactly two local modulation routes per track. AEG, FM, CLICK, FILTER and MIX have four controls each. The desktop-only LFO button may shortcut to MOD Page 2; the Eurorack has no LFO section button. Engine selection is outside OSC; see [the eight-button control specification](CONTROL_PAGE_SPEC.md).

## Slot availability (seven triggers / six synthesis voices)

| Play position | Engine category | Engine choice |
|---|---|---|
| Drum 1 | Drum/Perc | Any drum-compatible LXR, classic or experimental algorithm |
| Drum 2 | Drum/Perc | Independently selectable; can differ from Drum 1 |
| Drum 3 | Drum/Perc | Independently selectable; can differ from Drum 1/2 |
| Snare | Snare | LXR snare, classic snares, clap-oriented snare variants, experimental |
| Clap/Cym | Clap/Cymbal | One selectable clap, cymbal, cowbell or metallic percussion engine |
| Closed hat | Hat (articulation) | Separate saved engine and eight-control profile; shared HiHatVoice renderer |
| Open hat | Hat (articulation) | Separate saved engine and eight-control profile; shared HiHatVoice renderer |

## Stock engines

- **LXR Drum** (for Drum 1–3): existing two-oscillator FM/mix architecture, envelopes, transient, filter and drive are preserved internally. Eight performance-oriented edit controls are to be selected by listening comparisons; initial shortlist: Tune, Decay, Pitch Env, Pitch Sweep, FM Amount, FM Ratio, Transient, Tone. The underlying legacy parameters need not be reset or discarded.
- **LXR Snare**: oscillator/noise balance, tone, noise tune, decay, punch, transient, filter and drive.
- **LXR Clap/Cymbal**: central tune, spectral/metal ratios, FM amount, decay, noise/texture, transient and tone.
- **LXR Closed Hat / Open Hat**: separate articulation profiles while retaining the original shared voice and choke. Examples: Tone, Decay, Metal, Noise, Attack, Filter, Drive, Level.

For stock LXR imported patches, the eight exposed controls must map to extant parameters while hidden original parameters preserve the exact stored sound.

## Classic-inspired engine families

Original DSP models are the implementation target; avoid including unauthorized sample ROM content.

| Family | Drum 1–3 options | Snare options | Clap/Cym slot | Hat options |
|---|---|---|---|---|
| **909** | Kick, Tom / low percussion | 909-style snare | Clap, cymbal, ride-like engine | Closed/open 909-style hats (PCM assets may be needed for fidelity) |
| **808** | Kick, tom, conga and tuned percussion | 808-style snare | Clap, cymbal, cowbell / metallic | Closed/open 808-style hats |
| **707** | PCM kick, tom and percussion | PCM snare | PCM clap, cymbal | PCM closed/open hats |
| **606** | Kick, tom / tuned percussion | 606-style snare | Cymbal / metallic and clap approximation | Closed/open 606-style hats |

The 808 and 606 were primarily analog synth architectures; the 909 was a hybrid of analog synthesis and samples; the 707 was a PCM digital drum machine. A faithful 707-like implementation has **sample provenance/licensing, flash storage and quality constraints**. K2 must not bundle proprietary Roland ROMs or copyrighted sample libraries without rights. For the first 707 phase, choose either licensed original sample material or a transparently named 707-inspired synthesis approximation.

### Preliminary OSC-only core parameter examples

These are the eight **engine-specific** controls on OSC, not the full set of 36 visible controls in the OSC, MOD, AEG, FM, CLICK, FILTER and MIX editors. The first four should be the **most used and musically important**. Generic Level, Pan, Output and additional post-filter Drive belong on MIX/FILTER rather than consuming OSC slots.

| Engine | OSC Page 1 — priority four | OSC Page 2 — secondary four |
|---|---|---|
| LXR Drum | Tune · Decay · Sweep · Punch | FM Amount · Ratio · Transient · Tone |
| 909 Kick | Tune · Decay · Punch · Attack | Body · Pitch Curve · Osc Color · Click Tone |
| 808 Kick | Tune · Decay · Tone · Attack | Sub · Pitch Bend · Harmonics · Click Tone |
| 606 Kick | Tune · Decay · Punch · Attack | Body · Click Tone · Pitch Curve · Harmonics |
| 707 PCM Kick | Tune · Decay · Sample Choice · Tone | Start · Pitch Bend · Crunch · Tail |
| 909 Snare | Tune · Decay · Snappy · Tone | Noise Color · Body · Rattle · Attack |
| 808 Snare | Tune · Decay · Snappy · Tone | Noise Color · Body · Pitch Bend · Attack |
| 909 Clap | Decay · Tone · Spread · Attack | Texture · Slap Count · Tail · Width |
| 808 Clap | Decay · Tone · Scatter · Attack | Noise Color · Tail · Flutter · Width |
| 909 Cymbal | Tune · Decay · Bright · Attack | Metal Ratio · Texture · Wash · Accent Color |
| 808 Cymbal | Tune · Decay · Metal · Tone | Osc Spread · Ring · Noise Color · Wash |
| 808 Closed Hat | Decay · Tone · Metal · Attack | Tune · Noise Blend · Sizzle · Choke Shape |
| 808 Open Hat | Decay · Tone · Metal · Wash | Tune · Noise Blend · Attack · Choke Shape |
| 909 Closed/Open Hat | Decay · Bright · Tune · Start | Sample Color · Attack · Choke Shape · Pitch |
| 606 Closed/Open Hat | Decay · Tone · Metal · Attack | Tune · Noise Blend · Sizzle · Choke Shape |

Parameter tables are deliberately **not fixed**: decay/time curves, ranges, defaults and source-to-control mappings are to be established through A/B sound testing. The same family can use different algorithms per instrument. Accent can be a sound parameter or a trigger input modulation source; making it one of eight must be a deliberate musical decision.

## Experimental family (original algorithms, not recreations of third-party code)

- **K2 Phase** — complex phase modulation / feedback percussion: Tune, Decay, Ratio, Index | Feedback, Fold, Motion, Tone.
- **K2 Fold** — wavefolded oscillator with exciter: Tune, Decay, Excite, Fold | Bias, Mod, Filter, Drive.
- **K2 Metal** — inharmonic oscillators / resonators: Pitch, Decay, Spread, Detune | Ring, Noise, Motion, Drive.
- **K2 Dust** — stochastic transient and grain-like percussion: Density, Decay, Color, Strike | Scatter, Rate, Gate, Drive.
- **K2 Chaos** — bounded nonlinear feedback/percussive glitches: Tune, Decay, Feedback, Seed | Motion, Crush, Fold, Tone.

All need measured CPU budget and anti-aliasing strategy. 'Noise Engineering-like' describes an experimental aesthetic, not a claim of equivalence or permission to reproduce their algorithms or proprietary IP.

## Engine ID rules

Reserve stable IDs by category and variant; once patch files exist, no ID reuse. Prefer an explicit lookup table rather than using enum ordinal positions as persistent IDs. Pair each engine ID with a descriptor mapping eight raw controls to meaningful units, defaults, display names, engine DSP state and modulation destinations.

## Proposed build priority

1. LXR engine eight-control mapping, separate hat articulation profiles, engineer a compatible engine selector.
2. 909 kick and 808 kick (proof of new engine dispatch and DSP integration).
3. Complete 909 and 808 snare, cym/clap and hats.
4. 606 analog-inspired family.
5. 707 rights-cleared PCM family, or explicitly approximate 707-style digital sound if assets are unavailable.
6. K2 experimental family.

## What remains undecided

Precise per-engine sound parameters and their ranking; engine selection gesture; whether desktop LFO is a shortcut to MOD Page 2; 707 content rights and fidelity goals; sound/cpu tradeoffs; hat choke semantics during different engine selections; new versioned patch storage and full MOD routing.
