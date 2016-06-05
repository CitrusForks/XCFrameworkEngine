/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "TerrainMultiTex.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#if defined(XCGRAPHICS_DX11)
#include "Assets/Shaders/TerrainMultiTex/TerrainMultiTex_VS.h"
#include "Assets/Shaders/TerrainMultiTex/TerrainMultiTex_PS.h"
#endif

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#elif defined(XCGRAPHICS_GNM)
#include "Engine/Graphics/XC_Shaders/ShaderLoaderOrbis.h"
#endif

TerrainMultiTex::TerrainMultiTex(ID3DDevice& device)
    : LightTextureShader(device)
{
}


TerrainMultiTex::~TerrainMultiTex()
{
}

void TerrainMultiTex::loadShader()
{
#if defined(XCGRAPHICS_DX12)
    //Create the first basic pso
    //Create root signature describing cb
    m_pso = new PSO_Dx12();

    CD3DX12_DESCRIPTOR_RANGE descRange[7];
    CD3DX12_ROOT_PARAMETER rootParams[7];

    descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    rootParams[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);

    descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
    rootParams[1].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);

    descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    rootParams[2].InitAsDescriptorTable(1, &descRange[2], D3D12_SHADER_VISIBILITY_ALL);
    
    descRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    rootParams[3].InitAsDescriptorTable(1, &descRange[3], D3D12_SHADER_VISIBILITY_ALL);
    
    descRange[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
    rootParams[4].InitAsDescriptorTable(1, &descRange[4], D3D12_SHADER_VISIBILITY_ALL);

    descRange[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
    rootParams[5].InitAsDescriptorTable(1, &descRange[5], D3D12_SHADER_VISIBILITY_ALL);

    descRange[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
    rootParams[6].InitAsDescriptorTable(1, &descRange[6], D3D12_SHADER_VISIBILITY_ALL);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = 7;
    rootSignatureDesc.pParameters = rootParams;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
        | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
        | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    ID3DBlob* signature;
    ID3DBlob* errors;

    ValidateResult(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errors));
    ValidateResult(m_device.CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pso->m_rootSignature)));

    ReleaseCOM(signature);
    ReleaseCOM(errors);

    //Load our default solidcolor shader
    UINT compilerFlags = 0;

#if defined(_DEBUG)
    compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

#if defined(USE_D3D_COMPILER)
    ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\TerrainMultiTex\\TerrainMultiTexVS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "TerrainMultiTex_VS", "vs_5_0", compilerFlags, 0, &m_pVS, &errors));
    ReleaseCOM(errors);
    ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\TerrainMultiTex\\TerrainMultiTexPS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "TerrainMultiTex_PS", "ps_5_0", compilerFlags, 0, &m_pPS, &errors));
    ReleaseCOM(errors);
#else
    unsigned int vsSize, psSize;
    ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\TerrainMultiTex\\TerrainMultiTexVS.cso", &m_pVS, vsSize));
    ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\TerrainMultiTex\\TerrainMultiTexPS.cso", &m_pPS, psSize));
    XCASSERT(m_pVS || m_pPS);
#endif

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = VertexPosNormTexInputLayoutDesc;
    inputLayoutDesc.NumElements = ARRAYSIZE(VertexPosNormTexInputLayoutDesc);

    //Default rasterizer desc from CD3DX12 helper
    D3D12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.MultisampleEnable = FALSE;               //Check your samples
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.ForcedSampleCount = 0;
    rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    //Default blend desc from CD3DX12 helper
    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    m_pso->m_psoDesc.InputLayout = inputLayoutDesc;
    m_pso->m_psoDesc.RasterizerState = rasterizerDesc;
    m_pso->m_psoDesc.BlendState = blendDesc;
    m_pso->m_psoDesc.pRootSignature = m_pso->m_rootSignature;
#if defined(USE_D3D_COMPILER)
    m_pso->m_psoDesc.VS = { reinterpret_cast<UINT8*>(m_pVS->GetBufferPointer()), m_pVS->GetBufferSize() };
    m_pso->m_psoDesc.PS = { reinterpret_cast<UINT8*>(m_pPS->GetBufferPointer()), m_pPS->GetBufferSize() };
#else
    m_pso->m_psoDesc.VS = { m_pVS, vsSize };
    m_pso->m_psoDesc.PS = { m_pPS, psSize };
#endif
    m_pso->m_psoDesc.DepthStencilState = depthStencilDesc;
    m_pso->m_psoDesc.SampleMask = UINT_MAX;
    m_pso->m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    m_pso->m_psoDesc.NumRenderTargets = 1;
    m_pso->m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_pso->m_psoDesc.SampleDesc.Count = 1;
    m_pso->m_psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //Create the actual PSO.
    ValidateResult(m_device.CreateGraphicsPipelineState(&m_pso->m_psoDesc, IID_PPV_ARGS(&m_pso->m_pPso)));
#elif defined(XCGRAPHICS_DX11)
    ValidateResult(m_device.CreateVertexShader(g_TerrainMultiTex_vs,
        sizeof(g_TerrainMultiTex_vs),
        NULL,
        &m_pVS)
        );

    ValidateResult(m_device.CreatePixelShader(g_TerrainMultiTex_ps,
        sizeof(g_TerrainMultiTex_ps),
        NULL,
        &m_pPS)
        );

    ValidateResult(m_device.CreateInputLayout(VertexPosNormTexInputLayoutDesc,
        _countof(VertexPosNormTexInputLayoutDesc),
        g_TerrainMultiTex_vs,
        sizeof(g_TerrainMultiTex_vs),
        &m_pInputLayout)
        );
#elif defined(XCGRAPHICS_GNM)
    MemorySystemOrbis* memorySystem = (MemorySystemOrbis*) &SystemLocator::GetInstance()->RequestSystem<MemorySystem>("MemorySystem"); 
    
    // Load the shader binaries from disk
    m_pVS = loadVsShader("/hostapp/Assets/Shaders/TerrainMultiTex/TerrainMultiTexVS.sb", memorySystem->GetAllocator());
    m_pPS = loadPsShader("/hostapp/Assets/Shaders/TerrainMultiTex/TerrainMultiTexPS.sb", memorySystem->GetAllocator());
    
    XCASSERT(m_pVS != nullptr || m_pPS != nullptr);
    
    m_fetchVSShader = memorySystem->GetAllocator().allocate(sce::Gnmx::computeVsFetchShaderSize(m_pVS), sce::Gnm::kAlignmentOfFetchShaderInBytes, SCE_KERNEL_WC_GARLIC);
    XCASSERT(m_fetchVSShader);
    
    sce::Gnmx::generateVsFetchShader(m_fetchVSShader, &m_shaderModifier, m_pVS, nullptr);
#endif
}

void TerrainMultiTex::createBufferConstants()
{
    LightTextureShader::createBufferConstants();

#if defined(XCGRAPHICS_DX11)
    m_pTexture2DDiffuseMap1 = nullptr;
    m_pTexture2DDiffuseMap2 = nullptr;
    m_pTexture2DBlendMap    = nullptr;
#endif
}


void TerrainMultiTex::setTexture2D1(ID3DDeviceContext& context, D3DShaderResourceView* tex)
{
#if defined(XCGRAPHICS_DX12)
    if (tex)
    {
        context.SetGraphicsRootDescriptorTable(3, tex->m_gpuHandle);
    }
#elif defined(XCGRAPHICS_DX11)
    if (tex)
    {
        m_pTexture2DDiffuseMap1 = tex;
    }
    context.VSSetShaderResources(1, 1, &m_pTexture2DDiffuseMap1);
    context.PSSetShaderResources(1, 1, &m_pTexture2DDiffuseMap1);
#elif defined(XCGRAPHICS_GNM)
    context.setTextures(sce::Gnm::kShaderStagePs, 1, 1, tex);
#endif
}

void TerrainMultiTex::setTexture2D2(ID3DDeviceContext& context, D3DShaderResourceView* tex)
{
#if defined(XCGRAPHICS_DX12)
    if (tex)
    {
        context.SetGraphicsRootDescriptorTable(4, tex->m_gpuHandle);
    }
#elif defined(XCGRAPHICS_DX11)
    if (tex)
    {
        m_pTexture2DDiffuseMap2 = tex;
    }
    context.VSSetShaderResources(2, 1, &m_pTexture2DDiffuseMap2);
    context.PSSetShaderResources(2, 1, &m_pTexture2DDiffuseMap2);
#elif defined(XCGRAPHICS_GNM)
    context.setTextures(sce::Gnm::kShaderStagePs, 2, 1, tex);
#endif
}


void TerrainMultiTex::setTexture2DBlend(ID3DDeviceContext& context, D3DShaderResourceView* tex)
{
#if defined(XCGRAPHICS_DX12)
    if (tex)
    {
        context.SetGraphicsRootDescriptorTable(5, tex->m_gpuHandle);
    }
#elif defined(XCGRAPHICS_DX11)
    if (tex)
    {
        m_pTexture2DBlendMap = tex;
    }
    context.VSSetShaderResources(3, 1, &m_pTexture2DBlendMap);
    context.PSSetShaderResources(3, 1, &m_pTexture2DBlendMap);
#elif defined(XCGRAPHICS_GNM)
    context.setTextures(sce::Gnm::kShaderStagePs, 3, 1, tex);
#endif
}

void TerrainMultiTex::applyShader(ID3DDeviceContext& context)
{
#if defined(XCGRAPHICS_DX12)
    //Set the pso
    context.SetPipelineState(m_pso->m_pPso);

    //Set Root signature
    context.SetGraphicsRootSignature(&m_pso->GetRootSignature());

#elif defined(XCGRAPHICS_GNM)
    LightTextureShader::applyShader(context);
#endif
}

void TerrainMultiTex::destroy()
{
#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_pTexture2DBlendMap);
    ReleaseCOM(m_pTexture2DDiffuseMap1);
    ReleaseCOM(m_pTexture2DDiffuseMap2);
#endif
    LightTextureShader::destroy();
}

void TerrainMultiTex::reset(ID3DDeviceContext& context)
{
    LightTextureShader::reset(context);

    m_pTexture2DBlendMap = nullptr;
    m_pTexture2DDiffuseMap1 = nullptr;
    m_pTexture2DDiffuseMap2 = nullptr;
    
    setTexture2D1(context, m_pTexture2DDiffuseMap1);
    setTexture2D2(context, m_pTexture2DDiffuseMap2);
    setTexture2DBlend(context, m_pTexture2DBlendMap);
}