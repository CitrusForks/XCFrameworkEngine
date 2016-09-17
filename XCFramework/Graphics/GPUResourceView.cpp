/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "GPUResourceView.h"

GPUResourceView::GPUResourceView(GPUResourceType type)
    : m_resourceType(type)
    , m_isInUse(false)
{
#if defined(XCGRAPHICS_DX11)
    m_gpuHandle = nullptr;
    m_cpuHandle = nullptr;
#endif
}

GPUResourceView::~GPUResourceView()
{
    Destroy();
}

void GPUResourceView::Destroy()
{
#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_cpuHandle);
    ReleaseCOM(m_gpuHandle);
#endif
}