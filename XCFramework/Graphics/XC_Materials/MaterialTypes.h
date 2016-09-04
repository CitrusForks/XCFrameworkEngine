/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

struct Material
{
    XCVec4        Ambient;
    XCVec4        Diffuse;
    XCVec4        Specular;
    XCVec4        Padding;

    Material() :
        Ambient(0, 0, 0, 0),
        Diffuse(0, 0, 0, 0),
        Specular(0, 0, 0, 0),
        Padding(0, 0, 0, 0)
    { }

    Material(const XCVec4& ambient, const XCVec4& diffuse, const XCVec4& specular) :
        Ambient(ambient),
        Diffuse(diffuse),
        Specular(specular),
        Padding(0, 0, 0, 0)
    { }
};