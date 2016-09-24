/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/RenderWorkerTypeDefines.h"

class RenderContext;

class IRenderableObject
{
public:
    IRenderableObject()
        : m_workerType(WorkerType_PosDiffuseTex)
        , m_renderWorkerMask(WorkerMask_PosDiffuseTex_Pass1 | WorkerMask_Lighting_Pass2)
        , m_isRenderable(false)
    {
        m_isRenderable = false;
    }

    virtual ~IRenderableObject() {}

    virtual void        Draw(RenderContext& context) = 0;
    virtual void        RenderContextCallback(ID3DDeviceContext& renderContext) {}
    virtual void        OnRenderComplete() {}

    RenderWorkerType    GetRenderWorkerType()     { return m_workerType; }
    u32                 GetRenderWorkerMask()     { return m_renderWorkerMask; }
    bool                IsRenderable()            { return m_isRenderable; }

protected:

    RenderWorkerType    m_workerType;
    u32                 m_renderWorkerMask;
    bool                m_isRenderable;
};