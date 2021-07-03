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

#include "DialogPlayOrder.h"
#include "DialogMyTank.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogPopupView;


//-----------------------------------------------------------------------------
// Name: DialogProcPopupView()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcPopupView(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
    {
        if (WA_ACTIVE == (size_t)Param1)
        {
            std::string Temp;
            if (DialogPlayOrder->Flags & DIALOG_VISIBLE)
            {
                Temp = "Hide PlayOrder";
                DialogSend(Dialog->Children[0], WM_SETTEXT, (void*)&Temp, 0);
            }
            else
            {
                Temp = "Show PlayOrder";
                DialogSend(Dialog->Children[0], WM_SETTEXT, (void*)&Temp, 0);
            }

            if (DialogMyTank->Flags & DIALOG_VISIBLE)
            {
                Temp = "Hide MyTank";
                DialogSend(Dialog->Children[1], WM_SETTEXT, (void*)&Temp, 0);
            }
            else
            {
                Temp = "Show MyTank";
                DialogSend(Dialog->Children[1], WM_SETTEXT, (void*)&Temp, 0);
            }
        }
        return DIALOG_MSGHANDLED;
    }
    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
        {
            if (DialogPlayOrder->Flags & DIALOG_VISIBLE)
            {
                g_PlayOrderVisible = 0;
                DialogHide(DialogPlayOrder);
            }
            else
            {
                g_PlayOrderVisible = 1;
                DialogShow(DialogPlayOrder, 0);
            }

            DialogHide(Dialog);
        }
        else if (Param2 == Dialog->Children[ 1 ])
        {
            if (DialogMyTank->Flags & DIALOG_VISIBLE)
            {
                g_MyTankVisible = 0;
                DialogHide(DialogMyTank);
            }
            else
            {
                g_MyTankVisible = 1;
                DialogShow(DialogMyTank, 0);
            }

            DialogHide(Dialog);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogPopupViewCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPopupViewCreate(void)
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        (2 * buttonH) + DIALOG_PADDINGx2;

    DialogPopupView = DialogCreate("PopupView", DIALOG_POPUPMENU,
                                   0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPopupView, 0, 0);

    float buttonY = DIALOG_PADDING;

    DialogCreate("Hide PlayOrder", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupView, DialogProcButton, 0, 0); buttonY += buttonH;

    DialogCreate("Hide MyTank", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
                 DIALOG_PADDING, buttonY, buttonW, buttonH, DialogPopupView, DialogProcButton, 0, 0);

    return 1;
}
