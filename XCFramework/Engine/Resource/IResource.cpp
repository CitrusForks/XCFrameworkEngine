/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "IResource.h"


IResource::IResource()
    : m_resourceType(RESOURCETYPE_MAX)
    , m_resourceId(-1)
    , m_loaded(false)
    , m_userFriendlyName("")
    , m_resourcePath("")
    , m_resourceUpdated(false)
{
}

void IResource::Init(int id, std::string userFriendlyName, bool loaded /* =false*/)
{
    m_resourceId = id;
    m_loaded = loaded;
    m_userFriendlyName = userFriendlyName;
}

IResource::~IResource()
{
}

void IResource::Load(std::string resourcePath)
{
    m_resourcePath = resourcePath;
}

void IResource::Destroy()
{
    m_loaded = false;
}