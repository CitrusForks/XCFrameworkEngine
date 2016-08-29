/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"

#include "Engine/Resource/ResourceHandle.h"
#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"

#include "MeshData.h"
#include "SceneAnimator.h"

#if CUSTOM_ANIMATION
#include "MeshNode.h"
#include "MeshAnimator.h"
#endif

class XCShaderHandle;
class OrientedBoundingBox;

class XCMesh : public IResource
{
public:

    DECLARE_OBJECT_CREATION(XCMesh)

    XCMesh();
    virtual ~XCMesh(void);

    virtual void                            Init(i32 resourceId, std::string userFriendlyName)  override;
    virtual void                            Load(const void* buffer)                            override;
    virtual void                            Unload()                                            override;
    virtual void                            UpdateState()                                       override;
    virtual void                            Update(f32 dt)                                      override;
    virtual void                            Destroy()                                           override;

    virtual void                            Load(std::string fileName, f32 intialScaling = 1.0f);

    void                                    RegisterDrawable();
    void                                    UnregisterDrawable();

    void                                    InitDynamic(std::string resPath, ShaderType shaderUsage, std::string textureName, XCVec3Unaligned scaling, XCVec3Unaligned rotation);
    //Load Dynamic. Make sure the MeshData is filled up before calling this.
    void                                    LoadDynamic();

    MeshData*                               CreateAndGetSubMesh();
    void                                    DrawInstanced(PerObjectBuffer& objectBuffer);

    XCMatrix4Unaligned                      GetRootTransform();
    OrientedBoundingBox*                    GetAABB() { return m_computedBoundBox.get(); }

    bool                                    IsSkinnedMesh() { return m_isSkinnedMesh; }

#if CUSTOM_ANIMATION
    MeshAnimator*                           GetMeshAnimator() { return m_meshAnimator; }
#endif

    void                                    SetRasterType(RasterType type) { m_rasterType = type; }

protected:

    virtual bool                            LoadMesh();
    virtual void                            CreateBuffers();
    virtual void                            CreateConstantBuffer();
    void                                    FilterSubMeshes();

    //Draw calls
    virtual void                            Draw(RenderContext& context);
    virtual void                            DrawSubMesh(RenderContext& renderContext, u32 meshIndex);
    virtual void                            DrawSubMeshes(RenderContext& renderContext);


    //Member Variables--------------------------------------------------------------------------------------------------------------------------------------------------
    std::vector<MeshData*>                  m_subMeshes;
    ShaderType                              m_shaderType;
    RasterType                              m_rasterType;
    XCShaderHandle*                         m_shaderHandler;
    ResourceHandle*                         m_texture;

    //This is computed in the object space. Every actor having this mesh should maintain a clone of this and update that and set this every frame
    std::unique_ptr<OrientedBoundingBox>    m_computedBoundBox; 
    
    //Skinned mesh members
    bool                                    m_isSkinnedMesh;
    const aiScene*                          m_scene;
    SceneAnimator*                          m_sceneAnimator;
    Assimp::Importer                        m_importer;
    f32                                     m_lastPlayedAnimTime;

#if CUSTOM_ANIMATION
    MeshNode*                               m_meshRootNode; //Root node of the mesh.
    MeshAnimator*                           m_meshAnimator;
#endif

    //Mesh root node transforms
    XCVec3Unaligned                         m_globalTranslation;
    XCVec3Unaligned                         m_globalRotation;
    XCVec3Unaligned                         m_globalScaling;

    u32                                     m_instanceCount;

    //This buffer is created based on the shader type
    struct InstanceBuffer
    {
        D3DConstantBuffer*                  m_cbInstancedBufferGPU;
        cbInstancedBuffer                   m_cbInstancedBuffer;
    };

    struct BoneBuffer
    {
        D3DConstantBuffer*                  m_cbBoneBufferGPU;
        cbBoneBuffer                        m_cbBoneBuffer;
    };

    //If size > 0, then instance buffer are mapped per submeshes; 
    std::vector<InstanceBuffer>             m_instanceBuffers;
    std::vector<BoneBuffer>                 m_boneBuffers;
};