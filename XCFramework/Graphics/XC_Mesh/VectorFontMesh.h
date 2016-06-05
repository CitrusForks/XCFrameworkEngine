/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once
#include "Graphics/XC_Mesh/XCMeshFBX.h"

//This class will draw only the characters specified by the actor.
class VectorFontMesh : public XCMeshFBX
{
public:
    DECLARE_OBJECT_CREATION(VectorFontMesh)

    struct VectorFontInstanceBuffer
    {
        D3DConstantBuffer*      m_instanceBufferGPU;
        cbVectorFontInstanced   m_instanceBuffer;
    };

    struct FontData
    {
        unsigned int          submeshId;
        unsigned int          instanceCount;

        FontData()
        {
            instanceCount = 0;
        }
    };
    
    VectorFontMesh();
    virtual ~VectorFontMesh();

    void            Init(int resourceId, std::string userFriendlyName) override;
    void            DrawText(std::string text, XCVec3Unaligned position, RenderContext& context);
    void            Destroy() override;

protected:
    void            CreateConstantBuffer() override;

    void            Draw(RenderContext& context) override;
    void            DrawSubMesh(RenderContext& renderContext, unsigned int meshIndex, unsigned int instanceCount = 1);

private:
    std::vector<FontData>                  m_subMeshesIdBuffer;
    std::vector<VectorFontInstanceBuffer>  m_vectorFontInstanceBuffers;

    //This buffer is actual text to be drawn
    std::string                m_buffer;

    BasicMaterial              m_material;
};