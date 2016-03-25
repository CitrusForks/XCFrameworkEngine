/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"

#include "Engine/Resource/ResourceManager.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Engine/Graphics/BasicGeometry/OrientedBoundingBox.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"

#include "MeshData.h"
#include "SceneAnimator.h"

#if CUSTOM_ANIMATION
#include "MeshNode.h"
#include "MeshAnimator.h"
#endif

class XCMesh : public IResource
{
public:
    DECLARE_OBJECT_CREATION(XCMesh)

    XCMesh();
    virtual ~XCMesh(void);

    virtual void                            Init(int resourceId, std::string userFriendlyName, bool loaded = false);
    virtual void                            Load(const void* buffer);
    virtual void                            Load(std::string fileName, float intialScaling = 1.0f);
    virtual void                            Update(float dt);
    virtual void                            Destroy();

    void                                    DrawInstanced(PerObjectBuffer& objectBuffer);

    MeshData*                               CreateAndGetSubMesh();
    OrientedBoundingBox*                    GetComputedAABoundBox() { return m_computedBoundBox.get(); }

    bool                                    IsSkinnedMesh() { return m_isSkinnedMesh; }

#if CUSTOM_ANIMATION
    MeshAnimator*                           GetMeshAnimator() { return m_meshAnimator; }
#endif

    SceneAnimator*                          GetSceneAnimator() { return m_sceneAnimator; }
    const aiScene*                          GetSceneNode() { return m_scene; }

protected:

    virtual void                            CreateBuffers();
    void                                    CreateConstantBuffer();
#if defined(XCGRAPHICS_DX11)
    void                                    BuildBuffer(unsigned int sizeOfType, void* ptrToBuffer, unsigned int length, ID3D11Buffer* buffer, D3D11_BUFFER_DESC desc);
#endif

    virtual void                            Draw(RenderContext& context);
    virtual void                            DrawSubMesh(RenderContext& renderContext, unsigned int meshIndex);
    virtual void                            DrawSubMeshes(RenderContext& renderContext);



    int                                     GetSizeFromVertexFormat(VertexFormat format);
    void                                    FilterSubMeshes();

    //Member Variables
    std::vector<MeshData*>                  m_subMeshes;

#if CUSTOM_ANIMATION
    MeshNode*                               m_meshRootNode; //Root node of the mesh.
    MeshAnimator*                           m_meshAnimator;
#endif

    ShaderType                              m_shaderType;
    XCShaderHandle*                         m_shaderHandler;

    std::unique_ptr<OrientedBoundingBox>    m_computedBoundBox; //This is computed in the object space. Every actor having this mesh should maintain a clone of this and update that and set this every frame
    
    ResourceManager*                        m_resourceManager;
    Texture2D*                              m_texture;
    
    bool                                    m_isSkinnedMesh;
    const aiScene*                          m_scene;
    SceneAnimator*                          m_sceneAnimator;
    Assimp::Importer                        m_importer;

    float                                   m_lastPlayedAnimTime;

    //Mesh root node transforms
    XCVec3Unaligned                         m_globalTranslation;
    XCVec3Unaligned                         m_globalRotation;
    XCVec3Unaligned                         m_globalScaling;

    unsigned int                            m_instanceCount;

    //This buffer is created based on the shader type
    D3DConstantBuffer*                      m_cbInstancedBufferGPU;
    cbInstancedBuffer                       m_cbInstancedBuffer;
};