/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Mesh/XCMesh.h"

class XCMeshCustom : public XCMesh
{
public:
    DECLARE_OBJECT_CREATION(XCMeshCustom)

    XCMeshCustom()
    {
        m_isSkinnedMesh = true;
    }

    ~XCMeshCustom()
    {
    }

    void                    Load(const void* buffer);
    void                    DrawSubMeshes(RenderContext& renderContext, ShaderType shaderType);

private:
};