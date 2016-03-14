/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/PhysicsActor.h"
#include "Engine/Graphics/BasicGeometry/Plane.h"

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

    static XCVecIntrinsic4                     getContactNormalFromBoundBoxContainedPoints(OrientedBoundingBox* bbox, XCVec3* points, int noOfPoints);
    static XCVecIntrinsic4                     getContactNormalFromBoundBoxContainedBoundBox(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    static XCVecIntrinsic4                     getContactNormalFromBoundBoxContainedPlane(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    static XCVecIntrinsic4                     getTerrainPointOfContactWithBoundBox(PhysicsActor* bboxActor, PhysicsActor* terrain);
    static XCVecIntrinsic4                     getContactNormalFromOBBToOBBTriangleTest(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2);
    
    //Primitive Intersection tests
    static int                                 checkOBBTriangleIntersection(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2, XCVecIntrinsic4 v3, OrientedBoundingBox* bbox);
    static int                                 checkOBBPlaneIntersection(OrientedBoundingBox* bbox, Plane* p);

    static EOrderingType                       determineOrderOfTriangle(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2, XCVecIntrinsic4 v3, XCVecIntrinsic4 viewPoint);
};