/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "ResourceManager.h"

class LoadPackageFileFBTask : public AsyncTask
{
public:
    LoadPackageFileFBTask(std::string packageData);
    virtual ~LoadPackageFileFBTask();

    virtual void            Init();
    virtual void            Run();
    virtual void            Destroy();

protected:

    template<class Type>
    void LoadBuffer(Type buffer, ResourceManager& resMgr, std::string resourceTypeName);

private:
    std::string             m_packageData;
};


template<class Type>
void LoadPackageFileFBTask::LoadBuffer(Type buffer, ResourceManager& resMgr, std::string resourceTypeName)
{
    if (buffer)
    {
        for (auto it = buffer->begin(); it != buffer->end(); ++it)
        {
            IResource* resource = resMgr.GetResourceFactory().createResource(resourceTypeName);

            if (resource)
            {
                resource->Load((void*) *it);
                resMgr.AddResource(resource);
            }
        }
    }
}