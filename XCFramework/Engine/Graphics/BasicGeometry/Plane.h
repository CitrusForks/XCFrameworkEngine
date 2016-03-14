/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class Plane
{
public:
    Plane();
    Plane(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3);
    ~Plane();

    void                SetPlaneNormal(XCVecIntrinsic4 norm) { m_planeNormal = norm; }
    void                SetDConstant(XCVecIntrinsic4 value) { m_dVector = value; }

    XCVecIntrinsic4            GetPlaneNormal() { return m_planeNormal; }

protected:
    XCVecIntrinsic4            m_planeNormal;
    XCVecIntrinsic4            m_dVector;

    XCVecIntrinsic4            m_vertices[4];
};