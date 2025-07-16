#ifdef _WIN32

#include "DirectXLayer.hpp"
#include "engine/debug/Assert.hpp"
#include "engine/debug/Debug.hpp" // FIXME: remove dependency, ideally Gfx should not have dependency over Engine
#include "gfx/Geometry.hpp"
#include "gfx/ResourceID.hpp"
#include "gfx/ShadingParameters.hpp"
#if DEBUG
#include <sstream>
#endif

#if GFX_MULTI_API || GFX_DIRECTX_ONLY

using namespace Gfx;

#define VERTEX_SIZE (8 * sizeof(float))
#define PRIMITIVE_SIZE (4 * VERTEX_SIZE)

#if DEBUG
#define ENABLE_DIRECTX_ERROR_CHECK	1
#endif // DEBUG

#if ENABLE_DIRECTX_ERROR_CHECK

#define DX_CHECK(call, result) 											\
	do																	\
	{																	\
		if (FAILED(result))												\
		{																\
			std::ostringstream oss;										\
			oss << call << " returned: " << result;						\
			std::string message = oss.str();							\
			throw new std::exception(message.c_str());					\
		}																\
	} while (0)

#else // !ENABLE_DIRECTX_ERROR_CHECK

#define DX_CHECK(call, result)

#endif // !ENABLE_DIRECTX_ERROR_CHECK



DirectXLayer::DirectXLayer(): m_d3d(nullptr), m_device(nullptr)
{
}

bool DirectXLayer::CreateRenderingContext(const HWND& hWnd)
{
	ASSERT(m_d3d == nullptr);
	m_d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if (!m_d3d)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS params;
	ZeroMemory(&params, sizeof(params));
	params.AutoDepthStencilFormat = D3DFMT_D24X8;
	params.BackBufferCount = 1;
	params.BackBufferFormat = D3DFMT_X8R8G8B8;
	params.EnableAutoDepthStencil = TRUE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.Windowed = TRUE;
	params.hDeviceWindow = hWnd;

	ASSERT(m_device == nullptr);
	if (m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL,
							hWnd,
							D3DCREATE_HARDWARE_VERTEXPROCESSING,
							&params,
							&m_device))
	{
		return false;
	}

	return true;
}

void DirectXLayer::DestroyRenderingContext()
{
	ASSERT(m_device != nullptr);
	m_device->Release();
	m_device = nullptr;

	ASSERT(m_d3d != nullptr);
	m_d3d->Release();
	m_d3d = nullptr;
}

VertexBufferID DirectXLayer::CreateVertexBuffer()
{
	VertexBufferInfo newVertexBuffer;
	newVertexBuffer.buffer = nullptr;

	// Internal resource indexing
	m_vertexBuffers.add(newVertexBuffer);
	VertexBufferID id = { m_vertexBuffers.size - 1 };
	return id;
}

void DirectXLayer::DestroyVertexBuffer(const VertexBufferID id)
{
	ASSERT(m_vertexBuffers.size > id.index);
	IDirect3DVertexBuffer9* buffer = m_vertexBuffers[id.index].buffer = 0;
	m_vertexBuffers[id.index].buffer = nullptr;
	m_vertexBuffers[id.index].primitiveCount = 0;

	if (buffer != nullptr)
	{
		buffer->Release();
	}
}

void DirectXLayer::LoadVertexBuffer(const VertexBufferID id,
									PrimitiveType::Enum primitiveType,
									const VertexAttribute* vertexAttributes,
									int numberOfAttributes, int stride,
									int vertexDataSize, const void* vertexData,
									int indexDataSize, const void* indexData,
									VertexIndexType::Enum indexType)
{
	ASSERT(m_vertexBuffers.size > id.index);
	VertexBufferInfo vertexBuffer = m_vertexBuffers[id.index];
	vertexBuffer.buffer = nullptr;

	//
	// FIXME : have the layout depend on the vertexAttributes.
	// Have a custom init function to agree on the name of the attributes, then identify them here.
	//
	vertexBuffer.fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	vertexBuffer.stride = stride;
	vertexBuffer.primitiveCount = 12;

	// Creation of the buffer
	HRESULT createResult = m_device->CreateVertexBuffer(vertexDataSize, //4 * sizeof(CUSTOMVERTEX),
														0,
														vertexBuffer.fvf,
														D3DPOOL_MANAGED,
														&vertexBuffer.buffer,
														nullptr);
	DX_CHECK("CreateVertexBuffer", createResult);

	// Copy of the vertex data
	void* p;
	HRESULT lockResult = vertexBuffer.buffer->Lock(0, 0, &p, 0);
	DX_CHECK("Lock", lockResult);

	memcpy(p, vertexData, vertexDataSize);
	vertexBuffer.buffer->Unlock();
}

TextureID DirectXLayer::CreateTexture()
{
	TextureInfo newTexture;
	newTexture.buffer = nullptr;

	// Internal resource indexing
	m_textures.add(newTexture);
	TextureID id = { m_textures.size - 1 };
	return id;
}

void DirectXLayer::DestroyTexture(const TextureID id)
{
	IDirect3DTexture9* buffer = m_textures[id.index].buffer = 0;
	m_textures[id.index].buffer = nullptr;

	if (buffer != nullptr)
	{
		buffer->Release();
	}
}

void DirectXLayer::LoadTexture(const TextureID id,
							   int width, int height,
							   TextureType::Enum textureType,
							   TextureFormat::Enum textureFormat,
							   int side, int lodLevel,
							   const void* data,
							   const TextureSampling& textureSampling)
{
	ASSERT(m_textures.size > id.index);
	TextureInfo texture = m_textures[id.index];
	texture.buffer = nullptr;

	// Creation of the texture
	HRESULT createResult = m_device->CreateTexture(width, height, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8,
												   D3DPOOL_MANAGED, &texture.buffer, nullptr);
	DX_CHECK("CreateTexture", createResult);

	// Copy of the texture data
	D3DLOCKED_RECT d3drc;
	HRESULT lockResult = texture.buffer->LockRect(0, &d3drc, nullptr, 0);
	DX_CHECK("LockRect", lockResult);

	memcpy(d3drc.pBits, data, width * height * sizeof(unsigned int));
	texture.buffer->UnlockRect(0);
}

ShaderID DirectXLayer::CreateShader()
{
	NOT_IMPLEMENTED;

	ShaderID id = { -1 };
	return id;
}

void DirectXLayer::DestroyShader(const ShaderID id)
{
	NOT_IMPLEMENTED;
}

void DirectXLayer::LoadShader(const ShaderID id,
							  const ShaderStage* shaderStages,
							  int numberOfStages)
{
	NOT_IMPLEMENTED;
}

FrameBufferID DirectXLayer::CreateFrameBuffer(const TextureID* textures,
											  int numberOfTextures,
											  int side, int lodLevel)
{
	NOT_IMPLEMENTED;

	FrameBufferID id = { -1 };
	return id;
}

void DirectXLayer::DestroyFrameBuffer(const FrameBufferID id)
{
	NOT_IMPLEMENTED;
}

void DirectXLayer::ClearFrameBuffer(const FrameBufferID frameBuffer,
									float r, float g, float b,
									bool clearDepth)
{
	const D3DCOLOR clearColor = D3DCOLOR_XRGB((int)(255 * r), (int)(255 * g), (int)(255 * b));
	m_device->Clear(0, nullptr, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
	if (clearDepth)
	{
		m_device->Clear(0, nullptr, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	}

	HRESULT beginResult = m_device->BeginScene();
	DX_CHECK("BeginScene", beginResult);
}

/*
void DirectXLayer::SetCamera(int width, int height, float fov,
							 float yRotation, float distance)
{
	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix,
							   D3DXToRadian(fov), (float)width / (float)height, 1.0f, 2.f * distance);
	m_device->SetTransform(D3DTS_PROJECTION, &projectionMatrix);

	D3DXMATRIX t;  D3DXMatrixTranslation(&t, 0.0f, 0.0f, distance);
	D3DXMATRIX rx; D3DXMatrixRotationX(&rx, D3DXToRadian(-20.0f));
	D3DXMATRIX ry; D3DXMatrixRotationY(&ry, D3DXToRadian(-yRotation));
	m_device->SetTransform(D3DTS_WORLD, &(ry * rx * t));

	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
}
*/

void DirectXLayer::Draw(const DrawArea& drawArea,
						const RasterTests& rasterTests,
						const Geometry& geometry,
						const ShadingParameters& shadingParameters)
{
	if (geometry.vertexBuffer.index >= 0)
	{
		const VertexBufferInfo& vertexBuffer = m_vertexBuffers[geometry.vertexBuffer.index];

		if (shadingParameters.numberOfInstances <= 1)
		{
// 			if (vertexBuffer.indexBuffer != nullptr)
			{
				m_device->SetFVF(vertexBuffer.fvf);
				m_device->SetStreamSource(0, vertexBuffer.buffer, 0, vertexBuffer.stride);
				m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vertexBuffer.primitiveCount);
			}
// 			else
// 			{
// 				m_device->SetFVF(vertexBuffer.fvf);
// 				m_device->SetStreamSource(0, vertexBuffer.buffer, 0, vertexBuffer.stride);
// 				m_device->SetIndices(vertexBuffer.indexBuffer);
// 				m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
// 											   vertexBuffer.vertexCount, 0, vertexBuffer.primitiveCount);
// 			}
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
}

void DirectXLayer::EndFrame()
{
	HRESULT endResult = m_device->EndScene();
	DX_CHECK("EndScene", endResult);

	HRESULT presentResult = m_device->Present(nullptr, nullptr, nullptr, nullptr);
	DX_CHECK("Present", presentResult);
}

#endif // GFX_MULTI_API || GFX_DIRECTX_ONLY

#endif // _WIN32
