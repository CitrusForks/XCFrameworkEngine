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
#include "Assimp/include/assimp/matrix4x4.h"

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

inline XCVec4 ToXCVec4(const XCVec4Unaligned& r)
{
    return XCVec4(r.x, r.y, r.z, r.w);
}

inline float XMVec3CosineDot(XCVecIntrinsic4& vec1, XCVecIntrinsic4& vec2)
{
    XCVec3 v1, v2;
    XMStoreFloat3(&v1, vec1);
    XMStoreFloat3(&v2, vec2);

#if defined(WIN32)
    return acosf(XMVectorGetY(XMVector3Dot(vec1, vec2)) / (sqrt(((v1.x*v1.x) + (v1.y*v1.y) + (v1.z*v1.z))*((v2.x*v2.x) + (v2.y*v2.y) + (v2.z*v2.z)))));
#elif defined(XC_ORBIS)
    return acosf(XMVectorGetY(XMVector3Dot(vec1, vec2)) / (sqrt(((v1.getX()*v1.getX()) + (v1.getY()*v1.getY()) + (v1.getZ()*v1.getZ()))*((v2.getX()*v2.getX()) + (v2.getY()*v2.getY()) + (v2.getZ()*v2.getZ())))));
#endif
}

inline XCVecIntrinsic4 CreatePlaneFromPoints(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3)
{
    XCVecIntrinsic4 v1 = XMVector3Normalize(p1);
    XCVecIntrinsic4 v2 = XMVector3Normalize(p2);
    XCVecIntrinsic4 v3 = XMVector3Normalize(p3);

    XCVecIntrinsic4 axis1 = v1 - v2; // from 1 to 0
    XCVecIntrinsic4 axis2 = v3 - v2; // from 1 to 2

    return XMVector3Normalize(XMVector3Cross(axis1, axis2));
}

inline XCVecIntrinsic4 GetNormalFromPoints(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2, XCVecIntrinsic4 v3)
{
    //Find the 2 axis for v1 vertex
    XCVecIntrinsic4 axis1 = v2 - v1;
    XCVecIntrinsic4 axis2 = v3 - v1;

    //Normalize the axis
    XMVector3Normalize(axis1);
    XMVector3Normalize(axis2);

    //Now find the cross product of both the axis
    return XMVector3Normalize(XMVector3Cross(axis1, axis2));
}

inline bool IsVectorEqual(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2)
{
    if (XMVectorGetX(v1) > XMVectorGetX(v2) || XMVectorGetX(v1) < XMVectorGetX(v2)) return false;
    if (XMVectorGetY(v1) > XMVectorGetY(v2) || XMVectorGetY(v1) < XMVectorGetY(v2)) return false;
    if (XMVectorGetZ(v1) > XMVectorGetZ(v2) || XMVectorGetZ(v1) < XMVectorGetZ(v2)) return false;

    return true;
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