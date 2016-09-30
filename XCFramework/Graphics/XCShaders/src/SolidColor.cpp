/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Engine/Graphics/XC_Shaders/src/SolidColor.h"

#if defined(XCGRAPHICS_DX11)
#include "Assets/Shaders/SolidColor/SolidColor_VS.h"
#include "Assets/Shaders/SolidColor/SolidColor_PS.h"
#endif

#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#elif defined(XCGRAPHICS_GNM)
#include "Engine/Graphics/XC_Shaders/ShaderLoaderOrbis.h"
#endif

void SolidColorShader::createBufferConstants()
{
#if defined(XCGRAPHICS_DX12)
    m_pPerObject = createBuffer(BUFFERTYPE_CBV, sizeof(cbPerObjectBuffer));
#elif defined(XCXCGRAPHICS_DX11)
    m_pPerObject = createBuffer(sizeof(cbPerObjectBuffer));
#endif

    IShader::createBufferConstants();
}

void SolidColorShader::loadShader()
{
#if defined(XCGRAPHICS_DX12)
    //Create the first basic pso
    //Create root signature describing cb
    m_pso = new PSO_Dx12();

    CD3DX12_DESCRIPTOR_RANGE descRange[1];
    CD3DX12_ROOT_PARAMETER rootParams[1];

    descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    rootParams[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = 1;
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
    ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\SolidColor\\SolidColorVS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "SolidColor_VS", "vs_5_0", compilerFlags, 0, &m_pVS, &errors));
    ReleaseCOM(errors);
    ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\SolidColor\\SolidColorPS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "SolidColor_PS", "ps_5_0", compilerFlags, 0, &m_pPS, &errors));
    ReleaseCOM(errors);
#else
    unsigned int vsSize, psSize;
    ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\SolidColor\\SolidColorVS.cso", &m_pVS, vsSize));
    ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\SolidColor\\SolidColorPS.cso", &m_pPS, psSize));
    XCASSERT(m_pVS || m_pPS);
#endif

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = VertexPosColorInputLayoutDesc;
    inputLayoutDesc.NumElements = ARRAYSIZE(VertexPosColorInputLayoutDesc);

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

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = true;
    
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
    ValidateResult( m_device.CreateVertexShader( g_SolidColor_vs,
        sizeof( g_SolidColor_vs ),
        NULL,
        &m_pVS )
        );

    ValidateResult(m_device.CreatePixelShader(g_SolidColor_ps,
        sizeof( g_SolidColor_ps ),
        NULL,
        &m_pPS )
        );

    ValidateResult(m_device.CreateInputLayout(VertexPosColorInputLayoutDesc,
        _countof( VertexPosColorInputLayoutDesc ),
        g_SolidColor_vs,
        sizeof( g_SolidColor_vs ),
        &m_pInputLayout )
        );
#elif defined(XCGRAPHICS_GNM)
    MemorySystemOrbis* memorySystem = (MemorySystemOrbis*) &SystemLocator::GetInstance()->RequestSystem<MemorySystem>("MemorySystem"); 
    
    // Load the shader binaries from disk
    m_pVS = loadVsShader("/hostapp/Assets/Shaders/SolidColor/SolidColorVS.sb", memorySystem->GetAllocator());
    m_pPS = loadPsShader("/hostapp/Assets/Shaders/SolidColor/SolidColorPS.sb", memorySystem->GetAllocator());
    
    XCASSERT(m_pVS != nullptr || m_pPS != nullptr);
    
    m_fetchVSShader = memorySystem->GetAllocator().allocate(sce::Gnmx::computeVsFetchShaderSize(m_pVS), sce::Gnm::kAlignmentOfFetchShaderInBytes, SCE_KERNEL_WC_GARLIC);
    XCASSERT(m_fetchVSShader);
    
    sce::Gnmx::generateVsFetchShader(m_fetchVSShader, &m_shaderModifier, m_pVS, nullptr);
    
#endif
}

void SolidColorShader::setWVP(ID3DDeviceContext & context, cbWVP wvp)
{
#if defined(XCGRAPHICS_DX12)
    memcpy(m_pPerObject->m_cbDataBegin, &wvp, sizeof(cbPerObjectBuffer));
    context.SetGraphicsRootDescriptorTable(0, m_pPerObject->m_gpuHandle);
#elif defined(XCGRAPHICS_DX11)
    MemCpyConstants(context, (void*)m_pPerObject, (void*)&wvp, sizeof(cbPerObjectBuffer));
    context.PSSetConstantBuffers(0, 1, &m_pPerObject);
    context.VSSetConstantBuffers(0, 1, &m_pPerObject);
#endif
}

#if defined(XCGRAPHICS_DX12)
void SolidColorShader::setWVP(ID3DDeviceContext & context, D3D12_GPU_DESCRIPTOR_HANDLE & wvpGPUHandle)
{
    context.SetGraphicsRootDescriptorTable(0, wvpGPUHandle);
}
#endif

void SolidColorShader::applyShader(ID3DDeviceContext& context)
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
    context.setActiveShaderStages(sce::Gnm::kActiveShaderStagesVsPs);
    context.setVsShader(m_pVS, m_shaderModifier, m_fetchVSShader);
    context.setPsShader(m_pPS);
#endif
}

void SolidColorShader::reset(ID3DDeviceContext& context)
{
}

void SolidColorShader::destroy()
{
#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_pPerObject);
#endif
}