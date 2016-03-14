/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/ObjectFactory.h"
#include "IResource.h"
#include "ResourceTypes.h"

class ResourceFactory : public ObjectFactory, public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(ResourceFactory)

    ResourceFactory();
    virtual ~ResourceFactory();

    void                            InitFactory();

    IResource*                      createResource(std::string classKey, std::string userFriendlyName);
    IResource*                      createResource(std::string classKey);

    void                            loadResource(FILE* packageFile, IResource* const resource);
    void                            loadResource(const void* buffer, IResource* const resource);

    void                            DestroyFactory();

private:
    int                             m_resourceCount;
};