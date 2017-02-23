/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/RenderWorkerTypeDefines.h"
#include "Graphics/RenderContext.h"
#include "Graphics/IRenderableObject.h"
#include "Graphics/RenderTargetTypes.h"

#include "Engine/Resource/IResource.h"
#include "Base/Thread/Thread.h"
#include "Base/Thread/XCSyncEvent.h"
#include "Base/Thread/CriticalSection.h"

class XCGraphics;

class RenderingPool
{
public:

    static const u32 NbRenderWorkerThreads = WorkerType_Max;
    static const u32 NbFrameCommandList    = 1;
    static const u32 RenderWorkerTypeMaskMap[WorkerType_Max];

    RenderingPool();

    void                Init();

    //Gameplay Actors request
    void                AddRenderableObject(IRenderableObject* obj, i32 baseObjId);      //Objects/Actors to be drawn
    void                RemoveRenderableObject(IRenderableObject* obj, i32 baseObjId);

    //Resources related request
    void                RequestResourceDeviceContext(IRenderableObject* graphicsBuffer);

    void                Begin(std::vector<RenderTargetsType>& targetTypes);           //Signal the workers to draw
    void                Render();
    void                End();                                                        //Signal end and call execute command list on main thread
    void                OnRenderComplete();
    void                Destroy();

#if defined(XCGRAPHICS_DX12)
    void                Execute(ID3DCommandQueue* cmdQueue);
#endif

private:
    struct RenderWorker
    {
        XCSyncEvent                         m_beginRender;
        XCSyncEvent                         m_finishRender;

        u32                                 m_workerId;
        bool                                m_running;
        RenderContext                       m_renderContext;
        std::map<u32, IRenderableObject*>   m_renderableObjectRefList;
        std::vector<IRenderableObject*>     m_resourceRefList;

        Thread                              m_workerThread;

        void                                Init();
        void                                Destroy();

        static i32                          WorkerThreadFunc(void* param);
    };

private:

    CriticalSection     m_addRemoveLock;
    RenderWorker        m_renderWorkers[NbRenderWorkerThreads];
    RenderContext       m_FrameCommandList[NbFrameCommandList];

    XCGraphics*        m_graphicsSystem;

#if defined(XCGRAPHICS_DX12)
    ID3D12CommandList*  m_ppCmdList[NbRenderWorkerThreads + 1];
#endif
};