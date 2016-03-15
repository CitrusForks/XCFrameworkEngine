/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

/** @file quaternion.h
 *  @brief Quaternion structure, including operators when compiling in C++
 */
#ifndef AI_QUATERNION_H_INC
#define AI_QUATERNION_H_INC

#ifdef __cplusplus

template <typename TReal> class aiVector3t;
template <typename TReal> class aiMatrix3x3t;

// ---------------------------------------------------------------------------
/** Represents a quaternion in a 4D vector. */
template <typename TReal>
class aiQuaterniont
{
public:
    aiQuaterniont() : w(1.0), x(), y(), z() {}
    aiQuaterniont(TReal pw, TReal px, TReal py, TReal pz)
        : w(pw), x(px), y(py), z(pz) {}

    /** Construct from rotation matrix. Result is undefined if the matrix is not orthonormal. */
    explicit aiQuaterniont( const aiMatrix3x3t<TReal>& pRotMatrix);

    /** Construct from euler angles */
    aiQuaterniont( TReal rotx, TReal roty, TReal rotz);

    /** Construct from an axis-angle pair */
    aiQuaterniont( aiVector3t<TReal> axis, TReal angle);

    /** Construct from a normalized quaternion stored in a vec3 */
    explicit aiQuaterniont( aiVector3t<TReal> normalized);

    /** Returns a matrix representation of the quaternion */
    aiMatrix3x3t<TReal> GetMatrix() const;

public:

    bool operator== (const aiQuaterniont& o) const;
    bool operator!= (const aiQuaterniont& o) const;

    bool Equal(const aiQuaterniont& o, TReal epsilon = 1e-6) const;

public:

    /** Normalize the quaternion */
    aiQuaterniont& Normalize();

    /** Compute quaternion conjugate */
    aiQuaterniont& Conjugate ();

    /** Rotate a point by this quaternion */
    aiVector3t<TReal> Rotate (const aiVector3t<TReal>& in);

    /** Multiply two quaternions */
    aiQuaterniont operator* (const aiQuaterniont& two) const;

public:

    /** Performs a spherical interpolation between two quaternions and writes the result into the third.
     * @param pOut Target object to received the interpolated rotation.
     * @param pStart Start rotation of the interpolation at factor == 0.
     * @param pEnd End rotation, factor == 1.
     * @param pFactor Interpolation factor between 0 and 1. Values outside of this range yield undefined results.
     */
    static void Interpolate( aiQuaterniont& pOut, const aiQuaterniont& pStart,
        const aiQuaterniont& pEnd, TReal pFactor);

public:

    //! w,x,y,z components of the quaternion
    TReal w, x, y, z;
} ;

typedef aiQuaterniont<float> aiQuaternion;

#else

struct aiQuaternion {
    float w, x, y, z;
};

#endif


#endif // AI_QUATERNION_H_INC
