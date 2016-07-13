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
        XCFloat3Unaligned(float xVal, float yVal, float zVal)
            : XCFloat2Unaligned(xVal, yVal)
            , z(zVal)
        {}

        float z;
    };

    class XCFloat3 : public XCFloat2
    {
    public:

        XCFloat3()
        {
            SetValues(0.0f, 0.0f, 0.0f);
        }

        explicit XCFloat3(PackedVector4& vector)
            : XCFloat2(vector)
        {}

        explicit XCFloat3(float x, float y, float z)
        {
            SetValues(x, y, z);
        }

        explicit XCFloat3(DirectX::XMVECTOR& vector)
            : XCFloat2(vector)
        {
            Set<Z>(DirectX::XMVectorGetZ(vector));
        }

        explicit XCFloat3(DirectX::XMFLOAT3& vector)
            : XCFloat2(vector)
        {
            Set<Z>(vector.z);
        }

        explicit XCFloat3(DirectX::XMFLOAT4& vector)
            : XCFloat2(vector)
        {
            Set<Z>(vector.z);
        }

        //Setters
        inline void SetValues(float x, float y, float z)
        {
            XCFloat2::SetValues(x, y);

            Set<Z>(z);

            Set<W>(0.0f);
        }

        //Operations
        inline XCFloat3 operator +(XCFloat3& otherObj)
        {
            return XCFloat3(Add(m_vector, otherObj.GetData()));
        }

        inline void operator +=(XCFloat3& otherObj)
        {
            (*this) = (*this) + otherObj;
        }

        inline XCFloat3 operator -(XCFloat3& otherObj)
        {
            return XCFloat3(Sub(m_vector, otherObj.GetData()));
        }

        inline void operator -=(XCFloat3& otherObj)
        {
            (*this) = (*this) - otherObj;
        }

        inline XCFloat3 operator *(XCFloat3& otherObj)
        {
            return XCFloat3(Mul(m_vector, otherObj.GetData()));
        }

        inline XCFloat3 operator *(float value)
        {
            return XCFloat3(Get<X>() * value, Get<Y>() * value, Get<Z>() * value);
        }

        inline void operator *=(XCFloat3& otherObj)
        {
            (*this) = (*this) * otherObj;
        }

        inline XCFloat3 operator /(XCFloat3& otherObj)
        {
            return XCFloat3(Div(m_vector, otherObj.GetData()));
        }

        inline void operator /=(XCFloat3& otherObj)
        {
            (*this) = (*this) / otherObj;
        }

        //Conversions
        XCFloat3Unaligned GetUnaligned3()
        {
            return XCFloat3Unaligned(Get<X>(), Get<Y>(), Get<Z>());
        }
    };

    inline XCFloat3 operator *(float& value, XCFloat3& obj)
    {
        return XCFloat3(obj.Get<X>() * value, obj.Get<Y>() * value, obj.Get<Z>() * value);
    }

    inline XCFloat3 operator /(float& value, XCFloat3& obj)
    {
        float divvalue = 1 / value;
        return divvalue * obj;
    }
}