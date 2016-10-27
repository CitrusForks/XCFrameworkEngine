/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Assets/Packages/Schema/SceneNodeTypes_generated.h"

class SceneNode
{
public:
    explicit SceneNode(SceneNodeType type)
        : m_type(type)
    {
    }
    virtual ~SceneNode() {}

    SceneNodeType      GetNodeType() const { return m_type; }

    template<class T>
    T*                 GetTypedInstance()  { return static_cast<T*>(this); }

private:
    SceneNodeType      m_type;
};