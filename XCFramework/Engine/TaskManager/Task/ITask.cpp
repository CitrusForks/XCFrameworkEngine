/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "ITask.h"


ITask::ITask(std::string taskName)
    : m_isAsync(false)
    , m_eCurrentState(TASKSTATE_INVALID)
    , m_InstanceId(TASKSTATE_INVALID)
    , m_threadHandle(nullptr)
    , m_threadID(0)
    , m_taskPriority(THREAD_PRIORITY_NORMAL)
    , m_taskName(taskName)
{
}

ITask::~ITask(void)
{
    XCDELETE(m_threadHandle);
}

void ITask::Destroy()
{
    m_taskPromise.set_value(1);
}