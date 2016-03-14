/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/Resource/ResourceManager.h"

class LoadPackageFileTask : public AsyncTask
{
public:
    LoadPackageFileTask(std::string packageFileName);
    virtual ~LoadPackageFileTask();

    virtual void            Init();
    virtual void            Run();
    virtual void            Destroy();

private:
    std::string             m_packageFileName;
    ResourceManager*        m_resourceManager;
};