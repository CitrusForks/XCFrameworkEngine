/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "VectorFontMesh.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/XCGraphics.h"

VectorFontMesh::VectorFontMesh()
{
}

VectorFontMesh::~VectorFontMesh()
{
}

void VectorFontMesh::Init(i32 resourceId, std::string userFriendlyName)
{
    XCMeshFBX::Init(resourceId, userFriendlyName);

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    //Register with the rendering pool that this is drawable
    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddResourceDrawable((IResource*) this);
}

void VectorFontMesh::DrawText(std::string text, XCVec3Unaligned& position, RenderContext& context)
{
    //Decrypt the text and fill up the FontData Buffer to draw
    //Calculate start world for every character
    ICamera& cam = context.GetGlobalShaderData().m_camera;

    u32 CharacterSpacing = 10;
    u32 charPosition = 0;
    
    f32 scale = 0.5f;
    f32 computePos = 0.0f;

    FontData fontData = {};
    
    for (u32 charIndex = 0; charIndex < text.size(); ++charIndex)
    {
        std::string textchar = { text.at(charIndex) };
        auto findSubMesh = std::find_if(m_subMeshes.begin(), m_subMeshes.end(), [&textchar](MeshData* submesh)
        {
            return strcmp(submesh->GetSubMeshName().c_str(), textchar.c_str()) == 0;
        });

        if (findSubMesh != m_subMeshes.end())
        {
            u32 subMeshId = findSubMesh - m_subMeshes.begin();

            //Find if the submesh is already in our buffer. If yes Instance ++ it.
            auto subMeshExists = std::find_if(m_subMeshesIdBuffer.begin(), m_subMeshesIdBuffer.end(), [&subMeshId](FontData& fontData)
            {
                return fontData.submeshId == subMeshId;
            });

            XCMatrix4 world =  MatrixScale(scale, scale, scale) * MatrixTranslate(position.x + computePos /*(charPosition * CharacterSpacing)*/, position.y, position.z);
            computePos += (scale * (*findSubMesh)->GetMeshAbsWidth());

            if (subMeshExists != m_subMeshesIdBuffer.end())
            {
                FontData& existingFontData = (*subMeshExists);
                m_vectorFontInstanceBuffers[subMeshId].m_instanceBuffer.gWVP[existingFontData.instanceCount++]
                    = MatrixTranspose(world * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned();
            }
            else
            {
                FontData newFontData = {};
                newFontData.submeshId = subMeshId;
                m_vectorFontInstanceBuffers[subMeshId].m_instanceBuffer.gWVP[newFontData.instanceCount++]
                    = MatrixTranspose(world * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned();
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

    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
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
        XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");

        for (u32 subMeshIndex = 0; subMeshIndex < m_subMeshes.size(); ++subMeshIndex)
        {
            buffer.m_instanceBufferGPU = m_shaderHandler->CreateConstantBuffer("cbPerObjectInstanced");
            buffer.m_instanceBufferGPU->UploadZeroMemoryDataOnGPU(*graphicsSystem.GetDeviceContext(), sizeof(cbVectorFontInstanced));
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
    context.ApplyShader(m_shaderType);

    XCShaderHandle* shader = (XCShaderHandle*)context.GetShader(m_shaderType);

    if (m_subMeshesIdBuffer.size())
    {
        for (auto subMesh : m_subMeshesIdBuffer)
        {
            m_vectorFontInstanceBuffers[subMesh.submeshId].m_instanceBufferGPU->UploadDataOnGPU(
                context.GetDeviceContext(),
                &m_vectorFontInstanceBuffers[subMesh.submeshId].m_instanceBuffer.gWVP[0],
                sizeof(XCMatrix4Unaligned) * subMesh.instanceCount);

            shader->SetConstantBuffer("cbPerObjectInstanced", context.GetDeviceContext(), *m_vectorFontInstanceBuffers[subMesh.submeshId].m_instanceBufferGPU);
            DrawSubMesh(context, subMesh.submeshId, subMesh.instanceCount);
        }
    }

    //Clear the existing buffer
    m_subMeshesIdBuffer.clear();
}

void VectorFontMesh::DrawSubMesh(RenderContext & renderContext, u32 meshIndex, u32 instanceCount)
{
    m_shaderHandler->SetVertexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->GetVertexBuffer());
    m_shaderHandler->SetIndexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->GetIndexBuffer());

    renderContext.DrawIndexedInstanced(m_subMeshes[meshIndex]->GetNoOfFaces() * 3,
        m_subMeshes[meshIndex]->GetIndexBuffer().GetIndexBufferInGPUMem(),
        instanceCount);
}
