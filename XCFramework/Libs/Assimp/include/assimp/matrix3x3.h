/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

/** @file matrix3x3.h
 *  @brief Definition of a 3x3 matrix, including operators when compiling in C++
 */
#ifndef AI_MATRIX3x3_H_INC
#define AI_MATRIX3x3_H_INC

#include "./Compiler/pushpack1.h"

#ifdef __cplusplus

template <typename T> class aiMatrix4x4t;
template <typename T> class aiVector2t;

// ---------------------------------------------------------------------------
/** @brief Represents a row-major 3x3 matrix
 *
 *  There's much confusion about matrix layouts (column vs. row order).
 *  This is *always* a row-major matrix. Not even with the
 *  #aiProcess_ConvertToLeftHanded flag, which absolutely does not affect
 *  matrix order - it just affects the handedness of the coordinate system
 *  defined thereby.
 */
template <typename TReal>
class aiMatrix3x3t
{
public:

    aiMatrix3x3t () :
        a1(static_cast<TReal>(1.0f)), a2(), a3(),
        b1(), b2(static_cast<TReal>(1.0f)), b3(),
        c1(), c2(), c3(static_cast<TReal>(1.0f)) {}

    aiMatrix3x3t (  TReal _a1, TReal _a2, TReal _a3,
                    TReal _b1, TReal _b2, TReal _b3,
                    TReal _c1, TReal _c2, TReal _c3) :
        a1(_a1), a2(_a2), a3(_a3),
        b1(_b1), b2(_b2), b3(_b3),
        c1(_c1), c2(_c2), c3(_c3)
    {}

public:

    // matrix multiplication.
    aiMatrix3x3t& operator *= (const aiMatrix3x3t& m);
    aiMatrix3x3t  operator  * (const aiMatrix3x3t& m) const;

    // array access operators
    TReal* operator[]       (unsigned int p_iIndex);
    const TReal* operator[] (unsigned int p_iIndex) const;

    // comparison operators
    bool operator== (const aiMatrix4x4t<TReal>& m) const;
    bool operator!= (const aiMatrix4x4t<TReal>& m) const;

    bool Equal(const aiMatrix4x4t<TReal>& m, TReal epsilon = 1e-6) const;

    template <typename TOther>
    operator aiMatrix3x3t<TOther> () const;

public:

    // -------------------------------------------------------------------
    /** @brief Construction from a 4x4 matrix. The remaining parts
     *  of the matrix are ignored.
     */
    explicit aiMatrix3x3t( const aiMatrix4x4t<TReal>& pMatrix);

    // -------------------------------------------------------------------
    /** @brief Transpose the matrix
     */
    aiMatrix3x3t& Transpose();

    // -------------------------------------------------------------------
    /** @brief Invert the matrix.
     *  If the matrix is not invertible all elements are set to qnan.
     *  Beware, use (f != f) to check whether a TReal f is qnan.
     */
    aiMatrix3x3t& Inverse();
    TReal Determinant() const;

public:
    // -------------------------------------------------------------------
    /** @brief Returns a rotation matrix for a rotation around z
     *  @param a Rotation angle, in radians
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix3x3t& RotationZ(TReal a, aiMatrix3x3t& out);

    // -------------------------------------------------------------------
    /** @brief Returns a rotation matrix for a rotation around
     *    an arbitrary axis.
     *
     *  @param a Rotation angle, in radians
     *  @param axis Axis to rotate around
     *  @param out To be filled
     */
    static aiMatrix3x3t& Rotation( TReal a,
        const aiVector3t<TReal>& axis, aiMatrix3x3t& out);

    // -------------------------------------------------------------------
    /** @brief Returns a translation matrix
     *  @param v Translation vector
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix3x3t& Translation( const aiVector2t<TReal>& v, aiMatrix3x3t& out);

    // -------------------------------------------------------------------
    /** @brief A function for creating a rotation matrix that rotates a
     *  vector called "from" into another vector called "to".
     * Input : from[3], to[3] which both must be *normalized* non-zero vectors
     * Output: mtx[3][3] -- a 3x3 matrix in colum-major form
     * Authors: Tomas M�ller, John Hughes
     *          "Efficiently Building a Matrix to Rotate One Vector to Another"
     *          Journal of Graphics Tools, 4(4):1-4, 1999
     */
    static aiMatrix3x3t& FromToMatrix(const aiVector3t<TReal>& from,
        const aiVector3t<TReal>& to, aiMatrix3x3t& out);

public:


    TReal a1, a2, a3;
    TReal b1, b2, b3;
    TReal c1, c2, c3;
} PACK_STRUCT;

typedef aiMatrix3x3t<float> aiMatrix3x3;

#else

struct aiMatrix3x3 {

    float a1, a2, a3;
    float b1, b2, b3;
    float c1, c2, c3;
} PACK_STRUCT;

#endif

#include "./Compiler/poppack1.h"

#endif // AI_MATRIX3x3_H_INC
