/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define USE_STD_MUTEX 1

#if defined USE_STD_MUTEX
typedef std::mutex          CS;
#else
typedef HANDLE              CS;
#endif

class CriticalSection
{
public:
    CriticalSection(void);
    ~CriticalSection(void);

    void                        Create(bool isOwner = false, std::string mutexName = "Mutex");

    void                        Enter(i32 timeOut = INFINITE);
    void                        Exit();
    CS&                         GetRawMutex() { return m_mutex; }
    void                        Release();

private:
    CS                          m_mutex;
};