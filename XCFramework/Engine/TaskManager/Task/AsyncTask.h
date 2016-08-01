/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "ITask.h"

class AsyncTask : public ITask
{
public:
    AsyncTask(std::string taskName = "");
    virtual ~AsyncTask(void);

    virtual void                        Init();
    virtual void                        Run();
    virtual void                        Destroy();

    //virtual void                      GetNotifyFunc();

protected:
};