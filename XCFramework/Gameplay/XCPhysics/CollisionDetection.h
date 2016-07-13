/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/PhysicsActor.h"
#include "Graphics/BasicGeometry/Plane.h"

#if defined(WIN32)
#include <DirectXCollision.h>
#endif

enum EOrderingType
{
    ORDERINGTYPE_CLOCKWISE,
    ORDERINGTYPE_ANTICLOCKWISE,
    ORDERINGTYPE_UNDETERMINED
};

class CollisionDetection
{
public:
    CollisionDetection();
    ~CollisionDetection();

    static XCVec4                     GetContactNormalFromBoundBoxContainedPoints(OrientedBoundingBox* bbox, XCVec4* points, int noOfPoints);
    static XCVec4                     GetContactNormalFromBoundBoxContainedBoundBox(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    static XCVec4                     GetContactNormalFromBoundBoxContainedPlane(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    static XCVec4                     GetTerrainPointOfContactWithBoundBox(PhysicsActor* bboxActor, PhysicsActor* terrain);
    static XCVec4                     GetContactNormalFromOBBToOBBTriangleTest(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    
    //Primitive Intersection tests
    static int                        CheckOBBTriangleIntersection(XCVec4& v1, XCVec4& v2, XCVec4& v3, OrientedBoundingBox* bbox);
    static int                        CheckOBBPlaneIntersection(OrientedBoundingBox* bbox, Plane* p);

    static EOrderingType              DetermineOrderOfTriangle(XCVec4& v1, XCVec4& v2, XCVec4& v3, XCVec4& viewPoint);
};