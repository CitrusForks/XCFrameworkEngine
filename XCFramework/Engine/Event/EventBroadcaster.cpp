/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "EventBroadcaster.h"

EventBroadcaster::EventBroadcaster()
{
}

EventBroadcaster::~EventBroadcaster()
{
}

void EventBroadcaster::AddListener(IEventListener* listener)
{
    if (listener)
    {
        m_eventListeners.push_back(listener);
    }
}

void EventBroadcaster::RemoveListener(IEventListener* listener)
{
    if (listener)
    {
        for (std::vector<IEventListener*>::iterator it = m_eventListeners.begin();
            it != m_eventListeners.end(); ++it)
        {
            if (*it == listener)
            {
                m_eventListeners.erase(it);
                break;
            }
        }
    }
}

void EventBroadcaster::Update()
{
}

void EventBroadcaster::BroadcastEvent(IEvent* evt)
{
    for (std::vector<IEventListener*>::iterator it = m_eventListeners.begin();
        it < m_eventListeners.end(); it++)
    {
        if (*it)
        {
            (*it)->OnEvent(evt);
        }
    }
}
