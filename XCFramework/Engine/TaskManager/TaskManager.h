/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Task/ITask.h"
#include "Engine/System/ISystem.h"

class TaskManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(TaskManager)

    TaskManager()   {}
    ~TaskManager(void);

    void                    Destroy();

    std::future<i32>        RegisterTask(ITask *_pProcess);

    bool                    UnregisterTask(u64 threadId);
    void                    UnregisterAllTasks();

    void                    Update();
    bool                    CreateAsyncThread();

    ETaskState              GetTaskState(u64 threadId);

private:

    bool                    UnregisterTask(u32 _instanceId);
    i32                     GetTaskIndex(u64 threadId);
    
    std::vector<ITask*>     m_taskQueue;
};