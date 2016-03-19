/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "XCMesh.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"
#include "Engine/Graphics/XC_Mesh/XC3DSMeshLoader.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"

XCMesh::XCMesh()
{
    m_resourceType = RESOURCETYPE_MESH;
    m_areBuffersCreated = false;
    m_vertexFormat = VertexFormat_PositionNormalTexture;
    m_isSkinnedMesh = false;
    m_scene = nullptr;
    m_sceneAnimator = nullptr;

    m_lastPlayedAnimTime = 1.0f;
}

void XCMesh::Init(int resourceId, std::string userFriendlyName, bool loaded)
{
    IResource::Init(resourceId, userFriendlyName);

    m_computedBoundBox = std::make_unique<OrientedBoundingBox>();
    m_computedBoundBox->Init();

    m_resourceManager = &SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
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

    m_initialScaling = initialScaling;

    m_resourcePath = fileName;

    //Isolate this XC3DSMeshLoader... Make it static and not like this
    XC3DSMeshLoader loader;
    loader.loadMeshFromFile(fileName, this);

    //Filter out objects with zero vertices
    filterSubMeshes();
}

void XCMesh::Load(const void* buffer)
{
    const FBXCMesh* fbXCMesh = (const FBXCMesh*) buffer;

    m_userFriendlyName = fbXCMesh->MeshName()->c_str();
    m_resourcePath = getPlatformPath(fbXCMesh->MeshPath()->c_str());
    m_texture = (Texture2D*) m_resourceManager->GetResource(fbXCMesh->TextureRes()->c_str());
    m_initialScaling = fbXCMesh->InitialScaling();
    m_initialRotation = XCVec3Unaligned(fbXCMesh->InitialRotation()->x(), fbXCMesh->InitialRotation()->y(), fbXCMesh->InitialRotation()->z());

    //Isolate this XC3DSMeshLoader... Make it static and not like this
    XC3DSMeshLoader loader;
    loader.loadMeshFromFile(m_resourcePath, this);

    //Filter out objects with zero vertices
    filterSubMeshes();
}

SubMesh* XCMesh::createAndGetSubMesh()
{
    SubMesh* subMesh = new SubMesh(m_vertexFormat);
    m_subMeshes.push_back(subMesh);
    return m_subMeshes.back();
}

void XCMesh::CreateBuffers(EVertexFormat formatType)
{
    if (!m_areBuffersCreated)
    {
        m_vertexFormat = formatType;

        XCVecIntrinsic4 vMin = toXMVECTOR(Infinity, Infinity, Infinity, 1);
        XCVecIntrinsic4 vMax = toXMVECTOR(-Infinity, -Infinity, -Infinity, 1);
        
        aiMesh** mesh = nullptr;
        float boneIndex[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float boneWeight[] = { 0.0f, 0.0f, 0.0f, 0.0f };

        FILE* fp = nullptr;

        if (IsSkinnedMesh())
        {
            fp = fopen("xcmesh.txt", "w+");
            // collect weights on all vertices.
            if(m_scene)
                mesh = m_scene->mMeshes;
        }


        for (unsigned int objIndex = 0; objIndex < m_subMeshes.size(); objIndex++)
        {
            std::vector<std::vector<aiVertexWeight> > weightsPerVertex;

            if (IsSkinnedMesh())
            {
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

            switch (formatType)
            {
                case VertexFormat_PositionColor:
                    {
                        std::vector<VertexPosColor> vertices;
                        VertexPosColor vertex;

                        //Vertex Buffer
                        for (int vertIndex = 0; vertIndex < m_subMeshes[objIndex]->getNoOfVertices(); vertIndex++)
                        {
                            vertex = VertexPosColor(XCVec3Unaligned(m_subMeshes[objIndex]->m_vertices[vertIndex].x, m_subMeshes[objIndex]->m_vertices[vertIndex].y, m_subMeshes[objIndex]->m_vertices[vertIndex].z),
                                                    XCVec4Unaligned(0.0f, 0.0f, 0.0f, 0.0f));
                            vertices.push_back(vertex);
                        }

                        //Index Buffer
                        std::vector<unsigned int> indices;

                        for (int index = 0; index < m_subMeshes[objIndex]->getNoOfFaces(); index++)
                        {
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].a);
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].b);
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].c);
                        }

                        //buildBuffer(sizeof(VertexPosColor), &vertices[0], m_noOfVertices, m_pVB.Get());
                        //buildBuffer(sizeof(UINT), &indices[0], indices.size(), m_pIB.Get());
                        break;
                    }
                case VertexFormat_PositionNormal:
                    {
                        break;
                    }
                case VertexFormat_PositionNormalTexture:
                    {
                            std::vector<VertexPosNormTex>& vertices = m_subMeshes[objIndex]->getVertexBuffer<VertexPosNormTex>().m_vertexData;
                            VertexPosNormTex vertex;
                            XCVec2Unaligned mapCoord = { 0.0f, 0.0f };
                            XCVec3Unaligned vertexPos = { 0.0f, 0.0f, 0.0f };

                            //Calculate the initial rotation matrix
                            XCMatrix4 initialRotationMat = XMMatrixRotationX(m_initialRotation.x) * XMMatrixRotationY(m_initialRotation.y) * XMMatrixRotationZ(m_initialRotation.z);
                            XCVecIntrinsic4 transformedVertex;
                            
                            //Vertex Buffer
                            for (unsigned int vertIndex = 0; vertIndex < m_subMeshes[objIndex]->m_vertices.size(); vertIndex++)
                            {
                                mapCoord = vertIndex < m_subMeshes[objIndex]->m_mapCoord.size() ?
                                    XCVec2Unaligned(m_subMeshes[objIndex]->m_mapCoord[vertIndex].u, m_subMeshes[objIndex]->m_mapCoord[vertIndex].v)
                                    : XCVec2Unaligned(0.0f, 1.0f);

                                vertexPos = XCVec3Unaligned(m_subMeshes[objIndex]->m_vertices[vertIndex].x * m_initialScaling,
                                                     m_subMeshes[objIndex]->m_vertices[vertIndex].y * m_initialScaling,
                                                     m_subMeshes[objIndex]->m_vertices[vertIndex].z * m_initialScaling);
                                
                                transformedVertex = XMLoadFloat3(&vertexPos);
                                transformedVertex = XMVector3TransformNormal(transformedVertex, initialRotationMat);
                                XMStoreFloat3(&vertexPos, transformedVertex);

                                vertex = VertexPosNormTex(vertexPos,
                                                          XCVec3Unaligned(0.33f, 0.33f, 0.33f), //Todo - Get normal from .3ds model or calculate the normal here if data is not available
                                                          mapCoord);

                                vertices.push_back(vertex);

                                vMin = XMVectorMin(vMin, XMLoadFloat3(ToXCVec3(vertexPos)));
                                vMax = XMVectorMax(vMax, XMLoadFloat3(ToXCVec3(vertexPos)));
                            }

                            std::vector<unsigned int>& indices = m_subMeshes[objIndex]->getIndexBuffer().m_indexData;

                            for (unsigned int index = 0; index < m_subMeshes[objIndex]->m_faces.size(); index++)
                            {
                                indices.push_back(m_subMeshes[objIndex]->m_faces[index].a);
                                indices.push_back(m_subMeshes[objIndex]->m_faces[index].b);
                                indices.push_back(m_subMeshes[objIndex]->m_faces[index].c);

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

                            m_subMeshes[objIndex]->getVertexBuffer<VertexPosNormTex>().BuildVertexBuffer();
                            m_subMeshes[objIndex]->getIndexBuffer().BuildIndexBuffer();
                            break;
                    }
                case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
                    {
                        std::vector<VertexPosNormTexBIndexBWeight>& vertices = m_subMeshes[objIndex]->getVertexBuffer<VertexPosNormTexBIndexBWeight>().m_vertexData;
                        VertexPosNormTexBIndexBWeight vertex;
                        XCVec2Unaligned mapCoord = { 0.0f, 0.0f };
                        XCVec3Unaligned vertexPos = { 0.0f, 0.0f, 0.0f };
                        XCVec4Unaligned blendIndices, vertexWeight;

                        //Calculate the initial rotation matrix
                        XCMatrix4 initialRotationMat = XMMatrixIdentity();// XMMatrixRotationX(m_initialRotation.x) * XMMatrixRotationY(m_initialRotation.y) * XMMatrixRotationZ(m_initialRotation.z);
                        XCVecIntrinsic4 transformedVertex;
                        m_initialScaling = 1.0f;
                        XCMatrix4 initialScalingMat = XMMatrixScaling(m_initialScaling, m_initialScaling, m_initialScaling);

                        //Vertex Buffer
                        for (unsigned int vertIndex = 0; vertIndex < m_subMeshes[objIndex]->m_vertices.size(); vertIndex++)
                        {
                            mapCoord = vertIndex < m_subMeshes[objIndex]->m_mapCoord.size() ?
                                XCVec2Unaligned(m_subMeshes[objIndex]->m_mapCoord[vertIndex].u, m_subMeshes[objIndex]->m_mapCoord[vertIndex].v)
                                : XCVec2Unaligned(0.0f, 1.0f);

                            vertexPos = XCVec3Unaligned(m_subMeshes[objIndex]->m_vertices[vertIndex].x * m_initialScaling,
                                m_subMeshes[objIndex]->m_vertices[vertIndex].y * m_initialScaling,
                                m_subMeshes[objIndex]->m_vertices[vertIndex].z * m_initialScaling);

                            //Add bone info
                            if (mesh[objIndex]->HasBones())
                            {
                                XCASSERT(weightsPerVertex[objIndex].size() <= 4);

                                for (unsigned int weightIndex = 0; weightIndex < weightsPerVertex[vertIndex].size(); ++weightIndex)
                                {
                                    boneIndex[weightIndex] = (float) weightsPerVertex[vertIndex][weightIndex].mVertexId;
                                    boneWeight[weightIndex] = (float) (weightsPerVertex[vertIndex][weightIndex].mWeight * 255.0f);
                                }

                                blendIndices = XCVec4Unaligned(boneIndex[0], boneIndex[1], boneIndex[2], boneIndex[3]);
                                vertexWeight = XCVec4Unaligned(boneWeight[0], boneWeight[1], boneWeight[2], boneWeight[3]);

                                fprintf(fp, "Vertex : %d  BlendIndx %d %d %d %d  Weight %d %d %d %d\n",
                                    vertIndex, (int)blendIndices.x, (int)blendIndices.y, (int)blendIndices.z, (int)blendIndices.w,
                                    (int)vertexWeight.x, (int)vertexWeight.y, (int)vertexWeight.z, (int)vertexWeight.w);
                            }

                            transformedVertex = XMLoadFloat3(&vertexPos);
                            transformedVertex = XMVector3TransformNormal(transformedVertex, initialRotationMat);
                            XMStoreFloat3(&vertexPos, transformedVertex);

                            vertex = VertexPosNormTexBIndexBWeight(vertexPos,
                                XCVec3Unaligned(0.33f, 0.33f, 0.33f), //Todo - Get normal from .3ds model or calculate the normal here if data is not available
                                mapCoord,
                                blendIndices,
                                vertexWeight);

                            vertices.push_back(vertex);

                            vMin = XMVectorMin(vMin, XMLoadFloat3(ToXCVec3(vertexPos)));
                            vMax = XMVectorMax(vMax, XMLoadFloat3(ToXCVec3(vertexPos)));
                        }

                        std::vector<unsigned int>& indices = m_subMeshes[objIndex]->getIndexBuffer().m_indexData;

                        for (unsigned int index = 0; index < m_subMeshes[objIndex]->m_faces.size(); index++)
                        {
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].a);
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].b);
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].c);

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

                        m_subMeshes[objIndex]->getVertexBuffer<VertexPosNormTexBIndexBWeight>().BuildVertexBuffer();
                        m_subMeshes[objIndex]->getIndexBuffer().BuildIndexBuffer();

                        break;
                    }

                case VertexFormat_PositionColorInstanceIndex:
                    {
                        std::vector<VertexPosColorInstanceIndex>& vertices = m_subMeshes[objIndex]->getVertexBuffer<VertexPosColorInstanceIndex>().m_vertexData;
                        //std::vector<VertexPosColorInstanceIndex::InstanceBuffer>& instanceData = m_subMeshes[objIndex]->getInstanceBuffer<VertexPosColorInstanceIndex::InstanceBuffer>().m_vertexData;
                        //VertexPosColorInstanceIndex::InstanceBuffer sampleData = { XCVec4Unaligned(0.0f, 0.0f, 0.0f, 0.0f) };

                        VertexPosColorInstanceIndex vertex;
                        XCVec4Unaligned vertexPos = { 0.0f, 0.0f, 0.0f, 0.0f };

                        //Calculate the initial rotation matrix
                        XCMatrix4 initialRotationMat = XMMatrixRotationX(m_initialRotation.x) * XMMatrixRotationY(m_initialRotation.y) * XMMatrixRotationZ(m_initialRotation.z);
                        XCVecIntrinsic4 transformedVertex;

                        //Vertex Buffer
                        for (unsigned int vertIndex = 0; vertIndex < m_subMeshes[objIndex]->m_vertices.size(); vertIndex++)
                        {
                            vertexPos = XCVec4Unaligned(m_subMeshes[objIndex]->m_vertices[vertIndex].x * m_initialScaling,
                                m_subMeshes[objIndex]->m_vertices[vertIndex].y * m_initialScaling,
                                m_subMeshes[objIndex]->m_vertices[vertIndex].z * m_initialScaling, 1.0f);

                            transformedVertex = XMLoadFloat4(&vertexPos);
                            transformedVertex = XMVector3TransformNormal(transformedVertex, initialRotationMat);
                            XMStoreFloat4(&vertexPos, transformedVertex);

                            vertex = VertexPosColorInstanceIndex(vertexPos, XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f));

                            vertices.push_back(vertex);

                            vMin = XMVectorMin(vMin, XMLoadFloat4(&ToXCVec4(vertexPos)));
                            vMax = XMVectorMax(vMax, XMLoadFloat4(&ToXCVec4(vertexPos)));

                            //sampleData.SamplePad.x = rand() % 4;
                            //instanceData.push_back(sampleData);
                        }

                        std::vector<unsigned int>& indices = m_subMeshes[objIndex]->getIndexBuffer().m_indexData;

                        for (unsigned int index = 0; index < m_subMeshes[objIndex]->m_faces.size(); index++)
                        {
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].a);
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].b);
                            indices.push_back(m_subMeshes[objIndex]->m_faces[index].c);
                        }

                        m_subMeshes[objIndex]->getVertexBuffer<VertexPosColorInstanceIndex>().BuildVertexBuffer();
                        //m_subMeshes[objIndex]->getInstanceBuffer<VertexPosColorInstanceIndex::InstanceBuffer>().BuildVertexBuffer();
                        m_subMeshes[objIndex]->getIndexBuffer().BuildIndexBuffer();
                        break;
                    }

                default:
                    break;
            }
        }

        if(fp)
            fclose(fp);

        //Got the min max, compute the centre and extends
        m_computedBoundBox->m_boxCenter = 0.5f * (vMin + vMax);
        m_computedBoundBox->m_boxExtends = 0.5f * (vMax - vMin);
        m_computedBoundBox->CreateBoundBox();
        m_computedBoundBox->Load();

        m_areBuffersCreated = true;
        Logger("[XCMesh] Created buffer for Resource : %s", m_resourcePath.c_str());
    }
}

#if defined(XCGRAPHICS_DX11)
void XCMesh::buildBuffer(unsigned int sizeOfType, void* ptrToBuffer, unsigned int length, ID3D11Buffer* buffer, D3D11_BUFFER_DESC desc)
{
    D3D11_SUBRESOURCE_DATA vInitData;
    vInitData.pSysMem = ptrToBuffer;

    ValidateResult(m_resourceManager->GetXCGraphicsDx11System().GetDevice()->CreateBuffer(&desc, &vInitData, &buffer));
}
#endif

int XCMesh::getSizeFromVertexFormat(EVertexFormat format)
{
    switch(format)
    {
    case VertexFormat_PositionColor:
        return sizeof(VertexPosColor);
        break;

    case VertexFormat_PositionNormal:
        return sizeof(VertexPosNorm);
        break;

    case VertexFormat_PositionNormalTexture:
        return sizeof(VertexPosNormTex);
        break;

    case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
        return sizeof(VertexPosNormTexBIndexBWeight);
        break;

    case VertexFormat_PositionColorInstanceIndex:
        return sizeof(VertexPosColorInstanceIndex);
        break;

    default:
        return 0;
    }
}

void XCMesh::filterSubMeshes()
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

void XCMesh::Draw(RenderContext& context, SHADERTYPE shaderType)
{
    XCShaderHandle* shader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(shaderType);
    shader->setResource("gDiffuseMap", context.GetDeviceContext(), m_texture->getTextureResource());

    DrawSubMeshes(context, shaderType);
}

void XCMesh::DrawSubMesh(RenderContext& renderContext, SHADERTYPE shaderType, unsigned int meshIndex, unsigned int instanceCount)
{
    unsigned int offset = 0;

    switch (shaderType)
    {
    case SHADERTYPE_LIGHTTEXTURE:
        m_subMeshes[meshIndex]->getVertexBuffer<VertexPosNormTex>().SetVertexBuffer(renderContext.GetDeviceContext());
        break;

    case SHADERTYPE_SKINNEDCHARACTER:
    {
        std::vector<XCMatrix4Unaligned> boneMatrix = m_sceneAnimator->GetBoneMatrices(m_scene->mRootNode, meshIndex);

        cbBoneBuffer boneBuffer = {};
        for (unsigned int index = 0; index < 60; ++index)
        {
            boneBuffer.gBoneMatrix[index] = boneMatrix[index];
        }

        memcpy(m_subMeshes[meshIndex]->m_boneBuffer->m_cbDataBegin, &boneBuffer, sizeof(boneBuffer));

        XCShaderHandle* skinnedCharacterShader = (XCShaderHandle*)renderContext.GetShaderManagerSystem().GetShader(SHADERTYPE_SKINNEDCHARACTER);
        skinnedCharacterShader->setConstantBuffer("cbBoneBuffer", renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->m_boneBuffer->m_gpuHandle);

        m_subMeshes[meshIndex]->getVertexBuffer<VertexPosNormTexBIndexBWeight>().SetVertexBuffer(renderContext.GetDeviceContext());
        break;
    }

    case SHADERTYPE_VECTORFONT:
    {
        m_subMeshes[meshIndex]->getVertexBuffer<VertexPosColorInstanceIndex>().SetVertexBuffer(renderContext.GetDeviceContext());
        //renderContext.GetDeviceContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //vertexBufferView[0] = m_subMeshes[meshIndex]->getVertexBuffer<VertexPosColorInstanceIndex>().GetVertexBufferView();
        //vertexBufferView[1] = m_subMeshes[meshIndex]->getInstanceBuffer<VertexPosColorInstanceIndex::InstanceBuffer>().GetVertexBufferView();
        //renderContext.GetDeviceContext().IASetVertexBuffers(0, 1, &vertexBufferView[0]);
        break;
    }

    default:
        XCASSERT(false);
    }

    m_subMeshes[meshIndex]->getIndexBuffer().SetIndexBuffer(renderContext.GetDeviceContext());

    //Draw as per the faces listed for every material may be?
    renderContext.GetShaderManagerSystem().DrawIndexedInstanced(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->getNoOfFaces() * 3, m_subMeshes[meshIndex]->getIndexBuffer().GetIndexBufferInGPUMem(), instanceCount);
}

void XCMesh::DrawSubMeshes(RenderContext& renderContext, SHADERTYPE shaderType, unsigned int instanceCount)
{
    for(unsigned int index = 0; index < m_subMeshes.size(); index++)
    {
        DrawSubMesh(renderContext, shaderType, index, instanceCount);
    }

    //Need not draw. Draw the clone of this in every actor
    //m_computedBoundBox.Draw();
}

void XCMesh::Destroy()
{
    for (auto& subMesh : m_subMeshes)
    {
        subMesh->destroy();
        delete(subMesh);
    }

    m_subMeshes.clear();
}