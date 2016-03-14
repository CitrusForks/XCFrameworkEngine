/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "Engine/Thread/CriticalSection.h"

CriticalSection::CriticalSection(void)
{
}

CriticalSection::~CriticalSection(void)
{
#if !USE_STD_MUTEX
    if (m_mutex)
    {
        Release();
    }
#endif
}

void CriticalSection::Create(bool isOwner, std::string mutexName)
{
#if !USE_STD_MUTEX
    m_mutex = CreateMutex(nullptr, isOwner, mutexName.c_str());
    _ASSERT(m_mutex);
#endif
}

void CriticalSection::Enter(int timeOut)
{
#if USE_STD_MUTEX
    m_mutex.lock();
#else
    WaitForSingleObject(m_mutex, timeOut);
#endif
}

void CriticalSection::Exit()
{
#if USE_STD_MUTEX
    m_mutex.unlock();
#else
    _ASSERT(ReleaseMutex(m_mutex));
#endif
}

void CriticalSection::Release()
{
#if !USE_STD_MUTEX
    CloseHandle(m_mutex);
#endif
}