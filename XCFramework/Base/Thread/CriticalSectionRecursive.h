/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

typedef std::recursive_mutex    CSRecursive;

class CriticalSectionRecursive
{
public:
    CriticalSectionRecursive(void);
    ~CriticalSectionRecursive(void);

    void                        Create(bool isOwner = false, std::string mutexName = "RecurrsiveMutex");
    void                        Enter(i32 timeOut = INFINITE);
    void                        Exit();
    CSRecursive&               GetRawMutex() { return m_mutex; }
    void                        Release();

private:
    CSRecursive                m_mutex;
};