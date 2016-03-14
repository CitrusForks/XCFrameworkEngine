/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XC_ORBIS)

#include "XCMathIncludesOrbis.h"

#else

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>
#include "Libs/Assimp/include/assimp/matrix4x4.h"

typedef DirectX::XMFLOAT2    XCVec2;
typedef DirectX::XMFLOAT3    XCVec3;
typedef DirectX::XMFLOAT4    XCVec4;
typedef DirectX::XMMATRIX    XCMatrix4;
typedef DirectX::XMMATRIX    XCMatrix4Unaligned;
typedef DirectX::XMVECTOR    XCVecIntrinsic4;
typedef XCMatrix4            XCMatrix4Unaligned;
typedef XCVec3               XCVec3Unaligned;
typedef XCVec4               XCVec4Unaligned;
typedef XCVec2               XCVec2Unaligned;

inline XCMatrix4Unaligned ToXCMatrix4Unaligned(const XCMatrix4& r)
{
    return r;
}

inline XCVecIntrinsic4 XMLoadFloat3(const XCVec3 pSource)
{
    XCVecIntrinsic4 out = DirectX::XMLoadFloat3(&pSource);
    return out;
}

inline XCVec4Unaligned ToXCVec4Unaligned(const XCVec4& r)
{
    return r;
}


inline XCVec3Unaligned ToXCVec3Unaligned(const XCVec3& r)
{
    return r;
}

inline XCVec3Unaligned ToXCVec3Unaligned(const XCVec4& r)
{
    const XCVec3Unaligned result = { r.x, r.y, r.z };
    return result;
}

inline XCVec3 ToXCVec3(const XCVec3Unaligned& r)
{
    return XCVec3(r.x, r.y, r.z);
}

#endif

using namespace DirectX;

inline XCVecIntrinsic4 Float3ToVec(XCVec3 value)
{
    XCVecIntrinsic4 out = XMLoadFloat3(&value);
    return out;
}

inline XCVecIntrinsic4 toXMVECTOR(float x, float y, float z, float w)
{
    XCVecIntrinsic4 t = XMVectorSet(x, y, z, w);
    return t;
}

inline XCVec3 XMVectorToXMFloat3(XCVecIntrinsic4* value)
{
    XCVec3 out;
    XMStoreFloat3(&out, *value);
    return out;
}

inline XCMatrix4 InverseTranspose(CXMMATRIX M)
{
    // Inverse-transpose is just applied to normals.  So zero out 
    // translation row so that it doesn't get into our inverse-transpose
    // calculation--we don't want the inverse-transpose of the translation.
    XCMatrix4 A = M;

#if defined(WIN32)
    A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
#elif defined(XC_ORBIS)
    A.setCol3(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
#endif

    XCVecIntrinsic4 det = XMMatrixDeterminant(A);
    return XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
}


inline XCMatrix4Unaligned aiMatrixToMatrix4Unaligned(const aiMatrix4x4 matrix)
{
    XCMatrix4Unaligned transformation = XMMatrixIdentity();
    transformation._11 = matrix.a1; transformation._12 = matrix.a2; transformation._13 = matrix.a3; transformation._14 = matrix.a4;

    transformation._21 = matrix.b1; transformation._22 = matrix.b2; transformation._23 = matrix.b3; transformation._24 = matrix.b4;
    
    transformation._31 = matrix.c1; transformation._32 = matrix.c2; transformation._33 = matrix.c3; transformation._34 = matrix.c4;
    
    transformation._41 = matrix.d1; transformation._42 = matrix.d2; transformation._43 = matrix.d3; transformation._44 = matrix.d4;

    return transformation;
}