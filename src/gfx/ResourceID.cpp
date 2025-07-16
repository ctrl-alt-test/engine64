#include "ResourceID.hpp"

namespace Gfx
{
	const FrameBufferID FrameBufferID::InvalidID = { -1 };
	const ShaderID ShaderID::InvalidID = { -1 };
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
	const StorageBufferID StorageBufferID::InvalidID = { -1 };
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT	
	const TextureID TextureID::InvalidID = { -1 };
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	const UniformBufferID UniformBufferID::InvalidID = { -1 };
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	const VertexBufferID VertexBufferID::InvalidID = { -1 };
}
