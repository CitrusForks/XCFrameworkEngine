/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "ResourceManager.h"
#include "Engine/Utils/FileIO.h"

#if defined(LEGACY_LOADING)
#include "LoadPackageFileTask.h"
#endif

#include "LoadPackageFileFBTask.h"

ResourceManager::ResourceManager()
    : m_packageLoaded(false)
{
    m_PackagePoolTaskThreadId.resize(1);
}

ResourceManager::~ResourceManager(void)
{
}

void ResourceManager::Init(TaskManager& taskManger)
{
    //TODO : This shouldn't be here.
    HMODULE module = LoadLibrary("assimp.dll");
    XCASSERT(module != nullptr);

    m_taskManager = &taskManger;

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<ResourceFactory>("ResourceFactory");
    
    m_resourceFactory = (ResourceFactory*) &container.CreateNewSystem("ResourceFactory");
    m_resourceFactory->InitFactory();

    //Start the ResourceLoader task
    m_resourceLoaderTask = XCNEW(ResourceLoaderTask)();
    m_taskManager->RegisterTask(m_resourceLoaderTask);

    ISystem::Init();
}

void ResourceManager::Update()
{
    for (auto& resHandle : m_ResourcePool)
    {
        IResource* resource = resHandle.second.m_Resource;

        if(resource->GetResourceState() == IResource::ResourceState_Loaded 
            && resource->GetResourceState() != IResource::ResourceState_UnLoading 
            && resHandle.second.m_refCount == 0)
        {
            //Unload it. Or stay unloaded
            ResourceLoaderTask::ResourceRequestInfo info = { ResourceLoaderTask::ResourceRequestType_Unload, resHandle.second };
            resHandle.second.m_Resource->SetResourceState(IResource::ResourceState_UnLoading);

            m_resourceLoaderTask->AddRequest(info);
        }
        else if (resource->GetResourceState() == IResource::ResourceState_UnLoaded 
            && resource->GetResourceState() != IResource::ResourceState_Loading 
            && resHandle.second.m_refCount > 0)
        {
            //load it
            ResourceLoaderTask::ResourceRequestInfo info = { ResourceLoaderTask::ResourceRequestType_Load, resHandle.second };
            resource->SetResourceState(IResource::ResourceState_Loading);

            m_resourceLoaderTask->AddRequest(info);
        }
        else if(resource->GetResourceState() == IResource::ResourceState_Loading 
            || resource->GetResourceState() == IResource::ResourceState_UnLoading)
        {
            //Update the resource, they might be waiting for other resources to be loaded.
            resource->UpdateState();
        }
    }
}

ResourceHandle& ResourceManager::AcquireResource(const char* userFriendlyName)
{
    auto res = std::find_if(m_ResourcePool.begin(), m_ResourcePool.end(), [userFriendlyName](const std::pair<std::string, ResourceHandle> handle) -> bool
    {
        return strcmp(userFriendlyName, handle.second.m_Resource->GetUserFriendlyName().c_str()) == 0;
    });

    if (res != m_ResourcePool.end())
    {
        res->second.m_refCount++;
        return res->second;
    }

    XCASSERT(false);
    return ResourceHandle();
}

void ResourceManager::ReleaseResource(ResourceHandle * resHandle)
{
    XCASSERT(resHandle);

    if (resHandle->m_refCount > 0)
    {
        resHandle->m_refCount--;
    }
    else
    {
        Logger("[Resource] Trying to release a resource which has a refcount == 0");
        XCASSERT(false);
    }
}

#if defined(LEGACY_LOADING)
i32 ResourceManager::LoadResourcesFromPackage(char* filePath)
{
    //Read the file and create resources and load contents based on it
    m_loadPackageTask = new LoadPackageFileTask(filePath);
    m_futurePackageLoaded = m_taskManager->RegisterTask(m_loadPackageTask);

    return true;
}
#endif

bool ResourceManager::IsPackageLoaded()
{
    return m_packageLoaded;
}

IResource* ResourceManager::CreateResource(std::string resourceTypeName, std::string userFriendlyName)
{
    return m_resourceFactory->CreateResource(resourceTypeName, userFriendlyName);
}

void ResourceManager::AddResource(ResourceHandle& resource)
{
    if (resource.m_Resource)
    {
        auto exists = std::find_if(m_ResourcePool.begin(), m_ResourcePool.end(), [&resource](std::pair<std::string, ResourceHandle> res) -> bool
        {
            return strcmp(res.first.c_str(), resource.m_Resource->GetUserFriendlyName().c_str()) == 0;
        });

        if (exists == m_ResourcePool.end())
        {
            m_ResourcePool[resource.m_Resource->GetUserFriendlyName()] = resource;
        }
        else
        {
            XCASSERT(false);
            Logger("[ResourceManager] Adding a resource with same userfriendly name %s. Is it intended? ", resource.m_Resource->GetUserFriendlyName().c_str());
        }
    }
}

void ResourceManager::CreateAddResource(std::string resourceTypeName, std::string userFriendlyName, const void* startFBResourcePtr)
{
    IResource* resource = CreateResource(resourceTypeName, userFriendlyName);
    
    ResourceHandle resHandle = {};
    resHandle.m_Resource = resource;
    resHandle.m_serializerBuffer = startFBResourcePtr;

    AddResource(resHandle);
}

void ResourceManager::Destroy()
{
    for (auto& resource : m_ResourcePool)
    {
        if (resource.second.m_refCount == 0)
        {
            resource.second.m_Resource->Destroy();
        }
        else
        {
            Logger("[Resource] Trying to destroy a resource with refcount > 0.");
            XCASSERT(false);
        }
    }

    m_resourceLoaderTask->Destroy();
    m_taskManager->UnregisterTask(m_resourceLoaderTask->GetThreadId());
    XCDELETE(m_resourceLoaderTask);

    m_ResourcePool.clear();

    m_resourceFactory->DestroyFactory();

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("ResourceFactory");
}

#if defined(EDITOR)
const char* ResourceManager::GetResourceNameAtIndex(i32 index)
{
    i32 count = 0;
    for (auto& resource : m_ResourcePool)
    {
        if (count++ == index)
        {
            return resource.first.c_str();
        }
    }

    return nullptr;
}

void ResourceManager::GetResourceList(ResourceInfo* resourcePtr)
{
    i32 index = 0;
    for (auto& resource : m_ResourcePool)
    {
        resourcePtr[index].m_resourecId = resource.second->getResourecId();
        resourcePtr[index].m_resourecType = resource.second->getResourceType();
        strcpy(resourcePtr[index].m_userfriendlyName, resource.second->getUserFriendlyName().c_str());
        strcpy(resourcePtr[index].m_resourcePath, resource.second->getResourcePath().c_str());
        resourcePtr[index].m_isLoaded = (bool) resource.second->isLoaded();
        index++;
    }
}

extern "C" __declspec(dllexport) i32 GetNoOfResources()
{
    return RESOURCE_MANAGER->GetNoOfResources();
}

extern "C" __declspec(dllexport) void GetResourceList(ResourceInfo* ptrToList)
{
    RESOURCE_MANAGER->GetResourceList(ptrToList);
}

extern "C" __declspec(dllexport) const char* GetResourceAt(i32 index)
{
    return RESOURCE_MANAGER->GetResourceNameAtIndex(index);
}

#endif