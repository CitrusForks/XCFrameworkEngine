/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "ResourceManager.h"
#include "Engine/System/SystemLocator.h"
#include "Engine/Utils/FileIO.h"

#include "LoadPackageFileTask.h"
#include "LoadPackageFileFBTask.h"

ResourceManager::ResourceManager()
    : m_packageLoaded(false)
{
    m_PackagePoolTaskThreadId.resize(1);
}

ResourceManager::~ResourceManager(void)
{
    delete(m_resourceFactory);
}

void ResourceManager::Init(XC_Graphics& graphicsSystem, TaskManager& taskManger)
{
    HMODULE module = LoadLibrary("assimp.dll");
    XCASSERT(module != nullptr);

    m_XCGraphicsSystem = &graphicsSystem;
    m_taskManager = &taskManger;
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<ResourceFactory>("ResourceFactory");
    
    m_resourceFactory = (ResourceFactory*) &container.CreateNewSystem("ResourceFactory");
    m_resourceFactory->InitFactory();
}

void ResourceManager::Update()
{
    //TODO : Hot reloading of resources on the fly
    if (m_futurePackageLoaded._Is_ready() /*&& m_futurePackageLoaded.wait_for(std::chrono::seconds(1)) == std::future_status::ready */)
    {
        if(m_futurePackageLoaded.valid() && m_futurePackageLoaded.get() > 0)
        {
            m_packageLoaded = true;
        }
    }
}

template<class ResourceType, class ResourceParam>
void ResourceManager::LoadResource(ResourceParam param)
{
    //RESOURCE_FACTORY->createResource()
}

IResource* ResourceManager::GetResource(const char* userFriendlyName)
{
    auto res = m_ResourcePool.find(userFriendlyName);
    if (res != m_ResourcePool.end())
    {
        return res->second;
    }

    XCASSERT(false);
    return nullptr;
}

int ResourceManager::LoadResourcesFromPackage(char* filePath)
{
    //Read the file and create resources and load contents based on it
    //TODO : Creating FILE API and then reading it from there
    m_loadPackageTask = new LoadPackageFileTask(filePath);
    m_futurePackageLoaded = m_taskManager->RegisterTask(m_loadPackageTask);

    return 0;
}

int ResourceManager::LoadResourcesFromPackageFB(const char* dataPath)
{
    m_loadPackageTask = new LoadPackageFileFBTask(dataPath);
    m_futurePackageLoaded = m_taskManager->RegisterTask(m_loadPackageTask);

    return 0;
}

bool ResourceManager::IsPackageLoaded()
{
    return m_packageLoaded;
}

void ResourceManager::AddResource(IResource* resource)
{
    m_ResourcePool[resource->getUserFriendlyName()] = resource;
}

void ResourceManager::Destroy()
{
    for (auto& resource : m_ResourcePool)
    {
        resource.second->Destroy();
    }

    m_ResourcePool.clear();

    m_resourceFactory->DestroyFactory();

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("ResourceFactory");
}

const char* ResourceManager::GetResourceNameAtIndex(int index)
{
    int count = 0;
    for (auto& resource : m_ResourcePool)
    {
        if (count++ == index)
        {
            return resource.first.c_str();
        }
    }

    return nullptr;
}

#if defined(EDITOR)
void ResourceManager::GetResourceList(ResourceInfo* resourcePtr)
{
    int index = 0;
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

extern "C" __declspec(dllexport) int GetNoOfResources()
{
    return RESOURCE_MANAGER->GetNoOfResources();
}

extern "C" __declspec(dllexport) void GetResourceList(ResourceInfo* ptrToList)
{
    RESOURCE_MANAGER->GetResourceList(ptrToList);
}

extern "C" __declspec(dllexport) const char* GetResourceAt(int index)
{
    return RESOURCE_MANAGER->GetResourceNameAtIndex(index);
}

#endif