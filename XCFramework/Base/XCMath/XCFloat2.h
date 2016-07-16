/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "XCIntrincis.inl"

namespace XCMath
{
    enum VectorComponents
    {
        X,
        Y,
        Z,
        W
    };

    struct XCFloat2Unaligned
    {
        XCFloat2Unaligned() {}
        XCFloat2Unaligned(float xVal, float yVal)
            : x(xVal), y(yVal)
        {}

        float x, y;
    };

    class XCFloat2
    {
    public:

        XCFloat2()
        {
            SetValues(0.0f, 0.0f);
        }

        explicit XCFloat2(const PackedVector4& vector)
        {
            m_vector = vector;
        }

        explicit XCFloat2(float x, float y)
        {
            SetValues(x, y);
        }

        explicit XCFloat2(const DirectX::XMVECTOR& vector)
        {
            Set<X>(DirectX::XMVectorGetX(vector));
            Set<Y>(DirectX::XMVectorGetY(vector));
        }

        explicit XCFloat2(const DirectX::XMFLOAT3& vector)
        {
            Set<X>(vector.x);
            Set<Y>(vector.y);
        }

        explicit XCFloat2(const DirectX::XMFLOAT4& vector)
        {
            Set<X>(vector.x);
            Set<Y>(vector.y);
        }

        //Getters
        template<VectorComponents component>
        inline float Get() const
        {
            return m_vector[component];
        }

        inline PackedVector4 GetData() const
        {
            return m_vector;
        }

        //Setters
        template<VectorComponents component>
        inline void Set(float value)
        {
            m_vector[component] = value;
        }

        inline void SetValues(float x, float y)
        {
            Set<X>(x);
            Set<Y>(y);

            //Unused
            Set<Z>(0.0f);
            Set<W>(0.0f);
        }

        //Operations
        inline const float operator [](unsigned int index) const
        {
            return m_vector[index];
        }

        inline float& operator [](unsigned int index)
        {
            return m_vector[index];
        }

        inline const XCFloat2 operator +(const XCFloat2& otherObj) const
        {
            return XCFloat2(Add(m_vector, otherObj.GetData()));
        }

        inline void operator +=(const XCFloat2& otherObj)
        {
            (*this) = (*this) + otherObj;
        }

        inline const XCFloat2 operator -(const XCFloat2& otherObj) const
        {
            return XCFloat2(Sub(m_vector, otherObj.GetData()));
        }

        inline void operator -=(const XCFloat2& otherObj)
        {
            (*this) = (*this) - otherObj;
        }

        inline const XCFloat2 operator *(const XCFloat2& otherObj) const
        {
            return XCFloat2(Mul(m_vector, otherObj.GetData()));
        }

        inline const XCFloat2 operator *(float value) const
        {
            return XCFloat2(Get<X>() * value, Get<Y>() * value);
        }

        inline void operator *=(const XCFloat2& otherObj)
        {
            (*this) = (*this) * otherObj;
        }

        inline XCFloat2 operator /(const XCFloat2& otherObj)
        {
            return XCFloat2(Div(m_vector, otherObj.GetData()));
        }

        inline void operator /=(const XCFloat2& otherObj)
        {
            (*this) = (*this) / otherObj;
        }

        //Conversions
        XCFloat2Unaligned GetUnaligned2() const
        {
            return XCFloat2Unaligned(Get<X>(), Get<Y>());
        }

    protected:
        PackedVector4        m_vector;
    };

    inline bool operator ==(const XCFloat2& obj1, const XCFloat2& obj2)
    {
        XCFloat2 otherObj1 = obj1;
        XCFloat2 otherObj2 = obj2;

        return otherObj1.Get<X>() == otherObj2.Get<X>() && otherObj1.Get<Y>() == otherObj2.Get<Y>();
    }
}