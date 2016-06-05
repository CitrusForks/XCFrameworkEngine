/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Engine/Graphics/XC_Shaders/src/DefaultShader.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Graphics/XC_GraphicsOrbis.h"
#include "Engine/Memory/MemorySystemOrbis.h"

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#include <D3Dcompiler.h>
#elif defined(XCGRAPHICS_GNM)
#include "Engine/Graphics/XC_Shaders/ShaderLoaderOrbis.h"
#endif

void DefaultShader::createBufferConstants()
{
}

void DefaultShader::loadShader()
{
#if defined(XCGRAPHICS_GNM)
    MemorySystemOrbis* memorySystem = (MemorySystemOrbis*) &SystemLocator::GetInstance()->RequestSystem<MemorySystem>("MemorySystem"); 
    
    // Load the shader binaries from disk
    m_pVS = loadVsShader("/hostapp/Assets/Shaders/Default/default_vs.sb", memorySystem->GetAllocator());
    m_pPS = loadPsShader("/hostapp/Assets/Shaders/Default/default_ps.sb", memorySystem->GetAllocator());

    XCASSERT(m_pVS != nullptr || m_pPS != nullptr);

    m_fetchVSShader = memorySystem->GetAllocator().allocate(sce::Gnmx::computeVsFetchShaderSize(m_pVS), sce::Gnm::kAlignmentOfFetchShaderInBytes, SCE_KERNEL_WC_GARLIC);
    XCASSERT(m_fetchVSShader);

    sce::Gnmx::generateVsFetchShader(m_fetchVSShader, &m_shaderModifier, m_pVS, nullptr);

#endif
}

void DefaultShader::setWVP(ID3DDeviceContext& context, cbWVP& wvp)
{
#if defined(XCGRAPHICS_GNM)
    createBuffer<cbWVP>(context, &m_wvp, m_cbWvp);
    m_wvp->gWVP = wvp.gWVP;
    context.setConstantBuffers(sce::Gnm::kShaderStageVs, 0, 1, &m_cbWvp);
#endif
}

void DefaultShader::setTex2D(ID3DDeviceContext& context, D3DShaderResourceView* tex)
{
#if defined(XCGRAPHICS_GNM)
context.setTextures(sce::Gnm::kShaderStagePs, 0, 1, tex);
#endif
}

void DefaultShader::reset(ID3DDeviceContext& context)
{
}