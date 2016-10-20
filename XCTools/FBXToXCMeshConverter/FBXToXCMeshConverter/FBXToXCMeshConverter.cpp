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

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

using namespace DirectX;

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

void DisplaySkeleton(FbxNode* pNode);
void DisplayAnimation(FbxScene* pScene);
void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode);
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode);
void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve));
void DisplayCurveKeys(FbxAnimCurve* pCurve);

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

	DisplayAnimation(pScene);
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
		case FbxNodeAttribute::eSkeleton:
			DisplaySkeleton(pNode);
			break;
        case FbxNodeAttribute::eMesh:
            TraverseMesh(pNode);
            break;

		default:
			break;
        }
    }

    for (i = 0; i < pNode->GetChildCount(); i++)
    {
        TraverseContent(pNode->GetChild(i));
    }
}

static unsigned int g_currentMesh = 0;

void DisplayMetaDataConnections(FbxObject* pObject)
{
	int nbMetaData = pObject->GetSrcObjectCount<FbxObjectMetaData>();
	if (nbMetaData > 0)
		printf("    MetaData connections ");

	for (int i = 0; i < nbMetaData; i++)
	{
		FbxObjectMetaData* metaData = pObject->GetSrcObject<FbxObjectMetaData>(i);
		printf("        Name: %s", (char*)metaData->GetName());
	}
}

void DisplaySkeleton(FbxNode* pNode)
{
	FbxSkeleton* lSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();

	printf("Skeleton Name: %s\n", (char *)pNode->GetName());
	DisplayMetaDataConnections(lSkeleton);

	const char* lSkeletonTypes[] = { "Root", "Limb", "Limb Node", "Effector" };

	printf("    Type: %s", lSkeletonTypes[lSkeleton->GetSkeletonType()]);

	if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
	{
		printf("    Limb Length: %f", lSkeleton->LimbLength.Get());
	}
	else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
	{
		printf("    Limb Node Size: %f", lSkeleton->Size.Get());
	}
	else if (lSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
	{
		printf("    Limb Root Size: %f\n", lSkeleton->Size.Get());
	}
}

void DisplayAnimation(FbxScene* pScene)
{
	int i;
	for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

		FbxString lOutputString = "Animation Stack Name: ";
		lOutputString += lAnimStack->GetName();
		lOutputString += "\n\n";
		printf(lOutputString);

		DisplayAnimation(lAnimStack, pScene->GetRootNode());
	}
}

void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode)
{
	int l;
	int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
	FbxString lOutputString;

	lOutputString = "Animation stack contains ";
	lOutputString += nbAnimLayers;
	lOutputString += " Animation Layer(s)\n";
	printf(lOutputString);

	for (l = 0; l < nbAnimLayers; l++)
	{
		FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);

		lOutputString = "AnimLayer ";
		lOutputString += l;
		lOutputString += "\n";
		printf(lOutputString);

		DisplayAnimation(lAnimLayer, pNode);
	}
}

void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode)
{
	int lModelCount;
	FbxString lOutputString;

	lOutputString = "     Node Name: ";
	lOutputString += pNode->GetName();
	lOutputString += "\n\n";
	printf(lOutputString);

	DisplayChannels(pNode, pAnimLayer, DisplayCurveKeys);
	printf("\n");

	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		DisplayAnimation(pAnimLayer, pNode->GetChild(lModelCount));
	}
}

void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve))
{
	FbxAnimCurve* lAnimCurve = NULL;

	// Display general curves.
	{
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			printf("        TX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			printf("        TY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			printf("        TZ\n");
			DisplayCurve(lAnimCurve);
		}

		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			printf("        RX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			printf("        RY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			printf("        RZ\n");
			DisplayCurve(lAnimCurve);
		}

		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			printf("        SX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			printf("        SY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			printf("        SZ\n");
			DisplayCurve(lAnimCurve);
		}
	}

	// Display curves specific to a light or marker.
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		// Display curves specific to a geometry.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;

			int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
			for (int lBlendShapeIndex = 0; lBlendShapeIndex < lBlendShapeDeformerCount; ++lBlendShapeIndex)
			{
				FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

				int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
				for (int lChannelIndex = 0; lChannelIndex < lBlendShapeChannelCount; ++lChannelIndex)
				{
					FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
					const char* lChannelName = lChannel->GetName();

					lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
					if (lAnimCurve)
					{
						printf("        Shape %s\n", lChannelName);
						DisplayCurve(lAnimCurve);
					}
				}
			}
		}
	}
}

static int InterpolationFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
	return 0;
}

static int ConstantmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
	return 0;
}

static int TangentmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
	if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
	if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
	return 0;
}

static int TangentweightFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
	return 0;
}

static int TangentVelocityFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
	return 0;
}

void DisplayCurveKeys(FbxAnimCurve* pCurve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = pCurve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);

		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		printf(lOutputString);
	}
}

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
    XMVECTOR trans = XMLoadFloat4(&XMFLOAT4(translation[0], translation[1], translation[2], translation[3]));
    mesh.m_geometricTranslation.x = XMVectorGetX(trans);
    mesh.m_geometricTranslation.y = XMVectorGetY(trans);
    mesh.m_geometricTranslation.z = XMVectorGetZ(trans);

    XMVECTOR originTranslate = XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)) - trans;
    originTranslate.vector4_f32[1] = 0.0f; originTranslate.vector4_f32[2] = 0.0f; originTranslate.vector4_f32[3] = 1.0f;

    if (trans.vector4_f32[0] < 0)
        originTranslate = -XMVector3Length(originTranslate);
    else
        originTranslate = XMVector3Length(originTranslate);

    //fill the vertices & uv coords
    std::vector<XCMesh::Vertex> vertices;
    std::vector<unsigned int> indices;

    XMMATRIX originMat = XMMatrixTranslation(XMVectorGetX(originTranslate), 0.0f, 0.0f);
    XMVECTOR xmVertex;

    for (unsigned int polygonIndex = 0; polygonIndex < lMesh->GetPolygonCount(); ++polygonIndex)
    {
        for (unsigned int faceVertexIndex = 0; faceVertexIndex < lMesh->GetPolygonSize(polygonIndex); ++faceVertexIndex)
        {
            int vIndex = lMesh->GetPolygonVertex(polygonIndex, faceVertexIndex);

            FbxVector4 v1Data = lControlPoints[vIndex];
            //Translate here to origin
            xmVertex = XMLoadFloat3(&XMFLOAT3(v1Data[0], v1Data[1], v1Data[2]));
            xmVertex = XMVector3Transform(xmVertex, originMat);

            XCMesh::Vertex vertex = { XMVectorGetX(xmVertex), XMVectorGetY(xmVertex), XMVectorGetZ(xmVertex) };

            //printf("Polygon : %f %f %f ", vertex.x, vertex.y, vertex.z);

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

    std::ofstream outStream("jd_objectcenter.xcmesh", std::ios::out | std::ios::binary);
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

        if (strcmp(operation.c_str(), "export") == 0)
        {
            result = ExportMesh(meshFilename);
        }
        else if (strcmp(operation.c_str(), "import") == 0)
        {
            result = ImportMesh(meshFilename);
        }
        meshContainer.clear();
    }
	
	getchar();
    return !result;
}