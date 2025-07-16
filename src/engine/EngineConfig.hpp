#pragma once

// The following definitions can be overridden in the project settings.
// Be careful to have consistent definitions for the engine and
// executable.

//---------------------------------------------------------------------
// Editing and tweaking features

// Enable editing features like file watching, runtime compilation, and
// debug UI.
// This is a top-level setting from which other settings are derived.
#ifndef ENABLE_EDITING
#	define ENABLE_EDITING 0
#endif

// Enable automatic file monitoring and reloading when files are
// modified, for hot-reloading of assets.
#ifndef ENABLE_AUTOMATIC_FILE_RELOAD
#	define ENABLE_AUTOMATIC_FILE_RELOAD ENABLE_EDITING
#endif

// Enable keyboard and mouse input handling.
#ifndef ENABLE_KEYBOARD_AND_MOUSE
#	define ENABLE_KEYBOARD_AND_MOUSE ENABLE_EDITING
#endif

// Enable pause functionality (as opposed to non interactive mode).
#ifndef ENABLE_PAUSE
#	define ENABLE_PAUSE ENABLE_EDITING
#endif

// Enable remote control for development, by sending project files via
// network.
#ifndef ENABLE_REMOTE_CONTROL
#	define ENABLE_REMOTE_CONTROL ENABLE_EDITING
#endif

// Enable runtime code compilation features for hot-reloading code
// changes, using RuntimeCompiledC++.
#ifndef ENABLE_RUNTIME_COMPILATION
#	define ENABLE_RUNTIME_COMPILATION ENABLE_EDITING
#endif

// Enable tweaking values at runtime with the _TV() macro.
#ifndef ENABLE_TWEAKVAL
#	define ENABLE_TWEAKVAL ENABLE_EDITING
#endif

//---------------------------------------------------------------------
// Development features

// Enable logging functionality.
#ifndef ENABLE_LOG
#	define ENABLE_LOG (DEBUG || ENABLE_EDITING)
#endif

// Enable profiling features like scope logging and frame timing.
#ifndef ENABLE_PROFILING
#	define ENABLE_PROFILING DEBUG
#endif

// Maximum number of frames logged.
#ifndef MAX_NUMBER_FRAMES_LOGGED
#	define MAX_NUMBER_FRAMES_LOGGED 256
#endif

// Enable a graphics quality setting.
// The option costs 100 bytes on the compressed binary.
#ifndef ENABLE_QUALITY_OPTION
#	define ENABLE_QUALITY_OPTION 1
#endif

//---------------------------------------------------------------------
// Rendering features

// Enable rough frustum culling using a bounding sphere.
#ifndef ENABLE_SPHERE_CULLING
#	define ENABLE_SPHERE_CULLING 0
#endif

// Enable finer frustum culling using an axis aligned bounding box.
#ifndef ENABLE_AABB_CULLING
#	define ENABLE_AABB_CULLING 1
#endif

// Controls inclusion of position attribute in vertex structure
#ifndef VERTEX_ATTR_POSITION
#	define VERTEX_ATTR_POSITION 1
#endif

/// Controls inclusion of normal attribute in vertex structure
#ifndef VERTEX_ATTR_NORMAL
#	define VERTEX_ATTR_NORMAL 1
#endif

/// Controls inclusion of tangent attribute in vertex structure
#ifndef VERTEX_ATTR_TANGENT
#	define VERTEX_ATTR_TANGENT 1
#endif

/// Controls inclusion of texture coordinate attribute in vertex structure
#ifndef VERTEX_ATTR_TEX_COORD
#	define VERTEX_ATTR_TEX_COORD 1
#endif

//---------------------------------------------------------------------
// Engine configuration

// Maximum number of shots in the timeline.
#ifndef MAX_NUMBER_OF_SHOTS
#	define MAX_NUMBER_OF_SHOTS 512
#endif

// Maximum number of threads for heightmap operations.
#ifndef MAX_THREADS_HEIGHTMAP
#	define MAX_THREADS_HEIGHTMAP 64 // At 128 we hit the __chkstk link error.
#endif

// Maximum number of threads for texture operations.
#ifndef MAX_THREADS_TEXTURE
#	define MAX_THREADS_TEXTURE 128 // At 256 we hit the __chkstk link error.
#endif
