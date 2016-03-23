/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "XCMeshFBX.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"


void DisplayString(const char* pHeader, const char* pValue  = "", const char* pSuffix = "")
{
    FbxString lString;

    lString = pHeader;
    lString += pValue;
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void DisplayBool(const char* pHeader, bool pValue, const char* pSuffix = "")
{
    FbxString lString;

    lString = pHeader;
    lString += pValue ? "true" : "false";
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void DisplayInt(const char* pHeader, int pValue, const char* pSuffix = "")
{
    FbxString lString;

    lString = pHeader;
    lString += pValue;
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void DisplayDouble(const char* pHeader, double pValue, const char* pSuffix = "")
{
    FbxString lString;
    FbxString lFloatValue = (float)pValue;

    lFloatValue = pValue <= -HUGE_VAL ? "-INFINITY" : lFloatValue.Buffer();
    lFloatValue = pValue >= HUGE_VAL ? "INFINITY" : lFloatValue.Buffer();

    lString = pHeader;
    lString += lFloatValue;
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void Display2DVector(const char* pHeader, FbxVector2 pValue, const char* pSuffix = "")
{
    FbxString lString;
    FbxString lFloatValue1 = (float)pValue[0];
    FbxString lFloatValue2 = (float)pValue[1];

    lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
    lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
    lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
    lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();

    lString = pHeader;
    lString += lFloatValue1;
    lString += ", ";
    lString += lFloatValue2;
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix = "")
{
    FbxString lString;
    FbxString lFloatValue1 = (float)pValue[0];
    FbxString lFloatValue2 = (float)pValue[1];
    FbxString lFloatValue3 = (float)pValue[2];

    lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
    lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
    lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
    lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
    lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
    lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

    lString = pHeader;
    lString += lFloatValue1;
    lString += ", ";
    lString += lFloatValue2;
    lString += ", ";
    lString += lFloatValue3;
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}

void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix = "")
{
    FbxString lString;
    FbxString lFloatValue1 = (float)pValue[0];
    FbxString lFloatValue2 = (float)pValue[1];
    FbxString lFloatValue3 = (float)pValue[2];
    FbxString lFloatValue4 = (float)pValue[3];

    lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
    lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
    lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
    lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
    lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
    lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();
    lFloatValue4 = pValue[3] <= -HUGE_VAL ? "-INFINITY" : lFloatValue4.Buffer();
    lFloatValue4 = pValue[3] >= HUGE_VAL ? "INFINITY" : lFloatValue4.Buffer();

    lString = pHeader;
    lString += lFloatValue1;
    lString += ", ";
    lString += lFloatValue2;
    lString += ", ";
    lString += lFloatValue3;
    lString += ", ";
    lString += lFloatValue4;
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void DisplayColor(const char* pHeader, FbxPropertyT<FbxDouble3> pValue, const char* pSuffix = "")

{
    FbxString lString;

    lString = pHeader;
    //lString += (float) pValue.mRed;
    //lString += (double)pValue.GetArrayItem(0);
    lString += " (red), ";
    //lString += (float) pValue.mGreen;
    //lString += (double)pValue.GetArrayItem(1);
    lString += " (green), ";
    //lString += (float) pValue.mBlue;
    //lString += (double)pValue.GetArrayItem(2);
    lString += " (blue)";
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void DisplayColor(const char* pHeader, FbxColor pValue, const char* pSuffix = "")
{
    FbxString lString;

    lString = pHeader;
    lString += (float)pValue.mRed;

    lString += " (red), ";
    lString += (float)pValue.mGreen;

    lString += " (green), ";
    lString += (float)pValue.mBlue;

    lString += " (blue)";
    lString += pSuffix;
    lString += "\n";
    Logger(lString);
}


void XCMeshFBX::Load(const void * buffer)
{
    Logger("Loading FBX mesh");

    const FBXCMesh* fbXCMesh = (const FBXCMesh*)buffer;
    
    m_userFriendlyName = fbXCMesh->MeshName()->c_str();
    m_resourcePath = getPlatformPath(fbXCMesh->MeshPath()->c_str());
    m_texture = (Texture2D*)m_resourceManager->GetResource(fbXCMesh->TextureRes()->c_str());
    m_globalScaling = XCVec3Unaligned(fbXCMesh->InitialScaling()->x(), fbXCMesh->InitialScaling()->y(), fbXCMesh->InitialScaling()->z());
    m_globalRotation = XCVec3Unaligned(fbXCMesh->InitialRotation()->x(), fbXCMesh->InitialRotation()->x(), fbXCMesh->InitialRotation()->x());
    m_shaderType = fbXCMesh->ShaderUsage();

    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    m_shaderHandler = (XCShaderHandle*)graphicsSystem.GetShaderManagerSystem().GetShader(m_shaderType);

    const char* meshFilename = fbXCMesh->MeshPath()->c_str();

    //Initialize FBX Mgr
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    m_fbxMgr = FbxManager::Create();
    if (!m_fbxMgr)
    {
        Logger("Error: Unable to create FBX Manager!\n");
        return;
    }
    else
    {
        Logger("Autodesk FBX SDK version %s\n", m_fbxMgr->GetVersion());
    }

    //Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(m_fbxMgr, IOSROOT);
    m_fbxMgr->SetIOSettings(ios);

    //Load plugins from the executable directory (optional)
    FbxString lPath = FbxGetApplicationDirectory();
    m_fbxMgr->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    m_fbxScene = FbxScene::Create(m_fbxMgr, "My Scene");
    if (!m_fbxScene)
    {
        Logger("Error: Unable to create FBX scene!\n");
        return;
    }

    //Load the scene
    bool result = LoadScene(m_fbxMgr, m_fbxScene, meshFilename);

    XCASSERT(result);

    DisplayMetaData(m_fbxScene);
    DisplayHierarchy(m_fbxScene);
    DisplayContent(m_fbxScene);

    /*
    for (unsigned int meshIndex = 0; meshIndex < m_scene->mNumMeshes; ++meshIndex)
    {
                {
            submesh->setObjectName(fbXCMesh->MeshName()->c_str());
            submesh->setNoOfVertices(mesh[meshIndex]->mNumVertices);
            submesh->setNoOfFaces(mesh[meshIndex]->mNumFaces);
            submesh->setNoOfBones(mesh[meshIndex]->mNumBones);

            SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
            submesh->m_boneBuffer = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbBoneBuffer)));

            //fill the vertices & uv coords
            aiVector3D* vertex = mesh[meshIndex]->mVertices;
            aiVector3D** uvCoord = mesh[meshIndex]->mTextureCoords;
            
            float boneIndex[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            float boneWeight[] = { 0.0f, 0.0f, 0.0f, 0.0f };

            for (unsigned int vertexIndex = 0; vertexIndex < mesh[meshIndex]->mNumVertices; ++vertexIndex)
            {
                //Add vertex
                submesh->addVertex(vertex[vertexIndex].x, vertex[vertexIndex].y, vertex[vertexIndex].z);

                //Add UV
                if (mesh[meshIndex]->HasTextureCoords(0))
                {
                    submesh->addMapCoord(abs(uvCoord[0][vertexIndex].x), abs(uvCoord[0][vertexIndex].y));
                }
            }


            //fill the faces
            aiFace* face = mesh[meshIndex]->mFaces;
            unsigned int* indices = nullptr;
            for (unsigned int faceIndex = 0; faceIndex < mesh[meshIndex]->mNumFaces; ++faceIndex)
            {
                indices = face[faceIndex].mIndices;
                submesh->addFace(indices[0], indices[1], indices[2], 0);
            }
        }
    }
    */



    //filterSubMeshes();
    CreateBuffers();
    CreateConstantBuffer();
}

bool XCMeshFBX::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor, lSDKMinor, lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager, "");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if (!lImportStatus)
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        Logger("Call to FbxImporter::Initialize() failed.\n");
        Logger("Error returned: %s\n\n", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            Logger("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            Logger("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    Logger("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        Logger("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        Logger("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        Logger("    Number of Animation Stacks: %d\n", lAnimStackCount);
        Logger("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        Logger("\n");

        for (i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            Logger("    Animation Stack %d\n", i);
            Logger("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            Logger("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            Logger("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            Logger("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            Logger("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        FbxIOSettings* ioSettings = m_fbxMgr->GetIOSettings();
        ioSettings->SetBoolProp(IMP_FBX_MATERIAL, true);
        ioSettings->SetBoolProp(IMP_FBX_TEXTURE, true);
        ioSettings->SetBoolProp(IMP_FBX_LINK, true);
        ioSettings->SetBoolProp(IMP_FBX_SHAPE, true);
        ioSettings->SetBoolProp(IMP_FBX_GOBO, true);
        ioSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
        ioSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
    {
        Logger("Please enter password: ");

        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
            scanf("%s", lPassword);
        FBXSDK_CRT_SECURE_NO_WARNING_END

            FbxString lString(lPassword);

        FbxIOSettings* ioSettings = m_fbxMgr->GetIOSettings();
        ioSettings->SetStringProp(IMP_FBX_PASSWORD, lString);
        ioSettings->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
        {
            Logger("\nPassword is wrong, import aborted.\n");
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

void XCMeshFBX::DisplayMetaData(FbxScene* pScene)
{
    FbxDocumentInfo* sceneInfo = pScene->GetSceneInfo();
    if (sceneInfo)
    {
        Logger("\n\n--------------------\nMeta-Data\n--------------------\n\n");
        Logger("    Title: %s\n", sceneInfo->mTitle.Buffer());
        Logger("    Subject: %s\n", sceneInfo->mSubject.Buffer());
        Logger("    Author: %s\n", sceneInfo->mAuthor.Buffer());
        Logger("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
        Logger("    Revision: %s\n", sceneInfo->mRevision.Buffer());
        Logger("    Comment: %s\n", sceneInfo->mComment.Buffer());

        FbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
        if (thumbnail)
        {
            Logger("    Thumbnail:\n");

            switch (thumbnail->GetDataFormat())
            {
            case FbxThumbnail::eRGB_24:
                Logger("        Format: RGB\n");
                break;
            case FbxThumbnail::eRGBA_32:
                Logger("        Format: RGBA\n");
                break;
            }

            switch (thumbnail->GetSize())
            {
            default:
                break;
            case FbxThumbnail::eNotSet:
                Logger("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e64x64:
                Logger("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e128x128:
                Logger("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
            }
        }
    }
}

void XCMeshFBX::DisplayHierarchy(FbxScene* pScene)
{
    int i;
    FbxNode* lRootNode = pScene->GetRootNode();

    for (i = 0; i < lRootNode->GetChildCount(); i++)
    {
        DisplayHierarchy(lRootNode->GetChild(i), 0);
    }
}

void XCMeshFBX::DisplayHierarchy(FbxNode* pNode, int pDepth)
{
    FbxString lString;
    int i;

    for (i = 0; i < pDepth; i++)
    {
        lString += "     ";
    }

    lString += pNode->GetName();
    lString += "\n";

    Logger(lString.Buffer());

    for (i = 0; i < pNode->GetChildCount(); i++)
    {
        DisplayHierarchy(pNode->GetChild(i), pDepth + 1);
    }
}

void XCMeshFBX::DisplayContent(FbxScene* pScene)
{
    int i;
    FbxNode* lNode = pScene->GetRootNode();

    if (lNode)
    {
        for (i = 0; i < lNode->GetChildCount(); i++)
        {
            DisplayContent(lNode->GetChild(i));
        }
    }
}

void XCMeshFBX::DisplayContent(FbxNode* pNode)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    if (pNode->GetNodeAttribute() == NULL)
    {
        Logger("NULL Node Attribute\n\n");
    }
    else
    {
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {
        default:
            break;

        case FbxNodeAttribute::eMesh:
            DisplayMesh(pNode);
            break;

        }
    }

    for (i = 0; i < pNode->GetChildCount(); i++)
    {
        DisplayContent(pNode->GetChild(i));
    }
}

void XCMeshFBX::DisplayMesh(FbxNode* pNode)
{
    FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();
    FbxVector4* lControlPoints = lMesh->GetControlPoints();

    DisplayString("Mesh Name: ", (char *)pNode->GetName());

    MeshData* submesh = createAndGetSubMesh();
    {
        submesh->setObjectName(pNode->GetName());
        submesh->setNoOfVertices(lMesh->GetPolygonCount() * 3);
        submesh->setNoOfFaces(lMesh->GetPolygonCount());

        SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");

        //Get the geometry transform
        FbxVector4 translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        XCVecIntrinsic4 trans = toXMVECTOR(translation[0], translation[1], translation[2], translation[3]);

        Logger("[FONT] Located @ %f %f %f", trans.vector4_f32[0], trans.vector4_f32[1], trans.vector4_f32[2]);
        
        XCVecIntrinsic4 originTranslate = toXMVECTOR(0.0f, 0.0f, 0.0f, 1.0f) - trans;
        originTranslate.vector4_f32[1] = 0.0f; originTranslate.vector4_f32[2] = 0.0f; originTranslate.vector4_f32[3] = 1.0f;

        if(trans.vector4_f32[0] < 0)
            originTranslate = -XMVector3Length(originTranslate);
        else
            originTranslate = XMVector3Length(originTranslate);

        Logger("[FONT] Translating with a distance of : %f ", originTranslate.vector4_f32[0]);

        submesh->setGeometryTranslation(XCVec3Unaligned((XMVectorGetX(originTranslate)), 0.0f, 0.0f));

        //fill the vertices & uv coords
        for (int polygonIndex = 0; polygonIndex < lMesh->GetPolygonCount(); ++polygonIndex)
        {
            int faceValue[] = { -1, -1, -1, -1 };

            for (int faceVertexIndex = 0; faceVertexIndex < lMesh->GetPolygonSize(polygonIndex); ++faceVertexIndex)
            {
                int vIndex = lMesh->GetPolygonVertex(polygonIndex, faceVertexIndex);

                FbxVector4 v1Data = lControlPoints[vIndex];
                MeshData::Vertex vertex = { v1Data[0], v1Data[1], v1Data[2] };

                //Logger("Polygon : %f %f %f ", vertex.x, vertex.y, vertex.z);

                //Find if the vertex is present in our vertices.
                auto foundVertex = std::find_if(submesh->m_vertices.begin(), submesh->m_vertices.end(), [&vertex](MeshData::Vertex v) -> bool
                {
                    return fabs(v.x) == fabs(vertex.x) && fabs(v.y) == fabs(vertex.y) && fabs(v.z) == fabs(vertex.z);
                });

                if (foundVertex == submesh->m_vertices.end())
                {
                    //New vertex found. Add it to our vertices
                    submesh->addVertex(vertex);
                    faceValue[faceVertexIndex] = submesh->m_vertices.size() - 1;
                }
                else
                {
                    //Get the index no for the face
                    faceValue[faceVertexIndex] = foundVertex - submesh->m_vertices.begin();
                }

                submesh->addMapCoord(1.0f, 1.0f);
            }

            XCASSERT(faceValue[0] != -1);

            //Add the face to our submesh
            MeshData::Face face = { faceValue[0], faceValue[1], faceValue[2], faceValue[3] };
            submesh->addFace(face);
            //Logger("Face : %d %d %d", face.a, face.b, face.c);
        }
    }

    //DisplayAndGeneratePolygons(lMesh, submesh);
}

void XCMeshFBX::DisplayAndGeneratePolygons(FbxMesh* pMesh, MeshData* submesh)
{
    int i, j, lPolygonCount = pMesh->GetPolygonCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints();
    char header[100];

    DisplayString("    Polygons");

    int vertexId = 0;
    for (i = 0; i < lPolygonCount; i++)
    {
        DisplayInt("        Polygon ", i);
        int l;

        for (l = 0; l < pMesh->GetElementPolygonGroupCount(); l++)
        {
            FbxGeometryElementPolygonGroup* lePolgrp = pMesh->GetElementPolygonGroup(l);
            switch (lePolgrp->GetMappingMode())
            {
            case FbxGeometryElement::eByPolygon:
                if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
                {
                    FBXSDK_sprintf(header, 100, "        Assigned to group: ");
                    int polyGroupId = lePolgrp->GetIndexArray().GetAt(i);
                    DisplayInt(header, polyGroupId);
                    break;
                }
            default:
                // any other mapping modes don't make sense
                DisplayString("        \"unsupported group assignment\"");
                break;
            }
        }

        int lPolygonSize = pMesh->GetPolygonSize(i);

        for (j = 0; j < lPolygonSize; j++)
        {
            int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

            Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);

            for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
            {
                FbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(l);
                FBXSDK_sprintf(header, 100, "            Color vertex: ");

                switch (leVtxc->GetMappingMode())
                {
                default:
                    break;
                case FbxGeometryElement::eByControlPoint:
                    switch (leVtxc->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
                        DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                    break;

                case FbxGeometryElement::eByPolygonVertex:
                {
                    switch (leVtxc->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leVtxc->GetIndexArray().GetAt(vertexId);
                        DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }
                break;

                case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
                case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
                case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
                    break;
                }
            }
            for (l = 0; l < pMesh->GetElementUVCount(); ++l)
            {
                FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
                FBXSDK_sprintf(header, 100, "            Texture UV: ");

                switch (leUV->GetMappingMode())
                {
                default:
                    break;
                case FbxGeometryElement::eByControlPoint:
                    switch (leUV->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
                        Display2DVector(header, leUV->GetDirectArray().GetAt(id));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                    break;

                case FbxGeometryElement::eByPolygonVertex:
                {
                    int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
                    switch (leUV->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }
                break;

                case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
                case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
                case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
                    break;
                }
            }
            for (l = 0; l < pMesh->GetElementNormalCount(); ++l)
            {
                FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
                FBXSDK_sprintf(header, 100, "            Normal: ");

                if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                {
                    switch (leNormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leNormal->GetIndexArray().GetAt(vertexId);
                        Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }

            }
            for (l = 0; l < pMesh->GetElementTangentCount(); ++l)
            {
                FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);
                FBXSDK_sprintf(header, 100, "            Tangent: ");

                if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                {
                    switch (leTangent->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leTangent->GetIndexArray().GetAt(vertexId);
                        Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }

            }
            for (l = 0; l < pMesh->GetElementBinormalCount(); ++l)
            {

                FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(l);

                FBXSDK_sprintf(header, 100, "            Binormal: ");
                if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
                {
                    switch (leBinormal->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = leBinormal->GetIndexArray().GetAt(vertexId);
                        Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
                    }
                    break;
                    default:
                        break; // other reference modes not shown here!
                    }
                }
            }
            vertexId++;
        } // for polygonSize
    } // for polygonCount


      //check visibility for the edges of the mesh
    for (int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
    {
        FbxGeometryElementVisibility* leVisibility = pMesh->GetElementVisibility(l);
        FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
        DisplayString(header);
        switch (leVisibility->GetMappingMode())
        {
        default:
            break;
            //should be eByEdge
        case FbxGeometryElement::eByEdge:
            //should be eDirect
            for (j = 0; j != pMesh->GetMeshEdgeCount(); ++j)
            {
                DisplayInt("        Edge ", j);
                DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
            }

            break;
        }
    }
    DisplayString("");
}

void XCMeshFBX::Init(int resourceId, std::string userFriendlyName, bool loaded)
{
    XCMesh::Init(resourceId, userFriendlyName, loaded);
}

void XCMeshFBX::Draw(RenderContext& context, ShaderType shaderType)
{
    XCMesh::Draw(context, shaderType);
}

void XCMeshFBX::Destroy()
{
    XCMesh::Destroy();
}

void XCMeshFBX::DrawSubMeshes(RenderContext& renderContext, ShaderType shaderType)
{
    XCMesh::DrawSubMeshes(renderContext, shaderType);
}