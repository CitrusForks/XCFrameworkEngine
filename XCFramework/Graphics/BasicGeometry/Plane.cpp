/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "Plane.h"

Plane::Plane()
{
}

Plane::~Plane()
{
}

Plane::Plane(XCVec4& p1, XCVec4& p2, XCVec4& p3)
{
    XCVec4 v1 = VectorNormalize<3>(p1);
    XCVec4 v2 = VectorNormalize<3>(p2);
    XCVec4 v3 = VectorNormalize<3>(p3);

    XCVec4 axis1 = v1 - v2; // from 1 to 0
    XCVec4 axis2 = v3 - v2; // from 1 to 2

    m_planeNormal = VectorNormalize<3>(VectorCross(axis1, axis2));
    float dotVal = VectorDot(m_planeNormal, p1);
    
    m_dVector = XCVec4(dotVal, dotVal, dotVal, dotVal);

    m_vertices[0] = p1;
    m_vertices[1] = p2;
    m_vertices[2] = p3;
    m_vertices[3] = p1;
}