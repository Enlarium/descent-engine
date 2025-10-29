# Coding Standards

## General Principles

- Code must be **portable**, **readable**, **maintainable**, and **testable**.
- Follow **defensive programming**: validate inputs, check return values, handle errors gracefully.
- Code must compile cleanly with **strict compiler warnings**:
	- GCC/Clang: `-Wall -Wextra -Wpedantic`
	- MSVC: `/W4`
- Avoid platform-specific code; if necessary, isolate it behind abstraction layers.
- Avoid undefined or implementation-defined behavior.

## Language and Standard Rules

- Use **C17**.
- Maintain compatibility with **C++17** for interface files.
- Prefer **standard library functions** over platform-specific APIs when possible.
- Avoid C11 features not universally supported on all target platforms, and treat them as platform-dependent if necessary.
- Minimize dynamic memory allocation:
	- Use stack allocation where possible.
	- Aggregate dynamic allocations.
- Obey **Data-Oriented Design (DOD)** principles.
- All static/global features must be thread-safe.

## File Organization

- Keep public headers minimal; hide implementation details:
	- Use opaque types.
	- Split internal and public headers.
- File naming conventions:
	- Lowercase, words separated by underscores.
- Header guards:
	- Public: `DESCENT_<FILE_PATH>_H`
	- Internal: `DESCENT_SOURCE_<FILE_PATH>_H`
- Keep **one logical unit** per file if feasible.

## Naming Conventions

| Entity                   | Style                                     |
|:-------------------------|:------------------------------------------|
| Structures & Types       | `PascalCase`                              |
| Functions & Variables    | `snake_case`                              |
| Constants & Macros       | `UPPER_SNAKE_CASE`                        |
| Private/Internal Symbols | `snake_case`, prefixed with `_`           |
| Enumeration Values       | `UPPER_SNAKE_CASE`, prefixed by enum name |

## Formatting

- Use tabs for indentation, not spaces.
- Limit line length to 100–120 characters.
- Use blank lines to separate logical sections within functions.
- Spaces around operators

## Documentation

- Use Doxygen-style comments for public APIs.
- Document:
	- Purpose of the function/module
	- Parameters & return values
	- Side effects or assumptions
- Internal functions should use inline comments to explain complex logic.

## Error Handling

- Always check function return values.
- Use consistent error codes across modules.

## Memory Management

- Initialize all pointers to NULL and structures to {0}.
- Prefer stack allocation or fixed-size buffers over dynamic allocation.
- Clearly document ownership rules for all allocated resources.

## Concurrency

- Use thread-safe practices if multiple threads are used.
- Minimize shared mutable state.
- Prefer immutable data for shared resources.
- Always document locking strategy and expected thread safety.