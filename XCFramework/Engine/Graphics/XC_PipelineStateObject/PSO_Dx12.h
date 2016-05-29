/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)

#include "Engine/Graphics/XC_Shaders/XC_RasterizerTypes.h"

enum PSOType
{
    PSOType_RASTER_FILL_SOLID,
    PSOType_RASTER_FILL_WIREFRAME,

    PSOType_Max
};

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

        D3D12_GRAPHICS_PIPELINE_STATE_DESC      m_psoDesc;
        ID3D12PipelineState*                    m_pPso;
        ID3D12RootSignature*                    m_rootSignature;
    };

    bool                                    CreateRootSignature(ID3DDevice& device, void* bufferPtr, unsigned int bufferSize, PSOType type);
    void                                    CreateGraphicPSO(ID3DDevice& context, PSOType type);

    ID3D12RootSignature&                    GetRootSignature(PSOType type);
    static void                             GenerateDefaultPSO(PSO_Dx12* inPso, PSOType type);

    void                                    SetGraphicsPipelineState(ID3DDeviceContext& context, RasterType type = RasterType_FillSolid);

    std::vector<PSO>                        m_psos;
};

#endif