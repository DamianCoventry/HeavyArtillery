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
#include "StateFlyBy.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogPopupGame;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void CallbackEndGame(size_t RetValue);
static void CallbackExitGame(size_t RetValue);


//-----------------------------------------------------------------------------
// Name: DialogProcPopupGame()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcPopupGame(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_COMMAND :
        DialogHide(Dialog);
        //GamePushDialogState();

        if (Param2 == Dialog->Children[ 0 ])
            DialogMessageBox("Heavy Artillery", "Are you sure you want to end the game?", MB_YESNO, CallbackEndGame);
        else if (Param2 == Dialog->Children[ 1 ])
            DialogMessageBox("Heavy Artillery", "Do you really want to wuss out?", MB_YESNO, CallbackExitGame);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogPopupGameCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPopupGameCreate(void)
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        (2 * buttonH) + DIALOG_PADDINGx2;

    DialogPopupGame = DialogCreate("PopupGame", DIALOG_POPUPMENU,
                                   0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPopupGame, 0, 0);

    float buttonY = DIALOG_PADDING;

    DialogCreate("End Game", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupGame, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Exit App", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupGame, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: CallbackEndGame()
// Desc:
//-----------------------------------------------------------------------------

static void CallbackEndGame(size_t RetValue)
{
    if (IDYES == RetValue)
        SetGameState(&g_StateFlyBy);

    //GamePopDialogState();
}


//-----------------------------------------------------------------------------
// Name: CallbackExitGame()
// Desc:
//-----------------------------------------------------------------------------

static void CallbackExitGame(size_t RetValue)
{
    if (IDYES == RetValue)
        PostQuitMessage(0);

    //GamePopDialogState();
}
