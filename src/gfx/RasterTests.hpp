#pragma once

#include "GraphicLayerConfig.hpp"
#include "engine/debug/Debug.hpp"
// FIXME: ideally Gfx should not have dependency over Engine.

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <GL/gl.h>
#include "gfx/OpenGL/glext.h"
#include <cstring>

namespace Gfx
{
	struct DepthFunction
	{
		enum Enum {
			Never = GL_NEVER,
			Less = GL_LESS,
			Equal = GL_EQUAL,
			LessOrEqual = GL_LEQUAL,
			Greater = GL_GREATER,
			NotEqual = GL_NOTEQUAL,
			GreaterOrEqual = GL_GEQUAL,
			Always = GL_ALWAYS,
		};
	};
	typedef DepthFunction StencilFunction;

	struct StencilOperation
	{
		enum Enum {
			Keep = GL_KEEP,
			Zero = GL_ZERO,
			Increment = GL_INCR,
			Decrement = GL_DECR,
			IncrementWrap = GL_INCR_WRAP,
			DecrementWrap = GL_DECR_WRAP,
			Replace = GL_REPLACE,
			Invert = GL_INVERT,
		};
	};

	struct FaceCulling
	{
		enum Enum {
			None = 0,
			Front = GL_FRONT,
			Back = GL_BACK,
			FrontAndBack = GL_FRONT_AND_BACK,
		};
	};

	/// <summary>
	/// Parameters for the depth test.
	/// </summary>
	struct RasterTests
	{
		static const RasterTests NoDepthTest;
		static const RasterTests RegularDepthTest;

		static const RasterTests TwoSidedNoDepthTest;
		static const RasterTests TwoSidedRegularDepthTest;

#if GFX_ENABLE_FACE_CULLING
		FaceCulling::Enum			faceCulling;
#endif // GFX_ENABLE_FACE_CULLING

#if GFX_ENABLE_SCISSOR_TESTING
		bool						scissorTestEnabled;
		int							scissorX;
		int							scissorY;
		int							scissorWidth;
		int							scissorHeight;
#endif // GFX_ENABLE_SCISSOR_TESTING

#if GFX_ENABLE_STENCIL_TESTING
		StencilFunction::Enum		stencilFrontTest;
		StencilFunction::Enum		stencilBackTest;
		unsigned int				stencilFrontValue;
		unsigned int				stencilBackValue;
		unsigned int				stencilFrontMask;
		unsigned int				stencilBackMask;
		StencilOperation::Enum		stencilFrontOpStencilFail;
		StencilOperation::Enum		stencilFrontOpDepthFail;
		StencilOperation::Enum		stencilFrontOpPass;
		StencilOperation::Enum		stencilBackOpStencilFail;
		StencilOperation::Enum		stencilBackOpDepthFail;
		StencilOperation::Enum		stencilBackOpPass;
#endif // GFX_ENABLE_STENCIL_TESTING

#if GFX_ENABLE_DEPTH_TESTING
		DepthFunction::Enum			depthTest;
		bool						depthWrite;
#endif // GFX_ENABLE_DEPTH_TESTING

		bool						enableClipDistance;

		RasterTests():
#if GFX_ENABLE_FACE_CULLING
			faceCulling(FaceCulling::None),
#endif // GFX_ENABLE_FACE_CULLING
#if GFX_ENABLE_SCISSOR_TESTING
			scissorTestEnabled(false),
#endif // GFX_ENABLE_SCISSOR_TESTING
#if GFX_ENABLE_STENCIL_TESTING
			stencilFrontTest(StencilFunction::Always),
			stencilBackTest(StencilFunction::Always),
			stencilFrontOpPass(StencilOperation::Keep),
			stencilBackOpPass(StencilOperation::Keep),
#endif // GFX_ENABLE_STENCIL_TESTING
#if GFX_ENABLE_DEPTH_TESTING
			depthTest(DepthFunction::Always),
			depthWrite(false),
#endif // GFX_ENABLE_DEPTH_TESTING
			enableClipDistance(false)
		{
		}

		RasterTests(FaceCulling::Enum faceCulling,
					DepthFunction::Enum depthTest,
					bool depthWrite,
					bool enableClipDistance = false,
					bool enableScissorTest = false):
#if GFX_ENABLE_FACE_CULLING
			faceCulling(faceCulling),
#endif // GFX_ENABLE_FACE_CULLING
#if GFX_ENABLE_SCISSOR_TESTING
			scissorTestEnabled(enableScissorTest),
#endif // GFX_ENABLE_SCISSOR_TESTING
#if GFX_ENABLE_STENCIL_TESTING
			stencilFrontTest(StencilFunction::Always),
			stencilBackTest(StencilFunction::Always),
			stencilFrontOpPass(StencilOperation::Keep),
			stencilBackOpPass(StencilOperation::Keep),
#endif // GFX_ENABLE_STENCIL_TESTING
#if GFX_ENABLE_DEPTH_TESTING
			depthTest(depthTest),
			depthWrite(depthWrite),
#endif // GFX_ENABLE_DEPTH_TESTING
			enableClipDistance(enableClipDistance)
		{
#if !GFX_ENABLE_FACE_CULLING
			UNUSED_EXPR(faceCulling);
#endif // !GFX_ENABLE_FACE_CULLING
#if !GFX_ENABLE_SCISSOR_TESTING
			UNUSED_EXPR(enableScissorTest);
#endif // !GFX_ENABLE_SCISSOR_TESTING
#if !GFX_ENABLE_DEPTH_TESTING
			UNUSED_EXPR(depthTest);
			UNUSED_EXPR(depthWrite);
#endif // !GFX_ENABLE_DEPTH_TESTING
		}
	};

	inline
	bool operator == (const RasterTests& lhs, const RasterTests& rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const RasterTests& lhs, const RasterTests& rhs)
	{
		return !(lhs == rhs);
	}
}
