/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "Plane.h"

Plane::Plane()
{
}

Plane::~Plane()
{
}

Plane::Plane(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3)
{
    XCVecIntrinsic4 v1 = XMVector3Normalize(p1);
    XCVecIntrinsic4 v2 = XMVector3Normalize(p2);
    XCVecIntrinsic4 v3 = XMVector3Normalize(p3);

    XCVecIntrinsic4 axis1 = v1 - v2; // from 1 to 0
    XCVecIntrinsic4 axis2 = v3 - v2; // from 1 to 2

    m_planeNormal = XMVector3Normalize(XMVector3Cross(axis1, axis2));
    m_dVector = XMVector3Dot(m_planeNormal, p1);

    m_vertices[0] = p1;
    m_vertices[1] = p2;
    m_vertices[2] = p3;
    m_vertices[3] = p1;
}