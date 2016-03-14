/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"

class LoadPackageFileFBTask : public AsyncTask
{
public:
    LoadPackageFileFBTask(std::string packageData);
    virtual ~LoadPackageFileFBTask();

    virtual void            Init();
    virtual void            Run();
    virtual void            Destroy();

private:
    std::string             m_packageData;
};