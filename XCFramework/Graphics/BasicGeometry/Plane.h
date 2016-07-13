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
    Plane(XCVec4& p1, XCVec4& p2, XCVec4& p3);
    ~Plane();

    void       SetPlaneNormal(XCVec4& norm) { m_planeNormal = norm; }
    void       SetDConstant(XCVec4& value) { m_dVector = value; }

    XCVec4     GetPlaneNormal() { return m_planeNormal; }

protected:

    XCVec4     m_planeNormal;
    XCVec4     m_dVector;

    XCVec4     m_vertices[4];
};