/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Environment/Terrain/SimpleTerrain.h"
#include "Gameplay/XCPhysics/OBBHierarchy.h"
#include "Engine/Graphics/XC_Materials/MaterialTypes.h"

class Terrain : public SimpleTerrain
{
public:

    DECLARE_OBJECT_CREATION(Terrain)

    Terrain();
    virtual ~Terrain(void);
 
    virtual void                        PreLoad(const void* fbBuffer);
    virtual void                        PreLoad(const char* _pHeightMapFileName, std::string terrainTexture, std::string terrainTexture1, std::string terrainTexture2, std::string blendMap, XCVec3 _initialPosition, int _rows, int _column, float _rowSpacing, float _colSpacing);
    virtual void                        UpdateState();
    virtual void                        Load();
    virtual void                        GenerateVertices();
    virtual void                        GenerateVerticesNormal();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                LoadHeightMap();
    void                                UnloadHeightMap();

    char                                GetHeightAt(int _index) const;
    XCVec3                              GetTerrainNormal(float x, float z) const;
    XCVecIntrinsic4                     GetPointAtIndex(int pointIndex) const;

    XCVecIntrinsic4                     CheckTerrainCollisionFromPoint(OrientedBoundingBox* bbox);

protected:
    std::string                         m_pHeightMapFileName;    //Height Map Filename
    unsigned char*                      m_pBitmapImage;
    BasicMaterial                       m_material;

    //MultiTex
    std::vector<ResourceHandle*>        m_textures;

    std::unique_ptr<OBBHierarchy>       m_OBBHierarchy;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*                  m_pCBPerObject;
#endif
};