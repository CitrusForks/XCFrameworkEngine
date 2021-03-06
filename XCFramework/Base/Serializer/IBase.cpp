/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "BasePrecompiledHeader.h"
#include "IBase.h"

IBase::IBase()
{
    m_baseObjectId = -1;
}

IBase::~IBase()
{
}

void IBase::SetBaseObjectId(i32 baseId)
{
    m_baseObjectId = baseId;
}