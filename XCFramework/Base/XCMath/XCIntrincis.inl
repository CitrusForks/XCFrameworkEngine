/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include <intrin.h>
#include <assert.h>

 struct PackedVector4
{
    union
    {
        __declspec(align(16)) float   m_floatData[4];
        __declspec(align(16)) __m128  m_intrData;
    };

    PackedVector4()
    {
        memset(&m_intrData, 0, sizeof(__m128));
    }

    PackedVector4(const __m128& intr)
    {
        m_intrData = intr;
    }

    inline const float operator [](unsigned int index) const
    {
        assert(index < 4);
        return m_floatData[index];
    }

    inline float& operator [](unsigned int index)
    {
        assert(index < 4);
        return m_floatData[index];
    }
};

namespace XCMath
{
    //Operations
    inline PackedVector4 _vectorcall Add(const PackedVector4& val1, const PackedVector4& val2)
    {
        PackedVector4 result = _mm_add_ps(val1.m_intrData, val2.m_intrData);
        return result;
    }

    inline PackedVector4 _vectorcall Sub(const PackedVector4& val1, const PackedVector4& val2)
    {
        return _mm_sub_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 _vectorcall Mul(const PackedVector4& val1, const PackedVector4& val2)
    {
        return _mm_mul_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 _vectorcall Div(const PackedVector4& val1, const PackedVector4& val2)
    {
        return _mm_div_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 _vectorcall Min(const PackedVector4& val1, const PackedVector4& val2)
    {
        return _mm_min_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 _vectorcall Max(const PackedVector4& val1, const PackedVector4& val2)
    {
        return _mm_max_ps(val1.m_intrData, val2.m_intrData);
    }

    //Complex Operations
    template<unsigned int Components>
    inline float VectorLength(const PackedVector4& vec)
    {
        float length = 0.0f;
        for (unsigned int componentIndex = 0; componentIndex < Components; ++componentIndex)
        {
            length = length + (vec[componentIndex] * vec[componentIndex]);
        }
        return std::sqrt(length);
    }

    template<unsigned int Components>
    inline PackedVector4 VectorNormalize(const PackedVector4& vec)
    {
        PackedVector4 outVector;

        //Find the length of the vector.
        float length = 1.0f / VectorLength<Components>(vec);

        if (length > 0)
        {
            for (unsigned int componentIndex = 0; componentIndex < Components; ++componentIndex)
            {
                outVector[componentIndex] = vec[componentIndex] * length;
            }
        }
        else
        {
            //Divide by 0 length?
            //assert(false);
        }

        return outVector;
    }
}