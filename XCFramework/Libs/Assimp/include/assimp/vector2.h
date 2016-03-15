/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef AI_VECTOR2D_H_INC
#define AI_VECTOR2D_H_INC

#ifdef __cplusplus
#   include <cmath>
#else
#   include <math.h>
#endif

#include "./Compiler/pushpack1.h"

// ----------------------------------------------------------------------------------
/** Represents a two-dimensional vector.
 */

#ifdef __cplusplus
template <typename TReal>
class aiVector2t
{
public:

    aiVector2t () : x(), y() {}
    aiVector2t (TReal _x, TReal _y) : x(_x), y(_y) {}
    explicit aiVector2t (TReal _xyz) : x(_xyz), y(_xyz) {}
    aiVector2t (const aiVector2t& o) : x(o.x), y(o.y) {}

public:

    void Set( TReal pX, TReal pY);
    TReal SquareLength() const ;
    TReal Length() const ;
    aiVector2t& Normalize();

public:

    const aiVector2t& operator += (const aiVector2t& o);
    const aiVector2t& operator -= (const aiVector2t& o);
    const aiVector2t& operator *= (TReal f);
    const aiVector2t& operator /= (TReal f);

    TReal operator[](unsigned int i) const;
    TReal& operator[](unsigned int i);

    bool operator== (const aiVector2t& other) const;
    bool operator!= (const aiVector2t& other) const;

    bool Equal(const aiVector2t& other, TReal epsilon = 1e-6) const;

    aiVector2t& operator= (TReal f);
    const aiVector2t SymMul(const aiVector2t& o);

    template <typename TOther>
    operator aiVector2t<TOther> () const;

    TReal x, y;
} PACK_STRUCT;

typedef aiVector2t<float> aiVector2D;

#else

struct aiVector2D {
    float x,y;
};

#endif // __cplusplus

#include "./Compiler/poppack1.h"

#endif // AI_VECTOR2D_H_INC
