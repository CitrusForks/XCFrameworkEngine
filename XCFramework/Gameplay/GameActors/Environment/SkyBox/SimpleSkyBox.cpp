/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/Environment/SkyBox/SimpleSkyBox.h"

#include "Graphics/XCGraphics.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/GPUResourceSystem.h"

#include "Engine/Resource/ResourceManager.h"

SimpleSkyBox::SimpleSkyBox(void)
{
    m_workerType       = WorkerType_SkyBox;
    m_renderWorkerMask = WorkerMask_None;
}

SimpleSkyBox::~SimpleSkyBox(void)
{
}

IActor::ActorReturnState SimpleSkyBox::Init()
{
    m_useShaderType = ShaderType_SimpleCubeMap;

    return SimpleActor::Init();
}

IActor::ActorReturnState SimpleSkyBox::LoadMetaData( const void* metaData )
{
    const FBSimpleSkyBox* skyBoxBuff = (FBSimpleSkyBox*) metaData;

    SimpleActor::LoadMetaData(skyBoxBuff->Base());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_cubeMapTexture = &resMgr.AcquireResource(skyBoxBuff->CubeTexture3DResourceName()->c_str());

    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_CBwvp = gpuSys.CreateConstantBufferResourceView(GPUResourceDesc(GPUResourceType_CBV, sizeof(cbWVP)));

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState SimpleSkyBox::Load()
{
    m_MTranslation = MatrixTranslate(m_currentPosition);
    ApplyRotation(m_initialRotation);
    m_MScaling = MatrixScale(m_initialScaling);
    m_World = m_MScaling * m_MRotation * m_MTranslation;

    BuildBuffers();

    return SimpleActor::Load();
}

void SimpleSkyBox::BuildBuffers()
{
    XCGraphics& graphicsSystem = (XCGraphics&) SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    
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

IActor::ActorReturnState SimpleSkyBox::Update(f32 dt)
{
    return SimpleActor::Update(dt);
}

bool SimpleSkyBox::Draw(RenderContext& renderContext)
{
    IActor::Draw(renderContext);
    
    renderContext.ApplyShader(m_useShaderType);

    XCGraphics& graphicsSystem = (XCGraphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    graphicsSystem.SetLessEqualDepthStencilView(renderContext.GetDeviceContext(), true);

    // Set constants
    //Calculate wvp and set it to the constant.
    ICamera& cam = renderContext.GetGlobalShaderData().m_camera;

    cbWVP wbuffer = { MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned() };
    m_CBwvp->UploadDataOnGPU(renderContext.GetDeviceContext(), &wbuffer, sizeof(cbWVP));

    // Set constants
    XCShaderHandle* cubeMapShader = (XCShaderHandle*) renderContext.GetShader(ShaderType_SimpleCubeMap);

    cubeMapShader->SetVertexBuffer(renderContext.GetDeviceContext(), &m_vertexBuffer);
    cubeMapShader->SetIndexBuffer(renderContext.GetDeviceContext(), m_indexBuffer);

    cubeMapShader->SetConstantBuffer("cbWVP", renderContext.GetDeviceContext(), *m_CBwvp);
    cubeMapShader->SetResource("gCubeMap", renderContext.GetDeviceContext(), m_cubeMapTexture);
    
    renderContext.DrawIndexedInstanced(36, m_indexBuffer.GetIndexBufferInGPUMem());
    graphicsSystem.SetLessEqualDepthStencilView(renderContext.GetDeviceContext(), false);

    return true;
}

IActor::ActorReturnState SimpleSkyBox::Destroy()
{
    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    gpuSys.DestroyResource(m_CBwvp);

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_cubeMapTexture);

    return SimpleActor::Destroy();
}