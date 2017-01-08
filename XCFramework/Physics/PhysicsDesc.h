/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GraphicsBase/MeshTypes.h"

enum PhysicsBoundType
{
    PhysicsBoundType_Sphere         = 1 << 0,
    PhysicsBoundType_Plane          = 1 << 1,
    PhysicsBoundType_Capsule        = 1 << 2,
    PhysicsBoundType_Box            = 1 << 3,
    PhysicsBoundType_ConvexMesh     = 1 << 4,
    PhysicsBoundType_TriangleMesh   = 1 << 5,
    PhysicsBoundType_HeightField    = 1 << 6,

    PhysicsBoundType_Max            = 0xFFFFFFFF
};

enum PhysicsBodyType
{
    PhysicsBodyType_RigidDynamic,
    PhysicsBodyType_RigidStatic,
    PhysicsBodyType_RigidPlane,

    PhysicsBodyType_Max
};

struct PhysicsDesc
{
    //Required if m_bodyType = PhysicsBodyType_RigidStatic & m_boundType = PhysicsBoundType_TriangleMesh
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

    //Required if m_bodyType = PhysicsBodyType_RigidStatic & m_boundType = PhysicsBoundType_HeightField
    struct PhysicsHeightFieldDesc
    {
        PhysicsHeightFieldDesc()
            : m_meshInfo(nullptr)
        {}
        PhysicsHeightFieldDesc(std::vector<MeshInfo>& meshInfo, u32 rows, u32 cols)
            : m_meshInfo(&meshInfo)
            , m_rows(rows)
            , m_cols(cols)
        {}

        std::vector<MeshInfo>&        GetMeshInfo() const { return *m_meshInfo; }

        std::vector<MeshInfo>*        m_meshInfo;
        u32                           m_rows;
        u32                           m_cols;
    };

    //Fill the BoundType desc
    union BoundTypeDesc
    {
        BoundTypeDesc(PhysicsBoundType type)
            : m_boundType(type)
        {}

        PhysicsBoundType                        m_boundType;
        PhysicsBoundBoxDesc                     m_boundBoxDesc;
        PhysicsBoundSphereDesc                  m_boundSphereDesc;
        PhysicsMeshDesc                         m_meshDesc;
        PhysicsHeightFieldDesc                  m_heightFieldDesc;
    };

    PhysicsDesc(PhysicsBodyType actorType, PhysicsBoundType boundType, const XCVec4& pos, f32 mass, f32 dampForce)
        : m_bodyType(actorType)
        , m_boundBoxDesc(boundType)
        , m_position(pos)
        , m_mass(mass)
        , m_dampForce(dampForce)
    {}

    PhysicsBodyType                         m_bodyType;
    BoundTypeDesc                           m_boundBoxDesc;
    XCVec4                                  m_position;
    f32                                     m_mass;
    f32                                     m_dampForce;
};
