/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(WIN32)
#include <DirectXCollision.h>
#endif

enum TriangleOrderingType
{
    TriangleOrderingType_Clockwise,
    TriangleOrderingType_AntiClockwise,
    TriangleOrderingType_NotDefined
};

class Plane;
class OrientedBoundingBox;

class CollisionDetection
{
public:

    static XCVec4                     GetContactNormalFromBoundBoxContainedPoints(OrientedBoundingBox* bbox, XCVec4* points, i32 noOfPoints);
    static XCVec4                     GetContactNormalFromBoundBoxContainedBoundBox(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    static XCVec4                     GetContactNormalFromBoundBoxContainedPlane(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    //static XCVec4                     GetTerrainPointOfContactWithBoundBox(IPhysicsFeature* bboxActor, IPhysicsFeature* terrain);
    static XCVec4                     GetContactNormalFromOBBToOBBTriangleTest(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    
    //Primitive Intersection tests
    static i32                        CheckOBBTriangleIntersection(XCVec4& v1, XCVec4& v2, XCVec4& v3, OrientedBoundingBox* bbox);
    static i32                        CheckOBBPlaneIntersection(OrientedBoundingBox* bbox, Plane* p);

    static TriangleOrderingType       DetermineOrderOfTriangle(XCVec4& v1, XCVec4& v2, XCVec4& v3, XCVec4& viewPoint);
};