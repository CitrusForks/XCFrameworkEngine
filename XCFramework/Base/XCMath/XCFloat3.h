/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "XCFloat2.h"

namespace XCMath
{
    struct XCFloat3Unaligned : public XCFloat2Unaligned
    {
        XCFloat3Unaligned() {}
        XCFloat3Unaligned(f32 xVal, f32 yVal, f32 zVal)
            : XCFloat2Unaligned(xVal, yVal)
            , z(zVal)
        {}

        f32 z;
    };

    class XCFloat3 : public XCFloat2
    {
    public:

        XCFloat3()
        {
            SetValues(0.0f, 0.0f, 0.0f);
        }

        explicit XCFloat3(const PackedVector4& vector)
            : XCFloat2(vector)
        {}

        explicit XCFloat3(f32 x, f32 y, f32 z)
        {
            SetValues(x, y, z);
        }

        explicit XCFloat3(const DirectX::XMVECTOR& vector)
            : XCFloat2(vector)
        {
            Set<Z>(DirectX::XMVectorGetZ(vector));
        }

        explicit XCFloat3(const DirectX::XMFLOAT3& vector)
            : XCFloat2(vector)
        {
            Set<Z>(vector.z);
        }

        explicit XCFloat3(const DirectX::XMFLOAT4& vector)
            : XCFloat2(vector)
        {
            Set<Z>(vector.z);
        }

        //Setters
        inline void SetValues(f32 x, f32 y, f32 z)
        {
            XCFloat2::SetValues(x, y);

            Set<Z>(z);

            Set<W>(0.0f);
        }

        //Operations
        inline const XCFloat3 operator +(const XCFloat3& otherObj)
        {
            return XCFloat3(Add(m_vector, otherObj.GetData()));
        }

        inline void operator +=(const XCFloat3& otherObj)
        {
            (*this) = (*this) + otherObj;
        }

        inline const XCFloat3 operator -(const XCFloat3& otherObj)
        {
            return XCFloat3(Sub(m_vector, otherObj.GetData()));
        }

        inline void operator -=(const XCFloat3& otherObj)
        {
            (*this) = (*this) - otherObj;
        }

        inline const XCFloat3 operator *(const XCFloat3& otherObj)
        {
            return XCFloat3(Mul(m_vector, otherObj.GetData()));
        }

        inline const XCFloat3 operator *(f32 value)
        {
            return XCFloat3(Get<X>() * value, Get<Y>() * value, Get<Z>() * value);
        }

        inline void operator *=(const XCFloat3& otherObj)
        {
            (*this) = (*this) * otherObj;
        }

        inline const XCFloat3 operator /(const XCFloat3& otherObj)
        {
            return XCFloat3(Div(m_vector, otherObj.GetData()));
        }

        inline void operator /=(const XCFloat3& otherObj)
        {
            (*this) = (*this) / otherObj;
        }

        //Conversions
        XCFloat3Unaligned GetUnaligned3()
        {
            return XCFloat3Unaligned(Get<X>(), Get<Y>(), Get<Z>());
        }
    };
}