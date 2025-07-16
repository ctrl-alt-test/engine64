#pragma once

#include "GraphicLayerConfig.hpp"
#include "ResourceID.hpp"

namespace Gfx
{
	/// <summary>
	/// Geometry instances for a drawcall. Consists in a vertex buffer,
	/// a number of vertex indices to use and a number of instances to
	/// draw.
	/// The name "Geometry" is the same as in "geometry instancing".
	/// </summary>
	struct Geometry
	{
		VertexBufferID	vertexBuffer;
		int				numberOfIndices;
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		int				firstIndexOffset; // FIXME: find a good name.
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
	};
}
