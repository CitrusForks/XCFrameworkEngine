/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/TaskManager/TaskManager.h"

#include "Engine/Resource/IResource.h"
#include "Engine/Resource/ResourceHandle.h"

#include "Engine/Thread/CriticalSection.h"

class ResourceLoaderTask : public AsyncTask
{
public:
    enum ResourceRequestType
    {
        ResourceRequestType_Load,
        ResourceRequestType_Unload,
    };

    struct ResourceRequestInfo
    {
        ResourceRequestType m_requestType;
        ResourceHandle      m_resourceHandle;
    };

    ResourceLoaderTask();
    ~ResourceLoaderTask(void);

    virtual void            Init();
    virtual void            Run();
    virtual void            Destroy();

    void                    AddRequest(ResourceRequestInfo& requestInfo);

private:
    CriticalSection                  m_cs;
    std::vector<ResourceRequestInfo> m_resourceLoadLists;
};