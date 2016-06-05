/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XC_Mesh/XCMesh.h"
#include "fbxsdk.h"

class XCMeshFBX : public XCMesh
{
public:
    DECLARE_OBJECT_CREATION(XCMeshFBX)

    XCMeshFBX()
    {}

    virtual ~XCMeshFBX()
    {
        if (m_fbxMgr)
        {
            m_fbxMgr->Destroy();
        }
    }

    virtual void            Destroy();

protected:
    virtual bool            LoadMesh();
    virtual void            CreateConstantBuffer();
    virtual void            Draw(RenderContext& context);

    bool                    LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
    void                    DisplayMetaData(FbxScene* pScene);
    void                    DisplayHierarchy(FbxScene* pScene);
    void                    DisplayHierarchy(FbxNode* pNode, int pDepth);
    void                    DisplayContent(FbxScene* pScene);
    void                    DisplayContent(FbxNode* pNode);
    void                    ParseMesh(FbxNode* pNode);
    void                    DisplayAndGeneratePolygons(FbxMesh* pMesh, MeshData* submesh);

private:
    FbxManager*             m_fbxMgr;
    FbxScene*               m_fbxScene;
};