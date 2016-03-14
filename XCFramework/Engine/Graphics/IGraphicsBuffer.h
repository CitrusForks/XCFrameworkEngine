/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/GraphicUtils.h"
#include "Engine/Graphics/RenderContext.h"

class IGraphicsBuffer
{
public:

    IGraphicsBuffer() 
    {
        m_resourceUpdated = false;
    }

    void WaitResourceUpdate()
    {
        while (!m_resourceUpdated);
    }

    virtual void RenderContextCallback(RenderContext& renderContext)
    {
        m_resourceUpdated = true;
    }

    std::atomic<bool>                   m_resourceUpdated;
};