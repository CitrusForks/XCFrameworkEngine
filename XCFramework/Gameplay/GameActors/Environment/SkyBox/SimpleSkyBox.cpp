/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameActors/Environment/SkyBox/SimpleSkyBox.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

SimpleSkyBox::SimpleSkyBox(void)
{
    m_useRenderWorkerType = WorkerType_SkyBox;
}

SimpleSkyBox::~SimpleSkyBox(void)
{
}

void SimpleSkyBox::Init(int actorId)
{
    SimpleActor::Init(actorId);

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    m_useShaderType = SHADERTYPE_SIMPLECUBEMAP;

    m_rasterType = RASTERIZERTYPE_FILL_SOLID;
}

void SimpleSkyBox::PreLoad(const void* fbBuffer)
{
    const FBSimpleSkyBox* skyBoxBuff = (FBSimpleSkyBox*)fbBuffer;
    m_currentPosition = toXMVECTOR(skyBoxBuff->Position()->x(), skyBoxBuff->Position()->y(), skyBoxBuff->Position()->z(), skyBoxBuff->Position()->w());
    m_initialRotation = toXMVECTOR(skyBoxBuff->Rotation()->x(), skyBoxBuff->Rotation()->y(), skyBoxBuff->Rotation()->z(), skyBoxBuff->Rotation()->w());
    m_initialScaling  = toXMVECTOR(skyBoxBuff->Scaling()->x(),  skyBoxBuff->Scaling()->y(),  skyBoxBuff->Scaling()->z(),  skyBoxBuff->Scaling()->w());

    m_material.Ambient = XCVec4(skyBoxBuff->Material()->Ambient()->x(), skyBoxBuff->Material()->Ambient()->y(), skyBoxBuff->Material()->Ambient()->z(), skyBoxBuff->Material()->Ambient()->w());
    m_material.Diffuse = XCVec4(skyBoxBuff->Material()->Diffuse()->x(), skyBoxBuff->Material()->Diffuse()->y(), skyBoxBuff->Material()->Diffuse()->z(), skyBoxBuff->Material()->Diffuse()->w());
    m_material.Specular = XCVec4(skyBoxBuff->Material()->Specular()->x(), skyBoxBuff->Material()->Specular()->y(), skyBoxBuff->Material()->Specular()->z(), skyBoxBuff->Material()->Specular()->w());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_cubeMapTexture = (CubeTexture3D*) resMgr.GetResource(skyBoxBuff->CubeTexture3DResourceName()->c_str());

    m_rasterType = (ERasterizer_Type) skyBoxBuff->RasterizerType();

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_CBwvp = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbWVP)));
#endif

    SimpleActor::PreLoad(fbBuffer);
}

void SimpleSkyBox::PreLoad(XCVecIntrinsic4 initialPosition, XCVecIntrinsic4 initialRotation, XCVecIntrinsic4 initialScaling, BasicMaterial material, CubeTexture3D* texture, ERasterizer_Type rasterType)
{
    m_currentPosition = initialPosition;
    m_initialRotation = initialRotation;
    m_initialScaling  = initialScaling;
    m_material        = material;
    m_cubeMapTexture  = texture;
    m_rasterType      = rasterType;
}

void SimpleSkyBox::Load()
{
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));
    ApplyRotation(m_initialRotation);
    m_MScaling = XMMatrixScaling(XMVectorGetX(m_initialScaling), XMVectorGetY(m_initialScaling), XMVectorGetZ(m_initialScaling));
    m_World = m_MScaling * m_MRotation * m_MTranslation;

    BuildBuffers();
    SimpleActor::Load();
}

void SimpleSkyBox::BuildBuffers()
{
    XC_Graphics& graphicsSystem = (XC_Graphics&) SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    
    //Set up vertices
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, -1.0f, -1.0f), XCVec3Unaligned(-0.33f, -0.33f, -0.33f), XCVec2Unaligned(0.0f, 1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, 1.0f, -1.0f),  XCVec3Unaligned(-0.33f, 0.33f, -0.33f),  XCVec2Unaligned(0.0f, 0.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, 1.0f, -1.0f),   XCVec3Unaligned(0.33f, 0.33f, -0.33f),   XCVec2Unaligned(1.0f, 0.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, -1.0f, -1.0f),  XCVec3Unaligned(0.33f, -0.33f, -0.33f),  XCVec2Unaligned(1.0f, 1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, -1.0f, 1.0f),  XCVec3Unaligned(-0.33f, -0.33f, 0.33f),  XCVec2Unaligned(0.0f, 1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(-1.0f, 1.0f, 1.0f),   XCVec3Unaligned(-0.33f, 0.33f, 0.33f),   XCVec2Unaligned(0.0f, 0.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, 1.0f, 1.0f),    XCVec3Unaligned(0.33f, 0.33f, 0.33f),    XCVec2Unaligned(1.0f, 0.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3Unaligned(1.0f, -1.0f, 1.0f),   XCVec3Unaligned(0.33f, -0.33f, 0.33f),   XCVec2Unaligned(1.0f, 1.0f)));

    m_vertexBuffer.BuildVertexBuffer();

    m_Stride = sizeof(VertexPosNormTex);
    m_Offset = 0;

        //Set up index buffer
    m_indexBuffer.AddIndicesVA
        ({
            0, 2, 1,
            0, 3, 2,
            
            4, 5, 6,
            4, 6, 7,
            
            4, 1, 5,
            4, 0, 1,
            
            3, 6, 2,
            3, 7, 6,
            
            1, 6, 5,
            1, 2, 6,
            
            4, 3, 0,
            4, 7, 3
                    });

    m_indexBuffer.BuildIndexBuffer();
}

void SimpleSkyBox::Update(float dt)
{
    SimpleActor::Update(dt);
}

void SimpleSkyBox::Draw(RenderContext& context)
{
    IActor::Draw(context);

        context.SetRasterizerState(m_rasterType);
        context.ApplyShader(m_useShaderType);
        
        XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
        graphicsSystem.SetLessEqualDepthStencilView(context.GetDeviceContext(), true);
      
        m_vertexBuffer.SetVertexBuffer(context.GetDeviceContext());
        m_indexBuffer.SetIndexBuffer(context.GetDeviceContext());
        
        // Set constants
        //Calculate wvp and set it to the constant.
        XC_CameraManager* cam = (XC_CameraManager*) &SystemLocator::GetInstance()->RequestSystem<XC_CameraManager>("CameraManager");

        cbWVP wbuffer = { ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())) };

        // Set constants
        XCShaderHandle* cubeMapShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(SHADERTYPE_SIMPLECUBEMAP);
#if defined(XCGRAPHICS_DX12)
        memcpy(m_CBwvp->m_cbDataBegin, &wbuffer, sizeof(cbWVP));
        cubeMapShader->setConstantBuffer("cbWVP", context.GetDeviceContext(), m_CBwvp->m_gpuHandle);
#else
        cubeMapShader->setWVP(context.GetDeviceContext(), wbuffer);
#endif
        cubeMapShader->setResource("gCubeMap", context.GetDeviceContext(), m_cubeMapTexture->getTextureResource());
  
        context.GetShaderManagerSystem().DrawIndexedInstanced(context.GetDeviceContext(), 36, m_indexBuffer.GetIndexBufferInGPUMem());
        graphicsSystem.SetLessEqualDepthStencilView(context.GetDeviceContext(), false);
}

void SimpleSkyBox::Destroy()
{
    SimpleActor::Destroy();
}