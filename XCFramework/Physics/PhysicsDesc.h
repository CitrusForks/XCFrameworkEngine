/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GraphicsBase/MeshTypes.h"

enum PhysicsBoundType
{
    PhysicsBoundType_Sphere,
    PhysicsBoundType_Plane,
    PhysicsBoundType_Capsule,
    PhysicsBoundType_Box,
    PhysicsBoundType_ConvexMesh,
    PhysicsBoundType_TriangleMesh,
    PhysicsBoundType_HeightField,

    PhysicsBoundType_Max
};

enum PhysicsActorType
{
    PhysicsActorType_RigidDynamic,
    PhysicsActorType_RigidStatic,
    PhysicsActorType_RigidPlane,

    PhysicsActorType_Max
};

struct PhysicsDesc
{
    //Required if m_actorType = PhysicsActorType_RigidStatic & m_boundType = PhysicsBoundType_TriangleMesh
    struct PhysicsMeshDesc
    {
        PhysicsMeshDesc()
            : m_meshInfo(nullptr)
        {}
        PhysicsMeshDesc(std::vector<MeshInfo>& meshInfo)
            : m_meshInfo(&meshInfo)
        {}

        std::vector<MeshInfo>&        GetMeshInfo() const { return *m_meshInfo; }

        std::vector<MeshInfo>*        m_meshInfo;
    };

    //Required if m_boundType = PhysicsBoundType_Box
    struct PhysicsBoundBoxDesc
    {
        PhysicsBoundBoxDesc()
        {}
        PhysicsBoundBoxDesc(const XCVec4& dimensions)
            : m_dimensions(dimensions)
        {}

        XCVec4                              m_dimensions;
    };

    //Required if m_boundType = PhysicsBoundType_Sphere
    struct PhysicsBoundSphereDesc
    {
        PhysicsBoundSphereDesc()
            : m_sphereRadius(0.0f)
        {}
        PhysicsBoundSphereDesc(f32 sphereRadius)
            : m_sphereRadius(sphereRadius)
        {}

        f32                                 m_sphereRadius;
    };

    PhysicsDesc(PhysicsActorType actorType, PhysicsBoundType boundType, const XCVec4& pos, f32 density)
        : m_actorType(actorType)
        , m_boundType(boundType)
        , m_position(pos)
        , m_density(density)
    {}

    PhysicsActorType                        m_actorType;
    PhysicsBoundType                        m_boundType;
    XCVec4                                  m_position;
    f32                                     m_density;

    PhysicsBoundBoxDesc                     m_boundBoxDesc;
    PhysicsBoundSphereDesc                  m_boundSphereDesc;
    PhysicsMeshDesc                         m_meshDesc;
};
