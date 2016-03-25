/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once
#include "Engine\Graphics\XC_Mesh\XCMeshFBX.h"

//This class will draw only the characters specified by the actor.
class VectorFontMesh : public XCMeshFBX
{
public:
    DECLARE_OBJECT_CREATION(VectorFontMesh)

    struct FontData
    {
        cbPerObjectInstanced  orientation;
        unsigned int          submeshId;
        unsigned int          instanceCount;

        FontData()
        {
            instanceCount = 0;
        }
    };
    
    VectorFontMesh();
    ~VectorFontMesh();

    void            Init(int resourceId, std::string userFriendlyName, bool loaded = false);
    void            Load(const void* buffer);
    void            DrawText(std::string text, XCVec3Unaligned position, RenderContext& context);
    void            Destroy();

protected:
    void            Draw(RenderContext& context);
    void            DrawSubMesh(RenderContext& renderContext, unsigned int meshIndex, unsigned int instanceCount = 1);

private:
    //This buffer is actual text to be drawn
    std::string                m_buffer;
    std::vector<FontData>      m_subMeshesIdBuffer;
    BasicMaterial              m_material;
};