# Descent Engine

Descent Engine is an in-development game engine written primarily in C, with support for C++. Currently, only the threading library is in a usable state. The engine is primarily tested on Linux.

> ⚠️ **Work in Progress** – This is not a production-ready engine. Features, APIs, and performance are subject to change.

## Features

- **Threading library**:
	- Cross-platform abstraction over POSIX threads and Windows threads.
	- Supports thread creation, joining, detaching, priorities, and CPU affinity.
	- Cross platform atomic operations for 32- and 64-bit types.
	- Intra-process mutexes, recursive mutexes, and read-write locks.
- **Allocation library**:
	- Large-scale memory allocations with page-alignment
- **Planned features** (coming soon):
	- More synchronization primitives (spinlocks, semaphores, condition variables, and inter-process mutexes).
	- Futures and promises.
	- Work-stealing and task scheduling.
	- Allocation system
	- Vulkan and rendering backend.
	- Cross-platform builtins for fast bit-level operations.

## Coding Standards

All code is written in C and C++ standard 17. Code should compile with strict warnings and pedantic errors enabled.

## Building

Descent Engine uses CMake presets for configurable builds. 

- From the project directory, make and enter a build subdirectory:
	```bash
	mkdir build
	cd build
	```
- Configure, ideally using a preset:
	```bash
	cmake --preset=gcc-debug ..
	```
- Build the project
	```
	make
	```

Other CMake presets exist for other compilers and build configurations. From `/build`, presets can be listed with

```bash
cmake --list-presets=configure ..
```

## License

License will be added once the engine is ready for open-source contribution.

## Notes

Only Linux has been tested so far. Other platforms (Windows, macOS, BSD) are partially implemented by best guess.