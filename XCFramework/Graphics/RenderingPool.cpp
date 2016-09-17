/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/RenderingPool.h"
#include "Graphics/XC_Graphics.h"

#if defined(XCGRAPHICS_DX12)
#include "Graphics/XC_GraphicsDx12.h"
#endif

RenderingPool::RenderingPool()
{
}

void RenderingPool::Init()
{
    m_graphicsSystem = &SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");

    //Initialize the staged render contexts
    //These staging render context are used per frame states which are common and not be deferred. Such as clear rtv, execute deferred contexts...
    m_FrameCommandList[0].Init(m_graphicsSystem->GetDevice(), &m_graphicsSystem->GetShaderContainer());

#if defined(XCGRAPHICS_DX12)
    m_ppCmdList[0] = &m_FrameCommandList[0].GetDeviceContext();
#endif

    for (u32 workerIndex = 0; workerIndex < NbRenderWorkerThreads; ++workerIndex)
    {
        //Create worker threads and initialize their render context
        m_renderWorkers[workerIndex].Init();
        m_renderWorkers[workerIndex].m_workerId = workerIndex;
        m_renderWorkers[workerIndex].m_running = true;
        m_renderWorkers[workerIndex].m_renderContext.Init(m_graphicsSystem->GetDevice(), &m_graphicsSystem->GetShaderContainer());

#if !defined(SINGLE_THREAD_RENDER)
        m_renderWorkers[workerIndex].m_workerThread.CreateThread(m_renderWorkers[workerIndex].WorkerThreadFunc, &m_renderWorkers[workerIndex]);
        m_renderWorkers[workerIndex].m_workerThread.Run();
#endif

#if defined(XCGRAPHICS_DX12)
        m_ppCmdList[workerIndex + NbFrameCommandList] = &m_renderWorkers[workerIndex].m_renderContext.GetDeviceContext();
#endif
    }
}

void RenderingPool::Destroy()
{
    //Destroy the worker threads
    for (auto& workers : m_renderWorkers)
    {
        workers.Destroy();
    }
}

void RenderingPool::AddRenderableObject(IRenderableObject* obj, i32 baseObjId)
{
    //Add from main thread only.
    RenderWorkerType workerType = obj->GetRenderOnWorkerType();
    workerType = workerType < WorkerType_Max ? workerType : WorkerType_Misc;

    m_renderWorkers[workerType].m_renderableObjectRefList[baseObjId] = obj;
}

void RenderingPool::RemoveRenderableObject(IRenderableObject* obj, i32 baseObjId)
{
    RenderWorkerType workerType = obj->GetRenderOnWorkerType();
    workerType = workerType < WorkerType_Max ? workerType : WorkerType_Misc;

    auto objRef = m_renderWorkers[workerType].m_renderableObjectRefList.find(baseObjId);
    if (objRef != m_renderWorkers[workerType].m_renderableObjectRefList.end())
    {
        m_renderWorkers[workerType].m_renderableObjectRefList.erase(objRef);
    }
}

void RenderingPool::AddResourceDrawable(IResource* obj)
{
    switch (obj->GetResourceType())
    {
    case RESOURCETYPE_MESH:
        m_renderWorkers[WorkerType_XCMesh].m_resourceRefList.push_back(obj);
        break;

    default:
        Logger("Unkown resource draw request");
        XCASSERT(false);
    }
}

void RenderingPool::RemoveResourceDrawable(IResource* obj)
{
    switch (obj->GetResourceType())
    {
    case RESOURCETYPE_MESH:
    {
        auto refList = m_renderWorkers[WorkerType_XCMesh].m_resourceRefList;
        auto objRef = std::find_if(refList.begin(), refList.end(), [obj](IResource* resource) -> bool
        {
            return obj == resource;
        });

        if (objRef != refList.end())
        {
            m_renderWorkers[WorkerType_XCMesh].m_resourceRefList.erase(objRef);
        }
        break;
    }

    default:
        Logger("Unkown resource draw request");
        XCASSERT(false);
    }
}

void RenderingPool::RequestResourceDeviceContext(IResource* graphicsBuffer)
{
    m_renderWorkers[WorkerType_ResourceLoader].m_resourceRefList.push_back(graphicsBuffer);
}

void RenderingPool::Begin(RenderTargetsType targetType)
{
#if defined(XCGRAPHICS_DX12)
    //Clear the rtv and dsv
    m_FrameCommandList[0].BeginRender(targetType);
    m_graphicsSystem->ClearRTVAndDSV(&m_FrameCommandList[0].GetDeviceContext(), targetType);
#endif

    for (auto& workers : m_renderWorkers)
    {
        workers.m_renderContext.BeginRender(targetType);
    }
}

void RenderingPool::Render()
{
#if defined(SINGLE_THREAD_RENDER)
    for (auto& workers : m_renderWorkers)
    {
        for (auto& obj : workers.m_renderableObjectRefList)
        {
            if (obj.second->IsRenderable())
            {
                obj.second->Draw(workers.m_renderContext);
            }
        }

#if defined(XCGRAPHICS_DX12)
        while (workers.m_resourceRefList.size() > 0)
        {
            workers.m_resourceRefList.back()->RenderContextCallback(workers.m_renderContext);
            workers.m_resourceRefList.pop_back();
        }
#endif
    }
#else
    //Remove from main thread only
    for (auto& workers : m_renderWorkers)
    {
        //Signal workers to start
        workers.m_beginRender.SignalEvent(1);
    }

    //Wait for them to complete finish cmd.
    for (auto& workers : m_renderWorkers)
    {
        //Wait workers to end
        workers.m_finishRender.WaitEvent(1);
    }
#endif
}

void RenderingPool::End()
{
#if defined(XCGRAPHICS_DX12)
    m_FrameCommandList[0].FinishRender();
#endif

    //Now execute all cmd list
    for (auto& workers : m_renderWorkers)
    {
        //Finish cmdlist
        workers.m_renderContext.FinishRender();
    }
}

void RenderingPool::Execute(ID3DCommandQueue* cmdQueue)
{
#if defined(XCGRAPHICS_DX12)
    cmdQueue->ExecuteCommandLists(_countof(m_ppCmdList), m_ppCmdList);
#endif
}

void RenderingPool::RenderWorker::Init()
{
    m_beginRender.CreateEvent();
    m_finishRender.CreateEvent();
}

i32 RenderingPool::RenderWorker::WorkerThreadFunc(void* param)
{
    RenderWorker* worker = (RenderWorker*)param;

    while (worker->m_running)
    {
        //Wait for signal from main thread
        worker->m_beginRender.WaitEvent(1);

        if (worker->m_workerId != WorkerType_ResourceLoader)
        {
            //Call Render
            for (auto& obj : worker->m_renderableObjectRefList)
            {
                if (obj.second->IsRenderable())
                {
                    obj.second->Draw(worker->m_renderContext);
                }
            }
        }

        //In the case of instancing mesh rendering. The WorkerType_XCMesh renders all actors first which will accumulate all instance 
        //data and then we render the actual mesh resource.
        if (worker->m_workerId == WorkerType_XCMesh)
        {
            for (auto& obj : worker->m_resourceRefList)
            {
                if (obj->IsLoaded())
                {
                    obj->Draw(worker->m_renderContext);
                }
            }
        }
        else if (worker->m_workerId == WorkerType_ResourceLoader)
        {
            IResource* res = nullptr;
            while (worker->m_resourceRefList.size() > 0)
            {
                res = worker->m_resourceRefList.back();
                switch (res->GetResourceType())
                {
                case RESOURCETYPE_VERTEXBUFFER:
                case RESOURCETYPE_INDEXBUFFER:
                case RESOURCETYPE_TEXTURE2D:
                case RESOURCETYPE_CUBETEXTURE3D:
                    res->RenderContextCallback(worker->m_renderContext.GetDeviceContext());
                    break;

                default:
                    XCASSERT(false);
                    break;
                }

                worker->m_resourceRefList.pop_back();
            }
        }

        //Increase the semaphore to indicate that this worker has done the work.
        worker->m_finishRender.SignalEvent(1);
    }

    return 0;
}

void RenderingPool::RenderWorker::Destroy()
{
    m_renderContext.Destroy();
    m_workerThread.Join();

    m_beginRender.DestroyEvent();
    m_finishRender.DestroyEvent();
}