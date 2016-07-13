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

        explicit XCFloat2(PackedVector4& vector)
        {
            m_vector = vector;
        }

        explicit XCFloat2(float x, float y)
        {
            SetValues(x, y);
        }

        explicit XCFloat2(DirectX::XMVECTOR& vector)
        {
            Set<X>(DirectX::XMVectorGetX(vector));
            Set<Y>(DirectX::XMVectorGetY(vector));
        }

        explicit XCFloat2(DirectX::XMFLOAT3& vector)
        {
            Set<X>(vector.x);
            Set<Y>(vector.y);
        }

        explicit XCFloat2(DirectX::XMFLOAT4& vector)
        {
            Set<X>(vector.x);
            Set<Y>(vector.y);
        }

        //Setters
        inline void SetValues(float x, float y)
        {
            Set<X>(x);
            Set<Y>(y);

            //Unused
            Set<Z>(0.0f);
            Set<W>(0.0f);
        }

        //Getters
        template<VectorComponents component>
        inline float Get()
        {
            return m_vector[component];
        }

        //Setters
        template<VectorComponents component>
        inline void Set(float value)
        {
            m_vector[component] = value;
        }

        inline PackedVector4& GetData()
        {
            return m_vector;
        }

        //Operations
        inline float& operator [](unsigned int index)
        {
            return m_vector[index];
        }

        inline XCFloat2 operator +(XCFloat2& otherObj)
        {
            return XCFloat2(Add(m_vector, otherObj.GetData()));
        }

        inline void operator +=(XCFloat2& otherObj)
        {
            (*this) = (*this) + otherObj;
        }

        inline XCFloat2 operator -(XCFloat2& otherObj)
        {
            return XCFloat2(Sub(m_vector, otherObj.GetData()));
        }

        inline void operator -=(XCFloat2& otherObj)
        {
            (*this) = (*this) - otherObj;
        }

        inline XCFloat2 operator *(XCFloat2& otherObj)
        {
            return XCFloat2(Mul(m_vector, otherObj.GetData()));
        }

        inline XCFloat2 operator *(float value)
        {
            return XCFloat2(Get<X>() * value, Get<Y>() * value);
        }

        inline void operator *=(XCFloat2& otherObj)
        {
            (*this) = (*this) * otherObj;
        }

        inline XCFloat2 operator /(XCFloat2& otherObj)
        {
            return XCFloat2(Div(m_vector, otherObj.GetData()));
        }

        inline void operator /=(XCFloat2& otherObj)
        {
            (*this) = (*this) / otherObj;
        }

        //Conversions
        XCFloat2Unaligned GetUnaligned2()
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

    inline XCFloat2 operator *(float& value, XCFloat2& obj)
    {
        return XCFloat2(obj.Get<X>() * value, obj.Get<Y>() * value);
    }

    inline XCFloat2 operator /(float& value, XCFloat2& obj)
    {
        float divvalue = 1 / value;
        return divvalue * obj;
    }
}