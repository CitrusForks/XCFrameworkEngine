/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Engine/Event/IEvent.h"
#include "Engine/Event/IEventListener.h"

class EventBroadcaster : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(EventBroadcaster)

    EventBroadcaster();
    ~EventBroadcaster();

    void                Update();
    void                BroadcastEvent(IEvent* evt);
    void                AddListener(IEventListener* listener);
    void                RemoveListener(IEventListener* listener);

private:
    std::vector<IEventListener*>     m_eventListeners;
};

