/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "XCSyncEvent.h"

XCSyncEvent::XCSyncEvent(void)
{
}

XCSyncEvent::~XCSyncEvent(void)
{
}

i32 XCSyncEvent::CreateEvent()
{
    m_eventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    return true;
}

i32 XCSyncEvent::SignalEvent(i32 pattern)
{
    return SetEvent(m_eventHandle);
}

i32 XCSyncEvent::WaitEvent(i32 pattern)
{
    WaitForSingleObject(m_eventHandle, INFINITE);
    return true;
}

i32 XCSyncEvent::DestroyEvent()
{
    CloseHandle(m_eventHandle);
    return true;
}