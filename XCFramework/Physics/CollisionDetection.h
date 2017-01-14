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
    TriangleOrderingType_Undefined
};

class Plane;
class IPhysicsBoundVolume;

class CollisionDetection
{
public:

    static XCVec4                     GetHeightfieldPointOfContactWithBoundBox(const IPhysicsBoundVolume* bboxActor, const IPhysicsBoundVolume* terrain);
    static XCVec4                     GetContactNormalFromOBBToOBBTriangleTest(const IPhysicsBoundVolume* bbox1, const IPhysicsBoundVolume* bbox2);

    static DirectX::ContainmentType   CheckObbObbContainment(const IPhysicsBoundVolume* obb1, const IPhysicsBoundVolume* obb2);
    static TriangleOrderingType       DetermineOrderOfTriangle(XCVec4& v1, XCVec4& v2, XCVec4& v3, XCVec4& viewPoint);

#if defined(RESEARCH_IN_PROGRESS)
    static XCVec4                     GetContactNormalFromBoundBoxContainedPoints(IPhysicsBoundVolume* bbox, XCVec4* points, i32 noOfPoints);
    static XCVec4                     GetContactNormalFromBoundBoxContainedBoundBox(IPhysicsBoundVolume* bbox1, IPhysicsBoundVolume* bbox2);
    static XCVec4                     GetContactNormalFromBoundBoxContainedPlane(IPhysicsBoundVolume* bbox1, IPhysicsBoundVolume* bbox2);
    static i32                        CheckOBBTriangleIntersection(XCVec4& v1, XCVec4& v2, XCVec4& v3, IPhysicsBoundVolume* bbox);
    static i32                        CheckOBBPlaneIntersection(IPhysicsBoundVolume* bbox, Plane* p);
#endif
};