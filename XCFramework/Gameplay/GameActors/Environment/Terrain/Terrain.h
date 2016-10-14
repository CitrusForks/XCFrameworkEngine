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
 
    virtual void                        PreLoad(const void* fbBuffer);
    virtual void                        PreLoad(const char* _pHeightMapFileName, std::string terrainTexture, std::string terrainTexture1, std::string terrainTexture2, std::string blendMap, XCVec3& _initialPosition, i32 _rows, i32 _column, f32 _rowSpacing, f32 _colSpacing);
    virtual void                        UpdateState();
    virtual void                        Load();
    virtual void                        GenerateVertices();
    virtual void                        GenerateVerticesNormal();
    virtual void                        Update(f32 dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                LoadHeightMap();
    void                                UnloadHeightMap();

    char                                GetHeightAt(i32 _index) const;
    XCVec3                              GetTerrainNormal(f32 x, f32 z) const;
    XCVec4                              GetPointAtIndex(i32 pointIndex) const;

    XCVec4                              CheckTerrainCollisionFromPoint(OrientedBoundingBox* bbox);

protected:
    std::string                         m_pHeightMapFileName;    //Height Map Filename
    u8*                                 m_pBitmapImage;
    Material                            m_material;

    //MultiTex
    std::vector<ResourceHandle*>        m_textures;

    TerrainOBBHierarchy*                       m_OBBHierarchy;
};