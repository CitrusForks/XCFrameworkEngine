/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class XCSyncEvent
{
public:
    XCSyncEvent(void);
    ~XCSyncEvent(void);

    i32     CreateEvent();
    i32     SignalEvent(i32 pattern);
    i32     WaitEvent(i32 pattern);
    i32     DestroyEvent();

private:
    EventHandle      m_eventHandle;
};