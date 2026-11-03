# LXR-K2 — Eurorack firmware project

An independent, experimental GPL-3.0-only firmware project for the Erica Synths LXR-02 Eurorack hardware. This is **unofficial firmware, not affiliated with or endorsed by Erica Synths**.

Fork of https://github.com/erica-synths/lxr-eurorack-app, starting at upstream commit e7523396af31add278cfaf5ecb34767ebeb4ca56 (declared version 1.51). The original LICENSE, NOTICE.md, copyright and third-party notices are preserved.

## Current status

- Main branch: pristine fork of the upstream firmware, not a modified release.
- Develop branch: project specifications and changes under review.
- Synthesis-engine alterations: not implemented; design requirements still open.
- Hardware validation: not performed.
- Compiled firmware: do not assume a green build implies a safe device update.

## Project documents

- [Architecture](docs/ARCHITECTURE.md)
- [Roadmap](docs/ROADMAP.md)
- [Build and safety](docs/BUILD_AND_TEST.md)
- [Engine design specification](docs/ENGINE_DESIGN.md)

## Git workflow

Branch from develop with a feature branch, make narrow changes, open a PR to develop, and review source, preset compatibility, resource consumption and sound tests before merging. Keep main as a known-good reference until a version is hardware tested. Synchronize upstream deliberately.

## Licensing and safety

Preserve GPL-3.0 and NOTICE.md. When actual firmware modifications are distributed, clearly show the modified identity on the device start-up screen and avoid implying any official endorsement. Normal SD-card updates must not overwrite the bootloader. Never use the full-flash target for routine device updates.
