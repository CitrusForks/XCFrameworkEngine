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

#include "SubMesh.h"
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

    virtual void                         Init(int resourceId, std::string userFriendlyName, bool loaded = false);
    virtual void                         Load(const void* buffer);
    virtual void                         Load(std::string fileName, float intialScaling = 1.0f);
    virtual void                         Destroy();
    void                                 Update(float dt);
    void                                 Draw(RenderContext& context, SHADERTYPE shaderType);

    void                                 setVertexFormat(EVertexFormat vertexFormat)                     { m_vertexFormat = vertexFormat;   }
    virtual void                         createBuffers(EVertexFormat formatType);

    SubMesh*                             createAndGetSubMesh();
    OrientedBoundingBox*                 getComputedAABoundBox()                                         { return m_computedBoundBox.get(); }

    bool                                 IsSkinnedMesh() { return m_isSkinnedMesh; }


#if CUSTOM_ANIMATION
    MeshAnimator*                        GetMeshAnimator() { return m_meshAnimator; }
#endif

    SceneAnimator*                       GetSceneAnimator() { return m_sceneAnimator; }
    const aiScene*                       GetSceneNode() { return m_scene; }

protected:
    virtual void                         DrawSubMeshes(RenderContext& renderContext, SHADERTYPE shaderType);

#if defined(XCGRAPHICS_DX11)
    void                                 buildBuffer(unsigned int sizeOfType, void* ptrToBuffer, unsigned int length, ID3D11Buffer* buffer, D3D11_BUFFER_DESC desc);
#endif

    int                                  getSizeFromVertexFormat(EVertexFormat format);
    void                                 filterSubMeshes();

    std::vector<SubMesh*>                   m_subMeshes;

#if CUSTOM_ANIMATION
    MeshNode*                               m_meshRootNode; //Root node of the mesh.
    MeshAnimator*                           m_meshAnimator;
#endif

    EVertexFormat                           m_vertexFormat;
    std::unique_ptr<OrientedBoundingBox>    m_computedBoundBox; //This is computed in the object space. Every actor having this mesh should maintain a clone of this and update that and set this every frame
    
    //These will be applied in model space of the mesh, that is transformation on vertex based to align with the world.
    float                                   m_initialScaling;
    XCVec3Unaligned                         m_initialRotation;
    
    bool                                    m_areBuffersCreated;
    ResourceManager*                        m_resourceManager;
    Texture2D*                              m_texture;
    
    bool                                    m_isSkinnedMesh;
    const aiScene*                          m_scene;
    SceneAnimator*                          m_sceneAnimator;
    Assimp::Importer                        m_importer;

    float                                   m_lastPlayedAnimTime;
};