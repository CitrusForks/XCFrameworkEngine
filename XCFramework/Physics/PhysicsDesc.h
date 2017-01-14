/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GraphicsBase/MeshTypes.h"
#include "Graphics/XCShaders/XCVertexFormat.h"

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
        PhysicsBoundBoxDesc(const XCVec4& min, const XCVec4& max)
            : m_min(min)
            , m_max(max)
        {}

        XCVec4                              m_min;
        XCVec4                              m_max;
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
            : m_rows(0)
            , m_cols(0)
            , m_vertexBuffer(nullptr)
            , m_vertexFormat(VertexFormat_Invalid)
        {}
        PhysicsHeightFieldDesc(u32 rows, u32 cols, void* vertexBuffer, VertexFormat format)
            : m_rows(rows)
            , m_cols(cols)
            , m_vertexBuffer(vertexBuffer)
            , m_vertexFormat(format)
        {}

        u32                           m_rows;
        u32                           m_cols;

        void*                         m_vertexBuffer;
        VertexFormat                  m_vertexFormat;
    };

    //Fill the BoundType desc
    struct BoundTypeDesc
    {
        BoundTypeDesc(PhysicsBoundType boundType)
            : m_boundType(boundType)
        {}
        
        union BoundDesc
        {
            BoundDesc() {}

            PhysicsBoundBoxDesc        m_boundBoxDesc;
            PhysicsBoundSphereDesc     m_boundSphereDesc;
            PhysicsMeshDesc            m_meshDesc;
            PhysicsHeightFieldDesc     m_heightFieldDesc;
        };

        BoundDesc                      m_boundDesc;
        PhysicsBoundType               m_boundType;
    };

    PhysicsDesc(PhysicsBodyType actorType, PhysicsBoundType boundType, const XCVec4& pos, f32 mass, f32 dampForce)
        : m_bodyType(actorType)
        , m_boundVolumeDesc(boundType)
        , m_position(pos)
        , m_mass(mass)
        , m_dampForce(dampForce)
    {}

    PhysicsBodyType                         m_bodyType;
    BoundTypeDesc                           m_boundVolumeDesc;
    XCVec4                                  m_position;
    f32                                     m_mass;
    f32                                     m_dampForce;
};
