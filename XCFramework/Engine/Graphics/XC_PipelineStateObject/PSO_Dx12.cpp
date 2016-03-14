/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#if defined(XCGRAPHICS_DX12)
#include "Engine/Graphics/XC_PipelineStateObject/PSO_Dx12.h"

#include "Libs/Dx12Helpers/d3dx12.h"

PSO_Dx12::PSO_Dx12()
{
    ZeroMemory(&m_psoDesc, sizeof(m_psoDesc));
    m_psoDesc = {};
    m_pPso = nullptr;
    m_rootSignature = nullptr;
}

PSO_Dx12::~PSO_Dx12()
{
    m_pPso = nullptr;
    ReleaseCOM(m_rootSignature);
    m_rootSignature = nullptr;
}

void PSO_Dx12::GenerateDefaultPSO(PSO_Dx12* inPso, ERasterizer_Type rasterizerType, bool enableDepth)
{
    CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    switch (rasterizerType)
    {
    case RASTERIZERTYPE_FILL_SOLID:
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
        break;
    case RASTERIZERTYPE_FILL_WIREFRAME:
        rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
        break;

    default:
        XCASSERT(false);
        break;
    }

    //Default blend desc from CD3DX12 helper
    CD3DX12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    depthStencilDesc.DepthEnable = enableDepth;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = enableDepth ? D3D12_COMPARISON_FUNC_LESS : D3D12_COMPARISON_FUNC_LESS_EQUAL;
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

    inPso->m_psoDesc.RasterizerState = rasterizerDesc;
    inPso->m_psoDesc.BlendState = blendDesc;
    inPso->m_psoDesc.DepthStencilState = depthStencilDesc;
    inPso->m_psoDesc.SampleMask = UINT_MAX;
    inPso->m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    inPso->m_psoDesc.NumRenderTargets = 1;
    inPso->m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    inPso->m_psoDesc.SampleDesc.Count = 1;
    inPso->m_psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

#endif