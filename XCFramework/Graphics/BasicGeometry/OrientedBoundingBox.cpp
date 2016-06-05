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

void OrientedBoundingBox::CreateBoundBox()
{
    XCVecIntrinsic4 orientation = XMQuaternionRotationMatrix(XMMatrixIdentity());
    XCVec4 orient;
    XMStoreFloat4(&orient, orientation);
    m_bBox = BoundingOrientedBox(XMVectorToXMFloat3(&m_boxCenter), XMVectorToXMFloat3(&m_boxExtends), orient);
}

void OrientedBoundingBox::CreateBoundBox(XCVecIntrinsic4 min, XCVecIntrinsic4 max)
{
    m_boxCenter = 0.5f * (min + max);
    m_boxExtends = 0.5f * (max - min);
    CreateBoundBox();
}

void OrientedBoundingBox::CreateBoundBox(OrientedBoundingBox* const boundBox)
{
    m_boxCenter = XMLoadFloat3(&boundBox->m_bBox.Center);
    m_boxExtends = XMLoadFloat3(&boundBox->m_bBox.Extents);
    CreateBoundBox();
}

void OrientedBoundingBox::Transform(XCMatrix4 translateMat, XCMatrix4 rotateMatrix)
{
    BoundingOrientedBox box;
    XCVecIntrinsic4 rotate = XMQuaternionRotationMatrix(rotateMatrix);

#if defined(XC_ORBIS)
    m_bBox.Transform(m_TransformedBox, 1.0f, rotate, translateMat.getRow(3));
#else
    m_bBox.Transform(m_TransformedBox, 1.0f, rotate, translateMat.r[3]);
#endif
}