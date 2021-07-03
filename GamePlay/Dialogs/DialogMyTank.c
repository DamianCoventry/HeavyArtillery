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
#include <iomanip>
#include <sstream>

#include "DialogMyTank.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "Hardware.h"
#include "Utility.h"

#include "WeaponTypes.h"
#include "ItemTypes.h"
#include "GameItems.h"

#include "PlayerManager.h"
#include "LevelManager.h"

#include "State.h"
#include "StateFindTank.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogMyTank;
size_t g_MyTankVisible = 1;

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcMyTank()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcMyTank(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_CREATE :
        Dialog->X = (float)g_HWConfig.GfxWidth  - Dialog->W - 8.0f;
        Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - 8.0f;
        break;

    case WM_DISPLAYCHANGE :
        Dialog->X = (float)g_HWConfig.GfxWidth  - Dialog->W - 8.0f;
        Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - 8.0f;
        return DIALOG_MSGHANDLED;

    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
            DialogUpdateMyTank(Dialog);

        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 10 ])
            FindPlayer(GetCurrentPlayer());

        else if (Param2 == Dialog->Children[ 11 ])
            RepairPlayersTank(GetCurrentPlayer());

        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogMyTankCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogMyTankCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING + 2*buttonW + MESSAGEBOX_BUTTONWIDTHHALF + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        5 * labelH +
        buttonH + DIALOG_PADDINGx2;

    DialogMyTank = DialogCreate("My Tank", DIALOG_MOVEABLE | DIALOG_TITLE,
                                0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcMyTank, 0, 0);

    float labelX = DIALOG_PADDING;
    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Name:", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, buttonW, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate("Tank Power:", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, buttonW, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate("Shield Power:", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, buttonW, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate("Shield Type:", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, buttonW, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate("Weapon:", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, buttonW, buttonH, DialogMyTank, DialogProcLabel, 0, 0);

    labelX = DIALOG_PADDING + buttonW + DIALOG_PADDING;
    labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT, labelX, labelY, buttonW + MESSAGEBOX_BUTTONWIDTHHALF, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT, labelX, labelY, buttonW + MESSAGEBOX_BUTTONWIDTHHALF, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT, labelX, labelY, buttonW + MESSAGEBOX_BUTTONWIDTHHALF, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT, labelX, labelY, buttonW + MESSAGEBOX_BUTTONWIDTHHALF, buttonH, DialogMyTank, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT, labelX, labelY, buttonW + MESSAGEBOX_BUTTONWIDTHHALF, buttonH, DialogMyTank, DialogProcLabel, 0, 0);

    DialogCreate("Find Me", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                 dialogW - 2 * buttonW - 2 * DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogMyTank, DialogProcButton, 0, 0);

    DialogCreate("Repair", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogMyTank, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogUpdateMyTank()
// Desc:
//-----------------------------------------------------------------------------

void DialogUpdateMyTank(Dialog_t* Dialog)
{
    auto PlayerName = GetPlayerName(GetCurrentPlayer());
    DialogSend(Dialog->Children[ 5 ], WM_SETTEXT, (void*)&PlayerName, 0);

    std::ostringstream oss;
    oss << std::setprecision(2) << std::fixed << GetPlayerTankPower(GetCurrentPlayer());
    auto text = oss.str();
    DialogSend(Dialog->Children[ 6 ], WM_SETTEXT, (void*)&text, 0);

    oss.str("");
    oss << std::setprecision(2) << std::fixed << GetPlayerShieldPower(GetCurrentPlayer());
    text = oss.str();
    DialogSend(Dialog->Children[ 7 ], WM_SETTEXT, (void*)&text, 0);

    auto ShieldName = GetShieldName(GetPlayerShieldType(GetCurrentPlayer()));
    DialogSend(Dialog->Children[ 8 ], WM_SETTEXT, (void*)&ShieldName, 0);

    Weapon_t* Weapon = (Weapon_t*)BackpackGetItem(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEWEAPON, GetPlayerCurrentWeapon(GetCurrentPlayer()));

    // is this an unlimited weapon?
    oss.str("");
    if (!Weapon->Price)
        oss << Weapon->Name << " (Unlimited)";
    else
        oss << Weapon->Name << " (" << BackpackGetAmountLeft(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEWEAPON, (void*)Weapon) << ")";

    text = oss.str();
    DialogSend(Dialog->Children[9], WM_SETTEXT, (void*)&text, 0);
}
