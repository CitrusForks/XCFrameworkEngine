/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Resource/IResource.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Resource/ResourceFactory.h"

class ResourceManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(ResourceManager)

    ResourceManager();
    ~ResourceManager(void);

    void                                Init(XC_Graphics& grapAhicsSystem, TaskManager& taskManger);

    template<class ResourceType, class ResourceParam>
    void                                LoadResource(ResourceParam param);

    int                                 LoadResourcesFromPackage(char* filePath);
    int                                 LoadResourcesFromPackageFB(const char* dataPath);

    IResource*                          GetResource(const char* userFriendlyName);

    void                                AddResource(IResource* resource);
    bool                                IsPackageLoaded();

    //To provide hot reload of resources, we need a update function
    void                                Update();

    int                                 GetNoOfResources()                              { return m_ResourcePool.size();  }

#if defined(EDITOR)
    void                                GetResourceList(ResourceInfo* recourcePtr);
#endif

    const char*                         GetResourceNameAtIndex(int index);
    
    void                                Destroy();

    ResourceFactory&                    GetResourceFactory() { return *m_resourceFactory; }

    XC_Graphics&                        GetXCGraphicsDx11System() { return *m_XCGraphicsSystem; }

private:
    
    std::map<std::string, IResource*>                         m_ResourcePool;
    std::vector<unsigned long>                                m_PackagePoolTaskThreadId;

    ResourceFactory*                                          m_resourceFactory;
    TaskManager*                                              m_taskManager;
    XC_Graphics*                                              m_XCGraphicsSystem;

    ITask*                                                    m_loadPackageTask;
    bool                                                      m_packageLoaded;
    std::future<int>                                          m_futurePackageLoaded;
};