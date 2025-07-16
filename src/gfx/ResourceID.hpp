#pragma once

#include "GraphicLayerConfig.hpp"
#include <cstring>

namespace Gfx
{
	struct FrameBufferID
	{
		int index;
		static const FrameBufferID InvalidID;
	};

	struct ShaderID
	{
		int index;
		static const ShaderID InvalidID;
	};

#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
	struct StorageBufferID
	{
		int index;
		static const StorageBufferID  InvalidID;
	};
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

	struct TextureID
	{
		int index;
		static const TextureID InvalidID;
	};

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	struct UniformBufferID
	{
		int index;
		static const UniformBufferID InvalidID;
	};
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

	struct VertexBufferID
	{
		int index;
		static const VertexBufferID InvalidID;
	};

	inline
	bool operator == (const FrameBufferID lhs, const FrameBufferID rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const FrameBufferID lhs, const FrameBufferID rhs)
	{
		return !(lhs == rhs);
	}

	inline
	bool operator == (const ShaderID lhs, const ShaderID rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const ShaderID lhs, const ShaderID rhs)
	{
		return !(lhs == rhs);
	}

#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
	inline
	bool operator == (const StorageBufferID lhs, const StorageBufferID rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const StorageBufferID lhs, const StorageBufferID rhs)
	{
		return !(lhs == rhs);
	}
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

	inline
	bool operator == (const TextureID lhs, const TextureID rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const TextureID lhs, const TextureID rhs)
	{
		return !(lhs == rhs);
	}

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	inline
	bool operator == (const UniformBufferID lhs, const UniformBufferID rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const UniformBufferID lhs, const UniformBufferID rhs)
	{
		return !(lhs == rhs);
	}
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

	inline
	bool operator == (const VertexBufferID lhs, const VertexBufferID rhs)
	{
		return (memcmp(&lhs, &rhs, sizeof(rhs)) == 0);
	}

	inline
	bool operator != (const VertexBufferID lhs, const VertexBufferID rhs)
	{
		return !(lhs == rhs);
	}
}
