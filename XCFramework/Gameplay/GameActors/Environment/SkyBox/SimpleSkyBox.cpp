/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/Environment/SkyBox/SimpleSkyBox.h"

#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"

#include "Engine/Resource/ResourceManager.h"

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

    m_useShaderType = ShaderType_SimpleCubeMap;

    m_rasterType = RasterType_FillSolid;
}

void SimpleSkyBox::PreLoad(const void* fbBuffer)
{
    const FBSimpleSkyBox* skyBoxBuff = (FBSimpleSkyBox*)fbBuffer;
    m_currentPosition.SetValues(skyBoxBuff->Position()->x(), skyBoxBuff->Position()->y(), skyBoxBuff->Position()->z(), skyBoxBuff->Position()->w());
    m_initialRotation.SetValues(skyBoxBuff->Rotation()->x(), skyBoxBuff->Rotation()->y(), skyBoxBuff->Rotation()->z(), skyBoxBuff->Rotation()->w());
    m_initialScaling.SetValues(skyBoxBuff->Scaling()->x(),  skyBoxBuff->Scaling()->y(),  skyBoxBuff->Scaling()->z(),  skyBoxBuff->Scaling()->w());

    m_material.Ambient = XCVec4(skyBoxBuff->Material()->Ambient()->x(), skyBoxBuff->Material()->Ambient()->y(), skyBoxBuff->Material()->Ambient()->z(), skyBoxBuff->Material()->Ambient()->w());
    m_material.Diffuse = XCVec4(skyBoxBuff->Material()->Diffuse()->x(), skyBoxBuff->Material()->Diffuse()->y(), skyBoxBuff->Material()->Diffuse()->z(), skyBoxBuff->Material()->Diffuse()->w());
    m_material.Specular = XCVec4(skyBoxBuff->Material()->Specular()->x(), skyBoxBuff->Material()->Specular()->y(), skyBoxBuff->Material()->Specular()->z(), skyBoxBuff->Material()->Specular()->w());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_cubeMapTexture = &resMgr.AcquireResource(skyBoxBuff->CubeTexture3DResourceName()->c_str());

    m_rasterType = (RasterType) skyBoxBuff->RasterizerType();

    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_CBwvp = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbWVP)));

    SimpleActor::PreLoad(fbBuffer);
}

void SimpleSkyBox::Load()
{
    m_MTranslation = MatrixTranslate(m_currentPosition);
    ApplyRotation(m_initialRotation);
    m_MScaling = MatrixScale(m_initialScaling);
    m_World = m_MScaling * m_MRotation * m_MTranslation;

    BuildBuffers();
    SimpleActor::Load();
}

void SimpleSkyBox::UpdateState()
{
    if (m_cubeMapTexture->m_Resource->IsLoaded())
    {
        m_actorState = IActor::ActorState_Loaded;
    }

    SimpleActor::UpdateState();
}

void SimpleSkyBox::BuildBuffers()
{
    XC_Graphics& graphicsSystem = (XC_Graphics&) SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    
    //Set up vertices
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(-1.0f, -1.0f, -1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(-1.0f, 1.0f, -1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(1.0f, 1.0f, -1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(1.0f, -1.0f, -1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(-1.0f, -1.0f, 1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(-1.0f, 1.0f, 1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(1.0f, 1.0f, 1.0f)));
    m_vertexBuffer.m_vertexData.push_back(VertexPos(XCVec3Unaligned(1.0f, -1.0f, 1.0f)));

    m_vertexBuffer.BuildVertexBuffer();

    m_Stride = sizeof(VertexPos);
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

    // Set constants
    //Calculate wvp and set it to the constant.
    ICamera& cam = context.GetShaderManagerSystem().GetGlobalShaderData().m_camera;

    cbWVP wbuffer = { MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned() };
    m_CBwvp->UploadDataOnGPU(context.GetDeviceContext(), &wbuffer, sizeof(cbWVP));

    // Set constants
    XCShaderHandle* cubeMapShader = (XCShaderHandle*) context.GetShaderManagerSystem().GetShader(ShaderType_SimpleCubeMap);

    cubeMapShader->SetVertexBuffer(context.GetDeviceContext(), &m_vertexBuffer);
    cubeMapShader->SetIndexBuffer(context.GetDeviceContext(), m_indexBuffer);

    cubeMapShader->SetConstantBuffer("cbWVP", context.GetDeviceContext(), *m_CBwvp);
    cubeMapShader->SetResource("gCubeMap", context.GetDeviceContext(), m_cubeMapTexture);
    
    context.GetShaderManagerSystem().DrawIndexedInstanced(context.GetDeviceContext(), 36, m_indexBuffer.GetIndexBufferInGPUMem());
    graphicsSystem.SetLessEqualDepthStencilView(context.GetDeviceContext(), false);
}

void SimpleSkyBox::Destroy()
{
    SimpleActor::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_cubeMapTexture);
}