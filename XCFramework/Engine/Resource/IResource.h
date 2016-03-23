/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/IBase.h"
#include "Engine/Resource/ResourceTypes.h"
#include "Assets/Packages/PackageConsts.h"
#include "Engine/Graphics/RenderContext.h"

class IResource : public IBase
{
public:
    IResource();
    virtual ~IResource();

    virtual void            Init(int id, std::string userFriendlyName, bool loaded = false);

    //Dynamic loading/spawning or legacy data structure
    virtual void            Load(std::string resourcePath);

    //Flat buffer serialization
    virtual void            Load(const void* fbBuffer) 
    {
        m_loaded = true;
    }

    virtual void            Update(float dt) {}
    virtual void            Draw() {}
    virtual void            Draw(RenderContext& context) {}

    virtual void            Destroy();

    void WaitResourceUpdate()
    {
        while (!m_resourceUpdated);
    }

    virtual void RenderContextCallback(RenderContext& renderContext)
    {
        m_resourceUpdated = true;
    }

    int                     getResourecId()                     { return m_resourceId; }
    EResourceType           getResourceType()                   { return m_resourceType;  }
    
    std::string             getUserFriendlyName()               { return m_userFriendlyName;  }
    std::string             getResourcePath()                   { return m_resourcePath; }

    int                     isLoaded()                          { return m_loaded; }
    
protected:
    EResourceType           m_resourceType;
    int                     m_resourceId;
    bool                    m_loaded;

    std::string             m_resourcePath;
    std::string             m_userFriendlyName;

    std::atomic<bool>       m_resourceUpdated;
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