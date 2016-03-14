/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "CollisionDetection.h"

#include "Gameplay/XCPhysics/ParticleContact.h"
#include "Gameplay/GameActors/Environment/Terrain/Terrain.h"

CollisionDetection::CollisionDetection()
{
}


CollisionDetection::~CollisionDetection()
{
}

XCVecIntrinsic4 CollisionDetection::getTerrainPointOfContactWithBoundBox(PhysicsActor* bboxActor, PhysicsActor* terrain)
{
    Terrain* terrainActor = (Terrain*) terrain;
    XCVec3 result = XCVec3(0, 0, 0);

    XCVecIntrinsic4 point;
    for (int vertexIndex = 0; vertexIndex < terrainActor->GetNoOfVertices(); vertexIndex++)
    {
        point = terrainActor->GetPointAtIndex(vertexIndex);

        ContainmentType contain = bboxActor->getBoundBox()->m_TransformedBox.Contains(point);
        if (contain == CONTAINS || contain == INTERSECTS)
        {
            //Hit 
            XCVecIntrinsic4 currentPos = bboxActor->GetTransformedPosition();
            currentPos = XMVectorSetY(currentPos, (float)XMVectorGetY(point) + (float)0.1); //Adding impulse of 2
            bboxActor->SetTransformedPosition(currentPos);
            result = XCVec3(0, 1, 0);
            return XMLoadFloat3(&result);
        }
    }

    return XMLoadFloat3(&result);
}

EOrderingType CollisionDetection::determineOrderOfTriangle(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2, XCVecIntrinsic4 v3, XCVecIntrinsic4 viewPoint)
{
    XCMatrix4 detMatrix = XMMatrixIdentity();

    v1 = XMVectorSetW(v1, 1);
    v2 = XMVectorSetW(v2, 1);
    v3 = XMVectorSetW(v3, 1);
    viewPoint = XMVectorSetW(viewPoint, 1);

#if defined(WIN32)
    detMatrix.r[0] = v1;
    detMatrix.r[1] = v2;
    detMatrix.r[2] = v3;
    detMatrix.r[3] = viewPoint;
#elif defined(XC_ORBIS)
    detMatrix.setRow(0, v1);
    detMatrix.setRow(1, v2);
    detMatrix.setRow(2, v3);
    detMatrix.setRow(3, viewPoint);
#endif

    XCVecIntrinsic4 det = XMMatrixDeterminant(detMatrix);

    if (XMVectorGetX(det) > 0)
    {
        return ORDERINGTYPE_ANTICLOCKWISE;
    }
    else if (XMVectorGetX(det) < 0)
    {
        return ORDERINGTYPE_CLOCKWISE;
    }
    else
    {
        return ORDERINGTYPE_UNDETERMINED;
    }
}

XCVecIntrinsic4 CollisionDetection::getContactNormalFromOBBToOBBTriangleTest(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2)
{
    XCVecIntrinsic4 contactNormal = XMVectorZero();

    XCVec3* points = new XCVec3[OrientedBoundingBox::MAX_OBB_CORNER_POINTS_COUNT];
    bbox2->m_TransformedBox.GetCorners(points);

    //int val = checkOBBTriangleIntersection(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]), bbox1);
    //determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]), XMLoadFloat3(&bbox1->m_TransformedBox.Center));
    
    //1
    //0 1 2
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[1]), XMLoadFloat3(&points[2])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[1]));

        if (determineOrderOfTriangle(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[1]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //2
    //0 2 3
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //3
    //4 6 5 
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //4
    //4 7 6
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[6])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[6]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[6]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //5
    //4 5 1
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[1])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[1]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[1]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //6
    //4 1 0
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[1]), XMLoadFloat3(&points[0])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[1]), XMLoadFloat3(&points[0]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[1]), XMLoadFloat3(&points[0]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //7
    //3 2 6
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[6])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[6]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[6]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //8
    //3 6 7
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //9
    //1 5 6
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }
    /*if(bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7])))
    {
        //contactNormal = XMLoadFloat3(&XMFLOAT3(0, 0, -1));
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_ANTICLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        return contactNormal;
    }*/
    
    /*if(bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3])))
    {
        //contactNormal = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_ANTICLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        return contactNormal;
    }*/

    //0 4 7 3
    /*if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[4])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[4]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[4]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_ANTICLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        return contactNormal;
    }*/

    /*if(bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[0])))
    {
        //contactNormal = XMLoadFloat3(&XMFLOAT3(-1, 0, 0));
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[0]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[0]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_ANTICLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        return contactNormal;
    }*/

    //10
    //1 6 2
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[2])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[2]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[2]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0 || XMVectorGetY(contactNormal) < 0))
            return contactNormal;
    }

    //11
    //4 0 3
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[0]), XMLoadFloat3(&points[3])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[0]), XMLoadFloat3(&points[3]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[0]), XMLoadFloat3(&points[3]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }

        //if (!(XMVectorGetY(contactNormal) > 0 || XMVectorGetY(contactNormal) < 0))
            return contactNormal;
    }

    //12
    //4 3 7
    if (bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7])))
    {
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_CLOCKWISE)
        {
            contactNormal = -contactNormal;
        }

        //if (!(XMVectorGetY(contactNormal) > 0 || XMVectorGetY(contactNormal) < 0))
            return contactNormal;
    }
    /*
    if(bbox1->m_TransformedBox.Intersects(XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[2])))
    {
        //contactNormal = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[2]));
        if (determineOrderOfTriangle(XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[2]), XMLoadFloat3(&bbox1->m_TransformedBox.Center)) == ORDERINGTYPE_ANTICLOCKWISE)
        {
            contactNormal = -contactNormal;
        }
        return contactNormal;
    }*/

    return contactNormal;
}

int	CollisionDetection::checkOBBTriangleIntersection(XCVecIntrinsic4 v0, XCVecIntrinsic4 v1, XCVecIntrinsic4 v2, OrientedBoundingBox* bbox)
{
    float p0, p2, r;

    //Get center and extends of the bound box
    XCVecIntrinsic4 center = XMLoadFloat3(&bbox->m_TransformedBox.Center);

#if defined(WIN32)
    float extend0 = bbox->m_TransformedBox.Extents.x;
    float extend1 = bbox->m_TransformedBox.Extents.y;
    float extend2 = bbox->m_TransformedBox.Extents.z;
#elif defined(XC_ORBIS)
    float extend0 = bbox->m_TransformedBox.Extents.getX();
    float extend1 = bbox->m_TransformedBox.Extents.getY();
    float extend2 = bbox->m_TransformedBox.Extents.getZ();
#endif

    //Translate triangle as conceptually moving AABB to origin
    v0 = v0 - center;
    v1 = v1 - center;
    v2 = v2 - center;

    //Compute Edge vectors for triangles
    XCVecIntrinsic4 f0 = v1 - v0;
    XCVecIntrinsic4 f1 = v2 - v1;
    XCVecIntrinsic4 f2 = v0 - v2;

    //Test axes a00..a22
    //Test axis a00 = (0, -f0z, f0y)
    p0 = (XMVectorGetZ(v0) * XMVectorGetY(v1)) - (XMVectorGetY(v0) * XMVectorGetZ(v1));
    p2 = (XMVectorGetZ(v2) * (XMVectorGetY(v1) - XMVectorGetY(v0))) - (XMVectorGetY(v2) * (XMVectorGetZ(v1) - XMVectorGetZ(v0)));
    r = extend1 * abs(XMVectorGetZ(f0)) + extend2 * abs(XMVectorGetY(f0));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //Test axis a01 = (0, -f1z, f1y) f1 = v2 - v1 
    p0 = (XMVectorGetZ(v0) * XMVectorGetY(v2)) - (XMVectorGetY(v0) * XMVectorGetZ(v2));
    p2 = (XMVectorGetZ(v2) * (XMVectorGetY(v2) - XMVectorGetY(v1))) - (XMVectorGetY(v2) * (XMVectorGetZ(v2) - XMVectorGetZ(v1)));
    r = extend1 * abs(XMVectorGetZ(f1)) + extend2 * abs(XMVectorGetY(f1));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //Test axis a02 = (0, -f2z, f2y)  f2 = v0 - v2
    p0 = (XMVectorGetZ(v0) * XMVectorGetY(v0)) - (XMVectorGetY(v0) * XMVectorGetZ(v0));
    p2 = (XMVectorGetZ(v2) * (XMVectorGetY(v0) - XMVectorGetY(v2))) - (XMVectorGetY(v2) * (XMVectorGetZ(v0) - XMVectorGetZ(v2)));
    r = extend1 * abs(XMVectorGetZ(f2)) + extend2 * abs(XMVectorGetY(f2));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;


    //Test axis a10 = (f0z, 0, -f0x)  f0 = v1 - v0
    p0 = (XMVectorGetX(v0) * XMVectorGetZ(v1)) - (XMVectorGetZ(v0) * XMVectorGetX(v1));
    p2 = (XMVectorGetX(v2) * (XMVectorGetZ(v1) - XMVectorGetZ(v0))) - (XMVectorGetZ(v2) * (XMVectorGetX(v1) - XMVectorGetX(v0)));
    r = extend0 * abs(XMVectorGetZ(f0)) + extend2 * abs(XMVectorGetY(f0));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //Test axis a11 = (f1z, 0, -f1x)  f1 = v2 - v1
    p0 = (XMVectorGetX(v0) * XMVectorGetZ(v2)) - (XMVectorGetZ(v0) * XMVectorGetX(v2));
    p2 = (XMVectorGetX(v2) * (XMVectorGetZ(v2) - XMVectorGetZ(v1))) - (XMVectorGetZ(v2) * (XMVectorGetX(v2) - XMVectorGetX(v1)));
    r = extend0 * abs(XMVectorGetZ(f1)) + extend2 * abs(XMVectorGetY(f1));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //Test axis a12 = (f2z, 0, -f2x)  f2 = v0 - v2
    p0 = (XMVectorGetX(v0) * XMVectorGetZ(v0)) - (XMVectorGetZ(v0) * XMVectorGetX(v0));
    p2 = (XMVectorGetX(v2) * (XMVectorGetZ(v0) - XMVectorGetZ(v2))) - (XMVectorGetZ(v2) * (XMVectorGetX(v0) - XMVectorGetX(v2)));
    r = extend0 * abs(XMVectorGetZ(f2)) + extend2 * abs(XMVectorGetY(f2));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;


    //Test axis a20 = (-f0y, fox, 0)  f0 = v1 - v0
    p0 = (XMVectorGetY(v0) * XMVectorGetX(v1)) - (XMVectorGetX(v0) * XMVectorGetY(v1));
    p2 = (XMVectorGetY(v2) * (XMVectorGetX(v1) - XMVectorGetX(v0))) - (XMVectorGetX(v2) * (XMVectorGetY(v1) - XMVectorGetY(v0)));
    r = extend0 * abs(XMVectorGetZ(f0)) + extend1 * abs(XMVectorGetY(f0));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //Test axis a21 = (-f0y, fox, 0)  f1 = v2 - v1
    p0 = (XMVectorGetY(v0) * XMVectorGetX(v2)) - (XMVectorGetX(v0) * XMVectorGetY(v2));
    p2 = (XMVectorGetY(v2) * (XMVectorGetX(v2) - XMVectorGetX(v1))) - (XMVectorGetX(v2) * (XMVectorGetY(v2) - XMVectorGetY(v1)));
    r = extend0 * abs(XMVectorGetZ(f1)) + extend1 * abs(XMVectorGetY(f1));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //Test axis a22 = (-f0y, fox, 0)  f2 = v0 - v2
    p0 = (XMVectorGetY(v0) * XMVectorGetX(v0)) - (XMVectorGetX(v0) * XMVectorGetY(v0));
    p2 = (XMVectorGetY(v2) * (XMVectorGetX(v0) - XMVectorGetX(v2))) - (XMVectorGetX(v2) * (XMVectorGetY(v0) - XMVectorGetY(v2)));
    r = extend0 * abs(XMVectorGetZ(f2)) + extend1 * abs(XMVectorGetY(f2));
    if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0;

    //-------------------------------
    //Exit if
    if (std::max(std::max(XMVectorGetX(v0), XMVectorGetX(v1)), XMVectorGetX(v2)) < -extend0
        || std::min(std::min(XMVectorGetX(v0), XMVectorGetX(v1)), XMVectorGetX(v2)) > extend0)
        return 0;

    if (std::max(std::max(XMVectorGetY(v0), XMVectorGetY(v1)), XMVectorGetY(v2)) < -extend1
        || std::min(std::min(XMVectorGetY(v0), XMVectorGetY(v1)), XMVectorGetY(v2)) > extend1)
        return 0;

    if (std::max(std::max(XMVectorGetZ(v0), XMVectorGetZ(v1)), XMVectorGetZ(v2)) < -extend2
        || std::min(std::min(XMVectorGetZ(v0), XMVectorGetZ(v1)), XMVectorGetZ(v2)) > extend2)
        return 0;

    //Test separating axis corresponding to triangle face normal
    Plane plane;
    plane.SetPlaneNormal(XMVector3Cross(f0, f1));
    plane.SetDConstant(XMVector3Dot(plane.GetPlaneNormal(), f0));

    return checkOBBPlaneIntersection(bbox, &plane);
}

int CollisionDetection::checkOBBPlaneIntersection(OrientedBoundingBox* bbox, Plane* p)
{
    //Compute  projection interval radius of box onto L(t) = b.x + t * p.n
    //XMVECTOR normDotOrient = XMVector3Dot(p->GetPlaneNormal(), bbox->m_TransformedBox.)
    //float r = bbox->m_TransformedBox.Extents.x * abs(XMVector3Dot(p->GetPlaneNormal(), ))
    return 1;
}

XCVecIntrinsic4 CollisionDetection::getContactNormalFromBoundBoxContainedPoints(OrientedBoundingBox* bbox, XCVec3* points, int noOfPoints)
{
    XCVecIntrinsic4 contactNormal;
    //Got points
    //Now check for containment of these points within the obj1 box
    for (int index = 0; index < noOfPoints; index++)
    {
        XCVecIntrinsic4 point = XMLoadFloat3(&points[index]);
        ContainmentType type = bbox->m_TransformedBox.Contains(point);

        XCVec3 norm = XCVec3(-0.0, 0.0, 0.0);
        if (type == ContainmentType::CONTAINS)
        {
            //Got point calculate contact normal.
            switch (index)
            {
                //Seems to be front face, based on z axis
            case 0:
                norm = XCVec3(-0.5, 0, -0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            case 1:
                norm = XCVec3(0.5, 0, -0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            case 2:
                norm = XCVec3(-0.5, 0, -0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            case 3:
                norm = XCVec3(0.5, 0, -0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;

                //May be back face, yet to confirm
            case 4:
                norm = XCVec3(-0.5, 0, 0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            case 5:
                norm = XCVec3(0.5, 0, 0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            case 6:
                norm = XCVec3(-0.5, 0, 0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            case 7:
                norm = XCVec3(0.5, 0, 0.5);
                contactNormal += XMLoadFloat3(&norm);
                break;
            }
        }
    }

    return XMVector3Normalize(contactNormal);
}

XCVecIntrinsic4 CollisionDetection::getContactNormalFromBoundBoxContainedBoundBox(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2)
{
    //Get 4 points from every face, check containment of these points in the bound box, if at least 3 points contained or intersected, return the face normal
    //Face 1 : 4 5 6 7
    //Face 2 : 0 1 2 3
    XCVecIntrinsic4 contactNormal = XMVectorZero();

    XCVec3* points = new XCVec3[OrientedBoundingBox::MAX_OBB_CORNER_POINTS_COUNT];
    bbox2->m_TransformedBox.GetCorners(points);

    int hitCount = 0;
    ContainmentType type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[4]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[5]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[6]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[7]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;

    if (hitCount >= 2)
    {
        //This face is collided.
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]));

        return contactNormal;
    }
    /*else if (hitCount <= 2)
    {
        //May be check for 2 hitcounts, like 45 deg contact normal
    }
    */
    //Face 0 1 2 3
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[0]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[1]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[2]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[3]));
    hitCount += type == CONTAINS || type == INTERSECTS ? true : false;

    if (hitCount >= 2)
    {
        //This face is collided.
        contactNormal = GetNormalFromPoints(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[1]), XMLoadFloat3(&points[2]));

        return contactNormal;
    }

    return contactNormal;
}

XCVecIntrinsic4 CollisionDetection::getContactNormalFromBoundBoxContainedPlane(OrientedBoundingBox* bbox1, OrientedBoundingBox* bbox2)
{
    //This algorithm is not suitable, we have a situation where,
    //When collision occurs, some points of bbox1 are inside bbox2, now from the front side of plane normal, it's true and also from back side of plane normal is 
    //true because some points are behind the plane. So this case will occur for every plane and we cannot distinguish the distinct planes.

    //May be, create a separating axis just before collision occurs, we can detect which plane is closest to the bbox1, and then conclude with the closest plane normal.
    XCVecIntrinsic4 contactNormal;

    //Get the points of 2nd object
    XCVec3 box2Points[8];
    bbox2->m_TransformedBox.GetCorners(box2Points);

    //Form planes for -1 z axis to get normal (0, 0, -1);
    //We have [4] [5] [6] [7], find normal to one of the triangle and normalize and set it as plane
    XCMatrix4 R = XMMatrixRotationQuaternion(XMLoadFloat4(&bbox1->m_TransformedBox.Orientation));
    XCVecIntrinsic4 Outside, Inside;

    for (int index = 0; index < 6; index++)
    {
        switch (index)
        {
        case 0:
        {		  // 4 5 6 
                  XCVecIntrinsic4 plane = CreatePlaneFromPoints(XMLoadFloat3(&box2Points[5]), XMLoadFloat3(&box2Points[4]), XMLoadFloat3(&box2Points[6]));

                  //DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
                  //PlaneIntersectionType    XM_CALLCONV     Intersects(_In_ FXMVECTOR Plane) const;
                  PlaneIntersectionType type = bbox1->m_TransformedBox.Intersects(plane);
                  
                  if (type == FRONT || type == INTERSECTING)
                  {
                      return contactNormal = toXMVECTOR(0, 0, -1, 0);
                  }
                  break;
        }

        case 1:
        {		  // 0 1 2
                  XCVecIntrinsic4 plane = CreatePlaneFromPoints(XMLoadFloat3(&box2Points[1]), XMLoadFloat3(&box2Points[0]), XMLoadFloat3(&box2Points[2]));
#if defined(WIN32)
                  DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
#endif
                  contactNormal = toXMVECTOR(0, 0, 1, 0);
                  PlaneIntersectionType type = bbox1->m_TransformedBox.Intersects(plane);

                  if (type == FRONT || type == INTERSECTING)
                  {
                      return contactNormal = toXMVECTOR(0, 0, 1, 0);
                  }
                  break;
        }/*
        case 2:
        {		  // 0 4 7
        XMVECTOR plane = CreatePlaneFromPoints(XMLoadFloat3(&box2Points[0]), XMLoadFloat3(&box2Points[4]), XMLoadFloat3(&box2Points[7]));
        DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
        contactNormal = toXMVECTOR(-1, 0, 0, 0);
        break;
        }

        case 3:
        {		  // 1 5 6
        XMVECTOR plane = CreatePlaneFromPoints(XMLoadFloat3(&box2Points[1]), XMLoadFloat3(&box2Points[5]), XMLoadFloat3(&box2Points[6]));
        DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
        contactNormal = toXMVECTOR(1, 0, 0, 0);
        break;
        }
        case 4:
        {		  // 2 3 6
        XMVECTOR plane = CreatePlaneFromPoints(XMLoadFloat3(&box2Points[2]), XMLoadFloat3(&box2Points[3]), XMLoadFloat3(&box2Points[6]));
        DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
        contactNormal = toXMVECTOR(0, 1, 0, 0);
        break;
        }
        case 5:
        {		  // 0 1 4
        XMVECTOR plane = CreatePlaneFromPoints(XMLoadFloat3(&box2Points[1]), XMLoadFloat3(&box2Points[5]), XMLoadFloat3(&box2Points[6]));
        DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
        contactNormal = toXMVECTOR(-1, 0, 0, 0);
        break;
        }*/
        default:
            break;
        }
        /*
        XMVECTOR AnyOutside = XMVectorOrInt(AnyOutside, Outside);
        XMVECTOR AllInside = XMVectorAndInt(AllInside, Inside);

        ContainmentType type;
        // If the sphere is outside any plane it is outside.
        if (XMVector4EqualInt(AnyOutside, XMVectorTrueInt()))
            type = DISJOINT;
        else
            // If the sphere is inside all planes it is inside.
        if (XMVector4EqualInt(AllInside, XMVectorTrueInt()))
            type = CONTAINS;
        else
            // The sphere is not inside all planes or outside a plane, it may intersect.
            type = INTERSECTS;

        if (type == CONTAINS)
        {
            Logger("Perfect");
            return contactNormal;
        }
        if (type == INTERSECTS || type == CONTAINS)
        {
            //Return here, found the plane that intersects the box
            return contactNormal;
        }
        else
        {
            contactNormal = toXMVECTOR(0, 0, 0, 0);
        }
    */	
    }

    return contactNormal;
}