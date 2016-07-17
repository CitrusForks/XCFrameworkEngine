/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Thread/Thread.h"

enum ETaskState
{
    TASKSTATE_INIT,
    TASKSTATE_RUNNING,
    TASKSTATE_DESTROY,

    TASKSTATE_INVALID
};

class ITask
{

public:
    ITask();
    virtual ~ITask(void);
    
    virtual void            Init() = 0;
    virtual void            Run() = 0;
    virtual void            Destroy();

    ETaskState              GetState() { return m_eCurrentState; }
    void                    SetState(ETaskState _state);
    
    void                    SetInstanceId(i32 _id)              { m_InstanceId = _id; }
    i32                     GetInstanceId()                     { return m_InstanceId; }

    void                    SetThreadId(u64 id) { m_threadID = id; }
    u64           GetThreadId()         { return m_threadID; }

    bool                    IsAsync()                       { return m_isAsync; }
    void                    SetSynchronizable(bool _value)  { m_isAsync = _value; }

    Thread*                 GetThreadHandle()               { return m_threadHandle; }
    void                    SetThreadHandle(Thread* handle)  { m_threadHandle = handle; }

    std::future<i32>        GetFuture()                     { return m_taskPromise.get_future(); }

    i32                     GetTaskPriority()             { return m_taskPriority; }
    void                    SetTaskPriority(i32 priority) { m_taskPriority = priority; }

protected:
    bool                    m_isAsync;
    ETaskState              m_eCurrentState;
    i32                     m_InstanceId;
    std::promise<i32>       m_taskPromise;

private:
    Thread*                 m_threadHandle;
    std::mutex              m_stateLock;
    u64           m_threadID;
    i32                     m_taskPriority;
};