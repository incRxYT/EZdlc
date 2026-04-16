# EZdlc
A tool for implementing dlc into BO3 using boiii clients unlock features
# EZdlc

EZdlc is a lightweight C++ CLI tool designed to help manage and organize Call of Duty: Black Ops 3 game files for use with the BOIII client.

It allows users to scan, preview, and apply file operations (copy or symlink) between their official BO3 installation and BOIII-compatible directory structure.

> This tool does not include or distribute any game files. Users must own their game content.

---

## Features

- CLI-based interface (no GUI required)
- Supports BO3 folders:
  - zone
  - snd/en
  - video
  - all
- Dry-run mode (preview changes before applying)
- Copy or Symlink mode selection
- File validation system for DLC-related assets
- Safe file operation logging

---

## Requirements

- Windows 10/11
- C++17 compatible compiler (MSVC recommended)
- CMake (optional but supported)

---

## Build Instructions

### Using CMake

```bash
cmake -S . -B build
cmake --build build --config Release
