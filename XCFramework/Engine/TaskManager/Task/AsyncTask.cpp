/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "AsyncTask.h"


AsyncTask::AsyncTask(std::string taskName)
    : ITask(taskName)
{
    m_isAsync = true;
}


AsyncTask::~AsyncTask(void)
{
}


void AsyncTask::Init()
{
    SetState(TASKSTATE_INIT);
}

void AsyncTask::Run()
{
    SetState(TASKSTATE_RUNNING);
}

void AsyncTask::Destroy()
{
    SetState(TASKSTATE_DESTROY);
    ITask::Destroy();
}