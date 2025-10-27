
# Roadmap

## Planned Features

- **Threading Library**
	- Dynamic TLS
	- Call Once
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

## Future Milestones

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
