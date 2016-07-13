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

    PackedVector4(__m128& intr)
    {
        m_intrData = intr;
    }

    //Getters
    inline __m128& GetAlignedData()
    {
        return m_intrData;
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
    inline PackedVector4 Add(PackedVector4& val1, PackedVector4& val2)
    {
        PackedVector4 result = _mm_add_ps(val1.m_intrData, val2.m_intrData);
        return result;
    }

    inline PackedVector4 Sub(PackedVector4& val1, PackedVector4& val2)
    {
        return _mm_sub_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 Mul(PackedVector4& val1, PackedVector4& val2)
    {
        return _mm_mul_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 Div(PackedVector4& val1, PackedVector4& val2)
    {
        return _mm_div_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 Min(PackedVector4& val1, PackedVector4& val2)
    {
        return _mm_min_ps(val1.m_intrData, val2.m_intrData);
    }

    inline PackedVector4 Max(PackedVector4& val1, PackedVector4& val2)
    {
        return _mm_max_ps(val1.m_intrData, val2.m_intrData);
    }

    //Complex Operations
    template<unsigned int Components>
    inline float VectorLength(PackedVector4& vec)
    {
        float length = 0.0f;
        for (unsigned int componentIndex = 0; componentIndex < Components; ++componentIndex)
        {
            length += (vec[componentIndex] * vec[componentIndex]);
        }
        return length;
    }

    template<unsigned int Components>
    inline PackedVector4 VectorNormalize(PackedVector4& vec)
    {
        PackedVector4 outVector;

        //Find the length of the vector.
        float length = 1 / VectorLength<Components>(vec);

        if (length > 0)
        {
            for (unsigned int componentIndex = 0; componentIndex < Components; ++componentIndex)
            {
                outVector[componentIndex] = vec[componentIndex] * length;
            }
        }
        else
        {
            //Divide by 0 length
            assert(false);
        }

        return outVector;
    }
}