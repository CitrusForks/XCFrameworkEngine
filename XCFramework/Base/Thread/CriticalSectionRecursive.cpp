/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "BasePrecompiledHeader.h"
#include "Thread/CriticalSectionRecursive.h"

CriticalSectionRecursive::CriticalSectionRecursive(void)
{
}

CriticalSectionRecursive::~CriticalSectionRecursive(void)
{
}

void CriticalSectionRecursive::Create(bool isOwner, std::string mutexName)
{
}

void CriticalSectionRecursive::Enter(i32 timeOut)
{
    m_mutex.lock();
}

void CriticalSectionRecursive::Exit()
{
    m_mutex.unlock();
}

void CriticalSectionRecursive::Release()
{
}