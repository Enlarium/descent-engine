# Descent Engine

[![Cross-Platform Build and Test](https://github.com/Enlarium/descent-engine/actions/workflows/build.yml/badge.svg)](https://github.com/Enlarium/descent-engine/actions/workflows/build.yml)

**Descent Engine** is an in-development game engine written primarily in C, with optional C++ support. It’s currently in an early stage - only the threading library is functional and in testing.

The engine is primarily developed and tested on Linux.

> ⚠️ **Work in Progress** - This is not a production-ready engine. Features, APIs, and performance are subject to change.

> 🖼️ **Note:** This repository is currently just a personal showcase project. Contributions, pull requests, and issues are unlikely to be accepted at this time.

## Features

- **Threading Library**:
	- Cross-platform abstraction over POSIX threads and Windows threads.
	- Supports thread creation, joining, detaching, priorities, and CPU affinity.
	- Atomic operations for 32- and 64-bit types.
	- Intra-process mutexes, recursive mutexes, and read-write locks.
- **Allocation Library**:
	- Large-scale memory allocations with page-alignment
  
## Roadmap

### Planned Features

- **Threading Library**
	- More synchronization primitives:
		- Spin-locks
		- Semaphores
		- Condition variables
		- Inter-process lock variants
	- Futures and promises
	- Thread job system
		- Work-stealing and task scheduling
- **Allocation Library**
	- Master arena for large-scale memory management
	- Tiered sub-allocators for engine segments
	- Debugging and memory tracking tools
- **Rendering Library**
	- Vulkan backend
		- Low-level rendering pipeline
		- Resource management (textures, buffers, shaders)
		- Cross-platform abstraction
	- Future: abstraction for other graphics APIs as needed (OpenGL, Metal, DirectX)
- **Utility Library**
	- Cross-platform bit-level operations
	- Logging, debugging, and assertion utilities
	- Timing and profiling helpers

### Future Milestones

1. **Stabilize threading library**
	- Ensure API is consistent and intuitive
	- Fix known bugs: deadlocks, race conditions, and memory leaks
	- Confirm correct behavior across Linux, FreeBSD, macOS, and Windows
	- Document all functions and expected behavior
2. Implement allocation system
	- Complete master arena and tiered allocators
	- Include alignment guarantees
	- Add optional memory tracking for debugging
3. Design testing framework
	- Unit tests for core libraries
	- Cross-platform test harness
	- Stress tests for threading and allocation
4. Add basic rendering backend
	- Vulkan initialization and device selection
	- Resource creation and management (buffers, textures)
	- Simple rendering of primitives

## Coding Standards

All code is written in C 17 and C++ 17. Code should compile with strict warnings and pedantic errors enabled.

## Building

Descent Engine uses CMake presets for configurable builds. 

```bash
# From project root
mkdir build
cd build
cmake --preset=gcc-debug ..
make
```

Other CMake presets exist for other compilers and build configurations. From `/build`, presets can be listed with

```bash
cmake --list-presets=configure ..
```

If you're building without CMake, define _POSIX_C_SOURCE=200809L for Linux and FreeBSD, and _DARWIN_C_SOURCE for MacOS.

## Documentation

Documentation is in progress.

## License

This project is licensed under the **Apache License 2.0** - see the [LICENSE](./LICENSE) file for details.

Copyright © 2025 XavierHarkonnen9 and Enlarium

## Contribution Policy

This repository is not currently accepting contributions. Once the core systems are stable, contribution guidelines and a license will be published.

For now, feel free to explore the code, open it locally, and learn from the implementation — but please refrain from submitting PRs or issues.

## Notes

Only Linux has been tested so far. Other platforms (Windows, macOS, BSD) are partially implemented by best guess.