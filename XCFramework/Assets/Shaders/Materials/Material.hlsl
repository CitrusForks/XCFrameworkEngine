/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

struct Material
{
    float4  Ambient;
    float4  Diffuse;
    float4  Specular;
    float4  Padding;
};

#endif