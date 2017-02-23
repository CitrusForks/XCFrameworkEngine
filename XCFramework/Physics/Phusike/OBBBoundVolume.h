/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include <DirectXCollision.h>

#include "IPhysicsBoundVolume.h"

class OBBBoundVolume : public IPhysicsBoundVolume
{
public:
    static const u32              MaxOBBCornerPoints = 8;

    OBBBoundVolume(const PhysicsDesc::PhysicsBoundBoxDesc& obbDesc);
    ~OBBBoundVolume();

    void                          Update(f32 dt) override;
    void                          Transform(const XCVec4& translate, const XCVec4& orientation) override;

    DirectX::ContainmentType      Contains(const OBBBoundVolume* obb) const;
    DirectX::ContainmentType      Contains(const XCVec4& point) const;

    bool                          Intersects(const OBBBoundVolume* obb) const;
    
    DirectX::BoundingOrientedBox  GetTransformedBox() const { return m_TransformedBox; }

#if defined(DEBUG_PHYSICS_OBB)
    void GetOBBInfo(std::vector<PhysicsPlayground::OBBInfo>& outInfo) const override;
#endif

protected:
    void                          CreateBoundBox();
    void                          GenerateBoundBox(const XCVec4& min, const XCVec4& max);
    void                          GenerateBoundBox(const OBBBoundVolume* const boundBox);
    void                          Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix);

private:
    //Oriented Bounding Box
    DirectX::BoundingOrientedBox  m_bBox;
    DirectX::BoundingOrientedBox  m_TransformedBox;

    XCVec4                        m_boxCenter;
    XCVec4                        m_boxExtends;
};