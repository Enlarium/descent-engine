# Descent Engine

[![Build and Test (Linux)](https://github.com/Enlarium/descent-engine/actions/workflows/build-linux.yml/badge.svg)](https://github.com/Enlarium/descent-engine/actions/workflows/build-linux.yml)
[![Build and Test (macOS)](https://github.com/Enlarium/descent-engine/actions/workflows/build-macos.yml/badge.svg)](https://github.com/Enlarium/descent-engine/actions/workflows/build-macos.yml)
[![Build and Test (Windows)](https://github.com/Enlarium/descent-engine/actions/workflows/build-windows.yml/badge.svg)](https://github.com/Enlarium/descent-engine/actions/workflows/build-windows.yml)

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](./LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20freeBSD%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)]()
[![Language](https://img.shields.io/badge/language-C-green.svg)]()


**Descent Engine** is an in-development game engine written primarily in C, with support for C++ integration. It's currently in an early stage - only the threading library is functional and in testing.

The engine is primarily developed and tested on Linux.

> ⚠️ **Work in Progress** - This is not a production-ready engine. Features, APIs, and performance are subject to change.

> 🖼️ **Note:** This repository is currently just a personal showcase project. Contributions, pull requests, and issues are unlikely to be accepted at this time.

## Project Goals

Descent Engine aims to provide:
- A lightweight, portable foundation for systems-level game engine development.
- High performance and explicit control over systems and behavior
- Clean, consistent C interfaces compatible with C++.

## Core Features

### Threading Library

- Cross-platform abstraction over POSIX threads and Windows threads.
- Thread creation, joining, detaching, priorities, and CPU affinity.
- Atomic operations for 32- and 64-bit types.
- Intra-process spinlocks, ticket locks, condition variables, mutexes, recursive mutexes, read-write locks, semaphores, and barriers.

### Allocation Library (In-Progress)

- Page-aligned, large-scale memory allocation primitives.

## Roadmap

Descent Engine's development roadmap covers threading, memory, rendering, and utilities.

For full details and milestones, see the [Roadmap](./ROADMAP.md).

## Building

Descent Engine uses CMake as its primary build system, with CMake presets for common compilers and configurations.

### Quick Start

```sh
# From project root, create and enter the build directory
mkdir build && cd build

# Configure using a preset (example: GCC Debug build)
cmake --preset=gcc-debug .. -DDESCENT_BUILD_EXAMPLES=ON -DDESCENT_BUILD_TESTS=ON

# Enter the generated build folder and compile
cd gcc-debug
make -j$(nproc)
```

### Available Presets

Presets define compiler, build type, and platform settings. To list all available presets:

```sh
# From project root
cmake --list-presets=configure
```

### Build Options

You can enable or disable specific subsystems when configuring CMake:

|         Option         | Default |        Description        |
|:----------------------:|:-------:|:-------------------------:|
| DESCENT_BUILD_EXAMPLES |   OFF   | Build example executables |
|  DESCENT_BUILD_TESTS   |   OFF   |     Build test suite      |

### Manual Build

If you prefer to build manually or integrate Descent Engine into an existing project, you can compile the source directly.

When building manually, define the following platform feature macros:

| Platform |     Required Macros     |
|:--------:|:-----------------------:|
|  Linux   | _POSIX_C_SOURCE=200809L |
| FreeBSD  | _POSIX_C_SOURCE=200809L |
|  macOS   |    _DARWIN_C_SOURCE     |
| Windows  |     *None required*     |

## Using the project

### Examples

### Tests

If tests are enabled:

```sh
# From generated build folder (example: build/gcc-debug)
ctest --output-on-failure
```

## Documentation

Documentation is in progress.

Public headers use Doxygen-style comments and will be exported to HTML as the engine matures.

## License

This project is licensed under the **Apache License 2.0** - see the [License](./LICENSE) for details.

Copyright © 2025 XavierHarkonnen9 and Enlarium

## Contribution Policy

This repository is not currently accepting contributions. Once the core systems are stable, contribution guidelines will be published.

Feel free to explore the code and learn from the implementation, but please refrain from submitting PRs or issues for now.

## Notes

Only Linux has been tested so far.

Other platforms (Windows, macOS, BSD) are partially implemented and unverified.

## Related Files

- [LICENSE.md](./LICENSE.md) - Project License
- [ROADMAP.md](./ROADMAP.md) - Full feature and milestone roadmap
- [CODING_STANDARDS.md](./CODING_STANDARDS.md) - C style guide
<!-- - [docs/](./docs/) - Engine documentation (WIP) -->