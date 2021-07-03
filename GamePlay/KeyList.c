//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 1 July 2002
//
//========================================================================================


#include "stdafx.h"
#include "KeyList.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

KeyList_t g_MouseLookKeys =
{
    8,
    {
        { "E", "Forward" },
        { "D", "Backward" },
        { "S", "Left" },
        { "F", "Right" },
        { "Space", "Up" },
        { "Z", "Down" },
        { "Mouse", "Look Around" },
        { "Enter/Esc", "Exit MouseLook" }
    }
};

KeyList_t g_MouseLookWaitKeys =
{
    9,
    {
        { "E", "Forward" },
        { "D", "Backward" },
        { "S", "Left" },
        { "F", "Right" },
        { "Space", "Up" },
        { "Z", "Down" },
        { "Mouse", "Look Around" },
        { "Esc", "Follow Shot" },
        { "F10", "End Game" },
    }
};

KeyList_t g_WaitForShotKeys =
{
    2,
    {
        { "Esc", "MouseLook" },
        { "F9", "End Game" },
    }
};

KeyList_t g_GetShotInfoKeys =
{
    9,
    {
        { "Arrows", "Aim barrell" },
        { "Mouse", "Aim barrell" },
        { "PgUp", "Increase Power" },
        { "PgDn", "Decrease Power" },
        { "Shift", "Slow speed" },
        { "Ctrl", "Slower speed" },
        { "Enter/LMB", "Shoot" },
        { "F1-F5", "Camera View" },
        { "Esc", "Cancel Shot" }
    }
};

KeyList_t g_TankMoveKeys =
{
    8,
    {
        { "Left/Right", "Steer Tank" },
        { "Mouse", "Steer Tank" },
        { "Up", "Move Forward" },
        { "Down", "Move Backward" },
        { "Shift", "Slow speed" },
        { "Ctrl", "Slower speed" },
        { "Enter/Esc", "Finish" },
        { "F1-F5", "Camera View" }
    }
};
