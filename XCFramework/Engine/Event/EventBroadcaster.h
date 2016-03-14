/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Event/IEvent.h"
#include "Engine/Event/IEventListener.h"

class EventBroadcaster
{
public:
    ~EventBroadcaster();

    static EventBroadcaster*    GetInstance();
    virtual void                BroadcastEvent(IEvent* evt);
    virtual void                AddListener(IEventListener* listener);
    virtual void                RemoveListener(IEventListener* listener);

private:
    EventBroadcaster() { }

    static EventBroadcaster*         m_psEventBroadcaster;
    std::vector<IEventListener*>     m_eventListeners;
};

#define EVENTMANAGER            EventBroadcaster::GetInstance()

