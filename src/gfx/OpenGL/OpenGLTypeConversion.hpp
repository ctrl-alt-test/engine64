#pragma once

#include "engine/debug/Assert.hpp"
#include "gfx/BlendingMode.hpp"
#include "gfx/IGraphicLayer.hpp"
#include "gfx/PolygonMode.hpp"
#include "gfx/RasterTests.hpp"
#include "gfx/VertexAttribute.hpp"
#include <GL/gl.h>

namespace Gfx
{
	// glTexImage2D (format)
	struct TextureFormatConversion
	{
		TextureFormat::Enum			format;
		GLenum						glenum_internalFormat;	// Specifies the number of color components in the texture.
		GLenum						glenum_format;			// Specifies the format of the pixel data.
		GLenum						glenum_type;			// Specifies the data type of the pixel data.
	};

	extern const TextureFormatConversion textureFormatLUT[];

	inline GLenum getTextureFormat_InternalFormatGLenum(const TextureFormat::Enum& format)
	{
		ASSERT(textureFormatLUT[format].format == format);
		return textureFormatLUT[format].glenum_internalFormat;
	}

	inline GLenum getTextureFormat_FormatGLenum(const TextureFormat::Enum& format)
	{
		ASSERT(textureFormatLUT[format].format == format);
		return textureFormatLUT[format].glenum_format;
	}

	inline GLenum getTextureFormat_TypeGLenum(const TextureFormat::Enum& format)
	{
		ASSERT(textureFormatLUT[format].format == format);
		return textureFormatLUT[format].glenum_type;
	}

	// glVertexAttribPointer
	struct VertexAttributeTypeConversion
	{
		VertexAttributeType::Enum	type;
		GLenum						glenum;
		int							size;
	};

	extern const VertexAttributeTypeConversion vertexAttributeTypeLUT[];

	inline GLenum getVertexAttributeGLenum(const VertexAttributeType::Enum& type)
	{
		ASSERT(vertexAttributeTypeLUT[type].type == type);
		return vertexAttributeTypeLUT[type].glenum;
	}

	inline int getVertexAttributeSize(const VertexAttributeType::Enum& type)
	{
		ASSERT(vertexAttributeTypeLUT[type].type == type);
		return vertexAttributeTypeLUT[type].size;
	}

#if DEBUG
	// glGetError
	struct ErrorDescription
	{
		GLenum						error;
		const char*					errorString;
		const char*					description;
	};

	extern const ErrorDescription errorDescriptionLUT[];

	const char* getErrorCode(GLenum error);
	const char* getErrorDescription(GLenum error);

	// glDebugMessageCallback
	struct DebugType
	{
		GLenum						type;
		const char*					typeString;
	};

	extern const DebugType debugTypeLUT[];

	const char* getDebugType(GLenum type);
#endif // DEBUG
}
