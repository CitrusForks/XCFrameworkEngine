/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "TaskManager.h"
#include "Engine/Thread/Thread.h"

TaskManager::~TaskManager(void)
{
}

#if defined(WIN32)
unsigned long WINAPI RunProcess(void* lpParam)
#elif defined(XC_ORBIS)
void* RunProcess(void* lpParam)
#endif
{
    ITask *process = (ITask*)lpParam;

    while (process->GetState() != TASKSTATE_DESTROY)
    {
        process->Run();
    }

#if defined(WIN32)
    return 1;
#else
    return nullptr;
#endif
}

std::future<int> TaskManager::RegisterTask(ITask *_pTask)
{
    m_taskQueue.push_back(_pTask);

    m_taskQueue.back()->Init();

    if(m_taskQueue.back()->IsAsync() == true)
    {
        if (CreateAsyncThread())
        {
            Logger("Thread Created Successfully");
        }
        else
        {
            Logger("Could not create Thread");
        }
    }

    return m_taskQueue.back()->GetFuture();
}

bool TaskManager::UnregisterTask(unsigned int _id)
{
    m_taskQueue[_id]->GetThreadHandle()->Join();

    Logger("[Task Manager] Thread just exited %d", m_taskQueue[_id]->GetInstanceId());

    if (m_taskQueue[_id] != nullptr)
    {
        m_taskQueue.erase(m_taskQueue.begin() + _id);
    }

    return true;
}

bool TaskManager::UnregisterTask(unsigned long threadId)
{
    int index = GetTaskIndex(threadId);

    if (index != -1)
    {
        UnregisterTask((unsigned int) index);
    }

    return false;
}

ETaskState TaskManager::GetTaskState(unsigned long threadId)
{
    int index = GetTaskIndex(threadId);

    if (index != -1)
    {
        return m_taskQueue[index]->GetState();
    }
    return TASKSTATE_INVALID;
}

int TaskManager::GetTaskIndex(unsigned long threadId)
{
    //Find the task with the thread Id
    for (unsigned int index = 0; index < m_taskQueue.size(); index++)
    {
        if (m_taskQueue[index]->GetThreadId() == threadId)
        {
            //Found. Erase it
            return index;
        }
    }

    return -1;
}

void TaskManager::UnregisterAllTasks()
{
    for (unsigned int i = 0; i < m_taskQueue.size(); i++)
    {
        m_taskQueue.erase(m_taskQueue.begin() + i);
    }
}

void TaskManager::Update()
{
        for(unsigned int i=0; i < m_taskQueue.size(); i++)
        {
            switch(m_taskQueue[i]->GetState())
            {

                case TASKSTATE_INIT:
                                            Logger("\nProcess is Running");
                                            if (!m_taskQueue[i]->IsAsync())
                                                m_taskQueue[i]->Run();

                                            break;
                case TASKSTATE_RUNNING:
                                            if (!m_taskQueue[i]->IsAsync())
                                            {
                                                //It's like a update
                                                m_taskQueue[i]->Run();
                                            }
                                            break;

                case TASKSTATE_DESTROY:
                                            //Process has done the task. So Unregister it from ProcessQueue
                                            Logger("\nProcess is Done the task. Unregistering now");
                                            UnregisterTask(i);
                                            break;

                default:                    Logger("\nNo more Task to do");
                                            break;
            }
        }
}

bool TaskManager::CreateAsyncThread()
{
    //Create a new Thread and init the process
    unsigned long threadID = 0;
    
    Thread* threadHandle = XCNEW(Thread); 
    threadHandle->CreateThread((runFunction)RunProcess, m_taskQueue.back());
    threadHandle->SetThreadPriority(m_taskQueue.back()->GetTaskPriority());
    threadHandle->Run();

    m_taskQueue.back()->SetThreadHandle(threadHandle);
    m_taskQueue.back()->SetThreadId((long)threadHandle);
    m_taskQueue.back()->SetInstanceId(m_taskQueue.size() - 1);

    Logger("Thread Created with Instance Id : %d", threadID);

    return true;
}

void TaskManager::Destroy()
{
    UnregisterAllTasks();
}