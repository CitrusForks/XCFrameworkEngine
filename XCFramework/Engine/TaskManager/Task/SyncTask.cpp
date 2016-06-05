/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "SyncTask.h"

SyncTask::SyncTask(void)
{
    m_isAsync = false;
}


SyncTask::~SyncTask(void)
{
}

void SyncTask::Init()
{
    SetState(TASKSTATE_RUNNING);
}

void SyncTask::Run()
{


    //Do the task assigned to this process
    Logger("Doing the task :");

    for(unsigned int i=0; i < 1000; i++)
        Logger("Sync Process %d", i);

    Logger("Done with the Task");

    //When Done Call Destroy
    SetState(TASKSTATE_DESTROY);
}

void SyncTask::Destroy()
{
    ITask::Destroy();
}
