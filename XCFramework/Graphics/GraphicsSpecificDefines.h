/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//#define USE_WRAP_MODE

//This macros enables wireframe cube box around physics objects
//#define DEBUG_OBB 1

#if defined(XCGRAPHICS_DX12)

//#define SINGLE_THREAD_RENDER    1
//#define USE_IMMEDIATE_CONTEXT   1

//#define DEBUG_GRAPHICS_PIPELINE 1

#define LOAD_SHADERS_FROM_DATA 1

typedef IDXGISwapChain3                 ID3DSwapChain;
typedef ID3D12Device                    ID3DDevice;
typedef ID3D12GraphicsCommandList       ID3DDeviceContext;
typedef ID3D12CommandList               ID3DCommandList;
typedef ID3D12CommandAllocator          ID3DCommandAllocator;
typedef ID3D12CommandQueue              ID3DCommandQueue;
typedef ID3D12PipelineState             ID3DPipelineState;

typedef ID3D12Resource                  ID3DResource;
typedef ID3D12Resource                  ID3DRenderTargetView;
typedef ID3D12Resource                  ID3DTexture2D;
typedef ID3D12Resource                  ID3DDepthStencilView;
typedef ID3D12Resource                  ID3DShaderResourceView;
typedef ID3D12ShaderReflection          ID3DShaderReflection;

#if defined(USE_D3D_COMPILER)
typedef ID3DBlob                        ID3DVertexShader;
typedef ID3DBlob                        ID3DPixelShader;
#else
typedef UINT8                           ID3DVertexShader;
typedef UINT8                           ID3DPixelShader;
#endif

typedef D3D12_VIEWPORT                  D3D_VIEWPORT;
typedef D3D12_RECT                      D3D_RECT;
typedef D3D12_RASTERIZER_DESC           D3D_RASTERIZER_DESC;
typedef D3D12_SHADER_BUFFER_DESC        D3D_SHADER_BUFFER_DESC;
typedef D3D12_SHADER_INPUT_BIND_DESC    D3D_SHADER_INPUT_BIND_DESC;
typedef D3D12_SIGNATURE_PARAMETER_DESC  D3D_SIGNATURE_PARAMETER_DESC;
typedef D3D12_SHADER_DESC               D3D_SHADER_DESC;
typedef D3D12_INPUT_LAYOUT_DESC         D3D_INPUT_LAYOUT_DESC;

typedef D3D12_CPU_DESCRIPTOR_HANDLE     CPU_DESCRIPTOR_HANDLE;
typedef D3D12_GPU_DESCRIPTOR_HANDLE     GPU_DESCRIPTOR_HANDLE;

#elif defined(XCGRAPHICS_DX11)

//#define LIVE_DRIVE_ENABLED 1
#define LOAD_SHADERS_FROM_DATA 1

typedef IDXGISwapChain                  ID3DSwapChain;
typedef ID3D11Device                    ID3DDevice;
typedef ID3D11DeviceContext             ID3DDeviceContext;
typedef ID3D11CommandList               ID3DCommandList;
typedef ID3D11RenderTargetView          ID3DRenderTargetView;
typedef ID3D11Texture2D                 ID3DTexture2D;
typedef ID3D11ShaderReflection          ID3DShaderReflection;
typedef ID3D11DepthStencilView          ID3DDepthStencilView;
typedef ID3D11DepthStencilState         ID3DDepthStencilState;
typedef ID3D11VertexShader              ID3DVertexShader;
typedef ID3D11PixelShader               ID3DPixelShader;
typedef ID3D11ShaderResourceView        ID3DShaderResourceView;

typedef void*                           ID3DCommandAllocator;
typedef void*                           ID3DCommandQueue;
typedef void*                           ID3DResource;
typedef void*                           ID3DPipelineState;
typedef void*                           ID3D12GraphicsCommandList;

typedef ID3D11Buffer*                   CPU_DESCRIPTOR_HANDLE;
typedef ID3D11Buffer*                   GPU_DESCRIPTOR_HANDLE;

typedef D3D11_TEXTURE2D_DESC            D3D_TEXTURE2D_DESC;
typedef D3D11_VIEWPORT                  D3D_VIEWPORT;
typedef D3D11_RECT                      D3D_RECT;
typedef D3D11_RASTERIZER_DESC           D3D_RASTERIZER_DESC;
typedef D3D11_SHADER_BUFFER_DESC        D3D_SHADER_BUFFER_DESC;
typedef D3D11_SHADER_INPUT_BIND_DESC    D3D_SHADER_INPUT_BIND_DESC;
typedef D3D11_SIGNATURE_PARAMETER_DESC  D3D_SIGNATURE_PARAMETER_DESC;
typedef D3D11_SHADER_DESC               D3D_SHADER_DESC;

typedef struct D3D11_INPUT_LAYOUT_DESC
{
    _Field_size_full_(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs;
    UINT NumElements;
} D3D11_INPUT_LAYOUT_DESC;
typedef D3D11_INPUT_LAYOUT_DESC         D3D_INPUT_LAYOUT_DESC;

#elif defined(XCGRAPHICS_GL)

//#define USE_IMMEDIATE_CONTEXT   1
#define SINGLE_THREAD_RENDER    1
#define DEBUG_GRAPHICS_PIPELINE 1

struct ViewPort
{
    f32 TopLeftX;
    f32 TopLeftY;
    f32 Width;
    f32 Height;
    f32 MinDepth;
    f32 MaxDepth;
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
typedef void*      ID3DShaderResourceView;
typedef void*      ID3DPipelineState;
typedef void*      ID3D12GraphicsCommandList;
typedef void*      ID3DDepthStencilView;
typedef void*      ID3DTexture2D;
typedef void*      ID3DShaderResourceView;
typedef void*      ID3DRenderTargetView;
typedef void*      D3DConstantBuffer;

#endif