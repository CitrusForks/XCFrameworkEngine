/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "IPhysicsBoundVolume.h"

class HeightfieldOBBHierarchy;

class HeightfieldBoundVolume : public IPhysicsBoundVolume
{
public:
    HeightfieldBoundVolume(const PhysicsDesc::PhysicsHeightFieldDesc& heightfieldDesc);
    ~HeightfieldBoundVolume();

    void        Update(f32 dt) override;
    void        Transform(const XCVec4& translate, const XCVec4& orientation) override;

    XCVec4      GetPositionOfObject(const IPhysicsBoundVolume* volume) const;

private:
    HeightfieldOBBHierarchy*    m_OBBHierarchy;
};