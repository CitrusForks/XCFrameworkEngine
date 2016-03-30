/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "stdafx.h"

#include "Engine/Graphics/XC_Lighting/LightingTypes.h"
#include "Engine/Graphics/XC_Materials/MaterialTypes.h"

struct cbWorld
{
    XCMatrix4Unaligned gWorld; 
};

struct cbView
{
    XCMatrix4Unaligned gView; 
};

struct cbProj
{
    XCMatrix4Unaligned gProj; 
};

struct cbWVP
{
    XCMatrix4Unaligned gWVP;
};

//LightTexture.hlsl
struct cbInvTransposeWorld
{
    XCMatrix4Unaligned gWorldInvTranspose;
};

struct cbLightsPerFrame
{
    DirectionalLight gDirLight;
    PointLight       gPointLight;
    SpotLight        gSpotLight;
    XCVec3Unaligned  gEyePosW;
};

struct cbMatTexPerObject
{
    XCMatrix4Unaligned         gTexTransform;
    BasicMaterial              gMaterial;
};

struct PerObjectBuffer
{
    XCMatrix4Unaligned    gWorld;
    XCMatrix4Unaligned    gWVP;
    XCMatrix4Unaligned    gWorldInvTranspose;
    XCMatrix4Unaligned    gTexTransform;
    BasicMaterial         gMaterial;
};

struct cbInstancedBuffer
{
    PerObjectBuffer       gPerObject[100];
};

struct cbBoneBuffer
{
    XCMatrix4Unaligned    gBoneMatrix[60];
};

struct cbVectorFontInstanced
{
    XCMatrix4Unaligned    gWVP[100];
};