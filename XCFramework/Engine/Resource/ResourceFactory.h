/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Serializer/ObjectFactory.h"

#include "IResource.h"
#include "ResourceTypes.h"

class ResourceFactory : ObjectFactory, public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(ResourceFactory)

    ResourceFactory();
    ~ResourceFactory();

    void                            InitFactory()    override final;
    void                            DestroyFactory() override final;

    IResource*                      CreateResource(std::string classKey, std::string userFriendlyName);
    IResource*                      CreateResource(std::string classKey);

private:
    i32                             m_resourceCount;
};