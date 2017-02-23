/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "CollisionDetection.h"
#include "ParticleContact.h"

#include "Engine/GameplayBase/Actors/PhysicsActor.h"

#include "Gameplay/GameActors/Environment/Terrain/Terrain.h"

#include "IPhysicsBoundVolume.h"
#include "Phusike/OBBBoundVolume.h"
#include "Phusike/HeightfieldBoundVolume.h"


TriangleOrderingType CollisionDetection::DetermineOrderOfTriangle(XCVec4& v1, XCVec4& v2, XCVec4& v3, XCVec4& viewPoint)
{
    XCMatrix4 detMatrix;

    v1.Set<W>(1);
    v2.Set<W>(1);
    v3.Set<W>(1);
    viewPoint.Set<W>(1);

    detMatrix[0] = v1;
    detMatrix[1] = v2;
    detMatrix[2] = v3;
    detMatrix[3] = viewPoint;

    f32 det = 0.0f;
    MatrixDeterminant(detMatrix, det);

    if(det > 0)
    {
        return TriangleOrderingType_AntiClockwise;
    }
    else if(det < 0)
    {
        return TriangleOrderingType_Clockwise;
    }
    else
    {
        return TriangleOrderingType_Undefined;
    }
}

DirectX::ContainmentType CollisionDetection::CheckObbObbContainment(const IPhysicsBoundVolume* obb1, const IPhysicsBoundVolume* obb2)
{
    return static_cast<const OBBBoundVolume*>(obb1)->Contains(static_cast<const OBBBoundVolume*>(obb2));
}

XCVec4 CollisionDetection::GetHeightfieldPointOfContactWithBoundBox(const IPhysicsBoundVolume* bboxActor, const IPhysicsBoundVolume* terrain)
{
    return static_cast<const HeightfieldBoundVolume*>(terrain)->GetPositionOfContact(bboxActor);
}

XCVec4 CollisionDetection::GetContactNormalFromOBBToOBBTriangleTest(const IPhysicsBoundVolume* bboxVolume1, const IPhysicsBoundVolume* bboxVolume2)
{
    const OBBBoundVolume* bbox1 = static_cast<const OBBBoundVolume*>(bboxVolume1);
    const OBBBoundVolume* bbox2 = static_cast<const OBBBoundVolume*>(bboxVolume2);

    XCVec4 contactNormal;

    DirectX::XMFLOAT3 points[OBBBoundVolume::MaxOBBCornerPoints];
    bbox2->GetTransformedBox().GetCorners(&points[0]);

    //int val = checkOBBTriangleIntersection(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6]), bbox1);
    //determineOrderOfTriangle(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5]), XMLoadFloat3(&bbox1->m_TransformedBox.Center));
    
    //1
    //0 1 2
    if (bbox1->GetTransformedBox().Intersects(DirectX::XMLoadFloat3(&points[0]), DirectX::XMLoadFloat3(&points[1]), DirectX::XMLoadFloat3(&points[2])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[0]), XCVec4(points[2]), XCVec4(points[1]));

        if (DetermineOrderOfTriangle(XCVec4(points[0]), XCVec4(points[2]), XCVec4(points[1]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        if (!(contactNormal.Get<Y>() > 0.0f || contactNormal.Get<Y>() < 0.0f))
            return contactNormal;
    }

    //2
    //0 2 3
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[0]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[3])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[0]), XCVec4(points[2]), XCVec4(points[3]));
        if (DetermineOrderOfTriangle(XCVec4(points[0]), XCVec4(points[2]), XCVec4(points[3]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        if (!(contactNormal.Get<Y>() > 0.0f || contactNormal.Get<Y>() < 0.0f))
            return contactNormal;
    }

    //3
    //4 6 5 
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[5])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[6]), XCVec4(points[5]));
        if (DetermineOrderOfTriangle(XCVec4(points[4]), XCVec4(points[6]), XCVec4(points[5]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        if (!(contactNormal.Get<Y>() > 0.0f || contactNormal.Get<Y>() < 0.0f))
            return contactNormal;
    }

    //4
    //4 7 6
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[7]), XMLoadFloat3(&points[6])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[7]), XCVec4(points[6]));
        if (DetermineOrderOfTriangle(XCVec4(points[4]), XCVec4(points[7]), XCVec4(points[6]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        if (!(contactNormal.Get<Y>() > 0.0f || contactNormal.Get<Y>() < 0.0f))
            return contactNormal;
    }

    //5
    //4 5 1
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[1])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[5]), XCVec4(points[1]));
        if (DetermineOrderOfTriangle(XCVec4(points[4]), XCVec4(points[5]), XCVec4(points[1]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //6
    //4 1 0
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[1]), XMLoadFloat3(&points[0])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[1]), XCVec4(points[0]));
        if (DetermineOrderOfTriangle(XCVec4(points[4]), XCVec4(points[1]), XCVec4(points[0]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //7
    //3 2 6
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[2]), XMLoadFloat3(&points[6])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[3]), XCVec4(points[2]), XCVec4(points[6]));
        if (DetermineOrderOfTriangle(XCVec4(points[3]), XCVec4(points[2]), XCVec4(points[6]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //8
    //3 6 7
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[3]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[7])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[3]), XCVec4(points[6]), XCVec4(points[7]));
        if (DetermineOrderOfTriangle(XCVec4(points[3]), XCVec4(points[6]), XCVec4(points[7]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0.0f || XMVectorGetY(contactNormal) < 0.0f))
            return contactNormal;
    }

    //9
    //1 5 6
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[5]), XMLoadFloat3(&points[6])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[1]), XCVec4(points[5]), XCVec4(points[6]));
        if (DetermineOrderOfTriangle(XCVec4(points[1]), XCVec4(points[5]), XCVec4(points[6]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
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
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[1]), XMLoadFloat3(&points[6]), XMLoadFloat3(&points[2])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[1]), XCVec4(points[6]), XCVec4(points[2]));
        if (DetermineOrderOfTriangle(XCVec4(points[1]), XCVec4(points[6]), XCVec4(points[2]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }
        //if (!(XMVectorGetY(contactNormal) > 0 || XMVectorGetY(contactNormal) < 0))
            return contactNormal;
    }

    //11
    //4 0 3
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[0]), XMLoadFloat3(&points[3])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[0]), XCVec4(points[3]));
        if (DetermineOrderOfTriangle(XCVec4(points[4]), XCVec4(points[0]), XCVec4(points[3]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
        {
            contactNormal = -contactNormal;
        }

        //if (!(XMVectorGetY(contactNormal) > 0 || XMVectorGetY(contactNormal) < 0))
            return contactNormal;
    }

    //12
    //4 3 7
    if (bbox1->GetTransformedBox().Intersects(XMLoadFloat3(&points[4]), XMLoadFloat3(&points[3]), XMLoadFloat3(&points[7])))
    {
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[3]), XCVec4(points[7]));
        if (DetermineOrderOfTriangle(XCVec4(points[4]), XCVec4(points[3]), XCVec4(points[7]), XCVec4(bbox1->GetTransformedBox().Center)) == TriangleOrderingType_Clockwise)
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


#if defined(RESEARCH_IN_PROGRESS)

i32	CollisionDetection::CheckOBBTriangleIntersection(XCVec4& v1, XCVec4& v2, XCVec4& v3, IPhysicsBoundVolume* bbox)
{
    /*
    float p0, p2, r;

    //Get center and extends of the bound box
    XCVec4 center(bbox->m_TransformedBox.Center);

    float extend0 = bbox->m_TransformedBox.Extents.x;
    float extend1 = bbox->m_TransformedBox.Extents.y;
    float extend2 = bbox->m_TransformedBox.Extents.z;

    //Translate triangle as conceptually moving AABB to origin
    v0 = v0 - center;
    v1 = v1 - center;
    v2 = v2 - center;

    //Compute Edge vectors for triangles
    XCVec4 f0 = v1 - v0;
    XCVec4 f1 = v2 - v1;
    XCVec4 f2 = v0 - v2;

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
    plane.SetPlaneNormal(VectorCross(f0, f1));
    plane.SetDConstant(VectorDot(plane.GetPlaneNormal(), f0));

    return CheckOBBPlaneIntersection(bbox, &plane);
    */
    return false;
}

i32 CollisionDetection::CheckOBBPlaneIntersection(IPhysicsBoundVolume* bbox, Plane* p)
{
    //Compute  projection interval radius of box onto L(t) = b.x + t * p.n
    //XMVECTOR normDotOrient = VectorDot(p->GetPlaneNormal(), bbox->m_TransformedBox.)
    //float r = bbox->m_TransformedBox.Extents.x * abs(VectorDot(p->GetPlaneNormal(), ))
    return 1;
}


XCVec4 CollisionDetection::GetContactNormalFromBoundBoxContainedPoints(IPhysicsBoundVolume* bbox, XCVec4* points, i32 noOfPoints)
{
    XCVec4 contactNormal;
    //Got points
    //Now check for containment of these points within the obj1 box
    for (i32 index = 0; index < noOfPoints; index++)
    {
        DirectX::ContainmentType type = bbox->m_TransformedBox.Contains(points[index].GetPlatformIntrinsic());

        XCVec4 norm = XCVec4(0.0, 0.0, 0.0, 0.0);
        if (type == DirectX::ContainmentType::CONTAINS)
        {
            //Got point calculate contact normal.
            switch (index)
            {
                //Seems to be front face, based on z axis
            case 0:
                norm = XCVec4(-0.5, 0, -0.5, 0.0);
                contactNormal += norm;
                break;
            case 1:
                norm = XCVec4(0.5, 0, -0.5, 0.0f);
                contactNormal += norm;
                break;
            case 2:
                norm = XCVec4(-0.5, 0, -0.5, 0.0f);
                contactNormal += norm;
                break;
            case 3:
                norm = XCVec4(0.5, 0, -0.5, 0.0f);
                contactNormal += norm;
                break;

                //May be back face, yet to confirm
            case 4:
                norm = XCVec4(-0.5, 0, 0.5, 0.0f);
                contactNormal += norm;
                break;
            case 5:
                norm = XCVec4(0.5, 0, 0.5, 0.0f);
                contactNormal += norm;
                break;
            case 6:
                norm = XCVec4(-0.5, 0, 0.5, 0.0f);
                contactNormal += norm;
                break;
            case 7:
                norm = XCVec4(0.5, 0, 0.5, 0.0f);
                contactNormal += norm;
                break;
            }
        }
    }

    return VectorNormalize<3>(contactNormal);
}

XCVec4 CollisionDetection::GetContactNormalFromBoundBoxContainedBoundBox(IPhysicsBoundVolume* bbox1, IPhysicsBoundVolume* bbox2)
{
    //Get 4 points from every face, check containment of these points in the bound box, if at least 3 points contained or intersected, return the face normal
    //Face 1 : 4 5 6 7
    //Face 2 : 0 1 2 3
    XCVec4 contactNormal;

    DirectX::XMFLOAT3 points[OrientedBoundingBox::MaxOBBCornerPoints];
    bbox2->m_TransformedBox.GetCorners(&points[0]);

    i32 hitCount = 0;
    DirectX::ContainmentType type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[4]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[5]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[6]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[7]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;

    if (hitCount >= 2)
    {
        //This face is collided.
        contactNormal = GetNormalFromPoints(XCVec4(points[4]), XCVec4(points[6]), XCVec4(points[5]));

        return contactNormal;
    }
    /*else if (hitCount <= 2)
    {
        //May be check for 2 hitcounts, like 45 deg contact normal
    }
    */
    //Face 0 1 2 3
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[0]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[1]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[2]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;
    type = bbox1->m_TransformedBox.Contains(XMLoadFloat3(&points[3]));
    hitCount += type == DirectX::CONTAINS || type == DirectX::INTERSECTS ? true : false;

    if (hitCount >= 2)
    {
        //This face is collided.
        contactNormal = GetNormalFromPoints(XCVec4(points[0]), XCVec4(points[1]), XCVec4(points[2]));

        return contactNormal;
    }

    return contactNormal;
}


XCVec4 CollisionDetection::GetContactNormalFromBoundBoxContainedPlane(IPhysicsBoundVolume* bbox1, IPhysicsBoundVolume* bbox2)
{
    //This algorithm is not suitable, we have a situation where,
    //When collision occurs, some points of bbox1 are inside bbox2, now from the front side of plane normal, it's true and also from back side of plane normal is 
    //true because some points are behind the plane. So this case will occur for every plane and we cannot distinguish the distinct planes.

    //May be, create a separating axis just before collision occurs, we can detect which plane is closest to the bbox1, and then conclude with the closest plane normal.
    XCVec4 contactNormal;

    //Get the points of 2nd object
    DirectX::XMFLOAT3 box2Points[8];
    bbox2->m_TransformedBox.GetCorners(box2Points);

    //Form planes for -1 z axis to get normal (0, 0, -1);
    //We have [4] [5] [6] [7], find normal to one of the triangle and normalize and set it as plane
    XCMatrix4 R = MatrixRotationQuaternion(XCVec4(bbox1->m_TransformedBox.Orientation));
    XCVec4 Outside, Inside;

    for (i32 index = 0; index < 6; index++)
    {
        switch (index)
        {
        case 0:
        {		  // 4 5 6 
                  XCVec4 plane = CreatePlaneFromPoints(XCVec4(box2Points[5]), XCVec4(box2Points[4]), XCVec4(box2Points[6]));

                  //DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), XMLoadFloat3(&bbox1->m_TransformedBox.Extents), R.r[0], R.r[1], R.r[2], plane, Outside, Inside);
                  //PlaneIntersectionType    XM_CALLCONV     Intersects(_In_ FXMVECTOR Plane) const;
                  DirectX::PlaneIntersectionType type = bbox1->m_TransformedBox.Intersects(plane.GetPlatformIntrinsic());
                  
                  if (type == DirectX::FRONT || type == DirectX::INTERSECTING)
                  {
                      return XCVec4(0, 0, -1, 0);
                  }
                  break;
        }

        case 1:
        {		  // 0 1 2
                  XCVec4 plane = CreatePlaneFromPoints(XCVec4(box2Points[1]), XCVec4(box2Points[0]), XCVec4(box2Points[2]));
#if defined(WIN32)
                  DirectX::Internal::FastIntersectOrientedBoxPlane(XMLoadFloat3(&bbox1->m_TransformedBox.Center), 
                      XMLoadFloat3(&bbox1->m_TransformedBox.Extents), 
                      R[0].GetPlatformIntrinsic(), 
                      R[1].GetPlatformIntrinsic(), 
                      R[2].GetPlatformIntrinsic(), 
                      plane.GetPlatformIntrinsic(),
                      Outside.GetPlatformIntrinsic(),
                      Inside.GetPlatformIntrinsic());
#endif
                  contactNormal = XCVec4(0, 0, 1, 0);
                  DirectX::PlaneIntersectionType type = bbox1->m_TransformedBox.Intersects(plane.GetPlatformIntrinsic());

                  if (type == DirectX::FRONT || type == DirectX::INTERSECTING)
                  {
                      return XCVec4(0, 0, 1, 0);
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

#endif