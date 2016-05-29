/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "EventTypes.h"

class IEvent
{
public:
    IEvent(EventType type)
        : m_eventType(type)
    {}

    virtual ~IEvent();

    EventType    GetEventType()                 { return m_eventType; }
    void         SetEventType(EventType type)   { m_eventType = type; }

private:
    EventType    m_eventType;
};

