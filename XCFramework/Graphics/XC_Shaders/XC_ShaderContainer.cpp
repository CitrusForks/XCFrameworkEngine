/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/XC_Shaders/IShader.h"
#include "Graphics/XC_Shaders/XC_ShaderContainer.h"
#include "Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Graphics/XC_Shaders/XC_VertexShaderLayout.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Graphics/SharedDescriptorHeap.h"

#if !defined(LOAD_SHADERS_FROM_DATA)
#include "Graphics/XC_Shaders/src/DefaultShader.h"
#include "Graphics/XC_Shaders/src/SolidColor.h"
#include "Graphics/XC_Shaders/src/LightTexture.h"
#include "Graphics/XC_Shaders/src/TerrainMultiTex.h"
#include "Graphics/XC_Shaders/src/CubeMap.h"
#include "Graphics/XC_Shaders/src/SkinnedCharacter.h"
#endif

#include "Assets/Packages/PackageConsts.h"

XC_ShaderContainer::XC_ShaderContainer(ID3DDevice& device)
    : m_device(device)
{
}

XC_ShaderContainer::~XC_ShaderContainer(void)
{
}

void XC_ShaderContainer::Init()
{
    m_globalShaderData.m_camera.Init();

    m_Shaders.resize(ShaderType_Max);

#if defined(XCGRAPHICS_DX12) || defined(XCGRAPHICS_DX11)
    ZeroMemory(&m_rasterizerDesc, sizeof(D3D_RASTERIZER_DESC));
    ZeroMemory(&m_samplerDesc, sizeof(m_samplerDesc));
#endif

    LoadShaders();
    LoadRasterizers();
    LoadSamplers();
}

void XC_ShaderContainer::Destroy()
{
    for (u32 shaderIndex = 0; shaderIndex < ShaderType_Max; shaderIndex++)
    {
        if (m_Shaders[(ShaderType)shaderIndex] != nullptr)
        {
            m_Shaders[(ShaderType)shaderIndex]->Destroy();
            XCDELETE(m_Shaders[(ShaderType)shaderIndex]);
        }
    }
    m_Shaders.clear();
}

void XC_ShaderContainer::LoadShaders()
{
    IShader* binShader;

    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadFile(SHADER_SCHEMA_FILEPATH, m_fbBuffer);
    fbSystem.ParseAndLoadFile(SHADER_DATA_FILEPATH, m_fbBuffer);

#if defined(LOAD_SHADERS_FROM_DATA)
    auto FBShadersRoot = GetFBRootShader(m_fbBuffer.GetBufferFromLoadedData());

    for (auto it = FBShadersRoot->FBShaders()->begin(); it != FBShadersRoot->FBShaders()->end(); ++it)
    {
        binShader = XCNEW(XCShaderHandle)(m_device);
        binShader->Load((void*) *it);
        m_Shaders[it->ShaderUsage()] = binShader;
    }
#else
    for (u32 shaderIndex = 0; shaderIndex < ShaderType_Max; shaderIndex++)
    {
        switch (shaderIndex)
        {
            case ShaderType_DEFAULT:
                {
                    binShader = new DefaultShader(m_device);

                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_DEFAULT] = binShader;

                    break;
                }

            case ShaderType_SolidColor:
                {
                    binShader = new SolidColorShader(m_device);

                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_SolidColor] = binShader;

                    break;
                }
            case ShaderType_LightTexture:
                {
                    binShader = new LightTextureShader(m_device);

                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_LightTexture] = binShader;
                    break;
                }

            case ShaderType_REFELECTED_LIGHTTEXTURE:
                {
                    binShader = new XCShaderHandle(m_device);
                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_REFELECTED_LIGHTTEXTURE] = binShader;
                    break;
                }

            case ShaderType_TerrainMultiTexture:
                {
                    binShader = new TerrainMultiTex(m_device);

                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_TerrainMultiTexture] = binShader;
                    break;
                }

            case ShaderType_SimpleCubeMap:
                {
                    binShader = new CubeMapShader(m_device);

                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_SimpleCubeMap] = binShader;
                    break;
                }

            case ShaderType_SkinnedCharacter:
                {
                    binShader = new SkinnedCharacterShader(m_device);

                    binShader->LoadShader();
                    binShader->CreateConstantBuffers();
                    m_Shaders[ShaderType_SkinnedCharacter] = binShader;
                    break;
                }

            case ShaderType_Max:
                break;

            default: 
                break;
        }
    }
#endif
}

void XC_ShaderContainer::LoadRasterizers()
{
#if defined(XCGRAPHICS_DX11)
    m_rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    m_rasterizerDesc.CullMode = D3D11_CULL_BACK;

    m_device.CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerStates[RasterType_FillWireframe]);

    m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    m_rasterizerDesc.CullMode = D3D11_CULL_BACK;

    m_device.CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerStates[RasterType_FillSolid]);
#endif
}

void XC_ShaderContainer::LoadSamplers()
{
#if defined(XCGRAPHICS_DX11)
    m_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    m_samplerDesc.MinLOD = 0;
    m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ValidateResult(m_device.CreateSamplerState(&m_samplerDesc, &m_SamplerLinear));
#elif defined(XCGRAPHICS_DX12)

    m_samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    m_samplerDesc.MinLOD = 0;
    m_samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_device.CreateSampler(&m_samplerDesc, heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).m_heapDesc->GetCPUDescriptorHandleForHeapStart());
#endif
}

void XC_ShaderContainer::ApplyShader(ID3DDeviceContext& context, ShaderType _ShaderType, RasterType rasterType)
{
    m_Shaders[_ShaderType]->ApplyShader(context, rasterType);
#if defined(XCGRAPHICS_DX11)
    context.RSSetState(m_rasterizerStates[rasterType]);
    ((XCShaderHandle*)m_Shaders[_ShaderType])->SetSampler("samLinear", context, m_SamplerLinear);
#elif defined(XCGRAPHICS_DX12)
    //Set sampler
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    ((XCShaderHandle*)m_Shaders[_ShaderType])->SetSampler("samLinear", context, heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).m_heapDesc->GetGPUDescriptorHandleForHeapStart());
#endif
}

IShader* XC_ShaderContainer::GetShader(ShaderType _type)
{
    return m_Shaders[_type];
}

void XC_ShaderContainer::ClearShaderAndRenderStates(ID3DDeviceContext& context)
{
    for (u32 shaderIndex = 0; shaderIndex < ShaderType_Max; shaderIndex++)
    {
        if (m_Shaders[(ShaderType)shaderIndex] != nullptr)
        {
            m_Shaders[(ShaderType)shaderIndex]->Reset(context);
        }
    }
}