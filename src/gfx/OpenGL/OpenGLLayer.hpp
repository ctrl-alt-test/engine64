#pragma once

#include "engine/container/Array.hxx"
// FIXME: ideally Gfx should not have dependency over Engine.
#include "gfx/BlendingMode.hpp"
#include "gfx/DrawArea.hpp"
#include "gfx/IGraphicLayer.hpp"
#include "gfx/PolygonMode.hpp"
#include "gfx/RasterTests.hpp"
#include <GL/gl.h>

#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
#include "engine/container/HashTable.hpp"
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING

#if GFX_OPENGL_ONLY || GFX_MULTI_API

namespace Gfx
{
	struct BlendingMode;
	struct Uniform;

	/// <summary>
	/// OpenGL implementation of an IGraphicLayer.
	/// </summary>
	class OpenGLLayer
#ifdef GFX_MULTI_API
		: public IGraphicLayer
#endif // GFX_MULTI_API
	{
	public:
		bool					CreateRenderingContext();
		void					DestroyRenderingContext() {}

		VertexBufferID			CreateVertexBuffer();
		void					DestroyVertexBuffer(const VertexBufferID id);
		void					LoadVertexBuffer(const VertexBufferID id,
												 PrimitiveType::Enum primitiveType,
												 const VertexAttribute* vertexAttributes,
												 int numberOfAttributes, int stride,
												 int vertexDataSize, const void* vertexData,
												 int indexDataSize, const void* indexData,
												 VertexIndexType::Enum indexType);

		TextureID				CreateTexture();
		void					DestroyTexture(const TextureID id);
		void					LoadTexture(const TextureID id,
											int width, int height,
											TextureType::Enum textureType,
											TextureFormat::Enum textureFormat,
											int side, int lodLevel,
											const void* data,
											const TextureSampling& textureSampling);
		void					GenerateMipMaps(const TextureID id);

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		UniformBufferID			CreateUniformBuffer();
		void					DestroyUniformBuffer(const UniformBufferID id);
		void					LoadUniformBuffer(const UniformBufferID id,
												  int size,
												  const void* data);
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		StorageBufferID			CreateStorageBuffer();
		void					DestroyStorageBuffer(const StorageBufferID id);
		void					LoadStorageBuffer(const StorageBufferID id,
												  size_t size,
												  const void* data);
		void					ReadStorageBuffer(const StorageBufferID id,
												  size_t size,
												  void* dest);
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

		ShaderID				CreateShader();
		void					DestroyShader(const ShaderID id);
		void					LoadShader(const ShaderID id,
										   const ShaderStage* shaderStages,
										   int numberOfStages);

		FrameBufferID			CreateFrameBuffer(const TextureID* textures,
												  int numberOfTextures,
												  int side, int lodLevel);
		void					DestroyFrameBuffer(const FrameBufferID id);
		void					ClearFrameBuffer(const FrameBufferID frameBuffer,
												 float r, float g, float b,
												 bool clearDepth);

		void					Draw(const DrawArea& drawArea,
									 const RasterTests& rasterTests,
									 const Geometry& geometry,
									 const ShadingParameters& shadingParameters);
#if GFX_ENABLE_COMPUTE_SHADERS
		void					Compute(const ShaderID shader,
										const ComputeParameters& computeParameters,
										int x, int y = 1, int z = 1);
#endif // GFX_ENABLE_COMPUTE_SHADERS
		void					EndFrame() {}

	private:
		// These methods are private so from the outside the API looks stateless.
		void					SetRasterizerState(const Viewport& viewport,
												   const PolygonMode::Enum polygonMode,
												   const RasterTests& rasterTests,
												   const BlendingMode& blendingMode);
		void					BindFrameBuffer(const FrameBufferID framebuffer);
		void					BindShader(const ShaderID id);
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		void					BindStorageBuffer(const StorageBufferID id,
												  GLuint program,
												  int slot,
												  const char* name,
												  bool writing);
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
		void					BindTexture(const TextureID id, int slot);
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		void					BindUniformBuffer(const UniformBufferID id,
												  GLuint program,
												  int slot,
												  const char* name);
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		void					BindUniforms(const Uniform* uniforms, int numberOfUniforms);
		void					BindVertexBuffer(const VertexBufferID id);

	private:
		struct FBOInfo
		{
			GLuint	frameBuffer;
			int		width;
			int		height;
		};
		Container::Array<FBOInfo>	m_FBOs;

		struct ShaderInfo
		{
			GLuint	program;
			GLuint	shaders[2];
#if GFX_SKIP_REDUNDANT_UNIFORM_BINDING
#if GFX_HASH_UNIFORM_VALUE
			Container::HashTable<const char*, unsigned int> currentUniforms;
#else // !GFX_HASH_UNIFORM_VALUE
			Container::HashTable<const char*, Uniform> currentUniforms;
#endif // !GFX_HASH_UNIFORM_VALUE
#endif // GFX_SKIP_REDUNDANT_UNIFORM_BINDING
		};
		Container::Array<ShaderInfo> m_shaders;

#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		struct SSBOInfo
		{
			GLuint	storageBuffer;
			int		size;
			bool	writing;
		};
		Container::Array<SSBOInfo>	m_SSBOs;
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

		struct TextureInfo
		{
			GLuint	texture;
			int		width;
			int		height;
			GLenum	type;
			GLenum	format;
		};
		Container::Array<TextureInfo> m_textures;

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		struct UBOInfo
		{
			GLuint	uniformBuffer;
			int		size;
		};
		Container::Array<UBOInfo>	m_UBOs;
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

		struct VBOInfo
		{
			const VertexAttribute* vertexAttributes;
			int		numberOfAttributes;
			int		stride;

			GLenum	primitiveType;
			GLuint	vertexBuffer;
			GLuint	indexBuffer;
			GLenum	indexType;
			bool	indexed;
		};
		Container::Array<VBOInfo>	m_VBOs;

		Viewport					m_currentViewport;
		RasterTests					m_currentRasterTests;
		BlendingMode				m_currentBlendingMode;
		PolygonMode::Enum			m_currentPolygonMode;

		FrameBufferID				m_currentFrameBuffer;
		ShaderID					m_currentShader;
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		StorageBufferID				m_currentSSBOs[GFX_MAX_STORAGE_BUFFER_BINDINGS];
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
		TextureID					m_currentTextures[GFX_MAX_TEXTURE_SLOTS];
#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		UniformBufferID				m_currentUBOs[GFX_MAX_UNIFORM_BUFFER_SLOTS];
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		VertexBufferID				m_currentVBO;

		bool						m_enabledVertexAttributes[GFX_MAX_VERTEX_ATTRIBUTES];
	};
}

#endif // GFX_OPENGL_ONLY || GFX_MULTI_API
