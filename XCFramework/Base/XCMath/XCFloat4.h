/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "XCFloat3.h"

namespace XCMath
{
    struct XCFloat4Unaligned : public XCFloat3Unaligned
    {
        XCFloat4Unaligned() {}
        XCFloat4Unaligned(float xVal, float yVal, float zVal, float wVal)
            : XCFloat3Unaligned(xVal, yVal, zVal)
            , w(wVal)
        {}

        float w;
    };

    class XCFloat4 : public XCFloat3
    {
    public:
        static XCFloat4 XCFloat4ZeroVector;

        XCFloat4()
        {
            SetValues(0.0f, 0.0f, 0.0f, 0.0f);
        }

        explicit XCFloat4(const PackedVector4& vector)
            : XCFloat3(vector)
        {}

        explicit XCFloat4(float x, float y, float z, float w)
            : XCFloat3(x, y, z)
        {
            Set<W>(w);
        }

        explicit XCFloat4(const XCFloat3Unaligned& vector)
            : XCFloat3(vector.x, vector.y, vector.z)
        {
            Set<W>(0.0f);
        }

        explicit XCFloat4(const XCFloat4Unaligned& vector)
            : XCFloat3(vector.x, vector.y, vector.z)
        {
            Set<W>(vector.w);
        }

        explicit XCFloat4(const XCFloat3& vector)
            : XCFloat3(vector)
        {
            Set<W>(0.0f);
        }

        explicit XCFloat4(const DirectX::XMVECTOR& vector)
            : XCFloat3(vector)
        {
            Set<W>(DirectX::XMVectorGetW(vector));
        }

        explicit XCFloat4(const DirectX::XMFLOAT3& vector)
            : XCFloat3(vector)
        {
            Set<W>(0.0f);
        }

        explicit XCFloat4(const DirectX::XMFLOAT4& vector)
            : XCFloat3(vector)
        {
            Set<W>(vector.w);
        }

        //Setters
        inline void SetValues(float x, float y, float z, float w)
        {
            XCFloat3::SetValues(x, y, z);
            Set<W>(w);
        }

        //Operations
        inline const XCFloat4 operator +(const XCFloat4& otherObj) const
        {
            return XCFloat4(Add(m_vector, otherObj.GetData()));
        }

        inline void operator +=(const XCFloat4& otherObj)
        {
            (*this) = (*this) + otherObj;
        }

        inline const XCFloat4 operator -() const
        {
            XCFloat4 out(m_vector[0] * -1, m_vector[1] * -1, m_vector[2] * -1, m_vector[3] * -1);
            return out;
        }

        inline const XCFloat4 operator -(const XCFloat4& otherObj) const
        {
            return XCFloat4(Sub(m_vector, otherObj.GetData()));
        }

        inline void operator -=(const XCFloat4& otherObj)
        {
            (*this) = (*this) - otherObj;
        }

        inline const XCFloat4 operator *(const XCFloat4& otherObj) const
        {
            return XCFloat4(Mul(m_vector, otherObj.GetData()));
        }

        inline void operator *=(const XCFloat4& otherObj)
        {
            (*this) = (*this) * otherObj;
        }

        inline void operator *=(float value)
        {
            (*this) = (*this) * XCFloat4(value, value, value, value);
        }

        inline const XCFloat4 operator /(const XCFloat4& otherObj) const
        {
            return XCFloat4(Div(m_vector, otherObj.GetData()));
        }

        inline void operator /=(XCFloat4& otherObj)
        {
            (*this) = (*this) / otherObj;
        }

        inline void operator =(const DirectX::XMVECTOR& vec)
        {
            SetValues(DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec), DirectX::XMVectorGetW(vec));
        }

        inline void operator =(const XCFloat3& vec)
        {
            SetValues(vec.Get<X>(), vec.Get<Y>(), vec.Get<Z>(), 0.0f);
        }

        //Conversions
        XCFloat4Unaligned GetUnaligned4() const
        {
            return XCFloat4Unaligned(Get<X>(), Get<Y>(), Get<Z>(), Get<W>());
        }

        DirectX::XMVECTOR GetPlatformIntrinsic() const
        {
            DirectX::XMVECTOR v1;
            v1 = DirectX::XMVectorSet(Get<X>(), Get<Y>(), Get<Z>(), Get<W>());

            return v1;
        }

        DirectX::XMFLOAT4 GetPlatformUnIntrinsic() const
        {
            return DirectX::XMFLOAT4(Get<X>(), Get<Y>(), Get<Z>(), Get<W>());
        }
    };

    inline const XCFloat4 operator *(float value, const XCFloat4& obj)
    {
        return XCFloat4(obj.Get<X>() * value, obj.Get<Y>() * value, obj.Get<Z>() * value, obj.Get<W>() * value);
    }

    inline const XCFloat4 operator *(const XCFloat4& obj, float value)
    {
        return value * obj;
    }

    inline const XCFloat4 operator /(float value, const XCFloat4& obj)
    {
        float divvalue = 1 / value;
        return divvalue * obj;
    }

    template<unsigned int Components>
    inline float VectorLength(const XCFloat4& vec)
    {
        return VectorLength<Components>(vec.GetData());
    }

    template<unsigned int Components>
    inline XCFloat4 VectorNormalize(const XCFloat4& vec)
    {
        return XCFloat4(VectorNormalize<Components>(vec.GetData()));
    }

    inline float VectorDot(const XCFloat4& v1, const XCFloat4& v2)
    {
        PackedVector4& vec1 = v1.GetData();
        PackedVector4& vec2 = v2.GetData();

        return (vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2]) + (vec1[3] * vec2[3]);
    }

    inline float CosineVectorDot(const XCFloat4& v1, const XCFloat4& v2)
    {
        return acosf(VectorDot(v1, v2) / (sqrt(((v1.Get<X>() * v1.Get<X>()) + (v1.Get<Y>() * v1.Get<Y>()) + (v1.Get<Z>() * v1.Get<Z>())) * ((v2.Get<X>() * v2.Get<X>()) + (v2.Get<Y>() * v2.Get<Y>()) + (v2.Get<Z>() * v2.Get<Z>())))));
    }

    inline XCFloat4 VectorCross(const XCFloat4& V1, const XCFloat4& V2)
    {
        PackedVector4& vec1 = V1.GetData();
        PackedVector4& vec2 = V2.GetData();

        XCFloat4 vResult(
            (vec1[1] * vec2[2]) - (vec1[2] * vec2[1]),
            (vec1[2] * vec2[0]) - (vec1[0] * vec2[2]),
            (vec1[0] * vec2[1]) - (vec1[1] * vec2[0]),
            0.0f
        );

        return vResult;
    }

    inline bool IsVectorEqual(const XCFloat4& v1, const XCFloat4& v2)
    {
        if (v1[0] > v2[0] || v1[0] < v2[0]) return false;
        if (v1[1] > v2[1] || v1[1] < v2[1]) return false;
        if (v1[2] > v2[2] || v1[2] < v2[2]) return false;

        return true;
    }

    inline XCFloat4 VectorMin(const XCFloat4& v1, const XCFloat4& v2)
    {
        return XCFloat4(Min(v1.GetData(), v2.GetData()));
    }

    inline XCFloat4 VectorMax(const XCFloat4& v1, const XCFloat4& v2)
    {
        return XCFloat4(Max(v1.GetData(), v2.GetData()));
    }

    inline XCFloat4 QuaternionRotationAxis(const XCFloat4& Axis, float Angle)
    {
        return XCFloat4(DirectX::XMQuaternionRotationAxis(Axis.GetPlatformIntrinsic(), Angle));
    }
}