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

#include "DialogWeapon.h"
#include "DialogProperties.h"
#include "DialogCamera.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "GameItems.h"
#include "PlayerManager.h"
#include "LevelManager.h"

#include "State.h"
#include "StateEndTurn.h"
#include "StateWaitForShot.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogPopupTank;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void CallbackSuicide(size_t RetValue);


//-----------------------------------------------------------------------------
// Name: DialogProcPopupTank()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcPopupTank(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    float X, Y;
    Dialog_t* Child;

    switch (Msg)
    {
    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
        {
            DialogEnable(Dialog->Children[ 1 ], -1 != BackpackGetAbsoluteIndex(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEITEM, (void*)&ItemFuel));
            DialogEnable(Dialog->Children[ 3 ], 0 < BackpackGetItemCount(GetPlayerBackpack(GetCurrentPlayer())));
        }
        return DIALOG_MSGHANDLED;

    case WM_LBUTTONDOWN :
    case WM_RBUTTONDOWN :
    case WM_MBUTTONDOWN :
    {
        // close sub menu
        DialogHide(DialogCamera);

        Child = DialogControlAtCoords(Dialog, CursorX, CursorY);
        if (Child && (Child->Flags & DIALOG_VISIBLE) && (Child->Flags & DIALOG_ENABLED))
        {
            DialogSetFocus(Dialog, Child);
            return DialogSend(Child, Msg, Param1, Param2);
        }
        else if (!(Dialog->Flags & DIALOG_CHILD))
            DialogSetFocus(Dialog, nullptr);

        DialogCapture = Dialog;

        return DIALOG_MSGHANDLED;
    }
    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])          // shoot
        {
            DialogClientToScreen(Dialog->Children[ 0 ], &X, &Y);
            DialogCamera->DialogProc = DialogProcCameraShoot;
            DialogTrackPopup(DialogCamera, X + Dialog->Children[ 0 ]->W, Y, 0);
        }
        else if (Param2 == Dialog->Children[ 1 ])     // move
        {
            DialogClientToScreen(Dialog->Children[ 1 ], &X, &Y);
            DialogCamera->DialogProc = DialogProcCameraMove;
            DialogTrackPopup(DialogCamera, X + Dialog->Children[ 1 ]->W, Y, 0);
        }
        else if (Param2 == Dialog->Children[ 2 ])     // weapons
        {
            DialogHide(Dialog);
            DialogShow(DialogWeapon, 0);
        }
        else if (Param2 == Dialog->Children[ 3 ])     // items
        {
            DialogHide(Dialog);
            DialogShow(DialogProperties, 0);
        }
        else if (Param2 == Dialog->Children[ 4 ])     // suicide
        {
            DialogHide(Dialog);
            //GamePushDialogState();
            DialogMessageBox("Heavy Artillery", "Are you sure you want to commit suicide?", MB_YESNO, CallbackSuicide);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogPopupTankCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPopupTankCreate(void)
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        (5 * buttonH) + DIALOG_PADDINGx2;

    DialogPopupTank = DialogCreate("PopupTank", DIALOG_POPUPMENU,
                                   0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPopupTank, 0, 0);

    float buttonY = DIALOG_PADDING;

    DialogCreate("Shoot", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_HASSUBMENU,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTank, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Move", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_HASSUBMENU,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTank, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Weapons", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTank, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Items", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTank, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Suicide", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupTank, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: CallbackSuicide()
// Desc:
//-----------------------------------------------------------------------------

static void CallbackSuicide(size_t RetValue)
{
    if (RetValue == IDYES)
    {
        auto PlayerHistory = GetPlayerHistory(GetCurrentPlayer());
        PlayerHistory->HasHitThisTurn = 1;  // Hack to stop the following explosion counting as a 'missed shot'

        ExplodePlayer(GetCurrentPlayer());

        LookAtCurrentPlayer();

        SetGameState(&g_StateWaitForShot);
    }
}
