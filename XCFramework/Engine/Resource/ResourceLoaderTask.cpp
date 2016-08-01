/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "ResourceLoaderTask.h"

ResourceLoaderTask::ResourceLoaderTask()
    : AsyncTask("ResourceLoaderTask")
{
}

ResourceLoaderTask::~ResourceLoaderTask(void)
{
}

void ResourceLoaderTask::Init()
{
    AsyncTask::Init();
    m_cs.Create(true, "ResourceLoaderTask");
}

void ResourceLoaderTask::Run()
{
    AsyncTask::Run();

    ResourceRequestInfo resRequest;
    memset(&resRequest, 0, sizeof(ResourceRequestInfo));

    m_cs.Enter();

    if (m_resourceLoadLists.size() > 0)
    {
        resRequest = m_resourceLoadLists.front();

        //Remove request
        m_resourceLoadLists.erase(m_resourceLoadLists.begin());
    }

    m_cs.Exit();

    if (resRequest.m_resourceHandle.IsValid())
    {
        switch (resRequest.m_requestType)
        {
        case ResourceRequestType_Load:

            XCASSERT(!resRequest.m_resourceHandle.m_Resource->IsLoaded());

            if (resRequest.m_resourceHandle.m_serializerBuffer != nullptr && !resRequest.m_resourceHandle.m_Resource->IsLoaded())
            {
                resRequest.m_resourceHandle.m_Resource->Load(resRequest.m_resourceHandle.m_serializerBuffer);
            }
            break;

        case ResourceRequestType_Unload:

            XCASSERT(resRequest.m_resourceHandle.m_Resource->IsLoaded());

            if (resRequest.m_resourceHandle.m_Resource)
            {
                resRequest.m_resourceHandle.m_Resource->Unload();
            }
            break;

        default:
            XCASSERT(false);
            break;
        }
    }
}

void ResourceLoaderTask::Destroy()
{
    AsyncTask::Destroy();
}

void ResourceLoaderTask::AddRequest(ResourceRequestInfo& requestInfo)
{
    m_cs.Enter();
    m_resourceLoadLists.push_back(requestInfo);
    m_cs.Exit();
}
