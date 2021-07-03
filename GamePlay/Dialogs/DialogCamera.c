//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 9 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "DialogProcDefault.h"
#include "DialogProcLabel.h"
#include "DialogProcButton.h"

#include "View.h"
#include "State.h"

#include "StateGetShotInfo.h"
#include "StateMoveTank.h"

#include "LevelManager.h"
#include "PlayerManager.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogCamera;


//-----------------------------------------------------------------------------
// Name: DialogProcCameraMove()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcCameraMove(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
            SetFirstPersonView(GetCurrentPlayer(), -1);

        else if (Param2 == Dialog->Children[ 1 ])
            SetThirdPersonView(GetCurrentPlayer(), -1);

        else if (Param2 == Dialog->Children[ 2 ])
            SetChaseView(GetCurrentPlayer(), -1);

        else if (Param2 == Dialog->Children[ 3 ])
            SetBirdsEyeView(GetCurrentPlayer(), -1);

        else if (Param2 == Dialog->Children[ 4 ])
            SetMouseLookView(GetCurrentPlayer(), -1);

        SetGameState(&g_StateMoveTank);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogProcCameraShoot()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcCameraShoot(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
            SetFirstPersonView(-1, GetCurrentPlayer());

        else if (Param2 == Dialog->Children[ 1 ])
            SetThirdPersonView(-1, GetCurrentPlayer());

        else if (Param2 == Dialog->Children[ 2 ])
            SetChaseView(-1, GetCurrentPlayer());

        else if (Param2 == Dialog->Children[ 3 ])
            SetBirdsEyeView(-1, GetCurrentPlayer());

        else if (Param2 == Dialog->Children[ 4 ])
            SetMouseLookView(-1, GetCurrentPlayer());

        SetGameState(&g_StateGetShotInfo);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogCameraCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogCameraCreate(void)
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        (5 * buttonH) + DIALOG_PADDINGx2;

    DialogCamera = DialogCreate("Camera", DIALOG_POPUPMENU,
                                0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcCameraMove, 0, 0);

    float buttonY = DIALOG_PADDING;

    DialogCreate("First Person", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogCamera, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Third Person", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogCamera, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Chase", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogCamera, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Bird's Eye", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogCamera, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Mouse Look", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogCamera, DialogProcButton, 0, 0);

    return 1;
}
