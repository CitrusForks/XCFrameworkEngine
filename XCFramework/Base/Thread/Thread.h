/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

typedef i32 (*runFunction)(void* param);

class Thread
{
public:
    Thread(void);
    ~Thread(void);

    bool          CreateThread(runFunction asyncFunction, void* params);
    void          Run();
    void          Join();
    void          Destroy();
    void          SetThreadPriority(u32 priority);
    void          SetThreadIdentifier(std::string threadName);

private:
    HANDLE        m_threadHandle;
    DWORD         m_threadId;
};