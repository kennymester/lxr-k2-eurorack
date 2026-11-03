# LXR-K2 build and validation — Eurorack

Status: baseline build setup documented; **hardware safety and real-time correctness have not been verified**.

## Prerequisites

CMake >=3.21, Ninja, Python 3 and Arm GNU bare-metal GCC (arm-none-eabi-gcc). Official upstream readme documents tested toolchains. Windows tools can be installed with:

~~~powershell
winget install Kitware.CMake Ninja-build.Ninja Python.Python.3.12
# Separately install Arm GNU Toolchain and add its bin folder to PATH.
arm-none-eabi-gcc --version
~~~

## Clean build

~~~sh
cmake --preset release
cmake --build --preset release
~~~

Normal output: build/release/LXRE_V2_V<version>.img. ELF, BIN, HEX and MAP are also generated. If ITCM overflows with a recent compiler, inspect the map and retry with an explicit -DLXR_ITCM_OPT=-O2 setting. Record compiler version and exact commands. Automated CI builds are **unverified artifacts**, not release approval.

## Test gates before updating hardware

1. Back up SD card and all kits, projects and globals.
2. Retain an official recovery firmware image of the correct hardware type.
3. Confirm build toolchain, image filename and update header, firmware-only flash offset, .map region sizes and checksums.
4. Test all voices, old and new engine modes, max-voice load, extreme modulation, transient click behavior, distortion and output routing.
5. Test kit save/load and migration, front-panel navigation, MIDI, accent and CV modulation and input trigger timing.
6. Require clear modified-firmware startup branding before distributing altered binaries.
7. Do not use flash_full or otherwise overwrite the bootloader.

## SD card updater (after hardware test approval)

Copy only the device-specific LXRE_V2_V<version>.img to the root of a FAT32 card; remove conflicting older update files for the same device. With hardware powered off, insert card, hold encoder while powering on until updater launches and confirm successful completion. Consult upstream README for exact prompts and recovery steps. Never cross-flash desktop and Eurorack update images.

## Release notes

Include commit SHA, source version, compiler and flags, firmware SHA-256, map usage, measured timing and regression results. Keep GPL-3.0, NOTICE.md and third-party licences.
