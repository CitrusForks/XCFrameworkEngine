/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XCLighting/LightingTypes.h"
#include "Graphics/XCMaterials/MaterialTypes.h"

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
    LightSource         gLightSource[10];
    XCVec4Unaligned     gNoOfLights;
    XCVec4Unaligned     padding1;
    XCVec4Unaligned     padding2;
    XCVec4Unaligned     padding3;
};

struct cbMatTexPerObject
{
    XCMatrix4Unaligned         gTexTransform;
    Material                   gMaterial;
};

struct PerObjectBuffer
{
    XCMatrix4Unaligned    gWorld;
    XCMatrix4Unaligned    gWVP;
    XCMatrix4Unaligned    gWorldInvTranspose;
    XCMatrix4Unaligned    gTexTransform;
    Material              gMaterial;
};

struct cbInstancedBuffer
{
    PerObjectBuffer       gPerObject[100];
};

struct cbBoneBuffer
{
    XCMatrixUnaligned34   gBoneMatrix[100];
};

struct cbVectorFontInstanced
{
    XCMatrix4Unaligned    gWVP[100];
};