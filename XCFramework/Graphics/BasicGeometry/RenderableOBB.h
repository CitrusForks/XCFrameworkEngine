/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if !defined(XC_ORBIS)
#include <DirectXCollision.h>
#endif

#include "Engine/ObjectBase/IObjectDimensions.h"
#include "Engine/Resource/ResourceHandle.h"
#include "Graphics/BasicGeometry/OrientedBoundingBox.h"
#include "Graphics/XC_Materials/MaterialTypes.h"

class RenderableOBB : public IObjectDimensions, public OrientedBoundingBox
{
public:
    RenderableOBB();
    virtual ~RenderableOBB();

    virtual void               Init();
    virtual void               Update(float dt);
    virtual void               Draw(RenderContext& context);
    virtual void               Destroy();

private:

#if defined(DEBUG_OBB)
    ResourceHandle*            m_cubeMesh;
#endif

    BasicMaterial              m_material;
};