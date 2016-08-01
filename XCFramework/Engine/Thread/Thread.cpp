/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "Engine/Thread/Thread.h"

bool Thread::CreateThread(runFunction asyncFunction, void* params)
{
    //Create a new Thread and init the process
    m_threadHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)asyncFunction, params, CREATE_SUSPENDED, &m_threadId);
    Logger("Thread Created : in suspended mode. Call run!!");

    return true;
}

Thread::Thread(void)
{
}

Thread::~Thread(void)
{
    if (m_threadHandle)
    {
        CloseHandle(m_threadHandle);
    }
}

void Thread::Run()
{
    ResumeThread(m_threadHandle);
}

void Thread::Join()
{
    WaitForSingleObject(m_threadHandle, INFINITE);
}

void Thread::Destroy()
{
    CloseHandle(m_threadHandle);
}

void Thread::SetThreadPriority(u32 priority)
{
}

void Thread::SetThreadIdentifier(std::string threadName)
{
    SetThreadName(m_threadId, threadName.c_str());
}