/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Environment/Terrain/SimpleTerrain.h"
#include "Graphics/XCMaterials/MaterialTypes.h"

class TerrainOBBHierarchy;

class Terrain : public SimpleTerrain
{
public:

    DECLARE_OBJECT_CREATION(Terrain)

    Terrain();
    virtual ~Terrain(void);
 
    void                                PreLoad(const void* fbBuffer) override;
    void                                UpdateState() override;
    void                                Load() override;
    void                                Update(f32 dt) override;
    void                                Draw(RenderContext& context) override;
    void                                Destroy() override;

    XCVec4                              CheckTerrainCollisionFromPoint(OrientedBoundingBox* bbox);
    XCVec4                              GetPointAtIndex(i32 pointIndex) const;

protected:
    void                                LoadHeightMap();
    void                                UnloadHeightMap();

    void                                GenerateVertices() override;
    void                                GenerateVerticesNormal();
    char                                GetHeightAt(i32 _index) const;
    XCVec3                              GetTerrainNormal(f32 x, f32 z) const;

private:
    std::string                         m_pHeightMapFileName;    //Height Map Filename
    u8*                                 m_pBitmapImage;
    Material                            m_material;

    //MultiTex
    std::vector<ResourceHandle*>        m_textures;

    TerrainOBBHierarchy*                m_OBBHierarchy;
};