/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include <DirectXCollision.h>

class OrientedBoundingBox
{
public:
    static const i32 MaxOBBCornerPoints = 8;

    OrientedBoundingBox();
    virtual ~OrientedBoundingBox();

    virtual void               Init();

    void                       CreateBoundBox();
    void                       CreateBoundBox(XCVec4& min, XCVec4& max);
    void                       CreateBoundBox(OrientedBoundingBox* const boundBox);
    void                       Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix);

    //Oriented Bounding Box
    DirectX::BoundingOrientedBox  m_bBox;
    DirectX::BoundingOrientedBox  m_TransformedBox;

    XCVec4                        m_boxCenter;
    XCVec4                        m_boxExtends;
};