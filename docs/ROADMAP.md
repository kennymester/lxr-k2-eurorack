# LXR-K2 development roadmap

## Operating model

- main: protected-by-process upstream reference until an approved, hardware-tested release.
- develop: integration and engineering documentation.
- feature/<name>: isolated feature implementation; PR into develop after review.
- Never rewrite original Erica Synths licensing notices.
- Separate the shared DSP algorithm from desktop or Eurorack hardware/control integration.

## Milestones

- [x] Fork both desktop and Eurorack repositories.
- [x] Verify original main branch SHAs match upstream.
- [x] Create develop branches for the two devices.
- [x] Inspect six voice implementations, modulation, MIDI, presets, hardware and audio routing.
- [ ] Obtain repeatable clean baseline binary builds and inspect ELF/map resource usage.
- [ ] Confirm baseline hardware and stock data behavior.
- [ ] Inventory each parameter ID, mod target, CC/NRPN assignment, menu entry and serialized kit field.
- [ ] Collaboratively define new algorithms, modulation destinations, ranges, defaults and engine modes.
- [ ] Propose backward-compatible preset extension and migration/versioning.
- [ ] Add modified-firmware branding to start-up screens, before distributing modified images.
- [ ] Implement first engine with an explicit legacy fallback and deterministic regression tests.
- [ ] Port compatible engine changes to the second hardware target.
- [ ] Confirm timing, aliasing, clipping, DC, boot, kit round-trip, MIDI/trigger and CV tests.
- [ ] Publish separately tested desktop and Eurorack firmware-only .img prereleases.

## Review requirements

Every functional PR should document: affected hardware and voices; parameter IDs and preset compatibility; algorithm details; UI and MIDI/CV behavior; test cases; compiler/tool versions; binary size, ITCM, DTCM and flash use; real-time DSP performance; and known recovery path.

## Initial feature candidates (not approved designs)

Alternative oscillator waveforms, more FM topologies, pitch and amplitude envelope curvature, noise/transient options, new filters, expanded modulation destinations and improved CV/MIDI mappings. Design discussions must determine which to implement.
