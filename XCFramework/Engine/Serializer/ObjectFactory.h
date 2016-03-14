/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IBase.h"

class ObjectFactory
{
public:
    ObjectFactory() {}
    virtual ~ObjectFactory();

    typedef IBase* (*ptrToCreateObj)(void);

    template<class T>
    void RegisterObject(const std::string id)
    {
        ClassTypeInfo info;
        info.CreateObject = &(T::createObjectStatic);

        m_registeredObjects[id] = info;
    }

    IBase* CreateObject(const std::string id)
    {
        if (m_registeredObjects.find(id) != m_registeredObjects.end())
        {
            ClassTypeInfo info = m_registeredObjects[id];
            return info.CreateObject();
        }

        Logger("[OBJECT FACTORY] Could not serialize object with id %s", id.c_str());
        XCASSERT(false);

        return nullptr;
    }

    virtual void InitFactory() = 0;
    virtual void DestroyFactory() = 0;

private:
    
    class ClassTypeInfo
    {
    public:
        ptrToCreateObj CreateObject;
    };

    std::map<std::string, ClassTypeInfo>      m_registeredObjects;
};