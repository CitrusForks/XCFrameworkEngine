/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(XCGRAPHICS_DX12)
#include "Graphics/XC_PipelineStateObject/PSO_Dx12.h"
#include "Graphics/RenderTargetTypes.h"

#include "Libs/Dx12Helpers/d3dx12.h"

PSO_Dx12::PSO_Dx12()
{
    m_psos.resize(RasterType_MAX);
}

PSO_Dx12::~PSO_Dx12()
{
    m_psos.clear();
}

ID3D12RootSignature& PSO_Dx12::GetRootSignature(RasterType type)
{ 
    return *m_psos[type].m_rootSignature; 
}

bool PSO_Dx12::CreateRootSignature(ID3DDevice& device, void* bufferPtr, u32 bufferSize, RasterType type)
{
    bool result = ValidateResult(device.CreateRootSignature(0, bufferPtr, bufferSize, IID_PPV_ARGS(&m_psos[type].m_rootSignature))) == S_OK;
    m_psos[type].m_psoDesc.pRootSignature = m_psos[type].m_rootSignature;

    return result;
}

void PSO_Dx12::GenerateDefaultPSO(PSO_Dx12* inPso, PSODesc& desc)
{
    CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    switch (desc.m_rasterType)
    {
    case RasterType_FillSolid:
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        break;

    case RasterType_FillWireframe:
        rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
        break;

    default:
        XCASSERT(false);
    }

    //Default blend desc from CD3DX12 helper
    CD3DX12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = true ? D3D12_COMPARISON_FUNC_LESS : D3D12_COMPARISON_FUNC_LESS_EQUAL;
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

    inPso->m_psos[desc.m_rasterType].m_psoDesc.RasterizerState = rasterizerDesc;
    inPso->m_psos[desc.m_rasterType].m_psoDesc.BlendState = blendDesc;
    inPso->m_psos[desc.m_rasterType].m_psoDesc.DepthStencilState = depthStencilDesc;
    inPso->m_psos[desc.m_rasterType].m_psoDesc.SampleMask = UINT_MAX;
    inPso->m_psos[desc.m_rasterType].m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    inPso->m_psos[desc.m_rasterType].m_psoDesc.NumRenderTargets = desc.m_nbOfRTV;

    inPso->m_psos[desc.m_rasterType].m_psoDesc.RTVFormats[0] = gs_RenderTargetFormats[RenderTargetType_Main_0];

    for (u32 rIndex = RenderTargetType_Main_1 + 1; rIndex < desc.m_nbOfRTV + 1; ++rIndex)
    {
        inPso->m_psos[desc.m_rasterType].m_psoDesc.RTVFormats[rIndex - 1] = gs_RenderTargetFormats[(RenderTargetsType)rIndex];
    }

    inPso->m_psos[desc.m_rasterType].m_psoDesc.SampleDesc.Count = 1;
    inPso->m_psos[desc.m_rasterType].m_psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

void PSO_Dx12::CreateGraphicPSO(ID3DDevice& device, RasterType type)
{
    //Create the actual PSO.
    ValidateResult(device.CreateGraphicsPipelineState(&m_psos[type].m_psoDesc, IID_PPV_ARGS(&m_psos[type].m_pPso)));
}

void PSO_Dx12::SetGraphicsPipelineState(ID3DDeviceContext& context, RasterType rasterType)
{
    //Set the pso
    context.SetPipelineState(m_psos[rasterType].m_pPso);

    //Set Root signature
    context.SetGraphicsRootSignature(&GetRootSignature(rasterType));
}

#endif