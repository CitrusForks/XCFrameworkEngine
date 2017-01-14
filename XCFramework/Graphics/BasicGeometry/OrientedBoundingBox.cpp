/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "OrientedBoundingBox.h"

OrientedBoundingBox::OrientedBoundingBox()
{
}

OrientedBoundingBox::~OrientedBoundingBox()
{
}

void OrientedBoundingBox::Init()
{
}

void OrientedBoundingBox::GenerateBoundBox()
{
    XCVec4 orientation = QuaternionRotationMatrix(XCMatrix());
    m_bBox = DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(m_boxCenter.Get<X>(), m_boxCenter.Get<Y>(), m_boxCenter.Get<Z>()),
        DirectX::XMFLOAT3(m_boxExtends.Get<X>(), m_boxExtends.Get<Y>(), m_boxExtends.Get<Z>()),
        DirectX::XMFLOAT4(orientation.Get<X>(), orientation.Get<Y>(), orientation.Get<Z>(), orientation.Get<W>()));
}

void OrientedBoundingBox::GenerateBoundBox(XCVec4& min, XCVec4& max)
{
    m_boxCenter = 0.5f * (min + max);
    m_boxExtends = 0.5f * (max - min);
    GenerateBoundBox();
}

void OrientedBoundingBox::GenerateBoundBox(OrientedBoundingBox* const boundBox)
{
    m_boxCenter = XMLoadFloat3(&boundBox->m_bBox.Center);
    m_boxExtends = XMLoadFloat3(&boundBox->m_bBox.Extents);
    GenerateBoundBox();
}

void OrientedBoundingBox::Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix)
{
    DirectX::BoundingOrientedBox box;
    XCVec4 rotate = QuaternionRotationMatrix(rotateMatrix);

    m_bBox.Transform(m_TransformedBox, 1.0f, rotate.GetPlatformIntrinsic(), translateMat[3].GetPlatformIntrinsic());
}