/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/ResourceTypes.h"
#include "Assets/Packages/PackageConsts.h"

class RenderContext;

class IResource : public IBase
{
public:
    
    enum ResourceState
    {
        ResourceState_Loading,
        ResourceState_Loaded,

        ResourceState_UnLoading,
        ResourceState_UnLoaded
    };

    IResource();
    virtual ~IResource();

    virtual void            Init(int id, std::string userFriendlyName);

    //Dynamic loading/spawning or legacy data structure
    virtual void            Load(std::string resourcePath);

    //Flat buffer serialization
    virtual void            Load(const void* fbBuffer);
    virtual void            Unload();

    virtual void            UpdateState();
    virtual void            Update(float dt) {}
    virtual void            Draw() {}
    virtual void            Draw(RenderContext& context) {}

    virtual void            RenderContextCallback(RenderContext& renderContext);

    virtual void            Destroy();

    void                    WaitResourceUpdate();

    int                     GetResourecId()                                  { return m_resourceId; }
    EResourceType           GetResourceType()                                { return m_resourceType;  }
    
    std::string             SetUserFriendlyName(std::string usrFriendlyName) { m_userFriendlyName = usrFriendlyName; }
    std::string             GetUserFriendlyName()                            { return m_userFriendlyName;  }
    std::string             GetResourcePath()                                { return m_resourcePath; }
                                                                             
    int                     IsLoaded()                                       { return m_resourceState == ResourceState_Loaded; }

    ResourceState           GetResourceState()                               { return m_resourceState; }
    void                    SetResourceState(ResourceState state)            { m_resourceState = state; }

protected:
    EResourceType           m_resourceType;
    int                     m_resourceId;
    ResourceState           m_resourceState;

    std::string             m_resourcePath;
    std::string             m_userFriendlyName;

    std::atomic<bool>       m_resourceUpdated;
    bool                    m_isResourceReady;
};


#if defined(EDITOR)
struct ResourceInfo
{
    int                 m_resourecId;
    EResourceType       m_resourecType;
    char                m_userfriendlyName[50];
    char                m_resourcePath[50];
    bool                m_isLoaded;
};
#endif