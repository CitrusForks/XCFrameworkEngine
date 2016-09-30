/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(UNUSED)

#include "Graphics/XCPipelineStateObject/PSOManager_Dx12.h"
#include "Graphics/XCShaders/XCShaderContainer.h"
#include "Graphics/XCGraphicsDx12.h"
#include "Graphics/XCShaders/XCVertexShaderLayout.h"
#include "Assets/Packages/PackageConsts.h"


PSOManager_Dx12::PSOManager_Dx12(ID3D12Device& deviceRef)
    : m_device(deviceRef)
{
    m_psoList.clear();
}

PSOManager_Dx12::~PSOManager_Dx12(void)
{
}

void PSOManager_Dx12::Init()
{
    LoadPSOs();
}

void PSOManager_Dx12::LoadPSOs()
{
    //Create the first basic pso
}


void PSOManager_Dx12::Destroy()
{
    while (m_psoList.size() > 0)
    {
        m_psoList.erase(m_psoList.begin());
    }
}

#endif