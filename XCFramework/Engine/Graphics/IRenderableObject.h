/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/RenderWorkerTypeDefines.h"
#include "Engine/Graphics/RenderContext.h"

class IRenderableObject
{
public:
    IRenderableObject()
    {
        m_useRenderWorkerType = WorkerType_Max;
    }

    virtual void        Draw(RenderContext& context) = 0;
    RenderWorkerType    GetRenderOnWorkerType() { return m_useRenderWorkerType; }

protected:
    RenderWorkerType    m_useRenderWorkerType;
};