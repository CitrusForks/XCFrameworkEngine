/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//LightingTypes

//1. Ambient: The amount of ambient light emitted by the light source.
//2. Diffuse : The amount of diffuse light emitted by the light source.
//3. Specular : The amount of specular light emitted by the light source.
//4. Direction : The direction of the light.
//5. Position : The position of the light.
//6. Range : The range of the light.A point whose distance from the light source is greater than the range is not lit.
//7. Attenuation : Stores the three attenuation constants in the format(a0, a1, a2) that control how light intensity
//falls off with distance.
//8. Spot : The exponent used in the spotlight calculation to control the spotlight cone.

#include "Graphics/Utils/GraphicUtils.h"

enum ELightType
{
    LIGHTTYPE_SOURCE,
    
    LIGHTTYPE_MAX
};

struct ILight
{
    //virtual ~ILight() {}
};

struct LightSource : public ILight
{
    XCVec4Unaligned            LightColor;  //Color of the light that emits
    XCVec4Unaligned            Direction;   //Direction the light is pointing at
    XCVec4Unaligned            Intensity;   //Intensity of light in all 3 axis
    XCVec4Unaligned            Position;    //Physical position of light
};

struct DirectionalLight : public ILight
{
    DirectionalLight() 
    {
        ZeroMemory(this, sizeof(this));
    }

    XCVec4Unaligned            Ambient;
    XCVec4Unaligned            Diffuse;
    XCVec4Unaligned            Specular;
    XCVec3Unaligned            Direction;
};

struct PointLight : public ILight
{
    PointLight() 
    {
        ZeroMemory(this, sizeof(this));
    }

    XCVec4Unaligned            Ambient;
    XCVec4Unaligned            Diffuse;
    XCVec4Unaligned            Specular;

    //Packed in 4D vector : (Position, Range)
    XCVec3Unaligned            Position;
    f32                        Range;

    //Packed into 4D vector: (A0, A1, A2)
    XCVec3Unaligned            Att;
};

struct SpotLight : public ILight
{
    SpotLight()
    {
        ZeroMemory(this, sizeof(this));
    }

    XCVec4Unaligned            Ambient;
    XCVec4Unaligned            Diffuse;
    XCVec4Unaligned            Specular;

    //Packed into 4D vector : (Position, Range)
    XCVec3Unaligned            Position;
    f32                        Range;

    //Packed into 4D : (Direction, Spot)
    XCVec3Unaligned            Direction;
    f32                        Spot;

    //Packed into 4D vector: (A0, A1, A2)
    XCVec3Unaligned            Att;
};