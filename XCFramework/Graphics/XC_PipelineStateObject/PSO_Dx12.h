/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once


#include "Graphics/XC_Shaders/XC_RasterizerTypes.h"

struct PSODesc
{
    PSODesc(RasterType type, u32 nbOfRTv)
        : m_rasterType(type)
        , m_nbOfRTV(nbOfRTv)
    {}

    u32         m_nbOfRTV;
    RasterType  m_rasterType;
};

#if defined(XCGRAPHICS_DX12)

class PSO_Dx12
{
public:
    PSO_Dx12();
    ~PSO_Dx12();

    struct PSO
    {
        PSO()
        {
            ZeroMemory(&m_psoDesc, sizeof(m_psoDesc));
            m_psoDesc = {};
            m_pPso = nullptr;
            m_rootSignature = nullptr;
        }

        ~PSO()
        {
            m_pPso = nullptr;
            ReleaseCOM(m_rootSignature);
            m_rootSignature = nullptr;
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC  m_psoDesc;
        ID3DPipelineState*                  m_pPso;
        ID3D12RootSignature*                m_rootSignature;
    };

    bool                                    CreateRootSignature(ID3DDevice& device, void* bufferPtr, u32 bufferSize, RasterType type);
    void                                    CreateGraphicPSO(ID3DDevice& context, RasterType type);

    ID3D12RootSignature&                    GetRootSignature(RasterType type);
    static void                             GenerateDefaultPSO(PSO_Dx12* inPso, PSODesc& type);

    void                                    SetGraphicsPipelineState(ID3DDeviceContext& context, RasterType type = RasterType_FillSolid);

    std::vector<PSO>                        m_psos;
};

#endif