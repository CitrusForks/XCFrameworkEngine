/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "ITask.h"


ITask::ITask()
{
    m_isAsync = false;
    m_taskPriority = THREAD_PRIORITY_NORMAL;
}


ITask::~ITask(void)
{
    if (m_threadHandle)
    {
        XCDELETE(m_threadHandle);
    }
}

void ITask::SetState(ETaskState _state)
{
    std::unique_lock<std::mutex> m(m_stateLock);
    m_eCurrentState = _state;
    m.unlock();
}

void ITask::Destroy()
{
    m_taskPromise.set_value(1);
}