/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "Engine/Graphics/XC_Shaders/src/SkinnedCharacter.h"

#if defined(XCGRAPHICS_DX11)
#include "Engine/Graphics/XC_GraphicsDx11.h"
#include "Assets/Shaders/LightTexture/LightTexture_VS.h"
#include "Assets/Shaders/LightTexture/LightTexture_PS.h"
#endif

#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#elif defined(XCGRAPHICS_GNM)
#include "Engine/Graphics/XC_Shaders/ShaderLoaderOrbis.h"
#endif

void SkinnedCharacterShader::loadShader()
{
#if defined(XCGRAPHICS_DX12)
    //Create the first basic pso
    //Create root signature describing cb
    m_pso = new PSO_Dx12();

    CD3DX12_DESCRIPTOR_RANGE descRange[5];
    CD3DX12_ROOT_PARAMETER rootParams[5];

    descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
    descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);

    rootParams[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);
    rootParams[1].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);
    rootParams[2].InitAsDescriptorTable(1, &descRange[2], D3D12_SHADER_VISIBILITY_ALL);

    descRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    rootParams[3].InitAsDescriptorTable(1, &descRange[3], D3D12_SHADER_VISIBILITY_ALL);

    descRange[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
    rootParams[4].InitAsDescriptorTable(1, &descRange[4], D3D12_SHADER_VISIBILITY_ALL);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = 5;
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

    UINT compilerFlags = 0;

#if defined(_DEBUG)
    compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

#if defined(USE_D3D_COMPILER)
    ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\SkinnedCharacter\\SkinnedCharacterVS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "LightTexture_VS", "vs_5_0", compilerFlags, 0, &m_pVS, &errors));
    ReleaseCOM(errors);
    ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\SkinnedCharacter\\SkinnedCharacterPS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "LightTexture_PS", "ps_5_0", compilerFlags, 0, &m_pPS, &errors));
    ReleaseCOM(errors);
#else
    unsigned int vsSize, psSize;
    ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\SkinnedCharacter\\SkinnedCharacterVS.cso", &m_pVS, vsSize));
    ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\SkinnedCharacter\\SkinnedCharacterPS.cso", &m_pPS, psSize));
    XCASSERT(m_pVS || m_pPS);
#endif

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = VertexPosNormTexBIndexBWeightInputLayoutDesc;
    inputLayoutDesc.NumElements = ARRAYSIZE(VertexPosNormTexBIndexBWeightInputLayoutDesc);

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
    ValidateResult( m_device.CreateVertexShader( g_LightTexture_vs,
        sizeof( g_LightTexture_vs ),
        NULL,
        &m_pVS )
        );

    ValidateResult( m_device.CreatePixelShader( g_LightTexture_ps,
        sizeof( g_LightTexture_ps ),
        NULL,
        &m_pPS )
        );

    ValidateResult( m_device.CreateInputLayout( VertexPosNormTexInputLayoutDesc,
        _countof( VertexPosNormTexInputLayoutDesc ),
        g_LightTexture_vs,
        sizeof( g_LightTexture_vs ),
        &m_pInputLayout)
        );
#elif defined(XCGRAPHICS_GNM)
    MemorySystemOrbis* memorySystem = (MemorySystemOrbis*) &SystemLocator::GetInstance()->RequestSystem<MemorySystem>("MemorySystem"); 
    
    // Load the shader binaries from disk
    m_pVS = loadVsShader("/hostapp/Assets/Shaders/LightTexture/LightTextureVS.sb", memorySystem->GetAllocator());
    m_pPS = loadPsShader("/hostapp/Assets/Shaders/LightTexture/LightTexturePS.sb", memorySystem->GetAllocator());
    
    XCASSERT(m_pVS != nullptr || m_pPS != nullptr);
    
    m_fetchVSShader = memorySystem->GetAllocator().allocate(sce::Gnmx::computeVsFetchShaderSize(m_pVS), sce::Gnm::kAlignmentOfFetchShaderInBytes, SCE_KERNEL_WC_GARLIC);
    XCASSERT(m_fetchVSShader);
    
    sce::Gnmx::generateVsFetchShader(m_fetchVSShader, &m_shaderModifier, m_pVS, nullptr);
#endif
}

void SkinnedCharacterShader::applyShader(ID3DDeviceContext& context)
{
#if defined(XCGRAPHICS_DX12)
    //Set the pso
    context.SetPipelineState(m_pso->m_pPso);

    //Set Root signature
    context.SetGraphicsRootSignature(&m_pso->GetRootSignature());
#elif defined(XCGRAPHICS_DX11)
    context.IASetInputLayout(m_pInputLayout);
    context.VSSetShader(m_pVS, nullptr, 0);
    context.PSSetShader(m_pPS, nullptr, 0);
#elif defined(XCGRAPHICS_GNM)
    IShader::applyShader(context);
#endif
}

void SkinnedCharacterShader::createBufferConstants()
{
    IShader::createBufferConstants();

#if defined(XCGRAPHICS_DX12)
    m_pCBSkinnedCharacter   = createBuffer(BUFFERTYPE_CBV, sizeof(cbSkinnedCharacterBuffer));
    m_pCBLightsPerFrame     = createBuffer(BUFFERTYPE_CBV, sizeof(cbLightsPerFrame));
    m_pCBBoneBuffer         = createBuffer(BUFFERTYPE_CBV, sizeof(cbBoneBuffer));
#elif defined(XCGRAPHICS_DX11)
    m_pCBSkinnedCharacter   = createBuffer(sizeof(cbSkinnedCharacterBuffer));
    m_pCBLightsPerFrame     = createBuffer(sizeof(cbLightsPerFrame));
    m_pCBBoneBuffer         = createBuffer(sizeof(cbBoneBuffer));

    m_pTexture2DDiffuseMap = nullptr;
#endif
}


#if defined(XCGRAPHICS_DX12)
void SkinnedCharacterShader::setCBPerObject(ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    context.SetGraphicsRootDescriptorTable(0, gpuHandle);
}

void SkinnedCharacterShader::setCBBoneBuffers(ID3DDeviceContext & context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    context.SetGraphicsRootDescriptorTable(2, gpuHandle);
}
#endif

void SkinnedCharacterShader::setCBLightsPerFrame(ID3DDeviceContext& context, cbLightsPerFrame& lightsPerFrame)
{
#if defined(XCGRAPHICS_DX12)
    memcpy(m_pCBLightsPerFrame->m_cbDataBegin, &lightsPerFrame, sizeof(cbLightsPerFrame));
    context.SetGraphicsRootDescriptorTable(1, m_pCBLightsPerFrame->m_gpuHandle);
#elif defined(XCGRAPHICS_DX11)
    MemCpyConstants(context, (void*)m_pCBLightsPerFrame, (void*)&lightsPerFrame, sizeof(cbLightsPerFrame));
    context.PSSetConstantBuffers(1, 1, &m_pCBLightsPerFrame);
    context.VSSetConstantBuffers(1, 1, &m_pCBLightsPerFrame);
#elif defined(XCGRAPHICS_GNM)
    createBuffer<cbLightsPerFrame>(context, &m_LightsPerFrame, m_pCBLightsPerFrame);
    ::memcpy(m_LightsPerFrame, &lightsPerFrame, sizeof(cbLightsPerFrame));

    context.setConstantBuffers(sce::Gnm::kShaderStageVs, 4, 1, &m_pCBLightsPerFrame);
    context.setConstantBuffers(sce::Gnm::kShaderStagePs, 4, 1, &m_pCBLightsPerFrame);
#endif
}

void SkinnedCharacterShader::setTexture2D(ID3DDeviceContext& context, D3DShaderResourceView* tex)
{
#if defined(XCGRAPHICS_DX12)
    if (tex)
    {
        context.SetGraphicsRootDescriptorTable(3, tex->m_gpuHandle);
    }
#elif defined(XCGRAPHICS_DX11)
    if(tex)
    {
        m_pTexture2DDiffuseMap = tex;
    }

    context.VSSetShaderResources(0, 1, &m_pTexture2DDiffuseMap);
    context.PSSetShaderResources(0, 1, &m_pTexture2DDiffuseMap);
#elif defined(XCGRAPHICS_GNM)
    context.setTextures(sce::Gnm::kShaderStageVs, 0, 1, tex);
    context.setTextures(sce::Gnm::kShaderStagePs, 0, 1, tex);
#endif
}

void SkinnedCharacterShader::reset(ID3DDeviceContext& context)
{
    m_pTexture2DDiffuseMap = nullptr;
    setTexture2D(context, m_pTexture2DDiffuseMap);
}

void SkinnedCharacterShader::destroy()
{
#if defined(XCGRAPHICS_DX12)
    m_pCBSkinnedCharacter->Release();
    m_pCBLightsPerFrame->Release();
#elif defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_pCBSkinnedCharacter);
    ReleaseCOM(m_pCBPerObject);
    ReleaseCOM(m_pTexture2DDiffuseMap);
#endif
    IShader::destroy();
}