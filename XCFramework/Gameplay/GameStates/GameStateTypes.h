/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//GameStates Enums
enum EGameState { 
    GAMESTATE_EDITORLOADING, 
    GAMESTATE_EDITORRUNNING, 
    GAMESTATE_INTRO, 
    GAMESTATE_MAINMENU, 
    GAMESTATE_NEWGAME, 
    GAMESTATE_LOADING, 
    GAMESTATE_RUNNING, 
    GAMESTATE_PAUSE, 
    GAMESTATE_END, 
    GAMESTATE_MAX };

enum EWhatToDoWithPreviousState { 
    STATE_STORE, 
    STATE_DESTROY, 
    STATE_NONE};