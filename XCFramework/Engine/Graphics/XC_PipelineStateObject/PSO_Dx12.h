/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)

#include "Engine/Graphics/XC_Shaders/XC_RasterizerTypes.h"

class PSO_Dx12
{
public:
    PSO_Dx12();
    ~PSO_Dx12();

    ID3D12RootSignature&                    GetRootSignature() { return *m_rootSignature; }
    static void                             GenerateDefaultPSO(PSO_Dx12* inPso, RasterType rasterizerType, bool enableDepth);

    //TODO : Make it private
    D3D12_GRAPHICS_PIPELINE_STATE_DESC      m_psoDesc;
    ID3D12PipelineState*                    m_pPso;
    ID3D12RootSignature*                    m_rootSignature;

private:
};

#endif