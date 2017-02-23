/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

class IPhysicsBoundVolume
{
public:
    IPhysicsBoundVolume(PhysicsBoundType boundType)
        : m_boundType(boundType)
    {}
    virtual ~IPhysicsBoundVolume() {}

    virtual void     Update(float dt) {};
    virtual void     Transform(const XCVec4& translate, const XCVec4& orientation) {};
    
#if defined(DEBUG_PHYSICS_OBB)
    virtual void     GetOBBInfo(std::vector<PhysicsPlayground::OBBInfo>& outInfo) const {}
#endif

    PhysicsBoundType GetBoundType() const { return m_boundType; }

    template<class T>
    T*               GetTyped() { return static_cast<T*>(this); }
private:
    PhysicsBoundType m_boundType;
};