/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "IResource.h"

IResource::IResource()
    : m_resourceType(RESOURCETYPE_MAX)
    , m_resourceId(-1)
    , m_resourceState(ResourceState_UnLoaded)
    , m_userFriendlyName("")
    , m_resourcePath("")
    , m_resourceUpdated(false)
{
}

void IResource::Init(i32 id, std::string userFriendlyName)
{
    m_resourceId = id;
    m_userFriendlyName = userFriendlyName;
}

IResource::~IResource()
{
}

void IResource::Load(std::string resourcePath)
{
    m_resourcePath = resourcePath;
    m_resourceState = ResourceState_Loading;
}

void IResource::Load(const void* fbBuffer)
{
    m_resourceState = ResourceState_Loading;
}

void IResource::Unload()
{
    m_resourceState = ResourceState_UnLoaded;
}

void IResource::UpdateState()
{
    if (!m_isResourceReady && m_resourceState == ResourceState_Loaded)
    {
        m_isResourceReady = true;
        Logger("[IResource] %s is ready and loaded\n", m_userFriendlyName.c_str());
    }
    else if (m_isResourceReady && m_resourceState == ResourceState_UnLoaded)
    {
        m_isResourceReady = false;
        Logger("[IResource] %s is unloaded\n", m_userFriendlyName.c_str());
    }
}

void IResource::WaitResourceUpdate()
{
    while (!m_resourceUpdated);
}

void IResource::RenderContextCallback(ID3DDeviceContext& renderContext)
{
    m_resourceUpdated = true;
}

void IResource::Destroy()
{
}