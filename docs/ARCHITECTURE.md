# LXR-K2 — Eurorack firmware architecture

Source review: 2026-11-03. Upstream main at e7523396af31add278cfaf5ecb34767ebeb4ca56. This is code inspection, not hardware testing.

## Render pipeline

1. MIDI, triggers and CV modulation reach voice control and the parameter model.
2. Modulators, pitch and amplitude envelopes update voice state.
3. The six synthesis voices render audio blocks.
4. State-variable filtering, decimation and per-voice distortion shape the signals.
5. Mixer and shared FX route to I2S/DMA and audio DAC outputs.

The dispatch lives in Core/Src/Soundengine/Soundengine.c and rendering/mixing in Core/Src/Soundengine/DSPAudio/mixer.c.

## Engine inventory

| Voice | Source | Existing structure |
|---|---|---|
| Drum 1–3 | DSPAudio/DrumVoice.c | Main and modulation oscillators, FM or mix, pitch/amp EGs, transient, resonant filter, distortion |
| Snare | DSPAudio/Snare.c | Tuned oscillator and noise, transient, filter and distortion |
| Cymbal | DSPAudio/CymbalVoice.c | Main oscillator plus additional modulators, metallic FM, filters and transient |
| Hi-hat | DSPAudio/HiHat.c | Oscillators, open/closed articulation, envelopes, filters and transient |

Source paths in this table are relative to Core/Src/Soundengine/.

## Relevant DSP subsystems

| Category | Source path relative to Core/Src/Soundengine/ |
|---|---|
| Waveforms and phase/FM | DSPAudio/Oscillator.c, DSPAudio/wavetable.c |
| Filters | DSPAudio/ResonantFilter.c, DSPAudio/1PoleLp.c |
| Envelopes | DSPAudio/Decay.c, DSPAudio/SlopeEg2.c, DSPAudio/snapEg.c |
| Transients | DSPAudio/transientGenerator.c |
| LFO / modulation nodes | DSPAudio/lfo.c, DSPAudio/modulationNode.c |
| FX and mixer | DSPAudio/FX/, FxBus.c, DSPAudio/mixer.c |
| MIDI parameters | MIDI/MidiParser.c, MIDI/MidiVoiceControl.c, MIDI/ParameterArray.c |
| Kit files | ../Preset/presetManager.c, ../Preset/initKit.c |
| Menu | ../menu/menu.c and related UI files |

Oscillator.h declares SINE, TRI, SAW, REC, NOISE, and PWM waveform IDs. The main drum voice chooses mixing or FM behavior, whereas snare, cymbal and hi-hat use specialized DSP paths.

## Eurorack-specific routing

- Core/Src/Soundengine/TriggerProcessor.c: trigger/accent inputs and GPIO interrupts.
- Core/Src/Soundengine/ModMatrix_Processor.c: 18 slots, physical CV sources, LFO and accent sources.
- DrumVoice.c and Oscillator.c contain pitch CV fields not found in desktop versions.
- Desktop sequencer and USB-MIDI application loops are absent or compile-gated.
- CV, accent and independent audio output behavior must be independently regressed.

## Performance and memory boundaries

Target: STM32F765VG Cortex-M7 at 216 MHz. Firmware application flash: 992 KiB after the 32 KiB bootloader. ITCM: 16 KiB. Default OUTPUT_DMA_SIZE = 64 and REAL_FS = 44140.f in Core/Inc/Soundengine/config.h. Real-time source uses the IN_ITCM_RAM attribute and generally compiles with -O3. Review memory-map and worst-case callback timing for every added DSP primitive.

## Compatibility risk

The positional PAR_* enumeration in Core/Inc/Parameters.h maps into activeParameters through MIDI/ParameterArray.c. Preset/presetManager.c reads and writes indexed parameter bytes using END_OF_SOUND_PARAMETERS for kit data. Inserting an enum value inside existing IDs can invalidate kits, modulation destinations and CC/NRPN mappings.

Design engine selection with stable old parameter IDs, old sound as the default, a fully specified extension/migration scheme and tests for save/load and modulation. Do not wholesale copy the desktop DSP code into the Eurorack version.

## Not yet implemented

The LXR-K2 mode selector, novel algorithms, modified UI branding and preset migration are all future development tasks.
