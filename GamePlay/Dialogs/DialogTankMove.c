//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 13 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogTankMove.h"
#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLabel.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogTankMove;


//-----------------------------------------------------------------------------
// Name: DialogTankMoveCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogTankMoveCreate(void)
{
    const float labelWideW = 4 * MESSAGEBOX_BUTTONWIDTH;
    const float labelThinW = 2 * MESSAGEBOX_BUTTONWIDTH;
    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + labelWideW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        3 * (labelH + DIALOG_PADDING);

    DialogTankMove = DialogCreate("Tank Move", DIALOG_TITLE | DIALOG_MOVEABLE,
                                  DIALOG_PADDING, DIALOG_PADDING, dialogW, dialogH, nullptr, DialogProcDefault, 0, 0);

    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Tank Move Activated (Player Name)", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, labelY, labelWideW, labelH, DialogTankMove, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Fuel Remaing: xxx", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 DIALOG_PADDING, labelY, labelThinW, labelH, DialogTankMove, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Camera Mode: pee pee", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, labelY, labelThinW, labelH, DialogTankMove, DialogProcLabel, 0, 0);

    return 1;
}
