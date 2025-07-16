#pragma once

#include "ResourceID.hpp"
#include <cstring>

namespace Gfx
{
	struct Viewport
	{
		int				x;
		int				y;
		int				width;
		int				height;
	};

	/// <summary>
	/// Destination frame buffer and viewport information.
	/// </summary>
	struct DrawArea
	{
		FrameBufferID	frameBuffer;
		Viewport		viewport;
	};

	inline
	bool operator == (const Viewport& lhs, const Viewport& rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const Viewport& lhs, const Viewport& rhs)
	{
		return !(lhs == rhs);
	}
}
