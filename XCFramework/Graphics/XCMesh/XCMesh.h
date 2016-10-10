/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"

#include "Engine/Resource/ResourceHandle.h"
#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"

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

    //This returns the initial transform state from data multiplied by the root transform of the mesh
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

    //Computations
    template<class VertexFormat>
    void                                    CalculateMeshNormals(std::vector<VertexFormat>& vertices, std::vector<u32>& indices);

    //Draw calls
    virtual void                            Draw(RenderContext& context);
    virtual void                            DrawSubMesh(RenderContext& renderContext, u32 meshIndex);
    virtual void                            DrawSubMeshes(RenderContext& renderContext);
    void                                    OnRenderComplete();

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
        GPUResource*                        m_cbInstancedBufferGPU;
        cbInstancedBuffer                   m_cbInstancedBuffer;
    };

    struct BoneBuffer
    {
        GPUResource*                        m_cbBoneBufferGPU;
        cbBoneBuffer                        m_cbBoneBuffer;
    };

    //If size > 0, then instance buffer are mapped per submeshes; 
    std::vector<InstanceBuffer>             m_instanceBuffers;
    std::vector<BoneBuffer>                 m_boneBuffers;
};


template<class VertexFormat>
void XCMesh::CalculateMeshNormals(std::vector<VertexFormat>& vertices, std::vector<u32>& indices)
{
    //Calculate the normals of the mesh.
    for (u32 vertexIndex = 0; vertexIndex < indices.size() - 3; vertexIndex += 3)
    {
        XCVec4 v1(vertices[indices[vertexIndex]].Pos);
        XCVec4 v2(vertices[indices[vertexIndex + 1]].Pos);
        XCVec4 v3(vertices[indices[vertexIndex + 2]].Pos);

        v1.Set<W>(1.0f); v2.Set<W>(1.0f); v3.Set<W>(1.0f);

        //Average the normals if the vertex is shared in multiple faces.
        XCVec4 vertexNormal = GetNormalFromPoints(v1, v2, v3);
        vertexNormal += GetNormalFromPoints(v2, v1, v3);
        vertexNormal += GetNormalFromPoints(v3, v1, v2);

        vertexNormal = VectorNormalize<3>(vertexNormal);

        vertices[indices[vertexIndex]].Norm = vertexNormal.GetUnaligned3();
        vertices[indices[vertexIndex + 1]].Norm = vertexNormal.GetUnaligned3();
        vertices[indices[vertexIndex + 2]].Norm = vertexNormal.GetUnaligned3();
    }
}