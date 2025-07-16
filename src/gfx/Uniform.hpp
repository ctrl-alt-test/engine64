#pragma once

#include "ResourceID.hpp"

namespace Gfx
{
	struct UniformType
	{
		enum Enum {
			Float,
			Int,
			Sampler,
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
			UniformBuffer,
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
			StorageBufferInput,
			StorageBufferOutput,
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
		};
	};

	/// <summary>
	/// Single uniform parameter for a shader. Consists in a name, a
	/// type and a size/dimension. Int and floats will be passed as is,
	/// but samplers will need to be translated first.
	/// </summary>
	struct Uniform
	{
		const char*			name;
		int					size;
		UniformType::Enum	type;
		union
		{
			float			fValue[16];			// Up to a 4x4 matrix.
			int				iValue[4];			// Up to a 4d int vector.
			TextureID		textureId;			// One texture id.
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
			UniformBufferID	uniformBufferId;	// One uniform buffer id.
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
			StorageBufferID	storageBufferId;	// One shader storage buffer id.
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
		};

		static Uniform Float1(const char* name, float x);
		static Uniform Float2(const char* name, float x, float y);
		static Uniform Float3(const char* name, float x, float y, float z);
		static Uniform Float4(const char* name, float x, float y, float z, float w);
		static Uniform Int1(const char* name, int i);
		static Uniform Int2(const char* name, int i, int j);
		static Uniform Int3(const char* name, int i, int j, int k);
		static Uniform Int4(const char* name, int i, int j, int k, int l);
		static Uniform Sampler1(const char* name, TextureID id);
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		static Uniform UniformBuffer1(const char* name, UniformBufferID id);
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		static Uniform StorageBufferInput1(const char* name, StorageBufferID id);
		static Uniform StorageBufferOutput1(const char* name, StorageBufferID id);
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
#if !GFX_HASH_UNIFORM_VALUE
		bool ContainsSameValueAs(const Uniform& rhs) const
		{
			if (size != rhs.size ||
				type != rhs.type)
			{
				return false;
			}

			switch (type)
			{
			case Gfx::UniformType::Float:
				for (int i = 0; i < size; ++i)
				{
					if (fValue[i] != rhs.fValue[i])
					{
						return false;
					}
				}
				break;
			case Gfx::UniformType::Int:
				for (int i = 0; i < size; ++i)
				{
					if (iValue[i] != rhs.iValue[i])
					{
						return false;
					}
				}
				break;
			case Gfx::UniformType::Sampler:
				if (textureId != rhs.textureId)
				{
					return false;
				}
				break;
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
			case Gfx::UniformType::UniformBuffer:
				if (uniformBufferId != rhs.uniformBufferId)
				{
					return false;
				}
				break;
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
			case Gfx::UniformType::StorageBufferInput:
			case Gfx::UniformType::StorageBufferOutput:
				if (storageBufferId != rhs.storageBufferId)
				{
					return false;
				}
				break;
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
			};
			return true;
		}
#endif // !GFX_HASH_UNIFORM_VALUE
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING
	};
}
