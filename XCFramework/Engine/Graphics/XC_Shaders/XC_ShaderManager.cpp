/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"

#if !defined(LOAD_SHADERS_FROM_DATA)
#include "Engine/Graphics/XC_Shaders/src/DefaultShader.h"
#include "Engine/Graphics/XC_Shaders/src/SolidColor.h"
#include "Engine/Graphics/XC_Shaders/src/LightTexture.h"
#include "Engine/Graphics/XC_Shaders/src/TerrainMultiTex.h"
#include "Engine/Graphics/XC_Shaders/src/CubeMap.h"
#include "Engine/Graphics/XC_Shaders/src/SkinnedCharacter.h"
#endif

#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"


XC_ShaderManager::XC_ShaderManager(ID3DDevice& device)
    : m_device(device)
{
}

XC_ShaderManager::~XC_ShaderManager(void)
{
}

void XC_ShaderManager::Init()
{
    m_Shaders.resize(ShaderType_Max);

#if defined(XCGRAPHICS_DX12) || defined(XCGRAPHICS_DX11)
    ZeroMemory(&m_rasterizerDesc, sizeof(D3D_RASTERIZER_DESC));
    ZeroMemory(&m_samplerDesc, sizeof(m_samplerDesc));
#endif

#if defined(XCGRAPHICS_DX12)
    //Initialize shader shared descriptor heap
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_sharedDescriptorHeap = (SharedDescriptorHeap*)&container.CreateNewSystem("SharedDescriptorHeap");

    m_sharedDescriptorHeap->Init(m_device,
#if defined(LOAD_SHADERS_FROM_DATA)
        100
#else
        SolidColorShader::NbOfDescriptors 
        + LightTextureShader::NbOfDescriptors 
        + TerrainMultiTex::NbOfDescriptors 
        + CubeMapShader::NbOfDescriptors 
        + SkinnedCharacterShader::NbOfDescriptors 
        + NbOfTextureResources 
#endif
        + 100);
#endif

    LoadShaders();

    LoadRasterizers();
    LoadSamplers();
}

void XC_ShaderManager::LoadShaders()
{
    IShader* binShader;

    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadFile(SHADER_SCHEMA_FILEPATH);
    fbSystem.ParseAndLoadFile(SHADER_DATA_FILEPATH);

#if defined(LOAD_SHADERS_FROM_DATA)
    auto FBShadersRoot = GetFBRootShader(fbSystem.GetBufferFromLoadedData());

    for (auto it = FBShadersRoot->FBShaders()->begin(); it != FBShadersRoot->FBShaders()->end(); ++it)
    {
        binShader = new XCShaderHandle(m_device);
        binShader->Load((void*) *it);
        m_Shaders[it->ShaderUsage()] = binShader;
    }
#else
    for (unsigned int shaderIndex = 0; shaderIndex < ShaderType_Max; shaderIndex++)
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

void XC_ShaderManager::LoadRasterizers()
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

void XC_ShaderManager::LoadSamplers()
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
#else
    //Create descriptor heap for sampler
    D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
    samplerHeapDesc.NumDescriptors = 1;
    samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ValidateResult(m_device.CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)));

    m_samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    m_samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    m_samplerDesc.MinLOD = 0;
    m_samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

    m_device.CreateSampler(&m_samplerDesc, m_samplerHeap->GetCPUDescriptorHandleForHeapStart());
#endif
}

void XC_ShaderManager::SetRasterizerState(ID3DDeviceContext& context, RasterType type)
{
#if defined(XCGRAPHICS_DX11)
    context.RSSetState(m_rasterizerStates[type]);
#endif
}

void XC_ShaderManager::Destroy()
{
    for (unsigned int shaderIndex = 0; shaderIndex < ShaderType_Max; shaderIndex++)
    {
        if (m_Shaders[(ShaderType)shaderIndex] != nullptr)
        {
            m_Shaders[(ShaderType)shaderIndex]->destroy();
        }
    }
    m_Shaders.clear();

#if defined(XCGRAPHICS_DX12)
    m_sharedDescriptorHeap->Destroy();
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("SharedDescriptorHeap");
#endif
}

void XC_ShaderManager::ApplyShader(ID3DDeviceContext& context, ShaderType _ShaderType)
{
    m_Shaders[_ShaderType]->ApplyShader(context);
#if defined(XCGRAPHICS_DX11)
    context.PSSetSamplers(0, 1, &m_SamplerLinear);
#else
    //Set sampler
    ((XCShaderHandle*)m_Shaders[_ShaderType])->SetSampler("samLinear", context, m_samplerHeap->GetGPUDescriptorHandleForHeapStart());
#endif
}

IShader* XC_ShaderManager::GetShader(ShaderType _type)
{
    return m_Shaders[_type];
}

void XC_ShaderManager::DrawNonIndexed(ID3DDeviceContext& context, unsigned int vertexCount)
{
#if defined(XCGRAPHICS_DX12)
    context.DrawInstanced(vertexCount, 1, 0, 0);
#elif defined(XCGRAPHICS_DX11)
    context.Draw(vertexCount, 0);
#endif
}

void XC_ShaderManager::DrawIndexedInstanced(ID3DDeviceContext& context, unsigned int _indexCount, void* indexGpuAddr, unsigned int instanceCount)
{
#if defined(XCGRAPHICS_DX12)
    context.DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
#elif defined(XCGRAPHICS_DX11)
    context.DrawIndexed(_indexCount, 0, 0);
#endif
}

void XC_ShaderManager::ClearShaderAndRenderStates(ID3DDeviceContext& context)
{
    for (unsigned int shaderIndex = 0; shaderIndex < ShaderType_Max; shaderIndex++)
    {
        if (m_Shaders[(ShaderType)shaderIndex] != nullptr)
        {
            m_Shaders[(ShaderType)shaderIndex]->Reset(context);
        }
    }
}