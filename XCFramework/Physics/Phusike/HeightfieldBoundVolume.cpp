/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "HeightfieldBoundVolume.h"
#include "HeightfieldOBBHierarchy.h"

HeightfieldBoundVolume::HeightfieldBoundVolume(const PhysicsDesc::PhysicsHeightFieldDesc& heightfieldDesc)
    : IPhysicsBoundVolume(PhysicsBoundType_HeightField)
{
    m_OBBHierarchy = XCNEW(HeightfieldOBBHierarchy)(heightfieldDesc);
    m_OBBHierarchy->CreateHeightfieldOBBHierarchy();
}

HeightfieldBoundVolume::~HeightfieldBoundVolume()
{
    XCDELETE(m_OBBHierarchy);
}

void HeightfieldBoundVolume::Update(f32 dt)
{
    m_OBBHierarchy->Update(dt);
}

void HeightfieldBoundVolume::Transform(const XCVec4& translate, const XCVec4& orientation)
{
    m_OBBHierarchy->Transform(translate, orientation);
}

XCVec4 HeightfieldBoundVolume::GetPositionOfContact(const IPhysicsBoundVolume* volume) const
{
    return m_OBBHierarchy->GetPointOfContactWithOBB(volume);
}

#if defined(DEBUG_PHYSICS_OBB)
void HeightfieldBoundVolume::GetOBBInfo(std::vector<PhysicsPlayground::OBBInfo>& outInfo) const
{
    m_OBBHierarchy->GetAllOBBInfo(outInfo);
}
#endif
