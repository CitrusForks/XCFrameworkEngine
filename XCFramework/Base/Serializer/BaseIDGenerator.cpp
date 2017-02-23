/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "BasePrecompiledHeader.h"

#include "BaseIDGenerator.h"

BaseIDGenerator::BaseIDGenerator()
    : m_baseObjectIDCounter(0)
{
    m_criticalSection.Create();
}

BaseIDGenerator::~BaseIDGenerator()
{
    m_criticalSection.Exit();
}

u32 BaseIDGenerator::GetNextBaseObjectId()
{
    m_criticalSection.Enter();
    u32 out = ++m_baseObjectIDCounter;
    m_criticalSection.Exit();

    return out;
}