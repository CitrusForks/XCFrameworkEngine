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

    std::future<int>        RegisterTask(ITask *_pProcess);

    bool                    UnregisterTask(unsigned long threadId);
    void                    UnregisterAllTasks();

    void                    Update();
    bool                    CreateAsyncThread();

    ETaskState              GetTaskState(unsigned long threadId);

private:

    bool                    UnregisterTask(unsigned int _instanceId);
    int                     GetTaskIndex(unsigned long threadId);
    
    std::vector<ITask*>     m_taskQueue;
};