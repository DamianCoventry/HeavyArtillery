//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "WeaponTypes.h"
#include "Backpack.h"
#include "Utility.h"

#include "DialogShotInfo.h"
#include "DialogProcDefault.h"
#include "DialogProcLabel.h"

#include "PlayerManager.h"
#include "LevelManager.h"

#include <stdio.h>
#include <iomanip>
#include <sstream>


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogShotInfo;


//-----------------------------------------------------------------------------
// Name: DialogProcShotInfo()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcShotInfo(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    Weapon_t* Weapon;
    float Heading, Pitch, Power;

    switch (Msg)
    {
    case WM_ACTIVATE :
        if ((size_t)Param1 == WA_ACTIVE)
        {
            DialogShowCursor(0);

            GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << Heading;
            auto text = oss.str();
            DialogSend(Dialog->Children[ 3 ], WM_SETTEXT, (void*)&text, 0);

            oss.str("");
            oss << std::fixed << std::setprecision(1) << Pitch;
            text = oss.str();
            DialogSend(Dialog->Children[ 4 ], WM_SETTEXT, (void*)&text, 0);

            oss.str("");
            oss << std::fixed << std::setprecision(1) << Power;
            text = oss.str();
            DialogSend(Dialog->Children[ 5 ], WM_SETTEXT, (void*)&text, 0);

            Weapon = (Weapon_t*)BackpackGetItem(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEWEAPON, GetPlayerCurrentWeapon(GetCurrentPlayer()));
            DialogSend(Dialog->Children[ 7 ], WM_SETTEXT, (void*)&Weapon->Name, 0);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogShotInfoCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogShotInfoCreate(void)
{
    const float labelThinW = MESSAGEBOX_BUTTONWIDTH;
    const float labelWideW = MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + 2 * labelWideW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        4 * (labelH + DIALOG_PADDING);

    DialogShotInfo = DialogCreate("Shot Info", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE,
                                  DIALOG_PADDING, DIALOG_PADDING, dialogW, dialogH, nullptr, DialogProcShotInfo, 0, 0);

    float labelX = DIALOG_PADDING;
    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Heading:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelThinW, labelH, DialogShotInfo, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Pitch:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelThinW, labelH, DialogShotInfo, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Power", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelThinW, labelH, DialogShotInfo, DialogProcLabel, 0, 0);

    labelX = DIALOG_PADDING + labelThinW + DIALOG_PADDING;
    labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("999.99", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, labelWideW, labelH, DialogShotInfo, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("999.99", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, labelWideW, labelH, DialogShotInfo, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("999.99", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, labelWideW, labelH, DialogShotInfo, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Weapon:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, labelY, labelThinW, labelH, DialogShotInfo, DialogProcLabel, 0, 0);

    DialogCreate("blah blah", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 DIALOG_PADDING + labelThinW + DIALOG_PADDING, labelY, 64.0f, labelH, DialogShotInfo, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Camera Mode: pee pee", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, labelY, 2 * labelWideW, labelH, DialogShotInfo, DialogProcLabel, 0, 0);

    return 1;
}
