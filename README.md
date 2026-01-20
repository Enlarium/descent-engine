# Descent Engine

[![Build and Test (Linux)](https://github.com/Enlarium/descent-engine/actions/workflows/build-linux.yml/badge.svg)](https://github.com/Enlarium/descent-engine/actions/workflows/build-linux.yml)
[![Build and Test (Windows)](https://github.com/Enlarium/descent-engine/actions/workflows/build-windows.yml/badge.svg)](https://github.com/Enlarium/descent-engine/actions/workflows/build-windows.yml)

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](./LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)]()
[![Language](https://img.shields.io/badge/language-C-green.svg)]()

**Descent Engine** is an in-development, systems-level game engine written in C. It is currently in an **early stage** with many systems incomplete and under active testing.

> ⚠️ **Work in Progress:** This engine is not production-ready. APIs, features, and performance may change rapidly.
  
> 🖼️ **Note:** This repository is a personal showcase project; contributions, PRs, and issues are not accepted at this time.

## Project Goals

Descent Engine aims to provide:

- A lightweight, portable foundation for systems-level game engine development.
- High performance and explicit control over systems and behavior.
- Clean, consistent C interfaces compatible with C++.

## Supported Platforms and Compilers

Descent Engine is designed to be portable, but not all platforms or compilers are fully supported. Current tested and supported configurations:

### Platforms

Descent Engine supports the following platforms:

- **Linux** (x86, x86_64, ARM32, ARM64)
- **FreeBSD** (x86_64)
- **Windows** (x86/x64)

### Compilers

Descent Engine supports the following compilers:

- **GCC**
- **Clang / ICX**
- **Cygwin GCC**
- **MinGW GCC**

> ⚠️ Note: Descent Engine requires C11 or newer.

Unfortunately, MSVC isn't a full-featured C compiler and does not support C11.

### Architectures

Descent Engine supports the following architectures:

- **x86 32-bit**
- **x86_64 64-bit**
- **ARM 32-bit**
- **ARM 64-bit**

> ⚠️ Note: Descent Engine requires two’s complement representation for negative numbers.

## Building

## Building the Engine

Descent Engine uses **CMake** as its build system. You can use the included presets or build manually.

### Quick Start (Recommended)

```sh
# From project root, create and enter build folder
mkdir build && cd build

# Configure using a preset (example: GCC Debug build)
cmake --preset=gcc-debug .. -DDESCENT_BUILD_EXAMPLES=ON -DDESCENT_BUILD_TESTS=ON

# Build the engine
cd gcc-debug
make -j$(nproc)

```

### Available Presets

Presets define compiler, build type, and platform settings. To list all available presets, run

```sh
# From project root
cmake --list-presets=configure
```

### Build Options

You can enable or disable features when configuring CMake:

|         Option         | Default |               Description                |
|:----------------------:|:-------:|:----------------------------------------:|
|   DESCENT_BUILD_ARM    |   OFF   |       Build for ARM instead of x86       |
|    DESCENT_BUILD_32    |   OFF   |    Build for 32 bit instead of 64 bit    |
| DESCENT_BUILD_EXAMPLES |   OFF   |        Build example executables         |
|  DESCENT_BUILD_TESTS   |   OFF   |             Build test suite             |
|      DESCENT_IWYU      |   OFF   | Run include-what-you-use while compiling |

### Manual Build

If you prefer to manually build or integrate Descent Engine into an existing project, you can compile the source directly. Configured files, like the [metadata header](./include/descent/metadata.h) must be copied and updated from the [configuration source](./config/metadata.h.in).

## Using the project

### Tests

If tests are enabled:

```sh
# From generated build folder (example: build/gcc-debug)
ctest --output-on-failure
```

## Documentation

Live HTML documentation is available at: <https://enlarium.github.io/descent-engine/>. Please note that documentation is still in progress.

## License

This project is licensed under the **Apache License 2.0** - see the [License](./LICENSE) for details.

Copyright © 2025 XavierHarkonnen9 and Enlarium

## Contribution Policy

This repository is not currently accepting contributions. Once the core systems are stable, contribution guidelines will be published.

Feel free to explore the code and learn from the implementation, but please refrain from submitting PRs or issues for now.

## Notes

Only Linux has been tested so far.

Other platforms (Windows, BSD) are partially implemented and unverified.
