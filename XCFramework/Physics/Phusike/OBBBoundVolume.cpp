/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "OBBBoundVolume.h"

OBBBoundVolume::OBBBoundVolume(const PhysicsDesc::PhysicsBoundBoxDesc& obbDesc)
    : IPhysicsBoundVolume(PhysicsBoundType_Box)
{
    GenerateBoundBox(obbDesc.m_min, obbDesc.m_max);
}

OBBBoundVolume::~OBBBoundVolume()
{
}

void OBBBoundVolume::Update(f32 dt)
{
}

void OBBBoundVolume::CreateBoundBox()
{
    XCVec4 orientation = QuaternionRotationMatrix(XCMatrix());
    m_bBox = DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(m_boxCenter.Get<X>(), m_boxCenter.Get<Y>(), m_boxCenter.Get<Z>()),
                                          DirectX::XMFLOAT3(m_boxExtends.Get<X>(), m_boxExtends.Get<Y>(), m_boxExtends.Get<Z>()),
                                          DirectX::XMFLOAT4(orientation.Get<X>(), orientation.Get<Y>(), orientation.Get<Z>(), orientation.Get<W>()));
}

void OBBBoundVolume::GenerateBoundBox(const XCVec4& min, const XCVec4& max)
{
    m_boxCenter = 0.5f * (min + max);
    m_boxExtends = 0.5f * (max - min);
    CreateBoundBox();
}

void OBBBoundVolume::GenerateBoundBox(const OBBBoundVolume* boundBox)
{
    m_boxCenter = XMLoadFloat3(&boundBox->m_bBox.Center);
    m_boxExtends = XMLoadFloat3(&boundBox->m_bBox.Extents);
    CreateBoundBox();
}

void OBBBoundVolume::Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix)
{
    XCVec4 rotate = QuaternionRotationMatrix(rotateMatrix);

    m_bBox.Transform(m_TransformedBox, 1.0f, rotate.GetPlatformIntrinsic(), translateMat[3].GetPlatformIntrinsic());
}

DirectX::ContainmentType OBBBoundVolume::Contains(const OBBBoundVolume* obb) const
{
    return m_TransformedBox.Contains(obb->GetTransformedBox());
}

DirectX::ContainmentType OBBBoundVolume::Contains(const XCVec4& point) const
{
    return m_TransformedBox.Contains(point.GetPlatformIntrinsic());
}

bool OBBBoundVolume::Intersects(const OBBBoundVolume* obb) const
{
    return m_TransformedBox.Intersects(obb->GetTransformedBox());
}

void OBBBoundVolume::Transform(const XCVec4& translate, const XCVec4& orientation)
{
    m_bBox.Transform(m_TransformedBox, 1.0f, XCVec4(0, 0, 0, 1).GetPlatformIntrinsic(), translate.GetPlatformIntrinsic());
}

#if defined(DEBUG_PHYSICS_OBB)
void OBBBoundVolume::GetOBBInfo(std::vector<PhysicsPlayground::OBBInfo>& outInfo) const
{
    PhysicsPlayground::OBBInfo info = {
        XCVec4(m_TransformedBox.Extents),
        XCVec4(m_TransformedBox.Center),
        XCVec4(m_TransformedBox.Orientation),
    };
    outInfo.push_back(info);
}
#endif