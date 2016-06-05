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

int XCSyncEvent::CreateEvent()
{
    m_eventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    return true;
}

int XCSyncEvent::SignalEvent(int pattern)
{
    return SetEvent(m_eventHandle);
}

int XCSyncEvent::WaitEvent(int pattern)
{
    WaitForSingleObject(m_eventHandle, INFINITE);
    return true;
}

int XCSyncEvent::DestroyEvent()
{
    CloseHandle(m_eventHandle);
    return true;
}