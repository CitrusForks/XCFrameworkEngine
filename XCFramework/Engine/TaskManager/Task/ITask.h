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
    
    void                    SetInstanceId(int _id)              { m_InstanceId = _id; }
    int                     GetInstanceId()                     { return m_InstanceId; }

    void                    SetThreadId(unsigned long id) { m_threadID = id; }
    unsigned long           GetThreadId()         { return m_threadID; }

    bool                    IsAsync()                       { return m_isAsync; }
    void                    SetSynchronizable(bool _value)  { m_isAsync = _value; }

    Thread*                 GetThreadHandle()               { return m_threadHandle; }
    void                    SetThreadHandle(Thread* handle)  { m_threadHandle = handle; }

    std::future<int>        GetFuture()                     { return m_taskPromise.get_future(); }

    int                     GetTaskPriority()             { return m_taskPriority; }
    void                    SetTaskPriority(int priority) { m_taskPriority = priority; }

protected:
    bool                    m_isAsync;
    ETaskState              m_eCurrentState;
    int                     m_InstanceId;
    std::promise<int>       m_taskPromise;

private:
    Thread*                 m_threadHandle;
    std::mutex              m_stateLock;
    unsigned long           m_threadID;
    int                     m_taskPriority;
};