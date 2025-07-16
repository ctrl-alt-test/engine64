#pragma once

#ifdef _WIN32

#include "engine/container/Array.hpp" // FIXME: remove dependency, ideally Gfx should not have dependency over Engine
#include "gfx/IGraphicLayer.hpp"
#include <d3d9.h>

#if GFX_DIRECTX_ONLY || GFX_MULTI_API

namespace Gfx
{
	/// <summary>
	/// DirectX implementation of an IGraphicLayer (work in progress).
	/// </summary>
	class DirectXLayer
#ifdef GFX_MULTI_API
		: public IGraphicLayer
#endif // GFX_MULTI_API
	{
	public:
		DirectXLayer();

		bool					CreateRenderingContext(const HWND& hWnd);
		void					DestroyRenderingContext();

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

#if GFX_ENABLE_UNIFORM_BUFFER_OBJECT
		UniformBufferID			CreateUniformBuffer();
		void					DestroyUniformBuffer(const UniformBufferID id) = 0;
		void					LoadUniformBuffer(const UniformBufferID id,
												  int size,
												  const void* data);
#endif // GFX_ENABLE_UNIFORM_BUFFER_OBJECT

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
		void					EndFrame();

	private:
		struct VertexBufferInfo
		{
			IDirect3DVertexBuffer9*	buffer;
			DWORD					fvf;
			int						stride;
			int						primitiveCount;
		};

		struct TextureInfo
		{
			IDirect3DTexture9*		buffer;
		};

	private:
		LPDIRECT3D9				m_d3d;
		LPDIRECT3DDEVICE9		m_device;

		Container::Array<VertexBufferInfo>	m_vertexBuffers;
		Container::Array<TextureInfo>		m_textures;
	};
}

#endif // GFX_DIRECTX_ONLY || GFX_MULTI_API

#endif // _WIN32
