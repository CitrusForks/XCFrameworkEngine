#pragma once

#include "World.h"

class WorldEditor : public World
{
public:
    DECLARE_OBJECT_CREATION(WorldEditor)

    WorldEditor()
    {
        m_canUpdate = true;
        EnablePhysics(false);
    }

    virtual ~WorldEditor(void);
    
    virtual void             Init(TaskManager& mgr);
    virtual void             Update(float dt);
    virtual void             Draw(XC_Graphics& graphicsSystem);
    virtual void             Destroy();
    
    void                     disableUpdate(bool canUpdate) { m_canUpdate = canUpdate; }

private:
    bool                    m_canUpdate;
};