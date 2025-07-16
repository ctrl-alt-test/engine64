#include "OpenGLLayer.hpp"

#include "Extensions.hpp"
#include "OpenGLTypeConversion.hpp"
#include "engine/debug/Assert.hpp"
#include "engine/debug/Debug.hpp" // FIXME: ideally Gfx should not have dependency over Engine.
#include "gfx/Geometry.hpp"
#include "gfx/ShadingParameters.hpp"
#include <stdexcept>
#include <cstring>
#include <GL/gl.h>
#include "glext.h"

#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
#include "engine/container/HashTable.hxx"
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

#if GFX_MULTI_API || GFX_OPENGL_ONLY

#define MAX_MRT 4

// Defines whether after a shader compilation, we should try to get the
// log from the shader compiler.
#ifndef ENABLE_SHADER_COMPILATION_ERROR_CHECK
#define ENABLE_SHADER_COMPILATION_ERROR_CHECK 1
#endif

// Defines if, in case the log from the shader compiler is displayed,
// we should try to reformat it so the IDE can understand it.
#ifndef ENABLE_SHADER_COMPILER_MESSAGE_PARSING
#define ENABLE_SHADER_COMPILER_MESSAGE_PARSING 1
#endif

#if DEBUG
#define ENABLE_OPENGL_ERROR_CHECK	1
#endif // DEBUG

#if ENABLE_OPENGL_ERROR_CHECK

#define GL_CHECK(exp) 													\
	do																	\
	{																	\
		exp;															\
		GLenum error = glGetError();									\
		if (error != GL_NO_ERROR)										\
		{																\
			LOG_ERROR("\n%s(%d): OpenGL %s error.",						\
					  __FILE__, __LINE__, Gfx::getErrorCode(error));	\
			throw new std::runtime_error(Gfx::getErrorDescription(error));	\
		}																\
		break;															\
	} while (0)

#else // !ENABLE_OPENGL_ERROR_CHECK

#define GL_CHECK(exp) exp

#endif // !ENABLE_OPENGL_ERROR_CHECK

using namespace Gfx;

#if DEBUG
void MessageCallback(GLenum /* source */,
					 GLenum type,
					 GLuint /* id */,
					 GLenum severity,
					 GLsizei /* length */,
					 const GLchar *message,
					 GLvoid * /*userParam */)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		LOG_ERROR("%s; severity = 0x%x; %s",
			getDebugType(type), severity, message);
	}
	else
	{
		LOG_DEBUG("%s; severity = 0x%x; %s",
			getDebugType(type), severity, message);
	}
}
#endif // DEBUG

bool OpenGLLayer::CreateRenderingContext()
{
	if (!InitializeOpenGLExtensions())
	{
#ifndef DISABLE_TERMINATE_ON_EXTENSION_CHECK_FAILURE
		return false;
#endif // !DISABLE_TERMINATE_ON_EXTENSION_CHECK_FAILURE
	}

	m_FBOs.init(GFX_MAX_FRAME_BUFFERS);
	m_shaders.init(GFX_MAX_SHADERS);
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
	m_SSBOs.init(GFX_MAX_STORAGE_BUFFERS);
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
	m_textures.init(GFX_MAX_TEXTURES);
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	m_UBOs.init(GFX_MAX_UNIFORM_BUFFERS);
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	m_VBOs.init(GFX_MAX_VERTEX_BUFFERS);

	for (int i = 0; i < GFX_MAX_TEXTURE_SLOTS; ++i)
	{
		m_currentTextures[i] = TextureID::InvalidID;
	}
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	for (int i = 0; i < GFX_MAX_UNIFORM_BUFFER_SLOTS; ++i)
	{
		m_currentUBOs[i] = UniformBufferID::InvalidID;
	}
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
	for (int i = 0; i < GFX_MAX_VERTEX_ATTRIBUTES; ++i)
	{
		m_enabledVertexAttributes[i] = false;
	}

	m_currentViewport.x = -1; // Initializing to an invalid value
	m_currentViewport.y = -1;
	m_currentViewport.width = -1;
	m_currentViewport.height = -1;
	m_currentPolygonMode = PolygonMode::Filled;

	m_currentFrameBuffer = FrameBufferID::InvalidID;
	m_currentShader = ShaderID::InvalidID;
	m_currentVBO = VertexBufferID::InvalidID;

#if DEBUG && ENABLE_GLDEBUGMESSAGECALLBACK
	// This doens't work everywhere, hence the specific gate.
	GL_CHECK(glEnable(GL_DEBUG_OUTPUT));
	GL_CHECK(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
	GL_CHECK(glDebugMessageCallback((GLDEBUGPROC)MessageCallback, nullptr));
#endif // DEBUG && ENABLE_GLDEBUGMESSAGECALLBACK

	return true;
}

void OpenGLLayer::SetRasterizerState(const Viewport& viewport,
									 const PolygonMode::Enum polygonMode,
									 const RasterTests& rasterTests,
									 const BlendingMode& blendingMode)
{
	if (m_currentViewport != viewport)
	{
		GL_CHECK(glViewport(viewport.x, viewport.y, viewport.width, viewport.height));
		m_currentViewport = viewport;
	}

	if (m_currentPolygonMode != polygonMode)
	{
		GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, polygonMode));
		m_currentPolygonMode = polygonMode;
	}

	if (m_currentRasterTests != rasterTests)
	{
#if GFX_ENABLE_FACE_CULLING
		const bool enableFaceCulling =
			rasterTests.faceCulling != FaceCulling::None;
		if (enableFaceCulling)
		{
			GL_CHECK(glEnable(GL_CULL_FACE));
			GL_CHECK(glCullFace(rasterTests.faceCulling));
		}
		else
		{
			GL_CHECK(glDisable(GL_CULL_FACE));
		}
#endif // GFX_ENABLE_FACE_CULLING

#if GFX_ENABLE_SCISSOR_TESTING
		if (rasterTests.scissorTestEnabled)
		{
			GL_CHECK(glEnable(GL_SCISSOR_TEST));
			GL_CHECK(glScissor(rasterTests.scissorX, rasterTests.scissorY,
				rasterTests.scissorWidth, rasterTests.scissorHeight));
		}
		else
		{
			GL_CHECK(glDisable(GL_SCISSOR_TEST));
		}
#endif // GFX_ENABLE_SCISSOR_TESTING

#if GFX_ENABLE_STENCIL_TESTING
		const bool enableStencilTest =
			rasterTests.stencilFrontTest != StencilFunction::Always ||
			rasterTests.stencilBackTest != StencilFunction::Always ||
			rasterTests.stencilFrontOpPass != StencilOperation::Keep ||
			rasterTests.stencilBackOpPass != StencilOperation::Keep;
		if (enableStencilTest)
		{
			GL_CHECK(glEnable(GL_STENCIL_TEST));

			GL_CHECK(glStencilFuncSeparate(GL_FRONT,
				rasterTests.stencilFrontTest,
				rasterTests.stencilFrontValue,
				rasterTests.stencilFrontMask));
			GL_CHECK(glStencilOpSeparate(GL_FRONT,
				rasterTests.stencilFrontOpStencilFail,
				rasterTests.stencilFrontOpDepthFail,
				rasterTests.stencilFrontOpPass));

			GL_CHECK(glStencilFuncSeparate(GL_BACK,
				rasterTests.stencilBackTest,
				rasterTests.stencilBackValue,
				rasterTests.stencilBackMask));
			GL_CHECK(glStencilOpSeparate(GL_BACK,
				rasterTests.stencilBackOpStencilFail,
				rasterTests.stencilBackOpDepthFail,
				rasterTests.stencilBackOpPass));
		}
		else
		{
			GL_CHECK(glDisable(GL_STENCIL_TEST));
		}
#endif // GFX_ENABLE_STENCIL_TESTING

#if GFX_ENABLE_DEPTH_TESTING
		const bool enableDepthTest =
			rasterTests.depthTest != DepthFunction::Always ||
			rasterTests.depthWrite == true;
		if (enableDepthTest)
		{
			GL_CHECK(glEnable(GL_DEPTH_TEST));
			GL_CHECK(glDepthFunc(rasterTests.depthTest));
			GL_CHECK(glDepthMask(rasterTests.depthWrite ? GL_TRUE : GL_FALSE));
		}
		else
		{
			GL_CHECK(glDisable(GL_DEPTH_TEST));
		}
#endif // GFX_ENABLE_DEPTH_TESTING

#if GFX_ENABLE_CLIPPING
		if (rasterTests.enableClipDistance)
		{
			GL_CHECK(glEnable(GL_CLIP_DISTANCE0));
		}
		else
		{
			GL_CHECK(glDisable(GL_CLIP_DISTANCE0));
		}
#endif // GFX_ENABLE_CLIPPING

		m_currentRasterTests = rasterTests;
	}

	if (m_currentBlendingMode != blendingMode)
	{
		const bool enableBlending =
			blendingMode.rgbEquation != BlendEquation::SrcPlusDst ||
			blendingMode.alphaEquation != BlendEquation::SrcPlusDst ||
			blendingMode.srcRGBFunction != BlendFunction::One ||
			blendingMode.srcAlphaFunction != BlendFunction::One ||
			blendingMode.dstRGBFunction != BlendFunction::Zero ||
			blendingMode.dstAlphaFunction != BlendFunction::Zero;

		if (enableBlending)
		{
			GL_CHECK(glEnable(GL_BLEND));
			GL_CHECK(glBlendFuncSeparate(blendingMode.srcRGBFunction,
										 blendingMode.dstRGBFunction,
										 blendingMode.srcAlphaFunction,
										 blendingMode.dstAlphaFunction));
			GL_CHECK(glBlendEquationSeparate(blendingMode.rgbEquation,
											 blendingMode.alphaEquation));
		}
		else
		{
			GL_CHECK(glDisable(GL_BLEND));
		}

		m_currentBlendingMode = blendingMode;
	}
}

VertexBufferID OpenGLLayer::CreateVertexBuffer()
{
	VBOInfo newVBO;
	newVBO.primitiveType = GL_TRIANGLES;
	newVBO.vertexAttributes = nullptr;
	newVBO.numberOfAttributes = 0;
	GL_CHECK(glGenBuffers(1, &newVBO.vertexBuffer));
	GL_CHECK(glGenBuffers(1, &newVBO.indexBuffer));

	// Internal resource indexing
	m_VBOs.add(newVBO);
	VertexBufferID id = { m_VBOs.size - 1 };
	return id;
}

void OpenGLLayer::DestroyVertexBuffer(const VertexBufferID id)
{
	ASSERT(m_VBOs.size > id.index);
	GL_CHECK(glDeleteBuffers(1, &m_VBOs[id.index].vertexBuffer));
	GL_CHECK(glDeleteBuffers(1, &m_VBOs[id.index].indexBuffer));
	m_VBOs[id.index].vertexBuffer = 0;
	m_VBOs[id.index].indexBuffer = 0;
}

void OpenGLLayer::LoadVertexBuffer(const VertexBufferID id,
								   PrimitiveType::Enum primitiveType,
								   const VertexAttribute* vertexAttributes,
								   int numberOfAttributes, int stride,
								   int vertexDataSize, const void* vertexData,
								   int indexDataSize, const void* indexData,
								   VertexIndexType::Enum indexType)
{
	ASSERT(vertexAttributes != nullptr);
	ASSERT(numberOfAttributes > 0);
	ASSERT(vertexDataSize > 0);
	ASSERT(vertexData != nullptr);
	ASSERT(m_VBOs.size > id.index);

	VBOInfo& vboInfo = m_VBOs[id.index];
	vboInfo.primitiveType = primitiveType;
	vboInfo.vertexAttributes = vertexAttributes;
	vboInfo.numberOfAttributes = numberOfAttributes;
	vboInfo.stride = stride;

	int vertexBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vertexBufferToRestore));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboInfo.vertexBuffer));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferToRestore));

	vboInfo.indexType = indexType;
	vboInfo.indexed = false;
	if (indexDataSize != 0 && indexData != nullptr)
	{
		int indexBufferToRestore = 0;
		GL_CHECK(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &indexBufferToRestore));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboInfo.indexBuffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferToRestore));
		vboInfo.indexed = true;
	}
}

void OpenGLLayer::BindVertexBuffer(const VertexBufferID id)
{
	ASSERT(m_VBOs.size > id.index);
	const int vboIndex = id.index;
	if (m_currentVBO.index == vboIndex)
	{
		return;
	}

	int firstAttributeToDisable = 0;
	if (vboIndex >= 0)
	{
		const VBOInfo& vboInfo = m_VBOs[vboIndex];

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboInfo.vertexBuffer));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (vboInfo.indexed ? vboInfo.indexBuffer : 0)));

		unsigned long offset = 0;
		for (int i = 0; i < vboInfo.numberOfAttributes; ++i)
		{
			const VertexAttribute& attrib = vboInfo.vertexAttributes[i];

			const GLenum glenum = getVertexAttributeGLenum(attrib.type);
			const unsigned long size = attrib.num * getVertexAttributeSize(attrib.type);

			GL_CHECK(glEnableVertexAttribArray(i));
			GL_CHECK(glVertexAttribPointer(i, attrib.num, glenum, GL_FALSE, vboInfo.stride, (void*)((unsigned long)nullptr + offset)));
			offset += size;

			m_enabledVertexAttributes[i] = true;
		}
		firstAttributeToDisable = vboInfo.numberOfAttributes;
	}
	else
	{
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	for (int i = firstAttributeToDisable; i < GFX_MAX_VERTEX_ATTRIBUTES; ++i)
	{
		if (m_enabledVertexAttributes[i])
		{
			GL_CHECK(glDisableVertexAttribArray(i));
			m_enabledVertexAttributes[i] = false;
		}
	}

	m_currentVBO.index = vboIndex;
}

TextureID OpenGLLayer::CreateTexture()
{
	TextureInfo newTexture;
	newTexture.width = 0;
	newTexture.height = 0;
	GL_CHECK(glGenTextures(1, &newTexture.texture));

	// Internal resource indexing
	m_textures.add(newTexture);
	TextureID id = { m_textures.size - 1 };
	return id;
}

void OpenGLLayer::DestroyTexture(const TextureID id)
{
	ASSERT(m_textures.size > id.index);
	GL_CHECK(glDeleteTextures(1, &m_textures[id.index].texture));
	m_textures[id.index].texture = 0;
}

void OpenGLLayer::LoadTexture(const TextureID id,
							  int width, int height,
							  TextureType::Enum textureType,
							  TextureFormat::Enum textureFormat,
							  int side, int lodLevel,
							  const void* data,
							  const TextureSampling& textureSampling)
{
	ASSERT(width * height > 0);
	ASSERT(m_textures.size > id.index);

	TextureInfo& textureInfo = m_textures[id.index];

	ASSERT(width * height > 0);
	textureInfo.width = width;
	textureInfo.height = height;
	textureInfo.type = textureType;
	const GLenum target = (textureInfo.type == GL_TEXTURE_2D ? textureInfo.type : GL_TEXTURE_CUBE_MAP_POSITIVE_X + side);
	const GLenum internalFormat = getTextureFormat_InternalFormatGLenum(textureFormat);
	const GLenum format = getTextureFormat_FormatGLenum(textureFormat);
	const GLenum type = getTextureFormat_TypeGLenum(textureFormat);
	const GLenum minFilter = textureSampling.minifyingFilter;
	const GLenum magFilter = textureSampling.magnifyingFilter;
	textureInfo.format = format;

	int textureToRestore = 0;
	GL_CHECK(glGetIntegerv((textureInfo.type == GL_TEXTURE_2D ? GL_TEXTURE_BINDING_2D : GL_TEXTURE_BINDING_CUBE_MAP), &textureToRestore));
	GL_CHECK(glBindTexture(textureInfo.type, textureInfo.texture));

	GL_CHECK(glTexImage2D(target, (lodLevel < 0 ? 0 : lodLevel), internalFormat, width, height, 0, format, type, data));
	if (data != nullptr && textureInfo.type == GL_TEXTURE_2D)
	{
		if (lodLevel < 0 && textureSampling.minifyingFilter >= TextureFilter::NearestMipmapLinear)
		{
			GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
		}

		ASSERT(textureSampling.maxAnisotropy >= 1.f);
		GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_MIN_FILTER, minFilter));
		GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_MAG_FILTER, magFilter));
		GL_CHECK(glTexParameterf(textureInfo.type, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureSampling.maxAnisotropy));
	}
	else
	{
		GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_MIN_FILTER, minFilter));
		GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_MAG_FILTER, magFilter));
		GL_CHECK(glTexParameterf(textureInfo.type, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2));
	}

	const GLenum sWrap = textureSampling.sWrap;
	const GLenum tWrap = textureSampling.tWrap;
	if (textureInfo.type == GL_TEXTURE_CUBE_MAP)
	{
		const GLenum rWrap = textureSampling.rWrap;
		GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_WRAP_R, rWrap));
	}
	GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_WRAP_S, sWrap));
	GL_CHECK(glTexParameteri(textureInfo.type, GL_TEXTURE_WRAP_T, tWrap));

	GL_CHECK(glBindTexture(textureInfo.type, textureToRestore));
}

void OpenGLLayer::BindTexture(const TextureID id, int slot)
{
	ASSERT(m_textures.size > id.index);
	ASSERT(slot >= 0 && slot < GFX_MAX_TEXTURE_SLOTS);
	const int textureIndex = id.index;
	if (m_currentTextures[slot].index == textureIndex)
	{
		return;
	}

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
	if (textureIndex >= 0)
	{
		GL_CHECK(glBindTexture(m_textures[textureIndex].type, m_textures[textureIndex].texture));
	}
	else
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}
	m_currentTextures[slot].index = textureIndex;
}

void OpenGLLayer::GenerateMipMaps(const TextureID id)
{
	ASSERT(m_textures.size > id.index);
	TextureInfo& textureInfo = m_textures[id.index];

	int slot = 0;
	for (int i = 0; i < GFX_MAX_TEXTURE_SLOTS; ++i)
	{
		if (m_currentTextures[slot].index == id.index)
		{
			slot = i;
			break;
		}
	}
	BindTexture(id, slot);
	if (textureInfo.type == GL_TEXTURE_2D)
	{
		GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
	}
}

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
UniformBufferID OpenGLLayer::CreateUniformBuffer()
{
	UBOInfo newUBO;
	newUBO.size = 0;
	GL_CHECK(glGenBuffers(1, &newUBO.uniformBuffer));

	// Internal resource indexing
	m_UBOs.add(newUBO);
	UniformBufferID id = { m_UBOs.size - 1 };
	return id;
}

void OpenGLLayer::DestroyUniformBuffer(const UniformBufferID id)
{
	ASSERT(m_UBOs.size > id.index);
	GL_CHECK(glDeleteBuffers(1, &m_UBOs[id.index].uniformBuffer));
	m_UBOs[id.index].uniformBuffer = 0;
}

void OpenGLLayer::LoadUniformBuffer(const UniformBufferID id,
									int size,
									const void* data)
{
	ASSERT(size >= 0);
	ASSERT(m_UBOs.size > id.index);

	UBOInfo& uboInfo = m_UBOs[id.index];

	int uniformBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &uniformBufferToRestore));

	GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, uboInfo.uniformBuffer)); // Yes, questionable naming. :(
	if (uboInfo.size == 0)
	{
		GL_CHECK(glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_COPY));
		uboInfo.size = size;
	}
	else
	{
		ASSERT(uboInfo.size == size);
		GL_CHECK(glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data));
		//GLvoid* dst;
		//GL_CHECK(dst = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
		//memcpy(dst, data, size);
		//GL_CHECK(glUnmapBuffer(GL_UNIFORM_BUFFER));
	}
	GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferToRestore));
}

void OpenGLLayer::BindUniformBuffer(const UniformBufferID id,
									GLuint program,
									int slot,
									const char* name)
{
	ASSERT(m_UBOs.size > id.index);
	ASSERT(slot >= 0 && slot < GFX_MAX_UNIFORM_BUFFER_SLOTS);
	const int UBOIndex = id.index;

	unsigned int blockIndex;
	GL_CHECK(blockIndex = glGetUniformBlockIndex(program, name));
	GL_CHECK(glUniformBlockBinding(program, blockIndex, slot));

	if (m_currentUBOs[slot].index == UBOIndex)
	{
		return;
	}

	if (UBOIndex >= 0)
	{
		GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_UBOs[UBOIndex].uniformBuffer));
	}
	else
	{
		GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	m_currentUBOs[slot].index = UBOIndex;
}
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
StorageBufferID OpenGLLayer::CreateStorageBuffer()
{
	SSBOInfo newSSBO;
	//newSSBO.size = 0;
	GL_CHECK(glGenBuffers(1, &newSSBO.storageBuffer));

	// Internal resource indexing
	m_SSBOs.add(newSSBO);
	StorageBufferID id = { m_SSBOs.size - 1 };
	return id;
}

void OpenGLLayer::DestroyStorageBuffer(const StorageBufferID id)
{
	ASSERT(m_SSBOs.size > id.index);
	GL_CHECK(glDeleteBuffers(1, &m_SSBOs[id.index].storageBuffer));
	m_SSBOs[id.index].storageBuffer = 0;
}

void OpenGLLayer::LoadStorageBuffer(const StorageBufferID id, size_t size, const void* data)
{
	ASSERT(m_SSBOs.size > id.index);
	SSBOInfo& ssboInfo = m_SSBOs[id.index];

	int storageBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &storageBufferToRestore));

	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInfo.storageBuffer));
	GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageBufferToRestore));
}

void OpenGLLayer::ReadStorageBuffer(const StorageBufferID id, size_t size, void* dest)
{
	ASSERT(m_SSBOs.size > id.index);
	SSBOInfo& ssboInfo = m_SSBOs[id.index];

	int storageBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &storageBufferToRestore));

	// If the buffer was previously in a writing state, make sure to
	// synchronize before reading.
	if (ssboInfo.writing)
	{
		GL_CHECK(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
		ssboInfo.writing = false;
	}
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInfo.storageBuffer));

	// Map the buffer to client's memory space for reading.
	void* data;
	GL_CHECK(data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT));
	memcpy(dest, data, size);
	GL_CHECK(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
	GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, storageBufferToRestore));
}

void OpenGLLayer::BindStorageBuffer(const StorageBufferID id,
									GLuint program,
									int slot,
									const char* name,
									bool writing)
{
	ASSERT(m_SSBOs.size > id.index);
	ASSERT(slot >= 0 && slot < GFX_MAX_STORAGE_BUFFER_BINDINGS);
	const int SSBOIndex = id.index;
	if (m_currentSSBOs[slot].index == SSBOIndex)
	{
		return;
	}

	unsigned int blockIndex;
	GL_CHECK(blockIndex = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, name));
	ASSERT(blockIndex != GL_INVALID_INDEX);
	GL_CHECK(glShaderStorageBlockBinding(program, blockIndex, slot));

	if (SSBOIndex >= 0)
	{
		if (m_SSBOs[SSBOIndex].writing && !writing)
		{
			GL_CHECK(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
		}
		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_SSBOs[SSBOIndex].storageBuffer));
		m_SSBOs[SSBOIndex].writing = writing;
	}
	else
	{
		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
	}
	m_currentSSBOs[slot].index = SSBOIndex;
}

#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

#if ENABLE_SHADER_COMPILATION_ERROR_CHECK

// This function is a complete hack, and tries to recognize and
// partially reformat errors from the shader compiler, so Visual Studio
// is able to parse the first line. When it works Visual Studio can
// then show us directly where the error is.
//
// The formatting used by Visual Studio looks like:
// d:\ctrl-alt-test\h2o\src\gfx\opengl\opengllayer.cpp(411): error C2220: warning treated as error - no 'object' file generated
static void ReformatShaderCompilerMessage(char originalLog[], const char* srcInfo, size_t maxLen)
{
#if ENABLE_SHADER_COMPILER_MESSAGE_PARSING
	char buffer[2048];

	if (strncmp(originalLog, "0(", 2) == 0)
	{
		// It seems to be the NVidia message format used here:
		// 0(97) : error C0000: syntax error, unexpected '=', expecting "::" at token "="
		// The log starts lines with a '0', where the name of the
		// source file should be.
		strncpy(buffer, originalLog + 1, sizeof(buffer) - 1);
		strcpy(originalLog, srcInfo);
		size_t len = strlen(buffer) + strlen(srcInfo);
		strncpy(originalLog + strlen(srcInfo), buffer, (len >= maxLen ? maxLen - 1 : len));
	}
	else if (strncmp(originalLog, "(", 1) == 0)
	{
		// It seems to be the NVidia message format used here:
		// (1045) : error C0000: syntax error, unexpected string literal at token "<string-const>"
		strncpy(buffer, originalLog, sizeof(buffer) - 1);
		strcpy(originalLog, srcInfo);
		size_t len = strlen(buffer) + strlen(srcInfo);
		strncpy(originalLog + strlen(srcInfo), buffer, (len >= maxLen ? maxLen - 1 : len));
	}
	else if (strncmp(originalLog, "ERROR: 0:", 9) == 0)
	{
		// It seems to be the NVidia message format used here:
		// ERROR: 0:97: 'fragme' : undeclared identifier
		strncpy(buffer, originalLog + 8, sizeof(buffer) - 1);
		buffer[0] = '(';
		char* column = strstr(buffer, ":");
		if (column != nullptr && column - buffer < 6)
		{
			column[0] = ')';
			column[1] = ':';
		}
		strcpy(originalLog, srcInfo);
		size_t len = strlen(buffer) + strlen(srcInfo);
		strncpy(originalLog + strlen(srcInfo), buffer, (len >= maxLen ? maxLen - 1 : len));
	}
	else
	{
		LOG_DEBUG("Could not recognize the formatting used in the following message. Please consider implementing it in " __FILE__);
	}
#endif // ENABLE_SHADER_COMPILER_MESSAGE_PARSING
	LOG_RAW(originalLog);
}
#endif // ENABLE_SHADER_COMPILATION_ERROR_CHECK

GLuint CompileShader(ShaderType::Enum shaderType, const char* src, const char* srcInfo)
{
	if (src == nullptr)
	{
		return 0;
	}

	const GLuint shader = glCreateShader(shaderType);
	GL_CHECK(glShaderSource(shader, 1, (const GLchar**)&src, nullptr));
	GL_CHECK(glCompileShader(shader));

#if ENABLE_SHADER_COMPILATION_ERROR_CHECK
	GLint wentFine = GL_TRUE;
	GLsizei logLength = 0;
	GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &wentFine));
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));

#if ENABLE_LOG
	if (logLength != 0)
	{
		// Either I haven't understood how to use glGetShaderInfoLog, or
		// there is a bug in that function. Whether log is a static
		// array or a dynamically allocated one, I have cases where an
		// exception will happen here.
		// Pretending the buffer is smaller than it really is seems to
		// mitigate the issue.
		// 
		char log[2048];
		GL_CHECK(glGetShaderInfoLog(shader, sizeof(log) / sizeof(log[0]) - 100, &logLength, log));
		if (logLength != 0)
		{
			if (wentFine == GL_TRUE)
			{
				LOG_WARNING("Compilation of %s succeeded, but something might still be wrong with it:", srcInfo);
			}
			else
			{
				LOG_ERROR("Compilation of %s failed:", srcInfo);
			}
			ReformatShaderCompilerMessage(log, srcInfo, sizeof(log));
		}
	}
	else if (wentFine != GL_TRUE)
	{
		LOG_ERROR("Something went wrong during compilation of $s but there was no log from the shader compiler.", srcInfo);
	}
#endif // ENABLE_LOG

#if _HAS_EXCEPTIONS
	if (wentFine != GL_TRUE)
	{
		throw new std::exception("Shader compilation failed.");
	}
#endif // _HAS_EXCEPTIONS
#endif // ENABLE_SHADER_COMPILATION_ERROR_CHECK

	return shader;
}

GLuint CreateAndLinkProgram(const GLuint* shaders, int numShaders)
{
	const GLuint program = glCreateProgram();
	for (int i = 0; i < numShaders; ++i)
	{
		GL_CHECK(glAttachShader(program, shaders[i]));
	}
	GL_CHECK(glLinkProgram(program));

#if ENABLE_SHADER_COMPILATION_ERROR_CHECK
	GLint wentFine = GL_TRUE;
	GLsizei logsize = 0;
	GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &wentFine));
	GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize));

	if (logsize != 0)
	{
		// Using a static array to avoid using new[].
		char log[2048];

		GL_CHECK(glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &logsize, log));
		if (logsize != 0)
		{
			if (wentFine == GL_TRUE)
			{
				LOG_WARNING(log);
			}
			else
			{
				LOG_ERROR(log);
			}
		}
	}
	else if (wentFine != GL_TRUE)
	{
		LOG_ERROR("Something went wrong during linking but there was no log from the shader compiler.");
	}

	for (int i = 0; i < numShaders; ++i)
	{
		GL_CHECK(glDetachShader(program, shaders[i]));
	}

#if _HAS_EXCEPTIONS
	if (wentFine != GL_TRUE)
	{
		throw new std::exception("Shader linking failed.");
	}
#endif // _HAS_EXCEPTIONS
#endif // ENABLE_SHADER_COMPILATION_ERROR_CHECK

	return program;
}

ShaderID OpenGLLayer::CreateShader()
{
	ShaderInfo newShader;
	newShader.shaders[0] = 0;
	newShader.shaders[1] = 0;
	newShader.program = 0;

	// Internal resource indexing
	m_shaders.add(newShader);

#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
	// Initialize the array here instead of on newShader, to avoid
	// having it deallocated when exiting the function.
	// The purpose of the 3x is to reduce the chances of collisions.
	m_shaders.last().currentUniforms.init(3 * GFX_MAX_UNIFORMS);
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

	ShaderID id = { m_shaders.size - 1 };
	return id;
}

void OpenGLLayer::DestroyShader(const ShaderID id)
{
	ASSERT(m_shaders.size > id.index);
	ShaderInfo& shaderInfo = m_shaders[id.index];

	GL_CHECK(glDeleteProgram(shaderInfo.program));
	GL_CHECK(glDeleteShader(shaderInfo.shaders[0]));
	GL_CHECK(glDeleteShader(shaderInfo.shaders[1]));
	shaderInfo.program = 0;
	shaderInfo.shaders[0] = 0;
	shaderInfo.shaders[1] = 0;

	BindShader(ShaderID::InvalidID);
}

void OpenGLLayer::LoadShader(const ShaderID id,
							 const ShaderStage* shaderStages,
							 int numberOfStages)
{
	ASSERT(m_shaders.size > id.index);
	ShaderInfo& shaderInfo = m_shaders[id.index];

	GL_CHECK(glDeleteProgram(shaderInfo.program)); // From the manual: "A value of 0 for program will be silently ignored."
	for (int i = 0; i < sizeof(shaderInfo.shaders) / sizeof(shaderInfo.shaders[0]); ++i)
	{
		// From the manual: "A value of 0 for shader will be silently ignored."
		GL_CHECK(glDeleteShader(shaderInfo.shaders[i]));
		shaderInfo.shaders[i] = 0;
	}
	shaderInfo.program = 0;
#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
	shaderInfo.currentUniforms.clear();
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

	// Warning: the following functions can throw exceptions, which
	// means shader.program and shader.shaders[x] might not be set in
	// case of compilation error. That's why we set them to 0 first.
	for (int i = 0; i < numberOfStages; ++i)
	{
		const ShaderStage& stage = shaderStages[i];
		shaderInfo.shaders[i] = CompileShader(stage.shaderType, stage.source, stage.sourceInfo);
	}
	shaderInfo.program = CreateAndLinkProgram(shaderInfo.shaders, numberOfStages);
}

void OpenGLLayer::BindShader(const ShaderID id)
{
	ASSERT(m_shaders.size > id.index);
	const int shaderIndex = id.index;
	if (m_currentShader.index == shaderIndex)
	{
		return;
	}

	if (shaderIndex >= 0)
	{
		GL_CHECK(glUseProgram(m_shaders[shaderIndex].program));
	}
	else
	{
		GL_CHECK(glUseProgram(0));
	}
	m_currentShader.index = shaderIndex;
}

#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
//int uniformBindingsAvoided = 0;
//int uniformBindingsUpdated = 0;
//int uniformBindingsSet = 0;

#if GFX_HASH_UNIFORM_VALUE
bool SkipBindUniform(Container::HashTable<const char*, unsigned int>& currentlyBoundUniforms,
					 const Uniform& uniform)
{
	unsigned int* hashOfBoundUniform = currentlyBoundUniforms[uniform.name];
	unsigned int hashOfUniform = 0;
	switch (uniform.type)
	{
	case Gfx::UniformType::Float:
		hashOfUniform = Noise::Hash::get32(uniform.fValue, uniform.size * sizeof(float));
		break;
	case Gfx::UniformType::Int:
		hashOfUniform = Noise::Hash::get32(uniform.iValue, uniform.size * sizeof(int));
		break;
	case Gfx::UniformType::Sampler:
		hashOfUniform = Noise::Hash::get32(uniform.id);
		break;
	};

	if (hashOfBoundUniform != nullptr)
	{
		if (*hashOfBoundUniform == hashOfUniform)
		{
			//uniformBindingsAvoided++;
			return true;
		}
		else
		{
			//uniformBindingsUpdated++;
			*hashOfBoundUniform = hashOfUniform;
			return false;
		}
	}
	else
	{
		//uniformBindingsSet++;
		currentlyBoundUniforms.add(uniform.name, hashOfUniform);
		return false;
	}
}
#else // !GFX_HASH_UNIFORM_VALUE
bool SkipBindUniform(Container::HashTable<const char*, Uniform>& currentlyBoundUniforms,
					 const Uniform& uniform)
{
	Uniform* boundUniform = currentlyBoundUniforms[uniform.name];

	if (boundUniform != nullptr)
	{
		if (boundUniform->ContainsSameValueAs(uniform))
		{
			//uniformBindingsAvoided++;
			return true;
		}
		else
		{
			//uniformBindingsUpdated++;
			*boundUniform = uniform;
			return false;
		}
	}
	else
	{
		//uniformBindingsSet++;
		currentlyBoundUniforms.add(uniform.name, uniform);
		return false;
	}
}
#endif // !GFX_HASH_UNIFORM_VALUE
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

void OpenGLLayer::BindUniforms(const Uniform* uniforms, int numberOfUniforms)
{
	GLuint program = 0;
	if (m_currentShader.index >= 0)
	{
		program = m_shaders[m_currentShader.index].program;
	}

	if (program != 0)
	{
#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
#if GFX_HASH_UNIFORM_VALUE
		Container::HashTable<const char*, unsigned int>& currentlyBoundUniforms = m_shaders[m_currentShader.index].currentUniforms;
#else // !GFX_HASH_UNIFORM_VALUE
		Container::HashTable<const char*, Uniform>& currentlyBoundUniforms = m_shaders[m_currentShader.index].currentUniforms;
#endif // !GFX_HASH_UNIFORM_VALUE
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

		int textureSlot = 0;
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		int uniformBufferSlot = 0;
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		int storageBufferSlot = 0;
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
		for (int i = 0; i < numberOfUniforms; ++i)
		{
			const Uniform& uniform = uniforms[i];
			if (uniform.name == nullptr)
			{
				continue;
			}

#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
			if (SkipBindUniform(currentlyBoundUniforms, uniform))
			{
				if (uniform.type == UniformType::Sampler)
				{
					BindTexture(uniform.textureId, textureSlot++);
				}
				continue;
			}
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

			GLint location;
			GL_CHECK(location = glGetUniformLocation(program, uniform.name));
			ASSERT(uniform.size > 0 && uniform.size <= 16);

			switch (uniform.type)
			{
			case UniformType::Float:
				switch (uniform.size)
				{
				case 1: GL_CHECK(glUniform1fv(location, 1, uniform.fValue)); break;
				case 2: GL_CHECK(glUniform2fv(location, 1, uniform.fValue)); break;
				case 3: GL_CHECK(glUniform3fv(location, 1, uniform.fValue)); break;
				case 4: GL_CHECK(glUniform4fv(location, 1, uniform.fValue)); break;
// 				case 9: GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, uniform.fValue)); break;
				case 16:GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, uniform.fValue)); break;
				default:
					LOG_ERROR("Uniform size not supported: %s.", uniform.name);
					break;
				}
				break;
			case UniformType::Int:
				switch (uniform.size)
				{
				case 1: GL_CHECK(glUniform1iv(location, 1, uniform.iValue)); break;
				case 2: GL_CHECK(glUniform2iv(location, 1, uniform.iValue)); break;
				case 3: GL_CHECK(glUniform3iv(location, 1, uniform.iValue)); break;
				case 4: GL_CHECK(glUniform4iv(location, 1, uniform.iValue)); break;
				default:
					LOG_ERROR("Uniform size not supported: %s.", uniform.name);
					break;
				}
				break;
			case UniformType::Sampler:
				ASSERT(uniform.size == 1);
				ASSERT(textureSlot < GFX_MAX_TEXTURE_SLOTS);
				GL_CHECK(glUniform1iv(location, 1, &textureSlot));
				BindTexture(uniform.textureId, textureSlot++);
				break;
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
			case UniformType::UniformBuffer:
				ASSERT(uniform.size == 1);
				ASSERT(uniformBufferSlot < GFX_MAX_UNIFORM_BUFFER_SLOTS);
				BindUniformBuffer(uniform.uniformBufferId, program, uniformBufferSlot++, uniform.name);
				break;
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
			case UniformType::StorageBufferInput:
			case UniformType::StorageBufferOutput:
			{
				ASSERT(uniform.size == 1);
				ASSERT(storageBufferSlot < GFX_MAX_STORAGE_BUFFER_BINDINGS);
				const bool writing = (uniform.type == UniformType::StorageBufferOutput);
				BindStorageBuffer(uniform.storageBufferId, program, storageBufferSlot++, uniform.name, writing);
				break;
			}
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
			default:
				LOG_ERROR("Uniform type not supported.");
				break;
			}
		}
	}
}

FrameBufferID OpenGLLayer::CreateFrameBuffer(const TextureID* textures,
											 int numberOfTextures,
											 int side, int lodLevel)
{
	ASSERT(textures != nullptr && numberOfTextures > 0);

	FBOInfo newFBO;
	newFBO.width = m_textures[textures[0].index].width;
	newFBO.height = m_textures[textures[0].index].height;

	int frameBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &frameBufferToRestore));

	GL_CHECK(glGenFramebuffers(1, &newFBO.frameBuffer));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, newFBO.frameBuffer));
	m_currentFrameBuffer.index = m_FBOs.size - 1;

	GLenum buffers[MAX_MRT];
	int numberOfBuffers = 0;

	for (int i = 0; i < numberOfTextures; ++i)
	{
		const TextureInfo& textureInfo = m_textures[textures[i].index];
		ASSERT(textureInfo.width == newFBO.width && textureInfo.height == newFBO.height);

		GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
		switch (textureInfo.format)
		{
		case GL_STENCIL_INDEX  : attachment = GL_STENCIL_ATTACHMENT; break;
		case GL_DEPTH_COMPONENT: attachment = GL_DEPTH_ATTACHMENT; break;
		case GL_DEPTH_STENCIL  : attachment = GL_DEPTH_STENCIL_ATTACHMENT; break;
		default:
			ASSERT(numberOfBuffers < MAX_MRT);
			buffers[numberOfBuffers++] = attachment;
		}

		if (textureInfo.type == GL_TEXTURE_CUBE_MAP)
		{
			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
											GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
											textureInfo.texture, lodLevel));
		}
		else
		{
			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
											GL_TEXTURE_2D,
											textureInfo.texture, lodLevel));
		}
	}
	// According to the OpenGL 3.3 spec, the buffer selection state is per
	// frame buffer.
	GL_CHECK(glDrawBuffers(numberOfBuffers, buffers));

#if DEBUG
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ASSERT(status != GL_FRAMEBUFFER_UNDEFINED);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
	ASSERT(status != GL_FRAMEBUFFER_UNSUPPORTED);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	ASSERT(status != GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);

	ASSERT(status == GL_FRAMEBUFFER_COMPLETE);
#endif

	m_FBOs.add(newFBO);

	FrameBufferID id = { m_FBOs.size - 1 };
	return id;
}

void OpenGLLayer::DestroyFrameBuffer(const FrameBufferID id)
{
	GL_CHECK(glDeleteFramebuffers(1, &m_FBOs[id.index].frameBuffer));
	m_FBOs[id.index].frameBuffer = 0;
}

void OpenGLLayer::ClearFrameBuffer(const FrameBufferID frameBuffer,
								   float r, float g, float b,
								   bool clearDepth)
{
	BindFrameBuffer(frameBuffer);

	GL_CHECK(glClearColor(r, g, b, 0.0f));
#if GFX_ENABLE_SCISSOR_TESTING
	GL_CHECK(glDisable(GL_SCISSOR_TEST));
#endif // GFX_ENABLE_SCISSOR_TESTING

#if GFX_ENABLE_DEPTH_TESTING
	if (clearDepth)
	{
		m_currentRasterTests.depthWrite = true;
		GL_CHECK(glDepthMask(GL_TRUE));
		GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
	}
	else
#endif // GFX_ENABLE_DEPTH_TESTING
	{
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
	}
}

void OpenGLLayer::BindFrameBuffer(const FrameBufferID id)
{
	ASSERT(m_FBOs.size > id.index);
	const int frameBufferIndex = id.index;
	if (m_currentFrameBuffer.index == frameBufferIndex)
	{
		return;
	}

	if (frameBufferIndex >= 0)
	{
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_FBOs[frameBufferIndex].frameBuffer));
	}
	else
	{
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	m_currentFrameBuffer.index = frameBufferIndex;
}

void OpenGLLayer::Draw(const DrawArea& drawArea,
					   const RasterTests& rasterTests,
					   const Geometry& geometry,
					   const ShadingParameters& shadingParameters)
{
	BindVertexBuffer(geometry.vertexBuffer);
	BindShader(shadingParameters.shader);
	BindUniforms(shadingParameters.uniforms.elt, shadingParameters.uniforms.size);
	BindFrameBuffer(drawArea.frameBuffer);

	SetRasterizerState(drawArea.viewport,
		shadingParameters.polygonMode,
		rasterTests,
		shadingParameters.blendingMode);

	GL_CHECK(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

	if (geometry.vertexBuffer.index >= 0)
	{
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		const int firstIndexOffset = geometry.firstIndexOffset;
#else // !GFX_ENABLE_VERTEX_BUFFER_OFFSET
		const int firstIndexOffset = 0;
#endif // !GFX_ENABLE_VERTEX_BUFFER_OFFSET

		const VBOInfo& vboInfo = m_VBOs[geometry.vertexBuffer.index];
		if (vboInfo.indexed)
		{
			GL_CHECK(glDrawElementsInstanced(vboInfo.primitiveType, geometry.numberOfIndices, vboInfo.indexType, (void*)firstIndexOffset, shadingParameters.numberOfInstances));
		}
		else
		{
			GL_CHECK(glDrawArraysInstanced(vboInfo.primitiveType, 0, geometry.numberOfIndices, shadingParameters.numberOfInstances));
		}
	}
}

#if GFX_ENABLE_COMPUTE_SHADERS
void OpenGLLayer::Compute(const ShaderID shader,
						  const ComputeParameters& computeParameters,
						  int x, int y, int z)
{
	BindShader(shader);
	BindUniforms(computeParameters.uniforms.elt, computeParameters.uniforms.size);

	GL_CHECK(glDispatchCompute(x, y, z));
}
#endif // GFX_ENABLE_COMPUTE_SHADERS

//void OpenGLLayer::EndFrame()
//{
//#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
//	if (uniformBindingsAvoided != 0 ||
//		uniformBindingsUpdated != 0 ||
//		uniformBindingsSet != 0)
//	{
//		LOG_DEBUG("avoided: %d, updated: %d, set: %d", uniformBindingsAvoided, uniformBindingsUpdated, uniformBindingsSet);
//		uniformBindingsAvoided = 0;
//		uniformBindingsUpdated = 0;
//		uniformBindingsSet = 0;
//	}
//#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING
//}

#endif // GFX_MULTI_API || GFX_OPENGL_ONLY
