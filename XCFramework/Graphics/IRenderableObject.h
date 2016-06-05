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
    {
        m_useRenderWorkerType = WorkerType_Max;
        m_isRenderable = false;
    }
    virtual ~IRenderableObject() {}

    virtual void        Draw(RenderContext& context) = 0;

    RenderWorkerType    GetRenderOnWorkerType() { return m_useRenderWorkerType; }
    bool                IsRenderable()          { return m_isRenderable; }
protected:

    RenderWorkerType    m_useRenderWorkerType;
    bool                m_isRenderable;
};