/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//#define USE_WRAP_MODE

#if defined(XCGRAPHICS_DX12)

//#define SINGLE_THREAD_RENDER    1
//#define USE_IMMEDIATE_CONTEXT   1

//#define DEBUG_GRAPHICS_PIPELINE 1

#define LOAD_SHADERS_FROM_DATA 1

typedef ID3D12Device                    ID3DDevice;
typedef ID3D12GraphicsCommandList       ID3DDeviceContext;
typedef ID3D12CommandList               ID3DCommandList;
typedef D3D12_VIEWPORT                  D3D_VIEWPORT;
typedef D3D12_RECT                      D3D_RECT;
typedef D3D12_RASTERIZER_DESC           D3D_RASTERIZER_DESC;
#if defined(USE_D3D_COMPILER)
typedef ID3DBlob                        ID3DVertexShader;
typedef ID3DBlob                        ID3DPixelShader;
#else
typedef UINT8                           ID3DVertexShader;
typedef UINT8                           ID3DPixelShader;
#endif
typedef ID3D12CommandAllocator          ID3DCommandAllocator;
typedef ID3D12CommandQueue              ID3DCommandQueue;
typedef ID3D12Resource                  ID3DResource;
typedef D3D12_CPU_DESCRIPTOR_HANDLE     CPU_DESCRIPTOR_HANDLE;
//typedef ID3D12Resource                D3DShaderResourceView;
#elif defined(XCGRAPHICS_DX11)

#define LIVE_DRIVE_ENABLED 1

typedef ID3D11Device                ID3DDevice;
typedef ID3D11DeviceContext         ID3DDeviceContext;
typedef ID3D11CommandList           ID3DCommandList;
typedef void*                       ID3DCommandAllocator;
typedef void*                       ID3DCommandQueue;
typedef void*                       ID3DResource;
typedef void*                       ID3D12PipelineState;
typedef void*                       ID3D12GraphicsCommandList;
typedef int                         CPU_DESCRIPTOR_HANDLE;
typedef ID3D11RenderTargetView      ID3DRenderTargetView;
typedef ID3D11Texture2D             ID3DTexture2D;
typedef D3D11_TEXTURE2D_DESC        D3D_TEXTURE2D_DESC;
typedef ID3D11DepthStencilView      ID3DDepthStencilView;
typedef ID3D11DepthStencilState     ID3DDepthStencilState;
typedef D3D11_VIEWPORT              D3D_VIEWPORT;
typedef D3D11_RECT                  D3D_RECT;
typedef D3D11_RASTERIZER_DESC       D3D_RASTERIZER_DESC;
typedef ID3D11VertexShader          ID3DVertexShader;
typedef ID3D11PixelShader           ID3DPixelShader;
typedef ID3D11ShaderResourceView    ID3DShaderResourceView;
typedef ID3D11ShaderResourceView    D3DShaderResourceView;
typedef ID3D11ShaderResourceView    D3DShaderResource;
typedef ID3D11Buffer                D3DConstantBuffer;
#elif defined(XCGRAPHICS_GNM)

//#define USE_IMMEDIATE_CONTEXT   1
//#define SINGLE_THREAD_RENDER    1
#define DEBUG_GRAPHICS_PIPELINE 1

typedef sce::Gnmx::GfxContext      ID3DDevice;
typedef sce::Gnmx::GfxContext      ID3DDeviceContext;
typedef void*      ID3DCommandList;
typedef void*      D3D_VIEWPORT;
typedef void*      D3D_RECT;
typedef void*      D3D_RASTERIZER_DESC;
typedef sce::Gnmx::VsShader   ID3DVertexShader;
typedef sce::Gnmx::PsShader   ID3DPixelShader;
typedef void*      ID3DCommandAllocator;
typedef void*      ID3DCommandQueue;
typedef void*      ID3DResource;
typedef void       CPU_DESCRIPTOR_HANDLE;
typedef void*      ID3D11InputLayout;
typedef void*      ID3D11Buffer;
typedef sce::Gnm::Texture      D3DShaderResourceView;
typedef void*      ID3D12PipelineState;
typedef sce::Gnmx::GfxContext      ID3D12GraphicsCommandList;
typedef sce::Gnm::DepthRenderTarget ID3DDepthStencilView;
typedef void*      HWND;
typedef void*      HINSTANCE;
typedef void*      ID3DTexture2D;
typedef void*      ID3DShaderResourceView;
typedef sce::Gnm::RenderTarget      ID3DRenderTargetView;
typedef void*      ID3DBlob;
#elif defined(XCGRAPHICS_GL)

//#define USE_IMMEDIATE_CONTEXT   1
#define SINGLE_THREAD_RENDER    1
#define DEBUG_GRAPHICS_PIPELINE 1

struct ViewPort
{
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
};

struct Rect
{
    long    left;
    long    top;
    long    right;
    long    bottom;
};

typedef void*      ID3DDevice;
typedef void*      ID3DDeviceContext;
typedef void*      ID3DCommandList;
typedef ViewPort   D3D_VIEWPORT;
typedef Rect       D3D_RECT;
typedef void*      D3D_RASTERIZER_DESC;
typedef void*      ID3DVertexShader;
typedef void*      ID3DPixelShader;
typedef void*      ID3DCommandAllocator;
typedef void*      ID3DCommandQueue;
typedef void*      ID3DResource;
typedef void*      CPU_DESCRIPTOR_HANDLE;
typedef void*      ID3D11InputLayout;
typedef void*      ID3D11Buffer;
typedef void*      D3DShaderResourceView;
typedef void*      ID3D12PipelineState;
typedef void*      ID3D12GraphicsCommandList;
typedef void*      ID3DDepthStencilView;
typedef void*      ID3DTexture2D;
typedef void*      ID3DShaderResourceView;
typedef void*      ID3DRenderTargetView;
typedef void*      D3DConstantBuffer;
#endif