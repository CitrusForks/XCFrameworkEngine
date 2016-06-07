/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "VectorFontMesh.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Graphics/XC_Graphics.h"

VectorFontMesh::VectorFontMesh()
{
}

VectorFontMesh::~VectorFontMesh()
{
}

void VectorFontMesh::Init(int resourceId, std::string userFriendlyName)
{
    XCMeshFBX::Init(resourceId, userFriendlyName);

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    //Register with the rendering pool that this is drawable
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddResourceDrawable((IResource*) this);
}

void VectorFontMesh::DrawText(std::string text, XCVec3Unaligned position, RenderContext& context)
{
    //Decrypt the text and fill up the FontData Buffer to draw
    //Calculate start world for every character
    ICamera& cam = context.GetShaderManagerSystem().GetGlobalShaderData().m_camera;

    unsigned int CharacterSpacing = 10;
    unsigned int charPosition = 0;
    
    float scale = 0.5f;
    float computePos = 0.0f;

    FontData fontData = {};
    
    for (unsigned int charIndex = 0; charIndex < text.size(); ++charIndex)
    {
        std::string textchar = { text.at(charIndex) };
        auto findSubMesh = std::find_if(m_subMeshes.begin(), m_subMeshes.end(), [&textchar](MeshData* submesh)
        {
            return strcmp(submesh->GetSubMeshName().c_str(), textchar.c_str()) == 0;
        });

        if (findSubMesh != m_subMeshes.end())
        {
            unsigned int subMeshId = findSubMesh - m_subMeshes.begin();

            //Find if the submesh is already in our buffer. If yes Instance ++ it.
            auto subMeshExists = std::find_if(m_subMeshesIdBuffer.begin(), m_subMeshesIdBuffer.end(), [&subMeshId](FontData& fontData)
            {
                return fontData.submeshId == subMeshId;
            });

            XCMatrix4Unaligned world =  XMMatrixScaling(scale, scale, scale) * XMMatrixTranslation(position.x + computePos /*(charPosition * CharacterSpacing)*/, position.y, position.z);
            computePos += (scale * (*findSubMesh)->m_width);

            if (subMeshExists != m_subMeshesIdBuffer.end())
            {
                FontData& existingFontData = (*subMeshExists);
                m_vectorFontInstanceBuffers[subMeshId].m_instanceBuffer.gWVP[existingFontData.instanceCount++]
                    = XCMatrix4Unaligned(ToXCMatrix4Unaligned(XMMatrixTranspose(world * cam.GetViewMatrix() * cam.GetProjectionMatrix())));
            }
            else
            {
                FontData newFontData = {};
                newFontData.submeshId = subMeshId;
                m_vectorFontInstanceBuffers[subMeshId].m_instanceBuffer.gWVP[newFontData.instanceCount++]
                    = XCMatrix4Unaligned(ToXCMatrix4Unaligned(XMMatrixTranspose(world * cam.GetViewMatrix() * cam.GetProjectionMatrix())));
                m_subMeshesIdBuffer.push_back(newFontData);
            }
        }
        else
        {
            //Just skip this character or place some null char or just add the character spacing
            Logger("[VECTOR FONT] SubMesh not mapped with the character %s", textchar.c_str());
        }

        charPosition++;
    }
}

void VectorFontMesh::Destroy()
{
    XCMeshFBX::Destroy();

    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().RemoveResourceDrawable((IResource*) this);

    for (auto instanceBuffer : m_vectorFontInstanceBuffers)
    {
        m_shaderHandler->DestroyConstantBuffer(instanceBuffer.m_instanceBufferGPU);
    }
}

void VectorFontMesh::CreateConstantBuffer()
{
    switch (m_shaderType)
    {
    case ShaderType_VectorFont:
    {
        VectorFontInstanceBuffer buffer = {};
        SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");

        for (unsigned int subMeshIndex = 0; subMeshIndex < m_subMeshes.size(); ++subMeshIndex)
        {
            buffer.m_instanceBufferGPU = m_shaderHandler->CreateConstantBuffer("cbPerObjectInstanced");
            memset(buffer.m_instanceBufferGPU->m_cbDataBegin, 0, sizeof(cbVectorFontInstanced));
            m_vectorFontInstanceBuffers.push_back(buffer);
        }
        break;
    }

    default:
        break;
    }
}

void VectorFontMesh::Draw(RenderContext& context)
{
    context.SetRasterizerState(RasterType_FillSolid);
    context.ApplyShader(m_shaderType);

    XCShaderHandle* shader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(m_shaderType);

    for (auto subMesh : m_subMeshesIdBuffer)
    {
        memcpy(m_vectorFontInstanceBuffers[subMesh.submeshId].m_instanceBufferGPU->m_cbDataBegin, &m_vectorFontInstanceBuffers[subMesh.submeshId].m_instanceBuffer.gWVP[0], sizeof(XCMatrix4Unaligned) * subMesh.instanceCount);
        shader->SetConstantBuffer("cbPerObjectInstanced", context.GetDeviceContext(), m_vectorFontInstanceBuffers[subMesh.submeshId].m_instanceBufferGPU->m_gpuHandle);
        DrawSubMesh(context, subMesh.submeshId, subMesh.instanceCount);
    }

    //Clear the existing buffer
    m_subMeshesIdBuffer.clear();
}

void VectorFontMesh::DrawSubMesh(RenderContext & renderContext, unsigned int meshIndex, unsigned int instanceCount)
{
    m_shaderHandler->SetVertexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->GetVertexBuffer());
    m_shaderHandler->SetIndexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->GetIndexBuffer());

    renderContext.GetShaderManagerSystem().DrawIndexedInstanced(renderContext.GetDeviceContext(),
        m_subMeshes[meshIndex]->GetNoOfFaces() * 3,
        m_subMeshes[meshIndex]->GetIndexBuffer().GetIndexBufferInGPUMem(),
        instanceCount);
}