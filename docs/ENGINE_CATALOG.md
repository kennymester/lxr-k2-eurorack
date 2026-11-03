# LXR-K2 engine catalog — exploratory design matrix

**Proposal, not implemented.** Every engine below has **exactly eight sound-edit controls**, four per page; names and defaults are pending sound-design review. Engine selection is outside these eight. Performance, routing, global FX and CV matrix remain outside them too.

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

### Preliminary engine parameter examples

| Engine | Page 1 — 4 controls | Page 2 — 4 controls |
|---|---|---|
| LXR Drum | Tune · Decay · Sweep · Punch | FM · Ratio · Transient · Tone |
| 909 Kick | Tune · Decay · Pitch · Attack | Body · Click · Drive · Level |
| 808 Kick | Tune · Decay · Tone · Accent | Sub · Attack · Drive · Level |
| 606 Kick | Tune · Decay · Punch · Click | Body · Tone · Drive · Level |
| 707 PCM Kick | Tune · Decay · Start · Accent | Crunch · Filter · Drive · Level |
| 909 Snare | Tune · Decay · Tone · Snappy | Noise · Attack · Drive · Level |
| 808 Snare | Tune · Decay · Tone · Snappy | Noise · Attack · Drive · Level |
| 909 Clap | Tone · Decay · Spread · Attack | Texture · Width · Drive · Level |
| 808 Clap | Tone · Decay · Scatter · Attack | Noise · Tail · Drive · Level |
| 909 Cymbal | Tune · Decay · Bright · Attack | Texture · Filter · Drive · Level |
| 808 Cymbal | Tune · Decay · Metal · Tone | Noise · Filter · Drive · Level |
| 808 Closed Hat | Tune · Decay · Metal · Tone | Noise · Attack · Drive · Level |
| 808 Open Hat | Tune · Decay · Metal · Tone | Noise · Attack · Drive · Level |
| 909 Closed/Open Hat | Tune · Decay · Bright · Start | Filter · Crunch · Drive · Level |
| 606 Closed/Open Hat | Tune · Decay · Metal · Tone | Noise · Attack · Drive · Level |

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

Which eight LXR macro controls per instrument; whether two pages are accessed by existing OSC/AEG buttons, by page toggle, or another gesture; engine selection gesture; 707 content rights and fidelity goals; actual feature/dsp constraints; hat choke semantics during different engine selections; mixer page visibility.
