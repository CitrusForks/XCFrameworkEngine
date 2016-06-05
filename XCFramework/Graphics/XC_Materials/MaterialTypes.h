/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

struct BasicMaterial
{
    XCVec4        Ambient;
    XCVec4        Diffuse;
    XCVec4        Specular;

    BasicMaterial() :
        Ambient(0, 0, 0, 0),
        Diffuse(0, 0, 0, 0),
        Specular(0, 0, 0, 0)
    { }

    BasicMaterial(XCVec4 ambient, XCVec4 diffuse, XCVec4 specular) :
        Ambient(ambient),
        Diffuse(diffuse),
        Specular(specular)
    { }
};