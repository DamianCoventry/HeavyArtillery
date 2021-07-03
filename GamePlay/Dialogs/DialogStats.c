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

#include "DialogMyTank.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"
#include "DialogProcStatic.h"

#include "WeaponTypes.h"
#include "PlayerHistory.h"
#include "GameSounds.h"
#include "Camera.h"

#include "LevelManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "SoundManager.h"

#include <iomanip>
#include <sstream>


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogStats;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static DialogObject_t _DialogObject;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t OnActivate(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static size_t OnClickListbox0(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static size_t OnClickListbox1(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static size_t OnClickShots(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static size_t OnClickDamage(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static size_t OnClickKills(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static size_t OnClickCash(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t GetPlayerIndex(Dialog_t* Dialog);

static void DisplayShots(Dialog_t* Dialog);
static void DisplayDamage(Dialog_t* Dialog);
static void DisplayKills(Dialog_t* Dialog);
static void DisplayCash(Dialog_t* Dialog);


//-----------------------------------------------------------------------------
// Name: DialogProcStats()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcStats(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
            return OnActivate(Dialog, Msg, Param1, Param2);

        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 1 ])
            return OnClickListbox0(Dialog, Msg, Param1, Param2);
        else if (Param2 == Dialog->Children[ 3 ])
            return OnClickListbox1(Dialog, Msg, Param1, Param2);
        else if (Param2 == Dialog->Children[ 5 ])
            return OnClickShots(Dialog, Msg, Param1, Param2);
        else if (Param2 == Dialog->Children[ 6 ])
            return OnClickDamage(Dialog, Msg, Param1, Param2);
        else if (Param2 == Dialog->Children[ 7 ])
            return OnClickKills(Dialog, Msg, Param1, Param2);
        else if (Param2 == Dialog->Children[ 8 ])
            return OnClickCash(Dialog, Msg, Param1, Param2);
        else if (Param2 == Dialog->Children[ 9 ])
            DialogHide(Dialog);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogStatsCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogStatsCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 250;
    const float listBoxH = 200;

    const float labelWideW = 3 * MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float tabButtonY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + labelH;

    const float tabControlX = DIALOG_PADDING + listBoxW + DIALOG_PADDING;
    const float tabControlY = tabButtonY + buttonH;
    const float tabControlW = 4 * MESSAGEBOX_BUTTONWIDTH;

    const float labelAndListBoxHeights = labelH + listBoxH + DIALOG_PADDING + labelH + listBoxH;

    const float tabControlH = labelAndListBoxHeights - (labelH + buttonH);

    const float dialogW =
        DIALOG_PADDING + listBoxW + DIALOG_PADDING + tabControlW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        labelH +
        listBoxH + DIALOG_PADDING +
        labelH +
        listBoxH + DIALOG_PADDING +
        buttonH + DIALOG_PADDING;




    DialogStats = DialogCreate("Statistics", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                               0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcStats, 0, 0);




    float controlY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Surviving Players:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, buttonW, labelH,
                 DialogStats, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate("Listbox0", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, controlY, listBoxW, listBoxH,
                 DialogStats, DialogProcList, 0, 0); controlY += listBoxH + DIALOG_PADDING;

    DialogCreate("Deceased Players:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, buttonW, labelH,
                 DialogStats, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate("Listbox1", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, controlY, listBoxW, listBoxH,
                 DialogStats, DialogProcList, 0, 0);


    DialogCreate("Static", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_RAISED,
                 tabControlX, tabControlY, tabControlW, tabControlH,
                 DialogStats, DialogProcStatic, 0, 0);

    DialogCreate("Shots", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_TABSELECTED,
                 tabControlX, tabButtonY, buttonW, buttonH,
                 DialogStats, DialogProcButton, 0, 0);

    DialogCreate("Damage", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_TABUNSELECTED,
                 tabControlX + buttonW, tabButtonY, buttonW, buttonH,
                 DialogStats, DialogProcButton, 0, 0);

    DialogCreate("Kills", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_TABUNSELECTED,
                 tabControlX + 2 * buttonW, tabButtonY, buttonW, buttonH,
                 DialogStats, DialogProcButton, 0, 0);

    DialogCreate("Cash", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_TABUNSELECTED,
                 tabControlX + 3 * buttonW, tabButtonY, buttonW, buttonH,
                 DialogStats, DialogProcButton, 0, 0);

    DialogCreate("OK", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH,
                 DialogStats, DialogProcButton, 0, 0);


    const float tabClientX = tabControlX + DIALOG_PADDINGx2;
    const float tabClientY = tabButtonY + buttonH + DIALOG_PADDINGx2;

    controlY = tabClientY;

    DialogCreate("Fired:", DIALOG_CHILD | DIALOG_VISIBLE,
                 tabClientX, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0);
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 tabClientX + 2 * buttonW, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate("Hit:", DIALOG_CHILD | DIALOG_VISIBLE,
                 tabClientX, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0);
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 tabClientX + 2 * buttonW, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate("Missed:", DIALOG_CHILD | DIALOG_VISIBLE,
                 tabClientX, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0);
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 tabClientX + 2 * buttonW, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate("Accuracy:", DIALOG_CHILD | DIALOG_VISIBLE,
                 tabClientX, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0);
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 tabClientX + 2 * buttonW, controlY, buttonW, buttonH, DialogStats, DialogProcLabel, 0, 0);




    DialogCreate("Team X wins!", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 tabControlX, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW, labelH,
                 DialogStats, DialogProcLabel, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: OnActivate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnActivate(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    size_t PlyrIndex;
    Vector_t* Position;

    DialogSend(Dialog->Children[ 1 ], LB_RESETCONTENT, 0, 0);
    DialogSend(Dialog->Children[ 3 ], LB_RESETCONTENT, 0, 0);

    if (-1 == GetWinningTeam())
        Dialog->Children[18]->Name = "All teams were destroyed.";
    else
    {
        std::ostringstream oss;
        oss << "Team " << GetWinningTeam() << " wins!";
        Dialog->Children[18]->Name = oss.str();
    }

    DialogSend(Dialog->Children[ 18 ], WM_SIZE, 0, 0);

    for (PlyrIndex = 0; PlyrIndex < GetPlayerCount(); PlyrIndex++)
    {
        std::ostringstream oss;
        oss << GetPlayerName(PlyrIndex) << " (" << GetPlayerTeam(PlyrIndex) << ")";

        size_t i = (IsPlayerDead(PlyrIndex) ? 3 : 1);
        auto text = oss.str();
        DialogSend(Dialog->Children[i], LB_ADDSTRING, (void*)&text, 0);
        DialogSend(Dialog->Children[i], LB_GETCOUNT, 0, 0);
        DialogSend(Dialog->Children[i], LB_SETITEMDATA, (void*)(DialogRetValue() - 1), (void*)PlyrIndex);
    }

    // select a player from a listbox
    DialogSend(Dialog->Children[ 1 ], LB_GETCOUNT, 0, 0);
    if (DialogRetValue() > 0)
        DialogSend(Dialog->Children[ 1 ], LB_SETCURSEL, 0, 0);
    else
        DialogSend(Dialog->Children[ 3 ], LB_SETCURSEL, 0, 0);

    Position = GetCameraPosition();
    SetSoundPosition(SoundApplause0, Position->X, Position->Y, Position->Z);
    PlaySound3D(SoundApplause0, 0);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickListbox0()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickListbox0(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    // clear the selection in the other list box
    DialogSend(Dialog->Children[ 3 ], LB_SETCURSEL, (void*) - 1, 0);

    if (Dialog->Children[ 5 ]->Flags & DIALOG_TABSELECTED)       // shots
        return OnClickShots(Dialog, Msg, Param1, Param2);
    else if (Dialog->Children[ 6 ]->Flags & DIALOG_TABSELECTED)  // damage
        return OnClickDamage(Dialog, Msg, Param1, Param2);
    else if (Dialog->Children[ 7 ]->Flags & DIALOG_TABSELECTED)  // kills
        return OnClickKills(Dialog, Msg, Param1, Param2);
    else if (Dialog->Children[ 8 ]->Flags & DIALOG_TABSELECTED)  // cash
        return OnClickCash(Dialog, Msg, Param1, Param2);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickListbox1()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickListbox1(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    // clear the selection in the other list box
    DialogSend(Dialog->Children[ 1 ], LB_SETCURSEL, (void*) - 1, 0);

    if (Dialog->Children[ 5 ]->Flags & DIALOG_TABSELECTED)       // shots
        return OnClickShots(Dialog, Msg, Param1, Param2);
    else if (Dialog->Children[ 6 ]->Flags & DIALOG_TABSELECTED)  // damage
        return OnClickDamage(Dialog, Msg, Param1, Param2);
    else if (Dialog->Children[ 7 ]->Flags & DIALOG_TABSELECTED)  // kills
        return OnClickKills(Dialog, Msg, Param1, Param2);
    else if (Dialog->Children[ 8 ]->Flags & DIALOG_TABSELECTED)  // cash
        return OnClickCash(Dialog, Msg, Param1, Param2);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickShots()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickShots(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    DialogUpdateFlags(Dialog->Children[5], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);
    DialogUpdateFlags(Dialog->Children[6], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[7], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[8], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);

    Dialog->Children[10]->Name = "Fired:";
    Dialog->Children[12]->Name = "Hit:";
    Dialog->Children[14]->Name = "Missed:";
    Dialog->Children[16]->Name = "Accuracy:";

    DisplayShots(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickDamage()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickDamage(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    DialogUpdateFlags(Dialog->Children[5], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[6], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);
    DialogUpdateFlags(Dialog->Children[7], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[8], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);

    Dialog->Children[10]->Name = "To Enemys:";
    Dialog->Children[12]->Name = "To Self:";
    Dialog->Children[14]->Name = "To Teammates:";
    Dialog->Children[16]->Name = "Falling:";

    DisplayDamage(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickKills()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickKills(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    DialogUpdateFlags(Dialog->Children[5], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[6], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[7], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);
    DialogUpdateFlags(Dialog->Children[8], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);

    Dialog->Children[10]->Name = "Enemy:";
    Dialog->Children[12]->Name = "Self:";
    Dialog->Children[14]->Name = "Teammate:";
    Dialog->Children[16]->Name = " ";
    Dialog->Children[17]->Name = " ";

    DisplayKills(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickCash()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickCash(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    DialogUpdateFlags(Dialog->Children[5], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[6], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[7], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[8], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);

    Dialog->Children[10]->Name = "Survive Bonus:";
    Dialog->Children[12]->Name = "Team Win Bonus:";
    Dialog->Children[14]->Name = "Cash Won:";
    Dialog->Children[16]->Name = "Total Cash:";
    Dialog->Children[17]->Name = "0";

    DisplayCash(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: GetPlayerIndex()
// Desc:
//-----------------------------------------------------------------------------

static size_t GetPlayerIndex(Dialog_t* Dialog)
{
    size_t Index;

    DialogSend(Dialog->Children[ 1 ], LB_GETCURSEL, 0, 0);
    Index = DialogRetValue();
    if (-1 == Index)
    {
        DialogSend(Dialog->Children[ 3 ], LB_GETCURSEL, 0, 0);
        Index = DialogRetValue();

        DialogSend(Dialog->Children[ 3 ], LB_GETITEMDATA, (void*)Index, 0);
        Index = DialogRetValue();
    }
    else
    {
        DialogSend(Dialog->Children[ 1 ], LB_GETITEMDATA, (void*)Index, 0);
        Index = DialogRetValue();
    }

    return Index;
}


//-----------------------------------------------------------------------------
// Name: DisplayShots()
// Desc:
//-----------------------------------------------------------------------------

static void DisplayShots(Dialog_t* Dialog)
{
    PlayerHistory_t* History;
    size_t Index;

    Index = GetPlayerIndex(Dialog);
    if (-1 == Index)
        return;

    History = GetPlayerHistory(Index);

    Dialog->Children[ 11 ]->Name = std::to_string(History->NumShotsFired);
    Dialog->Children[ 13 ]->Name = std::to_string(History->NumShotsHit);
    Dialog->Children[ 15 ]->Name = std::to_string(History->NumShotsMissed);

    // prevent DBZ
    if (0 == History->NumShotsFired)
        Dialog->Children[17]->Name = "0.00 %";
    else
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << (float)History->NumShotsHit / (float)History->NumShotsFired * 100.0f << " %";
        Dialog->Children[17]->Name = oss.str();
    }
}


//-----------------------------------------------------------------------------
// Name: DisplayDamage()
// Desc:
//-----------------------------------------------------------------------------

static void DisplayDamage(Dialog_t* Dialog)
{
    PlayerHistory_t* History;
    size_t Index;

    Index = GetPlayerIndex(Dialog);
    if (-1 == Index)
        return;

    History = GetPlayerHistory(Index);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << History->DamageToEnemys << " %";
    Dialog->Children[11]->Name = oss.str();

    oss.str("");
    oss << std::fixed << std::setprecision(2) << History->DamageToSelf << " %";
    Dialog->Children[13]->Name = oss.str();

    oss.str("");
    oss << std::fixed << std::setprecision(2) << History->DamageToTeammates << " %";
    Dialog->Children[15]->Name = oss.str();

    oss.str("");
    oss << std::fixed << std::setprecision(2) << History->FallingDamage << " %";
    Dialog->Children[17]->Name = oss.str();
}


//-----------------------------------------------------------------------------
// Name: DisplayKills()
// Desc:
//-----------------------------------------------------------------------------

static void DisplayKills(Dialog_t* Dialog)
{
    PlayerHistory_t* History;
    size_t Index;

    Index = GetPlayerIndex(Dialog);
    if (-1 == Index)
        return;

    History = GetPlayerHistory(Index);

    Dialog->Children[11]->Name = std::to_string(History->EnemyKills);
    Dialog->Children[13]->Name = std::to_string(History->SelfKills);
    Dialog->Children[15]->Name = std::to_string(History->TeammateKills);
    Dialog->Children[17]->Name = " ";
}


//-----------------------------------------------------------------------------
// Name: DisplayCash()
// Desc:
//-----------------------------------------------------------------------------

static void DisplayCash(Dialog_t* Dialog)
{
    PlayerHistory_t* History;
    size_t Index;

    Index = GetPlayerIndex(Dialog);
    if (-1 == Index)
        return;

    History = GetPlayerHistory(Index);

    Dialog->Children[11]->Name = std::to_string(History->SurviveBonus);
    Dialog->Children[13]->Name = std::to_string(History->TeamWinBonus);
    Dialog->Children[15]->Name = std::to_string(History->CashWon);
    Dialog->Children[17]->Name = std::to_string(GetPlayerCash(Index));
}
