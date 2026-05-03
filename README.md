# `Cell Offset Generator`

## About

This Fallout 4 plugin improves the runtime performance of finding cells by generating offsets for them.

## General Requirements

### Build Dependencies

- A C++ 23 compiler:
  - [MSVC 2026](https://learn.microsoft.com/en-us/visualstudio/releases/2026/release-history#release-dates-and-build-numbers)
  - [clang-cl](https://github.com/llvm/llvm-project) with [ninja](https://github.com/ninja-build/ninja)
- [CommonLibF4](https://github.com/LucaDotGit/CommonLibF4)
- [spdlog](https://github.com/gabime/spdlog)
- [fmt](https://github.com/fmtlib/fmt)
- [toml11](https://github.com/ToruNiina/toml11)
- [xxhash](https://github.com/stbrumme/xxhash)

### Development

- [CMake v4.3.0+](https://cmake.org)
- [vcpkg](https://github.com/microsoft/vcpkg)
  - Create an environmental variable called `VCPKG_ROOT` where the value is the path to your vcpkg installation.

## F4SE Requirements

### End User Dependencies

- [F4SE](https://f4se.silverlock.org)
- [Address Library for F4SE Plugins](https://www.nexusmods.com/fallout4/mods/47327)

### Development

- [Address Library for F4SE Plugins](https://www.nexusmods.com/fallout4/mods/47327)
