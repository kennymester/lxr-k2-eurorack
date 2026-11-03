# LXR-K2 synthesis-engine design specification

Status: to be defined together; do not treat examples as approved features.

For each proposed sound engine, record:

| Requirement | Description to supply |
|---|---|
| Target devices | Desktop / Eurorack / both |
| Voice | Drum 1, 2, 3, snare, cymbal, hi-hat, or selectable across voices |
| Musical goal | Sound character, timbre, reference patches or synthesis method |
| Core DSP algorithm | Waveforms, FM/ringmod, noise, resonators, waveshaping, etc. |
| Pitch behavior | Tracking, range, fine tune, pitch envelopes and retrigger |
| Dynamics | Amp envelope stages, curve, click/transient behavior and accent |
| Filtering | Types, order, drive, resonance, modulation |
| Modulation | Sources, destinations, ranges, smoothing and audio-rate restrictions |
| Interface | Page layout, LCD names, encoder/knob operations and learn mappings |
| Hardware routing | MIDI CC/NRPN, note, trigger, gate, velocity, CV and output selection |
| State compatibility | Legacy default, additional engine IDs, kit storage and migration |
| Validation | CPU/ITCM/RAM budget, clipping, aliasing, regression and recovery |

Design constraints: retain existing numeric parameter identity, preserve the legacy sound when new modes are disabled, do not dynamically allocate inside the real-time audio path, and never assume desktop and Eurorack control routing are identical.

**First question:** Which existing voice is the priority, and what synthesis behavior do you want it to gain?
