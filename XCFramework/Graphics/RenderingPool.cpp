/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/RenderingPool.h"
#include "Graphics/XCGraphics.h"

const u32 RenderingPool::RenderWorkerTypeMaskMap[] = { WorkerMask_None,
                                                       WorkerMask_None,
                                                       WorkerMask_None };

RenderingPool::RenderingPool()
{
}

void RenderingPool::Init()
{
    m_graphicsSystem = &SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");

    //Initialize the staged render contexts
    //These staging render context are used per frame states which are common and not be deferred. Such as clear rtv, execute deferred contexts...
    m_FrameCommandList[0].Init(&m_graphicsSystem->GetShaderContainer());

#if defined(XCGRAPHICS_DX12)
    m_ppCmdList[0] = &m_FrameCommandList[0].GetDeviceContext();
#endif

    for (u32 workerIndex = 0; workerIndex < NbRenderWorkerThreads; ++workerIndex)
    {
        //Create worker threads and initialize their render context
        m_renderWorkers[workerIndex].Init();
        m_renderWorkers[workerIndex].m_workerId = workerIndex;
        m_renderWorkers[workerIndex].m_running = true;
        m_renderWorkers[workerIndex].m_renderContext.Init(&m_graphicsSystem->GetShaderContainer());

#if !defined(SINGLE_THREAD_RENDER)
        m_renderWorkers[workerIndex].m_workerThread.CreateThread(m_renderWorkers[workerIndex].WorkerThreadFunc, &m_renderWorkers[workerIndex]);
        m_renderWorkers[workerIndex].m_workerThread.Run();
#endif

#if defined(XCGRAPHICS_DX12)
        m_ppCmdList[workerIndex + NbFrameCommandList] = &m_renderWorkers[workerIndex].m_renderContext.GetDeviceContext();
#endif
    }

    m_addRemoveLock.Create(true);
}

void RenderingPool::Destroy()
{
    m_addRemoveLock.Release();

    //Destroy the worker threads
    for (auto& workers : m_renderWorkers)
    {
        workers.Destroy();
    }
}

void RenderingPool::AddRenderableObject(IRenderableObject* obj, i32 baseObjId)
{
    m_addRemoveLock.Enter();

    //Add from main thread only.
    u32 workerMask = obj->GetRenderWorkerMask();
    RenderWorkerType workerType = obj->GetRenderWorkerType();

    for (u32 maskIndex = 0; maskIndex < WorkerType_Max; ++maskIndex)
    {
        if (maskIndex == workerType || (workerMask != WorkerMask_None && workerMask & RenderWorkerTypeMaskMap[maskIndex]))
        {
            auto& renderObject = std::find_if(m_renderWorkers[maskIndex].m_renderableObjectRefList.begin(),
                                              m_renderWorkers[maskIndex].m_renderableObjectRefList.end(),
                                              [baseObjId](const std::pair<u32, IRenderableObject*>& obj) -> bool
            {
                return baseObjId == obj.first;
            });

            if(renderObject == m_renderWorkers[maskIndex].m_renderableObjectRefList.end())
            {
                m_renderWorkers[maskIndex].m_renderableObjectRefList[baseObjId] = obj;
                Logger("[Rendering Pool] Added Renderable object on worker id : %d Base Obj id : %d", maskIndex, baseObjId);
            }
            else
            {
                XCASSERTMSG("[Rendering Pool] Cannot add Renderable Object, since it already exists in the pool.", false);
            }
        }
    }

    m_addRemoveLock.Exit();
}

void RenderingPool::RemoveRenderableObject(IRenderableObject* obj, i32 baseObjId)
{
    m_addRemoveLock.Enter();

    u32 workerMask = obj->GetRenderWorkerMask();
    RenderWorkerType workerType = obj->GetRenderWorkerType();

    for (u32 maskIndex = 0; maskIndex < WorkerType_Max; ++maskIndex)
    {
        if (maskIndex == workerType || (workerMask != WorkerMask_None && workerMask & RenderWorkerTypeMaskMap[maskIndex]))
        {
            auto objRef = m_renderWorkers[maskIndex].m_renderableObjectRefList.find(baseObjId);
            if (objRef != m_renderWorkers[maskIndex].m_renderableObjectRefList.end())
            {
                Logger("[Rendering Pool] Removing Renderable object on worker id : %d ", maskIndex);
                m_renderWorkers[maskIndex].m_renderableObjectRefList.erase(objRef);
            }
            else
            {
                XCASSERTMSG("[Rendering Pool] Cannot remove Renderable Object, since it does not exist in the pool.", false);
            }
        }
    }

    m_addRemoveLock.Exit();
}

void RenderingPool::RequestResourceDeviceContext(IRenderableObject* obj)
{
    m_addRemoveLock.Enter();
    m_renderWorkers[WorkerType_ResourceLoader].m_resourceRefList.push_back(obj);
    m_addRemoveLock.Exit();
}

void RenderingPool::Begin(std::vector<RenderTargetsType>& targetTypes)
{
    //Don't allow any objects to be entered, while beginning to render.
    //The lock will be released in End()
    m_addRemoveLock.Enter();

    m_FrameCommandList[0].Reset();

    //Clear the rtv and dsv
    m_FrameCommandList[0].BeginRender(targetTypes);
    m_graphicsSystem->ClearRTVAndDSVs(m_FrameCommandList[0].GetDeviceContext(), targetTypes, XCVec4(1.0f, 1.0f, 1.0f, 1.0f));

    for (auto& workers : m_renderWorkers)
    {
        workers.m_renderContext.Reset();
        workers.m_renderContext.BeginRender(targetTypes);
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
            workers.m_resourceRefList.back()->RenderContextCallback(workers.m_renderContext.GetDeviceContext());
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
    m_FrameCommandList[0].FinishRender();

    //Now execute all cmd list
    for (auto& worker : m_renderWorkers)
    {
        //Finish cmdlist
        worker.m_renderContext.FinishRender();
    }
}

void RenderingPool::OnRenderComplete()
{
    //Call render complete on all the objects
    for(auto& worker : m_renderWorkers)
    {
        for(auto& obj : worker.m_renderableObjectRefList)
        {
            if(obj.second->IsRenderable())
            {
                obj.second->OnRenderComplete();
            }
        }

        for(auto& obj : worker.m_resourceRefList)
        {
            if(obj->IsRenderable())
            {
                obj->OnRenderComplete();
            }
        }
    }

    //Finish the rendering, and allow modification of renderable objects.
    m_addRemoveLock.Exit();
}

#if defined(XCGRAPHICS_DX12)
void RenderingPool::Execute(ID3DCommandQueue* cmdQueue)
{
    cmdQueue->ExecuteCommandLists(_countof(m_ppCmdList), m_ppCmdList);
}
#endif

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

        //if (worker->m_workerId != WorkerType_Lighting)
        {
            //Accumulate the objects having mask WorkerType_InstancedDraw .
            std::vector<IRenderableObject*> instancedObj;

            //Call Render
            for(auto& obj : worker->m_renderableObjectRefList)
            {
                if(obj.second->IsRenderable())
                {
                    if(obj.second->GetRenderWorkerMask() & WorkerMask_InstancedDraw)
                    {
                        instancedObj.push_back(obj.second);
                    }
                    else
                    {
                        obj.second->Draw(worker->m_renderContext);
                    }
                }
            }

            //The WorkerType_InstancedDraw allows rendering of actors first which will accumulate all instance 
            //data and then we render the actual resource here.
            for (auto& obj : instancedObj)
            {
                obj->Draw(worker->m_renderContext);
            }
            
            if (worker->m_workerId == WorkerType_ResourceLoader)
            {
                IRenderableObject* res = nullptr;

                while (worker->m_resourceRefList.size() > 0)
                {
                    res = worker->m_resourceRefList.back();
                    res->RenderContextCallback(worker->m_renderContext.GetDeviceContext());

                    worker->m_resourceRefList.pop_back();
                }
            }
        }
        //Increase the semaphore to indicate that this worker has done the work.
        worker->m_finishRender.SignalEvent(1);
    }

    return 0;
}

void RenderingPool::RenderWorker::Destroy()
{
    m_resourceRefList.clear();
    m_renderableObjectRefList.clear();

    m_renderContext.Destroy();
    m_workerThread.Join();

    m_beginRender.DestroyEvent();
    m_finishRender.DestroyEvent();
}