/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "ITask.h"

class SyncTask : public ITask
{
public:
    SyncTask(void);
    virtual ~SyncTask(void);

    virtual void                Init();
    virtual void                Run();
    virtual void                Destroy();
};