/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XC_ORBIS)

#include "XCMathIncludesOrbis.h"

#else

//#define USE_DIRECTX_MATH

#if defined(USE_DIRECTX_MATH)
#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>
#else

#include <DirectXMath.h>

#include "XCIntrincis.inl"
#include "XCFloat2.h"
#include "XCFloat3.h"
#include "XCFloat4.h"
#include "XCMatrix.h"
#endif

#include "Assimp/include/assimp/matrix4x4.h"
#endif

#if defined(USE_DIRECTX_MATH)

typedef DirectX::XMFLOAT2    XCVec2;
typedef DirectX::XMFLOAT3    XCVec3;
typedef DirectX::XMFLOAT4    XCVec4;
typedef DirectX::XMMATRIX    XCMatrix4;
typedef DirectX::XMMATRIX    XCMatrix4Unaligned;
typedef DirectX::XMVECTOR    XCVec4;
typedef XCMatrix4            XCMatrix4Unaligned;
typedef XCVec3               XCVec3Unaligned;
typedef XCVec4               XCVec4Unaligned;
typedef XCVec2               XCVec2Unaligned;

#else

//Common types aligned with 16bytes
typedef XCMath::XCFloat2              XCVec2;
typedef XCMath::XCFloat3              XCVec3;
typedef XCMath::XCFloat4              XCVec4;
typedef XCMath::XCMatrix              XCMatrix4;

//Unaligned versions for gpu structures which are 4byte aligned and not 16-byte aligned.
typedef XCMath::XCFloat2Unaligned     XCVec2Unaligned;
typedef XCMath::XCFloat3Unaligned     XCVec3Unaligned;
typedef XCMath::XCFloat4Unaligned     XCVec4Unaligned;
typedef XCMath::XCMatrixUnaligned     XCMatrix4Unaligned;

static const float XC_PI        = 3.141592654f;
static const float XC_2PI       = 6.283185307f;
static const float XC_1DIVPI    = 0.318309886f;
static const float XC_1DIV2PI   = 0.159154943f;
static const float XC_PIDIV2    = 1.570796327f;
static const float XC_PIDIV4    = 0.785398163f;

using namespace XCMath;

#endif

//Complex operations
inline XCVec4 CreatePlaneFromPoints(XCVec4& p1, XCVec4& p2, XCVec4& p3)
{
    XCVec4 v1(VectorNormalize<3>(p1.GetData()));
    XCVec4 v2(VectorNormalize<3>(p2.GetData()));
    XCVec4 v3(VectorNormalize<3>(p3.GetData()));

    XCVec4 axis1 = v1 - v2; // from 1 to 0
    XCVec4 axis2 = v3 - v2; // from 1 to 2

    return XCVec4(VectorNormalize<3>(VectorCross(axis1, axis2).GetData()));
}

inline XCVec4 GetNormalFromPoints(XCVec4& v1, XCVec4& v2, XCVec4& v3)
{
    //Find the 2 axis for v1 vertex
    XCVec4 axis1 = v2 - v1;
    XCVec4 axis2 = v3 - v1;

    //Normalize the axis
    VectorNormalize<3>(axis1.GetData());
    VectorNormalize<3>(axis2.GetData());

    //Now find the cross product of both the axis
    return XCVec4(VectorNormalize<3>(VectorCross(axis1, axis2).GetData()));
}

inline XCMatrix4Unaligned aiMatrixToMatrix4Unaligned(const aiMatrix4x4& matrix)
{
    XCMatrix4Unaligned transformation = XCMatrix4().GetUnaligned();
    transformation.r1.x = matrix.a1; transformation.r1.y = matrix.a2; transformation.r1.z = matrix.a3; transformation.r1.w = matrix.a4;
    transformation.r2.x = matrix.b1; transformation.r2.y = matrix.b2; transformation.r2.z = matrix.b3; transformation.r2.w = matrix.b4;
    transformation.r3.x = matrix.c1; transformation.r3.y = matrix.c2; transformation.r3.z = matrix.c3; transformation.r3.w = matrix.c4;
    transformation.r4.x = matrix.d1; transformation.r4.y = matrix.d2; transformation.r4.z = matrix.d3; transformation.r4.w = matrix.d4;

    return transformation;
}

inline XCMatrix4 aiMatrixToMatrix4(const aiMatrix4x4& matrix)
{
    XCMatrix4 transformation;
    transformation[0][0] = matrix.a1; transformation[0][1] = matrix.a2; transformation[0][2] = matrix.a3; transformation[0][3] = matrix.a4;
    transformation[1][0] = matrix.b1; transformation[1][1] = matrix.b2; transformation[1][2] = matrix.b3; transformation[1][3] = matrix.b4;
    transformation[2][0] = matrix.c1; transformation[2][1] = matrix.c2; transformation[2][2] = matrix.c3; transformation[2][3] = matrix.c4;
    transformation[3][0] = matrix.d1; transformation[3][1] = matrix.d2; transformation[3][2] = matrix.d3; transformation[3][3] = matrix.d4;

    return transformation;
}