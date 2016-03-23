/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

// FBXToXCMeshConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fbxsdk.h"

#include <fstream>



class XCMesh
{
public:
    XCMesh()
    {
        memset(&m_geometricTranslation, 0, sizeof(Vector4));
        memset(&m_geometricScaling, 0, sizeof(Vector4));
        memset(&m_geometricRotation, 0, sizeof(Vector4));
        m_vertices = nullptr;
        m_indices = nullptr;
        m_noOfvertices = 0;
        m_noOfIndices = 0;
    }

    struct Vertex
    {
        float x, y, z;
    };

    struct Vector4
    {
        float x, y, z, w;
    };

    void write(std::ofstream* fp)
    {
        //Translation
        unsigned int meshNameLength = strlen(m_meshName);
        fp->write(reinterpret_cast<char*>(&meshNameLength), sizeof(unsigned int));
        fp->write(reinterpret_cast<char*>(&m_meshName), strlen(m_meshName));
        fp->write(reinterpret_cast<char*>(&m_geometricTranslation), sizeof(Vector4));
        fp->write(reinterpret_cast<char*>(&m_geometricScaling), sizeof(Vector4));
        fp->write(reinterpret_cast<char*>(&m_geometricRotation), sizeof(Vector4));

        fp->write(reinterpret_cast<char*>(&m_noOfvertices), sizeof(m_noOfvertices));
        fp->write(reinterpret_cast<char*>(&m_vertices[0]), sizeof(Vertex) * m_noOfvertices);

        fp->write(reinterpret_cast<char*>(&m_noOfIndices), sizeof(m_noOfIndices));
        fp->write(reinterpret_cast<char*>(&m_indices[0]), sizeof(unsigned int) * m_noOfIndices);
    }

    void read(std::ifstream* fp)
    {
        //Translation
        unsigned int meshNameLength = 0;
        fp->read(reinterpret_cast<char*>(&meshNameLength), sizeof(unsigned int));
        fp->read(reinterpret_cast<char*>(&m_meshName), meshNameLength);
        fp->read(reinterpret_cast<char*>(&m_geometricTranslation), sizeof(Vector4));
        fp->read(reinterpret_cast<char*>(&m_geometricScaling), sizeof(Vector4));
        fp->read(reinterpret_cast<char*>(&m_geometricRotation), sizeof(Vector4));
        
        fp->read(reinterpret_cast<char*>(&m_noOfvertices), sizeof(m_noOfvertices));
        if (m_vertices == nullptr)
            m_vertices = new Vertex[m_noOfvertices];
        fp->read(reinterpret_cast<char*>(&m_vertices[0]), sizeof(Vertex) * m_noOfvertices);
        
        fp->read(reinterpret_cast<char*>(&m_noOfIndices), sizeof(m_noOfIndices));
        if (m_indices == nullptr)
            m_indices = new unsigned int[m_noOfIndices];
        fp->read(reinterpret_cast<char*>(&m_indices[0]), sizeof(unsigned int) * m_noOfIndices);
    }

    ~XCMesh()
    {
        if (m_vertices)
        {
            delete[] m_vertices;
        }

        if (m_indices)
        {
            delete[] m_indices;
        }
    }

    Vector4                   m_geometricTranslation;
    Vector4                   m_geometricRotation;
    Vector4                   m_geometricScaling;

    unsigned int                m_noOfvertices;
    unsigned int                m_noOfIndices;
    Vertex*                     m_vertices;
    unsigned int*               m_indices;
    char                        m_meshName[128];
};

static unsigned int g_nbOfMeshes = 0;
XCMesh* g_meshContainer = nullptr;

void TraverseContent(FbxScene* pScene);
void TraverseContent(FbxNode* pNode);
void TraverseMesh(FbxNode* pNode);


bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
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
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", error.Buffer());

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        printf("\n");

        for (i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            printf("    Animation Stack %d\n", i);
            printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            printf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        FbxIOSettings* ioSettings = pManager->GetIOSettings();
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
        printf("Please enter password: ");

        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
            scanf("%s", lPassword);
        FBXSDK_CRT_SECURE_NO_WARNING_END

            FbxString lString(lPassword);

        FbxIOSettings* ioSettings = pManager->GetIOSettings();
        ioSettings->SetStringProp(IMP_FBX_PASSWORD, lString);
        ioSettings->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
        {
            printf("\nPassword is wrong, import aborted.\n");
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

void TraverseContent(FbxScene* pScene)
{
    int i;
    FbxNode* lNode = pScene->GetRootNode();

    //calculate the no of mesh objects;
    if (lNode)
    {
        for (i = 0; i < lNode->GetChildCount(); i++)
        {
            FbxNode* pNode = lNode->GetChild(i);
            FbxNodeAttribute::EType lAttributeType;
            int i;

            if (pNode->GetNodeAttribute() == NULL)
            {
                printf("NULL Node Attribute\n\n");
            }
            else
            {
                lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

                switch (lAttributeType)
                {
                default:
                    break;

                case FbxNodeAttribute::eMesh:
                    g_nbOfMeshes++;
                    break;

                }
            }
        }
    }

    //New the nb of meshes
    g_meshContainer = new XCMesh[g_nbOfMeshes];

    if (lNode)
    {
        for (i = 0; i < lNode->GetChildCount(); i++)
        {
            TraverseContent(lNode->GetChild(i));
        }
    }
}

void TraverseContent(FbxNode* pNode)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    if (pNode->GetNodeAttribute() == NULL)
    {
        printf("NULL Node Attribute\n\n");
    }
    else
    {
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {
        default:
            break;

        case FbxNodeAttribute::eMesh:
            TraverseMesh(pNode);
            break;

        }
    }

    for (i = 0; i < pNode->GetChildCount(); i++)
    {
        TraverseContent(pNode->GetChild(i));
    }
}

static unsigned int g_currentMesh = 0;

void TraverseMesh(FbxNode* pNode)
{
    FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();
    FbxVector4* lControlPoints = lMesh->GetControlPoints();

    XCMesh& mesh = g_meshContainer[g_currentMesh++];

    const char* name = pNode->GetName();
    strcpy(mesh.m_meshName, name);
    mesh.m_meshName[strlen(name)] = '\0';

    //Get the geometry transform
    FbxVector4 translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    XCMesh::Vector4 trans = { translation[0], translation[1], translation[2], translation[3] };
    mesh.m_geometricTranslation = trans;
    
    //fill the vertices & uv coords
    std::vector<XCMesh::Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int polygonIndex = 0; polygonIndex < lMesh->GetPolygonCount(); ++polygonIndex)
    {
        for (unsigned int faceVertexIndex = 0; faceVertexIndex < lMesh->GetPolygonSize(polygonIndex); ++faceVertexIndex)
        {
            int vIndex = lMesh->GetPolygonVertex(polygonIndex, faceVertexIndex);

            FbxVector4 v1Data = lControlPoints[vIndex];
            XCMesh::Vertex vertex = { v1Data[0], v1Data[1], v1Data[2] };

            printf("Polygon : %f %f %f ", vertex.x, vertex.y, vertex.z);

            //Find if the vertex is present in our vertices.
            auto foundVertex = std::find_if(vertices.begin(), vertices.end(), [&vertex](XCMesh::Vertex v) -> bool
            {
                return fabs(v.x) == fabs(vertex.x) && fabs(v.y) == fabs(vertex.y) && fabs(v.z) == fabs(vertex.z);
            });

            if (foundVertex == vertices.end())
            {
                //New vertex found. Add it to our vertices
                vertices.push_back(vertex);
                indices.push_back(vertices.size() - 1);
            }
            else
            {
                //Get the index no for the face
                indices.push_back(foundVertex - vertices.begin());
            }
        }
    }

    mesh.m_noOfvertices = vertices.size();
    mesh.m_vertices = new XCMesh::Vertex[mesh.m_noOfvertices];

    mesh.m_noOfIndices = indices.size();
    mesh.m_indices = new unsigned int[mesh.m_noOfIndices];

    for (unsigned int vertexIndex = 0; vertexIndex < vertices.size(); ++vertexIndex)
    {
        mesh.m_vertices[vertexIndex] = vertices[vertexIndex];
    }

    for (unsigned int index = 0; index < indices.size(); ++index)
    {
        mesh.m_indices[index] = indices[index];
    }
}

bool ExportMesh(std::string meshFilename)
{

    FbxManager* fbxMgr = FbxManager::Create();
    if (!fbxMgr)
    {
        printf("Error: Unable to create FBX Manager!\n");
        return false;
    }
    else
    {
        printf("Autodesk FBX SDK version %s\n", fbxMgr->GetVersion());
    }

    //Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(fbxMgr, IOSROOT);
    fbxMgr->SetIOSettings(ios);

    //Load plugins from the executable directory (optional)
    FbxString lPath = FbxGetApplicationDirectory();
    fbxMgr->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    FbxScene* fbxScene = FbxScene::Create(fbxMgr, "My Scene");
    if (!fbxScene)
    {
        printf("Error: Unable to create FBX scene!\n");
        return false;
    }

    //Load the scene
    bool result = LoadScene(fbxMgr, fbxScene, meshFilename.c_str());

    if (!result)
    {
        printf("Failed to find the mesh\n");
        return false;
    }

    TraverseContent(fbxScene);

    unsigned int size = g_nbOfMeshes;

    std::ofstream outStream("ABC.xcmesh", std::ios::out | std::ios::binary);
    outStream.write(reinterpret_cast<char*>(&size), sizeof(size));

    for (unsigned int nbMesh = 0; nbMesh < size; ++nbMesh)
    {
        g_meshContainer[nbMesh].write(&outStream);
    }

    outStream.close();

    return true;
}

bool ImportMesh(std::string meshFilename)
{
    unsigned int size = 0;
    std::ifstream inStream(meshFilename.c_str(), std::ios::in | std::ios::binary);
    inStream.read(reinterpret_cast<char*>(&size), sizeof(size));
    //inStream.read(reinterpret_cast<char*>(&meshContainer[0]), meshContainer.size() * sizeof(XCMesh));
    XCMesh* mesh = new XCMesh[size];

    for (unsigned int nbMesh = 0; nbMesh < size; ++nbMesh)
    {
        mesh[nbMesh].read(&inStream);
    }

    printf("[VECTOR FONT] Size of XCMesh : %d\n", sizeof(XCMesh));
    inStream.close();

    delete[] mesh;
    return true;
}


int main(int argc, char* argv[])
{
    bool result = 0;

    if (argc < 0)
    {
        printf("Please specify mesh filename\n");
    }
    else
    {
        std::string operation    = argv[1];
        std::string meshFilename = argv[2];
        std::vector<XCMesh> meshContainer;

        //if (strcmp(operation.c_str(), "export") == 0)
        {
            result = ExportMesh("ABC.FBX");
        }
        //else if (strcmp(operation.c_str(), "import") == 0)
        {
            result = ImportMesh("ABC.xcmesh");
        }
        meshContainer.clear();
    }
    return !result;
}