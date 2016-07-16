/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "XCFloat4.h"

namespace XCMath
{
    //Forward decl
    class XCMatrix;
    XCMatrix MatrixMultiply(const XCMatrix& mat1, const XCMatrix& mat2);

    struct XCMatrixUnaligned
    {
        XCFloat4Unaligned r1;
        XCFloat4Unaligned r2;
        XCFloat4Unaligned r3;
        XCFloat4Unaligned r4;

        XCMatrixUnaligned() {}
        XCMatrixUnaligned(const XCFloat4& row1, const XCFloat4& row2, const XCFloat4& row3, const XCFloat4& row4)
        {
            r1 = row1.GetUnaligned4();
            r2 = row2.GetUnaligned4();
            r3 = row3.GetUnaligned4();
            r4 = row4.GetUnaligned4();
        }
    };

    class XCMatrix
    {
    public:
        static XCMatrix XCMatrixIdentity;

        XCMatrix()
        {
            SetIdentity();
        }

        XCMatrix(const DirectX::XMMATRIX& matrix)
        {
            SetIdentity();

            m_matrix[0] = matrix.r[0];
            m_matrix[1] = matrix.r[1];
            m_matrix[2] = matrix.r[2];
            m_matrix[3] = matrix.r[3];
        }

        XCMatrix(const XCMatrixUnaligned& matrix)
        {
            SetIdentity();

            m_matrix[0] = XCFloat4(matrix.r1);
            m_matrix[1] = XCFloat4(matrix.r2);
            m_matrix[2] = XCFloat4(matrix.r3);
            m_matrix[3] = XCFloat4(matrix.r4);
        }

        inline void SetIdentity()
        {
            m_matrix[0].SetValues(1.0f, 0.0f, 0.0f, 0.0f);
            m_matrix[1].SetValues(0.0f, 1.0f, 0.0f, 0.0f);
            m_matrix[2].SetValues(0.0f, 0.0f, 1.0f, 0.0f);
            m_matrix[3].SetValues(0.0f, 0.0f, 0.0f, 1.0f);
        }

        inline const XCFloat4 operator[](unsigned int rowIndex) const
        {
            return m_matrix[rowIndex];
        }

        inline XCFloat4& operator[](unsigned int rowIndex)
        {
            return m_matrix[rowIndex];
        }

        //Operations
        inline const XCMatrix operator *(const XCMatrix& M2)
        {
            return MatrixMultiply((*this), M2);
        }

        inline void operator *=(const XCMatrix& M2)
        {
            (*this) = (*this) * M2;
        }

        inline XCMatrixUnaligned GetUnaligned()
        {
            return XCMatrixUnaligned(m_matrix[0], m_matrix[1], m_matrix[2], m_matrix[3]);
        }

        inline DirectX::XMMATRIX GetPlatformIntrinsic() const
        {
            DirectX::XMMATRIX matrix;

            matrix.r[0] = DirectX::XMVectorSet(m_matrix[0].Get<X>(), m_matrix[0].Get<Y>(), m_matrix[0].Get<Z>(), m_matrix[0].Get<W>());
            matrix.r[1] = DirectX::XMVectorSet(m_matrix[1].Get<X>(), m_matrix[1].Get<Y>(), m_matrix[1].Get<Z>(), m_matrix[1].Get<W>());
            matrix.r[2] = DirectX::XMVectorSet(m_matrix[2].Get<X>(), m_matrix[2].Get<Y>(), m_matrix[2].Get<Z>(), m_matrix[2].Get<W>());
            matrix.r[3] = DirectX::XMVectorSet(m_matrix[3].Get<X>(), m_matrix[3].Get<Y>(), m_matrix[3].Get<Z>(), m_matrix[3].Get<W>());

            return matrix;
        }

    private:
        XCFloat4        m_matrix[4];
    };

    inline XCMatrix MatrixMultiply(const XCMatrix& mat1, const XCMatrix& mat2)
    {
        //Operations
        XCMatrix mResult, M1, M2;
        M1 = mat1;
        M2 = mat2;

        // Cache the invariants in registers
        float x = M1[0][0];
        float y = M1[0][1];
        float z = M1[0][2];
        float w = M1[0][3];

        // Perform the operation on the first row
        mResult[0][0] = (M2[0][0] * x) + (M2[1][0] * y) + (M2[2][0] * z) + (M2[3][0] * w);
        mResult[0][1] = (M2[0][1] * x) + (M2[1][1] * y) + (M2[2][1] * z) + (M2[3][1] * w);
        mResult[0][2] = (M2[0][2] * x) + (M2[1][2] * y) + (M2[2][2] * z) + (M2[3][2] * w);
        mResult[0][3] = (M2[0][3] * x) + (M2[1][3] * y) + (M2[2][3] * z) + (M2[3][3] * w);
        // Repeat for all the other rows
        x = M1[1][0];
        y = M1[1][1];
        z = M1[1][2];
        w = M1[1][3];
        mResult[1][0] = (M2[0][0] * x) + (M2[1][0] * y) + (M2[2][0] * z) + (M2[3][0] * w);
        mResult[1][1] = (M2[0][1] * x) + (M2[1][1] * y) + (M2[2][1] * z) + (M2[3][1] * w);
        mResult[1][2] = (M2[0][2] * x) + (M2[1][2] * y) + (M2[2][2] * z) + (M2[3][2] * w);
        mResult[1][3] = (M2[0][3] * x) + (M2[1][3] * y) + (M2[2][3] * z) + (M2[3][3] * w);
        x = M1[2][0];
        y = M1[2][1];
        z = M1[2][2];
        w = M1[2][3];
        mResult[2][0] = (M2[0][0] * x) + (M2[1][0] * y) + (M2[2][0] * z) + (M2[3][0] * w);
        mResult[2][1] = (M2[0][1] * x) + (M2[1][1] * y) + (M2[2][1] * z) + (M2[3][1] * w);
        mResult[2][2] = (M2[0][2] * x) + (M2[1][2] * y) + (M2[2][2] * z) + (M2[3][2] * w);
        mResult[2][3] = (M2[0][3] * x) + (M2[1][3] * y) + (M2[2][3] * z) + (M2[3][3] * w);
        x = M1[3][0];
        y = M1[3][1];
        z = M1[3][2];
        w = M1[3][3];
        mResult[3][0] = (M2[0][0] * x) + (M2[1][0] * y) + (M2[2][0] * z) + (M2[3][0] * w);
        mResult[3][1] = (M2[0][1] * x) + (M2[1][1] * y) + (M2[2][1] * z) + (M2[3][1] * w);
        mResult[3][2] = (M2[0][2] * x) + (M2[1][2] * y) + (M2[2][2] * z) + (M2[3][2] * w);
        mResult[3][3] = (M2[0][3] * x) + (M2[1][3] * y) + (M2[2][3] * z) + (M2[3][3] * w);

        return mResult;
    }

    inline const XCMatrix operator *(const XCMatrix& M1, const XCMatrix& M2)
    {
        return MatrixMultiply(M1, M2);
    }

    inline void MatrixDeterminant(const XCMatrix& matrix, float& outDeterminantValue)
    {
        outDeterminantValue = DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(matrix.GetPlatformIntrinsic()));
    }

    inline XCMatrix MatrixTranspose(const XCMatrix& matrix)
    {
        XCMatrix result;

        for (unsigned int rowIndex = 0; rowIndex < 4; ++rowIndex)
        {
            for (unsigned int colIndex = 0; colIndex < 4; ++colIndex)
            {
                result[rowIndex][colIndex] = matrix[colIndex][rowIndex];
            }
        }

        return result;
    }

    inline XCMatrix MatrixInverse(const XCMatrix& matrix)
    {
        return XCMatrix(DirectX::XMMatrixInverse(nullptr, matrix.GetPlatformIntrinsic()));
    }

    inline XCMatrix MatrixInverseTranspose(const XCMatrix& M)
    {
        // Inverse-transpose is just applied to normals.  So zero out 
        // translation row so that it doesn't get into our inverse-transpose
        // calculation--we don't want the inverse-transpose of the translation.
        XCMath::XCMatrix A = M;

        A[3] = XCMath::XCFloat4(0.0f, 0.0f, 0.0f, 1.0f);

        float determinantValue = 0.0f;
        MatrixDeterminant(A, determinantValue);
        return MatrixTranspose(MatrixInverse(M));
    }

    inline XCMatrix MatrixPerspective(float FovAngleY, float AspectHByW, float NearZ, float FarZ)
    {
        float    SinFov = sinf(0.5f * FovAngleY);
        float    CosFov = cosf(0.5f * FovAngleY);

        float Height = CosFov / SinFov;
        float Width = Height / AspectHByW;
        float fRange = FarZ / (FarZ - NearZ);

        XCMatrix M;
        M[0][0] = Width;
        M[0][1] = 0.0f;
        M[0][2] = 0.0f;
        M[0][3] = 0.0f;

        M[1][0] = 0.0f;
        M[1][1] = Height;
        M[1][2] = 0.0f;
        M[1][3] = 0.0f;

        M[2][0] = 0.0f;
        M[2][1] = 0.0f;
        M[2][2] = fRange;
        M[2][3] = 1.0f;

        M[3][0] = 0.0f;
        M[3][1] = 0.0f;
        M[3][2] = -fRange * NearZ;
        M[3][3] = 0.0f;

        return M;
    }

    inline XCMatrix MatrixLookAtLH(const XCFloat4& EyePosition, const XCFloat4& FocusPosition, const XCFloat4& UpDirection)
    {
        XCFloat4 EyeDirection = EyePosition - FocusPosition;
        XCFloat4 R2 = VectorNormalize<4>(EyeDirection);

        XCFloat4 R0 = VectorCross(UpDirection, R2);
        R0 = VectorNormalize<3>(R0);

        XCFloat4 R1 = VectorCross(R2, R0);

        XCFloat4 NegEyePosition = XCFloat4(0, 0, 0, 0) - EyePosition;

        float D0 = VectorDot(R0, NegEyePosition);
        float D1 = VectorDot(R1, NegEyePosition);
        float D2 = VectorDot(R2, NegEyePosition);

        XCMatrix M;
        M[0].Set<W>(D0);
        M[1].Set<W>(D1);
        M[2].Set<W>(D2);
        M[3].Set<W>(1);

        M = MatrixTranspose(M);

        return M;
    }

    inline XCMatrix MatrixTranslate(float x, float y, float z)
    {
        XCMatrix result;
        result[3].SetValues(x, y, z, 1.0f);

        return result;
    }

    inline XCMatrix MatrixTranslate(const XCFloat4& vec)
    {
        return MatrixTranslate(vec.Get<X>(), vec.Get<Y>(), vec.Get<Z>());
    }

    inline XCMatrix MatrixScale(float x, float y, float z)
    {
        XCMatrix result;
        result[0].SetValues(x, 0, 0, 0);
        result[1].SetValues(0, y, 0, 0);
        result[2].SetValues(0, 0, z, 0);
        result[3].SetValues(0, 0, 0, 1);

        return result;
    }

    inline XCMatrix MatrixScale(const XCFloat4& vec)
    {
        return MatrixScale(vec.Get<X>(), vec.Get<Y>(), vec.Get<Z>());
    }

    inline XCMatrix MatrixRotationX(float angle)
    {
        return XCMatrix(DirectX::XMMatrixRotationX(angle));
    }

    inline XCMatrix MatrixRotationY(float angle)
    {
        return XCMatrix(DirectX::XMMatrixRotationY(angle));
    }

    inline XCMatrix MatrixRotationZ(float angle)
    {
        return XCMatrix(DirectX::XMMatrixRotationZ(angle));
    }

    inline XCMatrix MatrixRotationAxis(const XCFloat4& axis, float angle)
    {
        return XCMatrix(DirectX::XMMatrixRotationAxis(axis.GetPlatformIntrinsic(), angle));
    }

    inline XCMatrix MatrixRotationRollPitchYawFromAxis(const XCFloat4& axis)
    {
        return XCMatrix(DirectX::XMMatrixRotationRollPitchYawFromVector(axis.GetPlatformIntrinsic()));
    }

    inline XCMatrix MatrixRotationQuaternion(const XCFloat4& vector)
    {
        return XCMatrix(DirectX::XMMatrixRotationQuaternion(vector.GetPlatformIntrinsic()));
    }

    inline XCFloat4 VectorTransformNormal(const XCFloat4& vec1, const XCMatrix& mat)
    {
        return XCFloat4(DirectX::XMVector3TransformNormal(vec1.GetPlatformIntrinsic(), mat.GetPlatformIntrinsic()));
    }

    inline XCFloat4 QuaternionRotationMatrix(const XCMatrix& matrix)
    {
        return XCFloat4(DirectX::XMQuaternionRotationMatrix(matrix.GetPlatformIntrinsic()));
    }

    inline XCFloat4 VectorTransformNormalMatrix(const XCFloat4& vec, const XCMatrix& matrix)
    {
        return XCFloat4(DirectX::XMVector3TransformNormal(vec.GetPlatformIntrinsic(), matrix.GetPlatformIntrinsic()));
    }

    inline XCFloat4 VectorTransformMatrix(const XCFloat4& vec, const XCMatrix& matrix)
    {
        return XCFloat4(DirectX::XMVector3Transform(vec.GetPlatformIntrinsic(), matrix.GetPlatformIntrinsic()));
    }
}