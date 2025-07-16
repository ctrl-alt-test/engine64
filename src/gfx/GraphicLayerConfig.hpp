#pragma once

// The following definitions can be overridden in the project settings.
// Be careful to have consistent definitions for the engine, graphic
// layer, and executable.

// Enable vertex clipping.
#ifndef GFX_ENABLE_CLIPPING
#	define GFX_ENABLE_CLIPPING 1
#endif

// Enable compute shaders.
// See GFX_ENABLE_STORAGE_BUFFER_OBJECT for SSBO.
#ifndef GFX_ENABLE_COMPUTE_SHADERS
#	define GFX_ENABLE_COMPUTE_SHADERS 0
#endif

// Enable depth testing.
#ifndef GFX_ENABLE_DEPTH_TESTING
#	define GFX_ENABLE_DEPTH_TESTING 1
#endif

// Enable polygon culling based on face orientation.
#ifndef GFX_ENABLE_FACE_CULLING
#	define GFX_ENABLE_FACE_CULLING 1
#endif

// Enable filtering with scissor testing.
#ifndef GFX_ENABLE_SCISSOR_TESTING
#	define GFX_ENABLE_SCISSOR_TESTING 0
#endif

// Enable filtering with the stencil buffer.
#ifndef GFX_ENABLE_STENCIL_TESTING
#	define GFX_ENABLE_STENCIL_TESTING 0
#endif

// Enable storage buffer objects (SSBO).
// In this implementation, SSBO are required for compute shaders, as no
// alternative data exchange mechanisms are provided.
#ifndef GFX_ENABLE_STORAGE_BUFFER_OBJECT
#	define GFX_ENABLE_STORAGE_BUFFER_OBJECT 0
#endif

// Enable uniform buffer objects (UBO).
#ifndef GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#	define GFX_ENABLE_UNIFORM_BUFFER_OBJECT 0
#endif

// Enable support for rendering from an offset within a vertex buffer.
// When enabled, Gfx::Geometry::firstIndexOffset specifies the offset
// in the vertex buffer where rendering starts.
// This is useful for storing multiple meshes in a single buffer.
#ifndef GFX_ENABLE_VERTEX_BUFFER_OFFSET
#	define GFX_ENABLE_VERTEX_BUFFER_OFFSET 0
#endif

// When GFX_SKIP_REDUNDANT_UNIFORM_BINDING is enabled, this controls
// how uniform value changes are detected when skipping redundant
// bindings.
// When 0: Stores full Uniform objects and compares values directly.
// When 1: Stores only hashes of uniform values (potentially faster,
// but requires more code).
#ifndef GFX_HASH_UNIFORM_VALUE
#	define GFX_HASH_UNIFORM_VALUE 0
#endif

// Maximum number of frame buffers (FBO).
#ifndef GFX_MAX_FRAME_BUFFERS
#	define GFX_MAX_FRAME_BUFFERS 1024
#endif

// Maximum number of shaders.
#ifndef GFX_MAX_SHADERS
#	define GFX_MAX_SHADERS 512
#endif

// Maximum number of storage buffers (SSBO).
// See GFX_ENABLE_STORAGE_BUFFER_OBJECT to enable SSBO.
#ifndef GFX_MAX_STORAGE_BUFFERS
#	define GFX_MAX_STORAGE_BUFFERS 512
#endif

// Maximum number of storage buffer bindings.
// See GFX_ENABLE_STORAGE_BUFFER_OBJECT to enable SSBO.
#ifndef GFX_MAX_STORAGE_BUFFER_BINDINGS
#	define GFX_MAX_STORAGE_BUFFER_BINDINGS 8
#endif

// Maximum number of textures.
#ifndef GFX_MAX_TEXTURES
#	define GFX_MAX_TEXTURES 512
#endif

// Maximum number of textures that can be bound to a shader.
#ifndef GFX_MAX_TEXTURE_SLOTS
#	define GFX_MAX_TEXTURE_SLOTS 16
#endif

// Maximum number of uniform buffers (UBO).
// See GFX_ENABLE_UNIFORM_BUFFER_OBJECT to enable UBO.
#ifndef GFX_MAX_UNIFORM_BUFFERS
#	define GFX_MAX_UNIFORM_BUFFERS 16
#endif

// Maximum number of UBO that can be bound to a shader.
// See GFX_ENABLE_UNIFORM_BUFFER_OBJECT to enable UBO.
#ifndef GFX_MAX_UNIFORM_BUFFER_SLOTS
#	define GFX_MAX_UNIFORM_BUFFER_SLOTS 16
#endif

// Maximum number of uniforms that can be bound to a shader.
#ifndef GFX_MAX_UNIFORMS
#	define GFX_MAX_UNIFORMS 64
#endif

// Maximum number of vertex attributes.
#ifndef GFX_MAX_VERTEX_ATTRIBUTES
#	define GFX_MAX_VERTEX_ATTRIBUTES 16
#endif

// Maximum number of vertex buffers.
#ifndef GFX_MAX_VERTEX_BUFFERS
#	define GFX_MAX_VERTEX_BUFFERS 256
#endif

// Avoid binding uniforms that already have the correct value.
// See GFX_HASH_UNIFORM_VALUE to control how value changes are detected.
#ifndef GFX_SKIP_REDUNDANT_UNIFORM_BINDING
#	define GFX_SKIP_REDUNDANT_UNIFORM_BINDING 1
#endif
