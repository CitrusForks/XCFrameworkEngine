/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef AI_MATRIX4X4_H_INC
#define AI_MATRIX4X4_H_INC

#include "vector3.h"
#include "./Compiler/pushpack1.h"

#ifdef __cplusplus

template<typename TReal> class aiMatrix3x3t;
template<typename TReal> class aiQuaterniont;

// ---------------------------------------------------------------------------
/** @brief Represents a row-major 4x4 matrix, use this for homogeneous
 *   coordinates.
 *
 *  There's much confusion about matrix layouts (column vs. row order).
 *  This is *always* a row-major matrix. Not even with the
 *  #aiProcess_ConvertToLeftHanded flag, which absolutely does not affect
 *  matrix order - it just affects the handedness of the coordinate system
 *  defined thereby.
 */
template<typename TReal>
class aiMatrix4x4t
{
public:

    /** set to identity */
    aiMatrix4x4t ();

    /** construction from single values */
    aiMatrix4x4t (  TReal _a1, TReal _a2, TReal _a3, TReal _a4,
                    TReal _b1, TReal _b2, TReal _b3, TReal _b4,
                    TReal _c1, TReal _c2, TReal _c3, TReal _c4,
                    TReal _d1, TReal _d2, TReal _d3, TReal _d4);


    /** construction from 3x3 matrix, remaining elements are set to identity */
    explicit aiMatrix4x4t( const aiMatrix3x3t<TReal>& m);

    /** construction from position, rotation and scaling components
     * @param scaling The scaling for the x,y,z axes
     * @param rotation The rotation as a hamilton quaternion
     * @param position The position for the x,y,z axes
     */
    aiMatrix4x4t(const aiVector3t<TReal>& scaling, const aiQuaterniont<TReal>& rotation,
        const aiVector3t<TReal>& position);

public:

    // array access operators
    TReal* operator[]       (unsigned int p_iIndex);
    const TReal* operator[] (unsigned int p_iIndex) const;

    // comparison operators
    bool operator== (const aiMatrix4x4t& m) const;
    bool operator!= (const aiMatrix4x4t& m) const;

    bool Equal(const aiMatrix4x4t& m, TReal epsilon = 1e-6) const;

    // matrix multiplication.
    aiMatrix4x4t& operator *= (const aiMatrix4x4t& m);
    aiMatrix4x4t  operator *  (const aiMatrix4x4t& m) const;

    template <typename TOther>
    operator aiMatrix4x4t<TOther> () const;

public:

    // -------------------------------------------------------------------
    /** @brief Transpose the matrix */
    aiMatrix4x4t& Transpose();

    // -------------------------------------------------------------------
    /** @brief Invert the matrix.
     *  If the matrix is not invertible all elements are set to qnan.
     *  Beware, use (f != f) to check whether a TReal f is qnan.
     */
    aiMatrix4x4t& Inverse();
    TReal Determinant() const;


    // -------------------------------------------------------------------
    /** @brief Returns true of the matrix is the identity matrix.
     *  The check is performed against a not so small epsilon.
     */
    inline bool IsIdentity() const;

    // -------------------------------------------------------------------
    /** @brief Decompose a trafo matrix into its original components
     *  @param scaling Receives the output scaling for the x,y,z axes
     *  @param rotation Receives the output rotation as a hamilton
     *   quaternion
     *  @param position Receives the output position for the x,y,z axes
     */
    void Decompose (aiVector3t<TReal>& scaling, aiQuaterniont<TReal>& rotation,
        aiVector3t<TReal>& position) const;

    // -------------------------------------------------------------------
    /** @brief Decompose a trafo matrix with no scaling into its
     *    original components
     *  @param rotation Receives the output rotation as a hamilton
     *    quaternion
     *  @param position Receives the output position for the x,y,z axes
     */
    void DecomposeNoScaling (aiQuaterniont<TReal>& rotation,
        aiVector3t<TReal>& position) const;


    // -------------------------------------------------------------------
    /** @brief Creates a trafo matrix from a set of euler angles
     *  @param x Rotation angle for the x-axis, in radians
     *  @param y Rotation angle for the y-axis, in radians
     *  @param z Rotation angle for the z-axis, in radians
     */
    aiMatrix4x4t& FromEulerAnglesXYZ(TReal x, TReal y, TReal z);
    aiMatrix4x4t& FromEulerAnglesXYZ(const aiVector3t<TReal>& blubb);

public:
    // -------------------------------------------------------------------
    /** @brief Returns a rotation matrix for a rotation around the x axis
     *  @param a Rotation angle, in radians
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix4x4t& RotationX(TReal a, aiMatrix4x4t& out);

    // -------------------------------------------------------------------
    /** @brief Returns a rotation matrix for a rotation around the y axis
     *  @param a Rotation angle, in radians
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix4x4t& RotationY(TReal a, aiMatrix4x4t& out);

    // -------------------------------------------------------------------
    /** @brief Returns a rotation matrix for a rotation around the z axis
     *  @param a Rotation angle, in radians
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix4x4t& RotationZ(TReal a, aiMatrix4x4t& out);

    // -------------------------------------------------------------------
    /** Returns a rotation matrix for a rotation around an arbitrary axis.
     *  @param a Rotation angle, in radians
     *  @param axis Rotation axis, should be a normalized vector.
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix4x4t& Rotation(TReal a, const aiVector3t<TReal>& axis,
        aiMatrix4x4t& out);

    // -------------------------------------------------------------------
    /** @brief Returns a translation matrix
     *  @param v Translation vector
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix4x4t& Translation( const aiVector3t<TReal>& v, aiMatrix4x4t& out);

    // -------------------------------------------------------------------
    /** @brief Returns a scaling matrix
     *  @param v Scaling vector
     *  @param out Receives the output matrix
     *  @return Reference to the output matrix
     */
    static aiMatrix4x4t& Scaling( const aiVector3t<TReal>& v, aiMatrix4x4t& out);

    // -------------------------------------------------------------------
    /** @brief A function for creating a rotation matrix that rotates a
     *  vector called "from" into another vector called "to".
     * Input : from[3], to[3] which both must be *normalized* non-zero vectors
     * Output: mtx[3][3] -- a 3x3 matrix in colum-major form
     * Authors: Tomas M�ller, John Hughes
     *          "Efficiently Building a Matrix to Rotate One Vector to Another"
     *          Journal of Graphics Tools, 4(4):1-4, 1999
     */
    static aiMatrix4x4t& FromToMatrix(const aiVector3t<TReal>& from,
        const aiVector3t<TReal>& to, aiMatrix4x4t& out);

public:

    TReal a1, a2, a3, a4;
    TReal b1, b2, b3, b4;
    TReal c1, c2, c3, c4;
    TReal d1, d2, d3, d4;

} PACK_STRUCT;

typedef aiMatrix4x4t<float> aiMatrix4x4;

#else

struct aiMatrix4x4 {
    float a1, a2, a3, a4;
    float b1, b2, b3, b4;
    float c1, c2, c3, c4;
    float d1, d2, d3, d4;
};


#endif // __cplusplus

#include "./Compiler/poppack1.h"

#endif // AI_MATRIX4X4_H_INC
