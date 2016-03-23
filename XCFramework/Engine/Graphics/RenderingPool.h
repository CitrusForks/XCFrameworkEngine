/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/RenderWorkerTypeDefines.h"
#include "Engine/Graphics/RenderContext.h"
#include "Engine/Thread/Thread.h"
#include "Engine/Graphics/IRenderableObject.h"
#include "Engine/Graphics/RenderTargetTypes.h"
#include "Engine/Graphics/IGraphicsBuffer.h"
#include "Engine/Resource/IResource.h"
#include "Engine/Thread/XCSyncEvent.h"

class XC_Graphics;

class RenderingPool
{
public:
    static const int    NbRenderWorkerThreads = WorkerType_Max;
    static const int    NbFrameCommandList    = 1;

    struct RenderWorker
    {
        XCSyncEvent                         m_beginRender;
        XCSyncEvent                         m_finishRender;

        int                                 m_workerId;
        bool                                m_running;
        RenderContext                       m_renderContext;
        std::map<int, IRenderableObject*>   m_renderableObjectRefList;
        std::vector<IResource*>             m_resourceRefList;

        Thread                              m_workerThread;

        void                                Init();

#if defined(WIN32)
        static int                          WorkerThreadFunc(void* param);
#elif defined(XC_ORBIS)
        static void*                        WorkerThreadFunc(void* param);
#endif

        void                                Destroy();
    };


    RenderingPool();

    void                Init();

    //Gameplay Actors request
    void                AddRenderableObject(IRenderableObject* obj, int baseObjId);      //Objects/Actors to be drawn
    void                RemoveRenderableObject(IRenderableObject* obj, int baseObjId);

    //Resources related request
    void                AddResourceDrawable(IResource* obj);
    void                RemoveResourceDrawable(IResource* obj);
    void                RequestResourceDeviceContext(IResource* graphicsBuffer);

    void                Update(float dt);                                               //Use it to push and pop renderable objects, since in update we remove the actors.
    void                Begin(RenderTargetsType targetType);                            //Signal the workers to draw
    void                Render();
    void                End();                                                          //Signal end and call execute command list on main thread
    void                Execute(ID3DCommandQueue* cmdQueue);
    void                Destroy();

    RenderWorker*       GetRenderWorkers() { return m_renderWorkers; }

private:
    RenderWorker        m_renderWorkers[NbRenderWorkerThreads];
    RenderContext       m_FrameCommandList[NbFrameCommandList];

    XC_Graphics*        m_graphicsSystem;

#if defined(XCGRAPHICS_DX12)
    ID3D12CommandList*  m_ppCmdList[NbRenderWorkerThreads + 1];
#endif
};