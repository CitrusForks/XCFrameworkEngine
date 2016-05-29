/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Event/IEvent.h"

 //World Events
class Event_World : public IEvent
{
public:
    Event_World(EventType type)
        : IEvent(type)
    {
    }
};