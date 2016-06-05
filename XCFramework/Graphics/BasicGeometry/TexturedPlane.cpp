/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "TexturedPlane.h"

#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

#include "Graphics/XC_Shaders/XC_VertexShaderLayout.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"

#include "Engine/Resource/ResourceManager.h"

TexturedPlane::TexturedPlane()
{
}

TexturedPlane::TexturedPlane(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3)
    : Plane(p1, p2, p3)
{
    m_texture = nullptr;
}

TexturedPlane::~TexturedPlane()
{
}

void TexturedPlane::Init(int actorId)
{
    m_useShaderType = ShaderType_LightTexture;
    m_rasterType = RasterType_FillSolid;
}

void TexturedPlane::PreLoad(const void* fbBuffer)
{
    const FBTexturePlane* texPlaneBuff = (FBTexturePlane*)fbBuffer;
    m_currentPosition = toXMVECTOR(texPlaneBuff->Position()->x(), texPlaneBuff->Position()->y(), texPlaneBuff->Position()->z(), texPlaneBuff->Position()->w());
    m_initialRotation = toXMVECTOR(texPlaneBuff->Rotation()->x(), texPlaneBuff->Rotation()->y(), texPlaneBuff->Rotation()->z(), texPlaneBuff->Rotation()->w());
    m_initialScaling  = toXMVECTOR(texPlaneBuff->Scaling()->x(), texPlaneBuff->Scaling()->y(), texPlaneBuff->Scaling()->z(), texPlaneBuff->Scaling()->w());

    m_material.Ambient  = XCVec4(texPlaneBuff->Material()->Ambient()->x(), texPlaneBuff->Material()->Ambient()->y(), texPlaneBuff->Material()->Ambient()->z(), texPlaneBuff->Material()->Ambient()->w());
    m_material.Diffuse  = XCVec4(texPlaneBuff->Material()->Diffuse()->x(), texPlaneBuff->Material()->Diffuse()->y(), texPlaneBuff->Material()->Diffuse()->z(), texPlaneBuff->Material()->Diffuse()->w());
    m_material.Specular = XCVec4(texPlaneBuff->Material()->Specular()->x(), texPlaneBuff->Material()->Specular()->y(), texPlaneBuff->Material()->Specular()->z(), texPlaneBuff->Material()->Specular()->w());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_texture = &resMgr.AcquireResource(texPlaneBuff->ResourceName()->c_str());

    m_rasterType = (RasterType) texPlaneBuff->RasterizerType();

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(PerObjectBuffer)));
#endif
}

void TexturedPlane::PreLoad(XCVecIntrinsic4 initialPosition, XCVecIntrinsic4 initialRotation, XCVecIntrinsic4 initialScaling, BasicMaterial material, std::string texture, RasterType rasterType)
{
    m_currentPosition = initialPosition;
    m_initialRotation = initialRotation;
    m_initialScaling  = initialScaling;
    m_material        = material;

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_texture = &resMgr.AcquireResource(texture.c_str());

    m_rasterType      = rasterType;
}

void TexturedPlane::Load()
{
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));
    ApplyRotation(m_initialRotation);
    m_MScaling = XMMatrixScaling(XMVectorGetX(m_initialScaling), XMVectorGetY(m_initialScaling), XMVectorGetZ(m_initialScaling));
    m_World = m_MScaling * m_MRotation * m_MTranslation;

    BuildGeometryBuffers();
}

void TexturedPlane::BuildGeometryBuffers()
{
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, 1.0f, 0.0f),  XCVec3Unaligned(0.33f, 0.33f, 0.34f), XCVec2Unaligned(0.0f, 0.0f)));   //TL
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, -1.0f, 0.0f),  XCVec3Unaligned(0.33f, 0.33f, 0.34f), XCVec2Unaligned(1.0f, 1.0f)));   //BR
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, -1.0f, 0.0f), XCVec3Unaligned(0.33f, 0.33f, 0.34f), XCVec2Unaligned(1.0f, 0.0f)));  //BL
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, 1.0f, 0.0f),   XCVec3Unaligned(0.33f, 0.33f, 0.34f), XCVec2Unaligned(0.0f, 1.0f)));    //TR
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, -1.0f, 0.0f),  XCVec3Unaligned(0.33f, 0.33f, 0.34f), XCVec2Unaligned(1.0f, 1.0f)));   //BR
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, 1.0f, 0.0f),  XCVec3Unaligned(0.33f, 0.33f, 0.34f), XCVec2Unaligned(0.0f, 0.0f)));   //TL

    m_vertexBuffer.BuildVertexBuffer();
}

void TexturedPlane::Update(float dt)
{
}

void TexturedPlane::Draw(RenderContext& context)
{
    unsigned int stride = sizeof(VertexPosNormTex);
    unsigned int offset = 0;

    context.SetRasterizerState(m_rasterType);
    context.ApplyShader(m_useShaderType);

    // Set constants
    //TODO: Remove all teh overrides from here. Need to create a textureplaneactor from gameplay that will inherit this and then make it renderable
   ICamera& cam = context.GetShaderManagerSystem().GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix())),
        ToXCMatrix4Unaligned(InverseTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixIdentity()),
        m_material
    };

    XCShaderHandle* lightTexShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(ShaderType_LightTexture);
    lightTexShader->SetVertexBuffer(context.GetDeviceContext(), &m_vertexBuffer);

#if defined(XCGRAPHICS_DX12)
    memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(PerObjectBuffer));
    lightTexShader->SetConstantBuffer("PerObjectBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);
#else
    lightTexShader->setCBPerObject(context.GetDeviceContext(), perObject);
#endif

    lightTexShader->SetResource("gDiffuseMap", context.GetDeviceContext(), m_texture);

    context.GetShaderManagerSystem().DrawNonIndexed(context.GetDeviceContext(), 6);
}

void TexturedPlane::Destroy()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_texture);
}