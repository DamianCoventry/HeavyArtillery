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

#include "DialogGameToolbar.h"
#include "DialogPopupGame.h"
#include "DialogPopupView.h"
#include "DialogPopupTank.h"
#include "DialogPopupTools.h"
#include "DialogPopupHelp.h"

#include "DialogPlayOrder.h"
#include "DialogMyTank.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "Hardware.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogGameToolbar;

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcGameToolbar()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcGameToolbar(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    float X, Y;

    switch (Msg)
    {
    case WM_CREATE :
        Dialog->X = ((float)g_HWConfig.GfxWidth / 2.0f) - (Dialog->W / 2.0f);
        Dialog->Y = 8.0f;
        break;

    case WM_DISPLAYCHANGE :
        Dialog->X = ((float)g_HWConfig.GfxWidth / 2.0f) - (Dialog->W / 2.0f);
        Dialog->Y = 8.0f;
        return DIALOG_MSGHANDLED;

    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
        {
            DialogHideAll(Dialog);
            DialogShowCursor(1);

            if (g_PlayOrderVisible)
                DialogShow(DialogPlayOrder, 0);

            if (g_MyTankVisible)
                DialogShow(DialogMyTank, 0);

            DialogBringToTop(Dialog);
        }
        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        DialogClientToScreen(Dialog, &X, &Y);

        if (Param2 == Dialog->Children[ 0 ])
        {
            DialogTrackPopup(DialogPopupGame, X + Dialog->Children[ 0 ]->X,
                             Y + Dialog->Children[ 0 ]->Y + Dialog->Children[ 0 ]->H, 0);
        }
        else if (Param2 == Dialog->Children[ 1 ])
        {
            DialogTrackPopup(DialogPopupView, X + Dialog->Children[ 1 ]->X,
                             Y + Dialog->Children[ 1 ]->Y + Dialog->Children[ 1 ]->H, 0);
        }
        else if (Param2 == Dialog->Children[ 2 ])
        {
            DialogTrackPopup(DialogPopupTank, X + Dialog->Children[ 2 ]->X,
                             Y + Dialog->Children[ 2 ]->Y + Dialog->Children[ 2 ]->H, 0);
        }
        else if (Param2 == Dialog->Children[ 3 ])
        {
            DialogTrackPopup(DialogPopupTools, X + Dialog->Children[ 3 ]->X,
                             Y + Dialog->Children[ 3 ]->Y + Dialog->Children[ 3 ]->H, 0);
        }
        else if (Param2 == Dialog->Children[ 4 ])
        {
            DialogTrackPopup(DialogPopupHelp, X + Dialog->Children[ 4 ]->X,
                             Y + Dialog->Children[ 4 ]->Y + Dialog->Children[ 4 ]->H, 0);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogGameToolbarCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogGameToolbarCreate(void)
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDINGx2 + (5 * buttonW) + DIALOG_PADDINGx2;
    const float dialogH =
        TOOLDIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING;

    DialogGameToolbar = DialogCreate("Game Toolbar", DIALOG_MOVEABLE | DIALOG_TOOLBAR | DIALOG_TITLE,
                                     0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcGameToolbar, 0, 0);

    float buttonX = DIALOG_PADDINGx2;
    const float buttonY = TOOLDIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Game", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_CENTER,
                 buttonX, buttonY, buttonW, buttonH, DialogGameToolbar, DialogProcButton, 0, 0); buttonX += buttonW;

    DialogCreate("View", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_CENTER,
                 buttonX, buttonY, buttonW, buttonH, DialogGameToolbar, DialogProcButton, 0, 0); buttonX += buttonW;

    DialogCreate("Tank", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_CENTER,
                 buttonX, buttonY, buttonW, buttonH, DialogGameToolbar, DialogProcButton, 0, 0); buttonX += buttonW;

    DialogCreate("Tools", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_CENTER,
                 buttonX, buttonY, buttonW, buttonH, DialogGameToolbar, DialogProcButton, 0, 0); buttonX += buttonW;

    DialogCreate("Help", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_CENTER,
                 buttonX, buttonY, buttonW, buttonH, DialogGameToolbar, DialogProcButton, 0, 0);

    return 1;
}
