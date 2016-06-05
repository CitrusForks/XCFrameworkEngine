/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(UNUSED)
#include "Graphics/XC_PipelineStateObject/PSO_Dx12.h"

enum EPSO_Type
{
    PSO_MAIN,
    PSO_SOLIDCOLOR,
    PSO_LIGHTTEX,
    PSO_MULTITEXTERRAIN,

    PSO_MAX
};


class PSOManager_Dx12
{
public:
    PSOManager_Dx12(ID3D12Device& deviceRef);
    ~PSOManager_Dx12(void);

    void                                Init();
    void                                LoadPSOs();
    void                                Destroy();
    
    PSO_Dx12&                           GetPSOType(EPSO_Type type) { return *m_psoList[type].get(); }
private:
    std::map<EPSO_Type, std::unique_ptr<PSO_Dx12>>      m_psoList;
    ID3D12Device&                                       m_device;

    //Move to shader loading specific
    ID3DVertexShader*                                   m_vs;
    ID3DPixelShader*                                    m_ps;
};

#endif