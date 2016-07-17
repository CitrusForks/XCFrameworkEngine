/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class IBase
{
public:
    IBase();
    virtual ~IBase();

#define DECLARE_OBJECT_CREATION(classname) \
    static IBase* createObjectStatic() { return static_cast<IBase*>(XCNEW(classname)); }

#define DECLARE_SYSTEMOBJECT_CREATION(classname) \
    static IBase* createObjectStatic() { return static_cast<IBase*>(XCNEW(classname)); }

    virtual void                    Init(i32 baseId);
    i32                             GetBaseObjectId() { return m_baseObjectId; }

private:
    i32                             m_baseObjectId;
};