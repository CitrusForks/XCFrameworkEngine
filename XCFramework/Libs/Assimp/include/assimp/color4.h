/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef AI_COLOR4D_H_INC
#define AI_COLOR4D_H_INC

#include "./Compiler/pushpack1.h"

#ifdef __cplusplus

// ----------------------------------------------------------------------------------
/** Represents a color in Red-Green-Blue space including an
*   alpha component. Color values range from 0 to 1. */
// ----------------------------------------------------------------------------------
template <typename TReal>
class aiColor4t
{
public:
    aiColor4t () : r(), g(), b(), a() {}
    aiColor4t (TReal _r, TReal _g, TReal _b, TReal _a)
        : r(_r), g(_g), b(_b), a(_a) {}
    explicit aiColor4t (TReal _r) : r(_r), g(_r), b(_r), a(_r) {}
    aiColor4t (const aiColor4t& o)
        : r(o.r), g(o.g), b(o.b), a(o.a) {}

public:
    // combined operators
    const aiColor4t& operator += (const aiColor4t& o);
    const aiColor4t& operator -= (const aiColor4t& o);
    const aiColor4t& operator *= (TReal f);
    const aiColor4t& operator /= (TReal f);

public:
    // comparison
    bool operator == (const aiColor4t& other) const;
    bool operator != (const aiColor4t& other) const;
    bool operator <  (const aiColor4t& other) const;

    // color tuple access, rgba order
    inline TReal operator[](unsigned int i) const;
    inline TReal& operator[](unsigned int i);

    /** check whether a color is (close to) black */
    inline bool IsBlack() const;

public:

    // Red, green, blue and alpha color values
    TReal r, g, b, a;
} PACK_STRUCT;  // !struct aiColor4D

typedef aiColor4t<float> aiColor4D;

#else

struct aiColor4D {
    float r, g, b, a;
} PACK_STRUCT;

#endif // __cplusplus

#include "./Compiler/poppack1.h"

#endif // AI_COLOR4D_H_INC
