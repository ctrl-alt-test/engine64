#pragma once

#include "ResourceID.hpp"
#include "TextureFormat.hpp"
#include "VertexAttribute.hpp"

namespace Gfx
{
#if GFX_ENABLE_COMPUTE_SHADERS
	struct ComputeParameters;
#endif // GFX_ENABLE_COMPUTE_SHADERS
	struct DrawArea;
	struct FrameBufferID;
	struct Geometry;
	struct RasterTests;
	struct ShaderID;
#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
	struct StorageBufferID;
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT
	struct ShaderStage;
	struct ShadingParameters;
	struct TextureID;
	struct VertexAttribute;
	struct VertexBufferID;

#ifdef GFX_MULTI_API
	/// <summary>
	/// A minimalist graphics API abstraction.
	/// </summary>
	class IGraphicLayer
	{
	public:
		virtual ~IGraphicLayer() {};

		/// <summary>
		/// Initializes the graphics API.
		/// </summary>
		/// <returns>True if the graphics API was initialized properly, false otherwise.</returns>
		virtual bool				CreateRenderingContext() = 0;
		virtual void				DestroyRenderingContext() = 0;

		/// <summary>
		/// Creates an uninitialized vertex buffer.
		/// </summary>
		virtual VertexBufferID		CreateVertexBuffer() = 0;
		virtual void				DestroyVertexBuffer(const VertexBufferID id) = 0;

		/// <summary>
		/// Sets the vertex data of an existing vertex buffer.
		/// </summary>
		///
		/// <param name="vertexAttributes">List of vertex attributes
		///     included in the data.</param>
		/// <param name="primitiveType">Type of primitive to assemble
		///     (triangles, quads, etc.).</param>
		/// <param name="numberOfAttributes">Number of vertex
		///     attributes of the list.</param>
		/// <param name="stride">Offset in bytes between one vertex
		///     and the next.</param>
		/// <param name="vertexDataSize">Size of the vertex data in
		///     bytes.</param>
		/// <param name="vertexData">Raw vertex data.</param>
		/// <param name="indexDataSize">Size of the index data in
		///     bytes. The vertex buffer is assumed to be non indexed
		///     if the value is 0.</param>
		/// <param name="indexData">Raw index data.</param>
		/// <param name="indexType">Type of the indices.</param>
		virtual void				LoadVertexBuffer(const VertexBufferID id,
													 PrimitiveType::Enum primitiveType,
													 const VertexAttribute* vertexAttributes,
													 int numberOfAttributes, int stride,
													 int vertexDataSize, const void* vertexData,
													 int indexDataSize, const void* indexData,
													 VertexIndexType::Enum indexType) = 0;

		/// <summary>
		/// Creates an uninitialized texture.
		/// </summary>
		virtual TextureID			CreateTexture() = 0;
		virtual void				DestroyTexture(const TextureID id) = 0;

		/// <summary>
		/// Sets the texture data of an existing texture.
		/// </summary>
		///
		/// <param name="textureType">2D texture or cube map.</param>
		/// <param name="textureFormat">Packing format to use for data.</param>
		/// <param name="side">In case of a cube map, face of the cube from 0 to 5.</param>
		/// <param name="lodLevel">-1 to automatically generate mipmap; mipmap level otherwise.</param>
		virtual void				LoadTexture(const TextureID id,
												int width, int height,
												TextureType::Enum textureType,
												TextureFormat::Enum textureFormat,
												int side, int lodLevel,
												const void* data,
												const TextureSampling& textureSampling) = 0;

		/// <summary>
		/// Generates mipmaps for the texture.
		/// Note: this is supported in OpenGL and DirectX until 11, but not in Vulkan and DirectX 12.
		/// </summary>
		virtual void				GenerateMipMaps(const TextureID id) = 0;

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		/// <summary>
		/// Creates an uninitialized uniform buffer.
		/// </summary>
		virtual UniformBufferID		CreateUniformBuffer() = 0;
		virtual void				DestroyUniformBuffer(const UniformBufferID id) = 0;

		/// <summary>
		/// Sets the data of an existing uniform buffer.
		/// </summary>
		virtual void				LoadUniformBuffer(const UniformBufferID id,
													  int size,
													  const void* data) = 0;
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

#if GFX_ENABLE_STORAGE_BUFFER_OBJECT
		/// <summary>
		/// Creates an uninitialized storage buffer.
		/// </summary>
		virtual StorageBufferID		CreateStorageBuffer() = 0;
		virtual void				DestroyStorageBuffer(const StorageBufferID id) = 0;

		/// <summary>
		/// Sets the data of an existing storage buffer.
		/// </summary>
		virtual void				LoadStorageBuffer(const StorageBufferID id,
													  size_t size,
													  const void* data) = 0;

		/// <summary>
		/// Reads the data from an existing storage buffer.
		/// </summary>
		virtual void				ReadStorageBuffer(const StorageBufferID id,
													  size_t size,
													  void* dest) = 0;
#endif // GFX_ENABLE_STORAGE_BUFFER_OBJECT

		/// <summary>
		/// Creates an uninitialized shader program.
		/// </summary>
		virtual ShaderID			CreateShader() = 0;
		virtual void				DestroyShader(const ShaderID id) = 0;

		/// <summary>
		/// Compiles and sets the code of an existing shader program.
		/// </summary>
		virtual void				LoadShader(const ShaderID id,
											   const ShaderStage* shaderStages,
											   int numberOfStages) = 0;

		/// <summary>
		/// Creates a frame buffer.
		/// </summary>
		virtual FrameBufferID		CreateFrameBuffer(const TextureID* textures,
													  int numberOfTextures,
													  int side, int lodLevel) = 0;
		virtual void				DestroyFrameBuffer(const FrameBufferID id) = 0;

		/// <summary>
		/// Clear the frame buffer.
		/// </summary>
		virtual void				ClearFrameBuffer(const FrameBufferID id,
													 float r,
													 float g,
													 float b,
													 bool clearDepth) = 0;

		/// <summary>
		/// Draws one mesh or more, using the given parameters.
		/// </summary>
		virtual void				Draw(const DrawArea& drawArea,
										 const RasterTests& rasterTests,
										 const Geometry& geometry,
										 const ShadingParameters& shadingParameters) = 0;

#if GFX_ENABLE_COMPUTE_SHADERS
		/// <summary>
		/// Dispatches a compute shader for execution on the GPU.
		/// The compute shader operates on data specified within the
		/// ComputeParameters, using a number of work groups specified by the
		/// x, y, and z dimensions.
		/// </summary>
		virtual void				Compute(const ShaderID shader,
											const ComputeParameters& computeParameters,
											int x, int y = 1, int z = 1) = 0;
#endif // GFX_ENABLE_COMPUTE_SHADERS

		virtual void				EndFrame() = 0;
	};
#endif // GFX_MULTI_API

// FIXME: this should not be in this file. IGraphicsLayer should stay
// clear of any dependency over existing implementations.
#ifdef GFX_DIRECTX_ONLY
#define DirectXLayer IGraphicLayer
#endif // GFX_DIRECTX_ONLY

#ifdef GFX_OPENGL_ONLY
#define OpenGLLayer IGraphicLayer
#endif // GFX_OPENGL_ONLY
}
