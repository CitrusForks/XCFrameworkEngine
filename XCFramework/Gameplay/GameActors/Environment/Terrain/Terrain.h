/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Environment/Terrain/SimpleTerrain.h"
#include "Graphics/XCMaterials/MaterialTypes.h"

class IPhysicsFeature;

class Terrain : public SimpleTerrain
{
public:

    DECLARE_OBJECT_CREATION(Terrain)

    Terrain();
    virtual ~Terrain(void);
 
    IActor::ActorReturnState            LoadMetaData(const void* metaData) override;
    IActor::ActorReturnState            Load() override;
    void                                SetInitialPhysicsProperties() override;
    IActor::ActorReturnState            OnLoaded() override;
    IActor::ActorReturnState            Update(f32 dt) override;
    bool                                Draw(RenderContext& renderContext) override;
    IActor::ActorReturnState            Destroy() override;

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
    IPhysicsFeature*                    m_physicsFeature;
};