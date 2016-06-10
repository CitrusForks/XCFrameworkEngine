/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "XCMesh.h"

#include "Engine/Resource/ResourceManager.h"
#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Mesh/XC3DSMeshLoader.h"
#include "Graphics/XC_Lighting/XC_LightManager.h"

XCMesh::XCMesh()
{
    m_resourceType = RESOURCETYPE_MESH;
    m_isSkinnedMesh = false;
    m_scene = nullptr;
    m_sceneAnimator = nullptr;

    m_lastPlayedAnimTime = 1.0f;
    m_instanceCount = 0;
}

void XCMesh::Init(int resourceId, std::string userFriendlyName)
{
    IResource::Init(resourceId, userFriendlyName);

    m_computedBoundBox = std::make_unique<OrientedBoundingBox>();
    m_computedBoundBox->Init();

    RegisterDrawable();
}

XCMesh::~XCMesh(void)
{
    if (m_sceneAnimator)
    {
        delete m_sceneAnimator;
    }

    if (m_scene)
    {
        delete m_scene;
    }
}

void XCMesh::Load(std::string fileName, float initialScaling /* = 1.0f */)
{
    IResource::Load(fileName);

    m_globalScaling = XCVec3Unaligned(initialScaling, initialScaling, initialScaling);

    m_resourcePath = fileName;

    //Isolate this XC3DSMeshLoader... Make it static and not like this
    XC3DSMeshLoader loader;
    loader.loadMeshFromFile(fileName, this);

    //Filter out objects with zero vertices
    FilterSubMeshes();

    //Create the buffers
    CreateBuffers();

    //Create Constant Buffers
    CreateConstantBuffer();
}

void XCMesh::Unload()
{
    //Clear the instance bufferss
    for (auto instanceBuffer : m_instanceBuffers)
    {
        m_shaderHandler->DestroyConstantBuffer(instanceBuffer.m_cbInstancedBufferGPU);
    }

    m_instanceBuffers.clear();

    for (auto boneBuffer : m_boneBuffers)
    {
        m_shaderHandler->DestroyConstantBuffer(boneBuffer.m_cbBoneBufferGPU);
    }

    m_boneBuffers.clear();


    for (auto& subMesh : m_subMeshes)
    {
        subMesh->Destroy();
        delete(subMesh);
    }

    m_subMeshes.clear();
}

void XCMesh::UpdateState()
{
    if (m_resourceUpdated && m_texture && m_texture->m_Resource->IsLoaded())
    {
        m_resourceState = IResource::ResourceState_Loaded;
    }

    IResource::UpdateState();
}

void XCMesh::Load(const void* buffer)
{
    if (buffer)
    {
        const FBXCMesh* fbXCMesh = (const FBXCMesh*)buffer;

        m_resourcePath = getPlatformPath(fbXCMesh->ResourcePath()->c_str());

        ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
        m_texture = &resMgr.AcquireResource(fbXCMesh->TextureRes()->c_str());

        //Transforms
        m_globalScaling = XCVec3Unaligned(fbXCMesh->InitialScaling()->x(), fbXCMesh->InitialScaling()->y(), fbXCMesh->InitialScaling()->z());
        m_globalRotation = XCVec3Unaligned(fbXCMesh->InitialRotation()->x(), fbXCMesh->InitialRotation()->y(), fbXCMesh->InitialRotation()->z());
        m_shaderType = fbXCMesh->ShaderUsage();

        XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
        m_shaderHandler = (XCShaderHandle*)graphicsSystem.GetShaderManagerSystem().GetShader(m_shaderType);
    }

    LoadDynamic();

    IResource::Load(buffer);
}

void XCMesh::InitDynamic(std::string resPath, ShaderType shaderUsage, std::string textureName, XCVec3Unaligned scaling, XCVec3Unaligned rotation)
{
    m_resourcePath = resPath;

    m_computedBoundBox = std::make_unique<OrientedBoundingBox>();
    m_computedBoundBox->Init();

    RegisterDrawable();

    m_shaderType = shaderUsage;

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_texture = &resMgr.AcquireResource(textureName.c_str());

    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    m_shaderHandler = (XCShaderHandle*)graphicsSystem.GetShaderManagerSystem().GetShader(m_shaderType);

    m_globalScaling = scaling;
    m_globalRotation = rotation;
}

void XCMesh::LoadDynamic()
{
    LoadMesh();

    //Filter out objects with zero vertices
    FilterSubMeshes();

    //Create the buffers
    CreateBuffers();

    //Create Constant Buffers
    CreateConstantBuffer();

    m_resourceUpdated = true;
}

XCMatrix4Unaligned XCMesh::GetRootTransform()
{
    return aiMatrixToMatrix4Unaligned(m_sceneAnimator->GetGlobalTransform(m_scene->mRootNode));
}

bool XCMesh::LoadMesh()
{
    //Default loader will load 3ds mesh
    XC3DSMeshLoader loader;
    return loader.loadMeshFromFile(m_resourcePath, this);
}

MeshData* XCMesh::CreateAndGetSubMesh()
{
    MeshData* subMesh = new MeshData(m_shaderType);
    m_subMeshes.push_back(subMesh);
    return m_subMeshes.back();
}

void XCMesh::CreateBuffers()
{
    XCVecIntrinsic4 vMin = toXMVECTOR(Infinity, Infinity, Infinity, 1);
    XCVecIntrinsic4 vMax = toXMVECTOR(-Infinity, -Infinity, -Infinity, 1);

    aiMesh** mesh = nullptr;
    if (IsSkinnedMesh())
    {
        if (m_scene)
            mesh = m_scene->mMeshes;
    }
    
    for (unsigned int objIndex = 0; objIndex < m_subMeshes.size(); objIndex++)
    {
        MeshData* submesh = m_subMeshes[objIndex];

        //Calculate the global translation matrix * this sub mesh trnaslation matrix
        const XCVec3Unaligned& localTrans = submesh->GetGeometryTranslation();
        const XCMatrix4 globalTranslationMat = XMMatrixTranslation(localTrans.x, localTrans.y, localTrans.z);

        //Calculate the global rotation matrix * this sub mesh rotation matrix
        const XCVec3Unaligned& localRot = submesh->GetGeometryRotation();
        const XCMatrix4 globalRotationMat = XMMatrixRotationX(m_globalRotation.x + localRot.x) * XMMatrixRotationY(m_globalRotation.y + localRot.y) * XMMatrixRotationZ(m_globalRotation.z + localRot.z);

        //Calculate the global scaling matrix * this sub mesh scaling matrix
        const XCVec3Unaligned& localScale = submesh->GetGeometryScaling();
        const XCMatrix4 globalScalingMat = XMMatrixScaling(m_globalScaling.x, m_globalScaling.y, m_globalScaling.z) * XMMatrixScaling(localScale.x, localScale.y, localScale.z);

        const XCMatrix4 transformMatrix = globalScalingMat * globalRotationMat * globalTranslationMat;

        XCVecIntrinsic4 transformedVertex;
        XCVec3Unaligned vertexPos = { 0.0f, 0.0f, 0.0f };

        float min = 0.0f;
        float max = 0.0f;

        switch (m_shaderHandler->GetVertexFormat())
        {
        case VertexFormat_PositionColor:
        {
            std::vector<VertexPosColor> vertices;
            VertexPosColor vertex;

            //Vertex Buffer
            for (int vertIndex = 0; vertIndex < submesh->GetNoOfVertices(); vertIndex++)
            {
                vertexPos = XCVec3Unaligned(submesh->m_vertices[vertIndex].x, submesh->m_vertices[vertIndex].y, submesh->m_vertices[vertIndex].z);
                vertex = VertexPosColor(vertexPos,
                    XCVec4Unaligned(0.0f, 0.0f, 0.0f, 0.0f));
                vertices.push_back(vertex);
            }

            //Index Buffer
            std::vector<unsigned int> indices;

            for (int index = 0; index < submesh->GetNoOfFaces(); index++)
            {
                indices.push_back(submesh->m_faces[index].a);
                indices.push_back(submesh->m_faces[index].b);
                indices.push_back(submesh->m_faces[index].c);
            }

            break;
        }

        case VertexFormat_PositionNormalTexture:
        {
            VertexBuffer<VertexPosNormTex>* vertexBuffer = (VertexBuffer<VertexPosNormTex>*) submesh->GetVertexBuffer();
            std::vector<VertexPosNormTex>& vertices = vertexBuffer->m_vertexData;
            VertexPosNormTex vertex;
            XCVec2Unaligned mapCoord = { 0.0f, 0.0f };

            //Vertex Buffer
            for (unsigned int vertIndex = 0; vertIndex < submesh->m_vertices.size(); vertIndex++)
            {
                mapCoord = vertIndex < submesh->m_mapCoord.size() ?
                    XCVec2Unaligned(submesh->m_mapCoord[vertIndex].u, submesh->m_mapCoord[vertIndex].v)
                    : XCVec2Unaligned(0.0f, 1.0f);

                vertexPos = XCVec3Unaligned(submesh->m_vertices[vertIndex].x,
                    submesh->m_vertices[vertIndex].y,
                    submesh->m_vertices[vertIndex].z);

                transformedVertex = XMLoadFloat3(&vertexPos);
                transformedVertex = XMVector3TransformNormal(transformedVertex, transformMatrix);
                XMStoreFloat3(&vertexPos, transformedVertex);

                vertex = VertexPosNormTex(vertexPos,
                    XCVec3Unaligned(0.33f, 0.33f, 0.33f), //Todo - Get normal from .3ds model or calculate the normal here if data is not available
                    mapCoord);

                vertices.push_back(vertex);

                vMin = XMVectorMin(vMin, XMLoadFloat3(ToXCVec3(vertexPos)));
                vMax = XMVectorMax(vMax, XMLoadFloat3(ToXCVec3(vertexPos)));
            }

            std::vector<unsigned int>& indices = submesh->GetIndexBuffer().m_indexData;

            for (unsigned int index = 0; index < submesh->m_faces.size(); index++)
            {
                indices.push_back(submesh->m_faces[index].a);
                indices.push_back(submesh->m_faces[index].b);
                indices.push_back(submesh->m_faces[index].c);

                //TODO : Calculate normal here
            }

            //Traverse through the vertices
            for (unsigned int vertexIndex = 0; vertexIndex < indices.size() - 3; vertexIndex += 3)
            {
                XCVecIntrinsic4 v1 = XMLoadFloat3(ToXCVec3(vertices[indices[vertexIndex]].Pos));
                XCVecIntrinsic4 v2 = XMLoadFloat3(ToXCVec3(vertices[indices[vertexIndex + 1]].Pos));
                XCVecIntrinsic4 v3 = XMLoadFloat3(ToXCVec3(vertices[indices[vertexIndex + 2]].Pos));

                XCVecIntrinsic4 vertexNormal = GetNormalFromPoints(v1, v2, v3);
                XMStoreFloat3(&vertices[indices[vertexIndex]].Norm, vertexNormal);

                vertexNormal = GetNormalFromPoints(v2, v1, v3);
                XMStoreFloat3(&vertices[indices[vertexIndex + 1]].Norm, vertexNormal);

                vertexNormal = GetNormalFromPoints(v3, v1, v2);
                XMStoreFloat3(&vertices[indices[vertexIndex + 2]].Norm, vertexNormal);
            }

            vertexBuffer->BuildVertexBuffer();
            submesh->GetIndexBuffer().BuildIndexBuffer();
            break;
        }

        case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
        {
            VertexBuffer<VertexPosNormTexBIndexBWeight>* vertexBuffer = (VertexBuffer<VertexPosNormTexBIndexBWeight>*)submesh->GetVertexBuffer();
            std::vector<VertexPosNormTexBIndexBWeight>& vertices = vertexBuffer->m_vertexData;
            VertexPosNormTexBIndexBWeight vertex;
            std::vector<std::vector<aiVertexWeight> > weightsPerVertex;
            
            float boneIndex[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            float boneWeight[] = { 0.0f, 0.0f, 0.0f, 0.0f };

            XCVec2Unaligned mapCoord = { 0.0f, 0.0f };
            XCVec4Unaligned blendIndices, vertexWeight;

            FILE* fp = nullptr;
            
            if (IsSkinnedMesh())
            {
                fp = fopen("xcmesh_log.txt", "w+");
                if (m_scene)
                    mesh = m_scene->mMeshes;

                fprintf(fp, "--------------- Submesh %d ----------------\n", objIndex);
                weightsPerVertex.clear();
                weightsPerVertex.resize(mesh[objIndex]->mNumVertices);

                //Tranverse every bone and calculate the weight of every vertex in that bone
                for (unsigned int boneIdx = 0; boneIdx < mesh[objIndex]->mNumBones; ++boneIdx)
                {
                    const aiBone* bone = mesh[objIndex]->mBones[boneIdx];

                    for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
                    {
                        weightsPerVertex[bone->mWeights[weightIndex].mVertexId].push_back(aiVertexWeight(boneIdx, bone->mWeights[weightIndex].mWeight));
                        fprintf(fp, "Pushing in weightsPerVertex @ mVertexId %d : boneIndex %d  bone->mWeights[weightIndex].mWeight : %d\n", bone->mWeights[weightIndex].mVertexId, boneIdx, (int)bone->mWeights[weightIndex].mWeight);
                    }
                }
            }

            //Vertex Buffer
            for (unsigned int vertIndex = 0; vertIndex < submesh->m_vertices.size(); vertIndex++)
            {
                mapCoord = vertIndex < submesh->m_mapCoord.size() ?
                    XCVec2Unaligned(submesh->m_mapCoord[vertIndex].u, submesh->m_mapCoord[vertIndex].v)
                    : XCVec2Unaligned(0.0f, 1.0f);

                vertexPos = XCVec3Unaligned(submesh->m_vertices[vertIndex].x,
                    submesh->m_vertices[vertIndex].y,
                    submesh->m_vertices[vertIndex].z);

                //Add bone info
                if (mesh[objIndex]->HasBones())
                {
                    XCASSERT(weightsPerVertex[objIndex].size() <= 4);

                    for (unsigned int weightIndex = 0; weightIndex < weightsPerVertex[vertIndex].size(); ++weightIndex)
                    {
                        boneIndex[weightIndex] = (float)weightsPerVertex[vertIndex][weightIndex].mVertexId;
                        boneWeight[weightIndex] = (float)(weightsPerVertex[vertIndex][weightIndex].mWeight /** 255.0f*/);
                    }

                    blendIndices = XCVec4Unaligned(boneIndex[0], boneIndex[1], boneIndex[2], boneIndex[3]);
                    vertexWeight = XCVec4Unaligned(boneWeight[0], boneWeight[1], boneWeight[2], boneWeight[3]);

                    fprintf(fp, "Vertex : %d  BlendIndx %d %d %d %d  Weight %d %d %d %d\n",
                        vertIndex, (int)blendIndices.x, (int)blendIndices.y, (int)blendIndices.z, (int)blendIndices.w,
                        (int)vertexWeight.x, (int)vertexWeight.y, (int)vertexWeight.z, (int)vertexWeight.w);
                }

                transformedVertex = XMLoadFloat3(&vertexPos);
                transformedVertex = XMVector3TransformNormal(transformedVertex, transformMatrix);
                XMStoreFloat3(&vertexPos, transformedVertex);

                vertex = VertexPosNormTexBIndexBWeight(vertexPos,
                    XCVec3Unaligned(0.33f, 0.33f, 0.33f),
                    mapCoord,
                    blendIndices,
                    vertexWeight);

                vertices.push_back(vertex);

                vMin = XMVectorMin(vMin, XMLoadFloat3(ToXCVec3(vertexPos)));
                vMax = XMVectorMax(vMax, XMLoadFloat3(ToXCVec3(vertexPos)));

                blendIndices = XCVec4Unaligned(0.0f, 0.0f, 0.0f, 0.0f);
                vertexWeight = XCVec4Unaligned(0.0f, 0.0f, 0.0f, 0.0f);
            }

            std::vector<unsigned int>& indices = submesh->GetIndexBuffer().m_indexData;

            for (unsigned int index = 0; index < submesh->m_faces.size(); index++)
            {
                indices.push_back(submesh->m_faces[index].a);
                indices.push_back(submesh->m_faces[index].b);
                indices.push_back(submesh->m_faces[index].c);

                //TODO : Calculate normal here
            }

            //Traverse through the vertices
            for (unsigned int vertexIndex = 0; vertexIndex < indices.size() - 3; vertexIndex += 3)
            {
                XCVecIntrinsic4 v1 = XMLoadFloat3(ToXCVec3(vertices[indices[vertexIndex]].Pos));
                XCVecIntrinsic4 v2 = XMLoadFloat3(ToXCVec3(vertices[indices[vertexIndex + 1]].Pos));
                XCVecIntrinsic4 v3 = XMLoadFloat3(ToXCVec3(vertices[indices[vertexIndex + 2]].Pos));

                XCVecIntrinsic4 vertexNormal = GetNormalFromPoints(v1, v2, v3);
                XMStoreFloat3(&vertices[indices[vertexIndex]].Norm, vertexNormal);

                vertexNormal = GetNormalFromPoints(v2, v1, v3);
                XMStoreFloat3(&vertices[indices[vertexIndex + 1]].Norm, vertexNormal);

                vertexNormal = GetNormalFromPoints(v3, v1, v2);
                XMStoreFloat3(&vertices[indices[vertexIndex + 2]].Norm, vertexNormal);
            }

            vertexBuffer->BuildVertexBuffer();
            submesh->GetIndexBuffer().BuildIndexBuffer();

            if (fp)
                fclose(fp);

            break;
        }

        case VertexFormat_PositionColorInstanceIndex:
        {
            VertexBuffer<VertexPosColorInstanceIndex>* vertexBuffer = (VertexBuffer<VertexPosColorInstanceIndex>*)submesh->GetVertexBuffer();
            std::vector<VertexPosColorInstanceIndex>& vertices = vertexBuffer->m_vertexData;
            //std::vector<VertexPosColorInstanceIndex::InstanceBuffer>& instanceData = m_subMeshes[objIndex]->getInstanceBuffer<VertexPosColorInstanceIndex::InstanceBuffer>().m_vertexData;
            //VertexPosColorInstanceIndex::InstanceBuffer sampleData = { XCVec4Unaligned(0.0f, 0.0f, 0.0f, 0.0f) };

            XCVec4Unaligned vertexPos = { 0.0f, 0.0f, 0.0f, 0.0f };
            VertexPosColorInstanceIndex vertex;

            //Vertex Buffer
            for (unsigned int vertIndex = 0; vertIndex < submesh->m_vertices.size(); vertIndex++)
            {
                vertexPos = XCVec4Unaligned(submesh->m_vertices[vertIndex].x,
                    submesh->m_vertices[vertIndex].y,
                    submesh->m_vertices[vertIndex].z,
                    1.0f);

                //Compute width
                if (vertexPos.x > max)
                {
                    max = vertexPos.x;
                }
                else if (vertexPos.x < min)
                {
                    min = vertexPos.x;
                }

                transformedVertex = XMLoadFloat4(&vertexPos);
                transformedVertex = XMVector3Transform(transformedVertex, transformMatrix);
                XMStoreFloat4(&vertexPos, transformedVertex);

                vertex = VertexPosColorInstanceIndex(vertexPos, XCVec4Unaligned(0.0f, 0.0f, 0.0f, 1.0f));

                vertices.push_back(vertex);

                vMin = XMVectorMin(vMin, XMLoadFloat4(&ToXCVec4(vertexPos)));
                vMax = XMVectorMax(vMax, XMLoadFloat4(&ToXCVec4(vertexPos)));

                //sampleData.SamplePad.x = rand() % 4;
                //instanceData.push_back(sampleData);
            }

            std::vector<unsigned int>& indices = submesh->GetIndexBuffer().m_indexData;

            for (unsigned int index = 0; index < submesh->m_faces.size(); index++)
            {
                indices.push_back(submesh->m_faces[index].a);
                indices.push_back(submesh->m_faces[index].b);
                indices.push_back(submesh->m_faces[index].c);
            }

            vertexBuffer->BuildVertexBuffer();
            //m_subMeshes[objIndex]->getInstanceBuffer<VertexPosColorInstanceIndex::InstanceBuffer>().BuildVertexBuffer();
            submesh->GetIndexBuffer().BuildIndexBuffer();
            break;
        }

        default:
            break;
        }
        submesh->m_width = abs(max - min);
    }

    //Got the min max, compute the centre and extends
    m_computedBoundBox->m_boxCenter = 0.5f * (vMin + vMax);
    m_computedBoundBox->m_boxExtends = 0.5f * (vMax - vMin);
    m_computedBoundBox->CreateBoundBox();

    Logger("[XCMesh] Created buffer for Resource : %s", m_resourcePath.c_str());
}

void XCMesh::CreateConstantBuffer()
{
    //Generate PerObject Constant Buffers;
    switch (m_shaderType)
    {
    case ShaderType_LightTexture:
        {
            InstanceBuffer buffer = {};
            buffer.m_cbInstancedBufferGPU = m_shaderHandler->CreateConstantBuffer("cbInstancedBuffer");
            m_instanceBuffers.push_back(buffer);
            break;
        }

    case ShaderType_SkinnedCharacter:
        {
            InstanceBuffer buffer = {};
            buffer.m_cbInstancedBufferGPU = m_shaderHandler->CreateConstantBuffer("cbInstancedBuffer");
            m_instanceBuffers.push_back(buffer);

            if (m_isSkinnedMesh)
            {
                BoneBuffer boneBuffer = {};
                for (unsigned int meshIndex = 0; meshIndex < m_subMeshes.size(); ++meshIndex)
                {
                    boneBuffer.m_cbBoneBufferGPU = m_shaderHandler->CreateConstantBuffer("cbBoneBuffer");
                    m_boneBuffers.push_back(boneBuffer);
                }
            }
            break;
        }

    default:
        Logger("[XCMesh] Shader doesn't support");
    }
}

void XCMesh::FilterSubMeshes()
{
    unsigned int index = 0;

    while (index < m_subMeshes.size())
    {
        auto it = m_subMeshes.begin() + index;
        if ((*it)->m_vertices.size() == 0)
        {
            delete(*it);
            m_subMeshes.erase(it);
        }
        else
        {
            ++index;
        }
    }
}

void XCMesh::Update(float dt)
{
    //Need not update. Update the clone of this of every actor
    //m_computedBoundBox.Update(dt);
    if (m_resourceState)
    {
        if (m_sceneAnimator)
        {
            aiAnimation* anim = m_sceneAnimator->CurrentAnim();

            static double g_dCurrent = 0.0f;
            g_dCurrent += clock() / double(CLOCKS_PER_SEC) - m_lastPlayedAnimTime;

            double time = g_dCurrent;

            if (anim && anim->mDuration > 0.0) {
                double tps = anim->mTicksPerSecond ? anim->mTicksPerSecond : 25.f;
                time = fmod(time, anim->mDuration / tps);
            }

            m_sceneAnimator->Calculate(time);
            m_lastPlayedAnimTime = (float)g_dCurrent;
            /*
            if (m_lastPlayedAnimTime > anim->mDuration)
            {
                Logger("Resetting @ %f", currentTime);
                m_lastPlayedAnimTime = 0.0f;
            }
            else
            {
                Logger("Updating @ %f", currentTime);
                m_lastPlayedAnimTime += dt;
            }
            */
        }
    }
}


void XCMesh::RegisterDrawable()
{
    //Register with the rendering pool that this is drawable
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddResourceDrawable((IResource*) this);
}

void XCMesh::UnregisterDrawable()
{
    //UnRegister with the rendering pool that this is no more drawable
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    RenderingPool& renderPool = graphicsSystem.GetRenderingPool();
    renderPool.RemoveResourceDrawable(this);
}

void XCMesh::Draw(RenderContext& context)
{
    if (m_instanceCount > 0)
    {
        context.SetRasterizerState(RasterType_FillSolid);
        context.ApplyShader(m_shaderType);

        XC_LightManager* lightMgr = (XC_LightManager*)&SystemLocator::GetInstance()->RequestSystem("LightsManager");
        m_shaderHandler->SetConstantBuffer("cbLightsPerFrame", context.GetDeviceContext(), lightMgr->GetLightConstantBuffer());
        m_shaderHandler->SetResource("gDiffuseMap", context.GetDeviceContext(), m_texture);

        DrawSubMeshes(context);

        m_instanceCount = 0;
    }
}

void XCMesh::DrawSubMesh(RenderContext& renderContext, unsigned int meshIndex)
{
    //For instance based
    switch (m_shaderType)
    {
    case ShaderType_LightTexture:
    {
        memcpy(m_instanceBuffers[0].m_cbInstancedBufferGPU->m_cbDataBegin, &m_instanceBuffers[0].m_cbInstancedBuffer, sizeof(cbInstancedBuffer));
        m_shaderHandler->SetConstantBuffer("cbInstancedBuffer", renderContext.GetDeviceContext(), *m_instanceBuffers[0].m_cbInstancedBufferGPU);
        break;
    }

    case ShaderType_SkinnedCharacter:
    {
        memcpy(m_instanceBuffers[0].m_cbInstancedBufferGPU->m_cbDataBegin, &m_instanceBuffers[0].m_cbInstancedBuffer, sizeof(cbInstancedBuffer));
        m_shaderHandler->SetConstantBuffer("cbInstancedBuffer", renderContext.GetDeviceContext(), *m_instanceBuffers[0].m_cbInstancedBufferGPU);

        std::vector<aiMatrix4x4> boneMatrix = m_sceneAnimator->GetBoneMatrices(m_scene->mRootNode, meshIndex);

        for (unsigned int index = 0; index < 60; ++index)
        {
            m_boneBuffers[meshIndex].m_cbBoneBuffer.gBoneMatrix[index] = aiMatrixToMatrix4Unaligned(boneMatrix[index]);
        }

        static float matrices[4 * 4 * 60];
        float* tempmat = matrices;

        for (unsigned int matIndex = 0; matIndex < boneMatrix.size(); ++matIndex)
        {
            aiMatrix4x4& mat = boneMatrix[matIndex];
            mat.Transpose();
            *tempmat++ = mat.a1; *tempmat++ = mat.a2; *tempmat++ = mat.a3; *tempmat++ = mat.a4;
            *tempmat++ = mat.b1; *tempmat++ = mat.b2; *tempmat++ = mat.b3; *tempmat++ = mat.b4;
            *tempmat++ = mat.c1; *tempmat++ = mat.c2; *tempmat++ = mat.c3; *tempmat++ = mat.c4;
            *tempmat++ = mat.d1; *tempmat++ = mat.d2; *tempmat++ = mat.d3; *tempmat++ = mat.d4;
        }

        memcpy(m_boneBuffers[meshIndex].m_cbBoneBufferGPU->m_cbDataBegin, &matrices, sizeof(cbBoneBuffer));
        m_shaderHandler->SetConstantBuffer("cbBoneBuffer", renderContext.GetDeviceContext(), *m_boneBuffers[meshIndex].m_cbBoneBufferGPU);
        break;
    }

    default:
        XCASSERT(false);
    }

    m_shaderHandler->SetVertexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->GetVertexBuffer());
    m_shaderHandler->SetIndexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->GetIndexBuffer());

    renderContext.GetShaderManagerSystem().DrawIndexedInstanced(renderContext.GetDeviceContext(),
        m_subMeshes[meshIndex]->GetNoOfFaces() * 3,
        m_subMeshes[meshIndex]->GetIndexBuffer().GetIndexBufferInGPUMem(),
        m_instanceCount);
}

void XCMesh::DrawSubMeshes(RenderContext& renderContext)
{
    if (m_resourceState)
    {
        for (unsigned int index = 0; index < m_subMeshes.size(); index++)
        {
            DrawSubMesh(renderContext, index);
        }
    }
    //Need not draw. Draw the clone of this in every actor
    //m_computedBoundBox.Draw();
}

void XCMesh::DrawInstanced(PerObjectBuffer& objectBuffer)
{
    if (m_resourceState)
    {
        m_instanceBuffers[0].m_cbInstancedBuffer.gPerObject[m_instanceCount++] = objectBuffer;
    }
}

void XCMesh::Destroy()
{
    UnregisterDrawable();
}

