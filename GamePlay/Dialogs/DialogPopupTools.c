//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 10 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "State.h"
#include "StateMouseLook.h"

#include "Game.h"
#include "DialogFindTank.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogPopupTools;


//-----------------------------------------------------------------------------
// Name: DialogProcPopupTools()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcPopupTools(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_COMMAND :
        DialogHide(Dialog);

        if (Param2 == Dialog->Children[ 0 ])
            SetGameState(&g_StateMouseLook);

        else if (Param2 == Dialog->Children[ 1 ])
            DialogShow(DialogFindTank, 0);

        else if (Param2 == Dialog->Children[ 2 ])
            ScreenShot();

        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogPopupToolsCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPopupToolsCreate(void)
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        (3 * buttonH) + DIALOG_PADDINGx2;

    DialogPopupTools = DialogCreate("PopupTools", DIALOG_POPUPMENU,
                                    0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPopupTools, 0, 0);

    float buttonY = DIALOG_PADDING;

    DialogCreate("MouseLook", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTools, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Find Tank (F3)", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTools, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("ScreenShot (F8)", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTools, DialogProcButton, 0, 0);

    return 1;
}
