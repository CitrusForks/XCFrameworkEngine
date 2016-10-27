/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "TPCChaseableActor.h"

class PlayableCharacterActor : public TPCChaseableActor
{
public:
    PlayableCharacterActor(void);
    virtual ~PlayableCharacterActor(void);

    virtual void                    setControl(bool isControllable)         { m_isCurrentControlled = isControllable; }
    bool                            getIsControlled()                       { return m_isCurrentControlled; }

private:
    //This specifies whether it is controlled by the user inputs, set it true from world may be whenever this object has focus, also make sure other all are disabled
    //We want only one PC object to be controlled.
    bool                            m_isCurrentControlled;
};