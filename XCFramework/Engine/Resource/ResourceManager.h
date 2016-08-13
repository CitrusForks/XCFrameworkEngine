/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Resource/IResource.h"
#include "Engine/Resource/ResourceFactory.h"
#include "Engine/Resource/ResourceLoaderTask.h"
#include "Engine/Resource/ResourceHandle.h"

class ResourceManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(ResourceManager)

    ResourceManager();
    ~ResourceManager(void);

    void                                   Init(TaskManager& taskManger);

    //To provide hot reload of resources, we need a update function
    void                                   Update();

    void                                   AddResource(ResourceHandle& resource);
    IResource*                             CreateResource(std::string resourceTypeName, std::string userFriendlyName);

    //Used to add resource from serialized flat buffer data, calls load.
    void                                   CreateAddResource(std::string resourceTypeName, std::string userFriendlyName, const void* startFBResourcePtr);

#if defined(LEGACY_LOADING)
    i32                                    LoadResourcesFromPackage(char* filePath);
#endif

    i32                                    LoadResourcesFromPackageFB(const char* dataPath);

    ResourceHandle&                        AcquireResource(const char* userFriendlyName);
    void                                   ReleaseResource(ResourceHandle* resHandle);

    ResourceFactory&                       GetResourceFactory() { return *m_resourceFactory; }
    i32                                    GetNoOfResources()   { return m_ResourcePool.size();  }
    bool                                   IsPackageLoaded();

#if defined(EDITOR)
    void                                   GetResourceList(ResourceInfo* recourcePtr);
    const char*                            GetResourceNameAtIndex(i32 index);
#endif
    
    void                                   Destroy();

private:
    
    std::map<std::string, ResourceHandle>  m_ResourcePool;
    std::vector<u64>                       m_PackagePoolTaskThreadId;
                                           
    ResourceFactory*                       m_resourceFactory;
    TaskManager*                           m_taskManager;
                                           
    bool                                   m_packageLoaded;

    ResourceLoaderTask*                    m_resourceLoaderTask;
};