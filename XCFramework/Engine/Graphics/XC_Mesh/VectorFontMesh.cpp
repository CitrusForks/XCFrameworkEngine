/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "VectorFontMesh.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

VectorFontMesh::VectorFontMesh()
{
}

VectorFontMesh::~VectorFontMesh()
{
}

void VectorFontMesh::Init(int resourceId, std::string userFriendlyName, bool loaded)
{
    XCMeshFBX::Init(resourceId, userFriendlyName, loaded);

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    //Register with the rendering pool that this is drawable
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddResourceDrawable((IResource*) this);
}

void VectorFontMesh::Load(const void * buffer)
{
    XCMeshFBX::Load(buffer);
    
    for (unsigned int subIndex = 0; subIndex < m_subMeshes.size(); ++subIndex)
    {
        m_subMeshes[subIndex]->createConstantBuffers();
    }
}

void VectorFontMesh::DrawText(std::string text, XCVec3Unaligned position, RenderContext& context)
{
    //Decrypt the text and fill up the FontData Buffer to draw
    //Calculate start world for every character
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    
    unsigned int CharacterSpacing = 10;
    unsigned int charPosition = 0;

    FontData fontData = {};
    
    for (unsigned int charIndex = 0; charIndex < text.size(); ++charIndex)
    {
        std::string textchar = { text.at(charIndex) };
        auto findSubMesh = std::find_if(m_subMeshes.begin(), m_subMeshes.end(), [&textchar](MeshData* submesh)
        {
            return strcmp(submesh->getSubMeshName().c_str(), textchar.c_str()) == 0;
        });

        if (findSubMesh != m_subMeshes.end())
        {
            unsigned int subMeshId = findSubMesh - m_subMeshes.begin();

            //Find if the submesh is already in our buffer. If yes Instance ++ it.
            auto subMeshExists = std::find_if(m_subMeshesIdBuffer.begin(), m_subMeshesIdBuffer.end(), [&subMeshId](FontData& fontData)
            {
                return fontData.submeshId == subMeshId;
            });

            XCMatrix4Unaligned world =  XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixTranslation(position.x + (charPosition * CharacterSpacing), position.y, position.z);

            if (subMeshExists != m_subMeshesIdBuffer.end())
            {
                FontData& existingFontData = (*subMeshExists);
                existingFontData.orientation.gWVP[existingFontData.instanceCount++] = XCMatrix4Unaligned(ToXCMatrix4Unaligned(XMMatrixTranspose(world * cam->GetViewMatrix() * cam->GetProjMatrix())));
            }
            else
            {
                FontData newFontData = {};
                newFontData.orientation.gWVP[newFontData.instanceCount++] = XCMatrix4Unaligned(ToXCMatrix4Unaligned(XMMatrixTranspose(world * cam->GetViewMatrix() * cam->GetProjMatrix())));
                newFontData.submeshId = subMeshId;
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
}

void VectorFontMesh::Draw(RenderContext& context)
{
    context.SetRasterizerState(RasterType_FillSolid);
    context.ApplyShader(m_shaderType);

    XCShaderHandle* shader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(m_shaderType);

    for (auto subMesh : m_subMeshesIdBuffer)
    {
        memcpy(m_subMeshes[subMesh.submeshId]->m_constantBuffer->m_cbDataBegin, &subMesh.orientation.gWVP[0], sizeof(XCMatrix4Unaligned) * subMesh.instanceCount);
        shader->SetConstantBuffer("cbPerObjectInstanced", context.GetDeviceContext(), m_subMeshes[subMesh.submeshId]->m_constantBuffer->m_gpuHandle);
        DrawSubMesh(context, subMesh.submeshId, subMesh.instanceCount);
    }

    //Clear the existing buffer
    m_subMeshesIdBuffer.clear();
}

void VectorFontMesh::DrawSubMesh(RenderContext & renderContext, unsigned int meshIndex, unsigned int instanceCount)
{
    m_shaderHandler->SetVertexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->getVertexBuffer());
    m_shaderHandler->SetIndexBuffer(renderContext.GetDeviceContext(), m_subMeshes[meshIndex]->getIndexBuffer());

    renderContext.GetShaderManagerSystem().DrawIndexedInstanced(renderContext.GetDeviceContext(),
        m_subMeshes[meshIndex]->getNoOfFaces() * 3,
        m_subMeshes[meshIndex]->getIndexBuffer().GetIndexBufferInGPUMem(),
        instanceCount);
}
