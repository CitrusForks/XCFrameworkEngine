/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(DEBUG_PHYSICS_OBB)

#include "DebugInterface/IDebugFeature.h"
#include "Graphics/IRenderableObject.h"
#include "Graphics/XCMaterials/MaterialTypes.h"

class ResourceHandle;
class RenderableOBB;

class DebugPhysicsOBB : public IDebugFeature, public IRenderableObject
{
public:
    DECLARE_OBJECT_CREATION(DebugPhysicsOBB);

    DebugPhysicsOBB();
    ~DebugPhysicsOBB();

    virtual void Init() override;
    virtual void Update(float dt) override;
    virtual bool Draw(RenderContext& context) override;
    virtual void Destroy() override;

private:
    struct OBBWorldInfo
    {
        XCMatrix4   m_world;
    };

    ResourceHandle*                 m_cubeMesh;
    Material                        m_material;
    std::vector<OBBWorldInfo>       m_renderableOBBs;
};

#endif