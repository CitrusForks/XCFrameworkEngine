/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "fbxsdk.h"

class XCMeshFBX : public XCMesh
{
public:
    DECLARE_OBJECT_CREATION(XCMeshFBX)

    XCMeshFBX()
    {
        m_isSkinnedMesh = false;
    }

    virtual ~XCMeshFBX()
    {
        if (m_fbxMgr)
            m_fbxMgr->Destroy();
    }

    virtual void            Init(int resourceId, std::string userFriendlyName, bool loaded = false);
    virtual void            Draw(RenderContext& context);
    virtual void            Draw(RenderContext& context, ShaderType shaderType);
    virtual void            Destroy();

    void                    Load(const void* buffer);
    void                    DrawSubMeshes(RenderContext& renderContext, ShaderType shaderType);

    bool                    LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

protected:
    virtual void            DrawSubMesh(RenderContext& renderContext, ShaderType shaderType, unsigned int meshIndex, unsigned int instanceCount = 1)
    {
        XCMesh::DrawSubMesh(renderContext, shaderType, meshIndex, instanceCount);
    }

    void                    DisplayMetaData(FbxScene* pScene);
    void                    DisplayHierarchy(FbxScene* pScene);
    void                    DisplayHierarchy(FbxNode* pNode, int pDepth);
    void                    DisplayContent(FbxScene* pScene);
    void                    DisplayContent(FbxNode* pNode);
    void                    DisplayMesh(FbxNode* pNode);
    void                    DisplayAndGeneratePolygons(FbxMesh* pMesh, MeshData* submesh);

private:
    FbxManager*             m_fbxMgr;
    FbxScene*               m_fbxScene;
};