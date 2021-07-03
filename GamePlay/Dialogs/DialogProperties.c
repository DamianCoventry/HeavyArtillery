//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 13 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogProperties.h"
#include "DialogMyTank.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "Utility.h"
#include "GameItems.h"
#include "GameTypes.h"
#include "GameModels.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"

#include "State.h"
#include "StateBeginTurn.h"

#include "BotDefense.h"

#include <iomanip>
#include <sstream>
#include <stdio.h>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogProperties;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static DialogObject_t _DialogObject;
static size_t _AutoDefensePlayer = -1;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t DialogProcProperties(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);
static void AddItemsToListBox(Dialog_t* Dialog);

static size_t OnCreate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnDelete(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnSelectItem(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickOK(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickUseActivateDeactivate(Dialog_t* Dialog, void* Param1, void* Param2);


//-----------------------------------------------------------------------------
// Name: DialogPropertiesCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPropertiesCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 200;
    const float listBoxH = 350;

    const float labelWideW = 2 * MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float modelW = 200;
    const float modelH = 200;

    const float dialogW =
        DIALOG_PADDING + listBoxW + DIALOG_PADDING + modelW + DIALOG_PADDING + buttonW + labelWideW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        listBoxH + DIALOG_PADDING +
        buttonH + DIALOG_PADDING;

    DialogProperties = DialogCreate("Properties", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                    0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcProperties, 0, 0);

    DialogCreate("Properties ListBox", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, listBoxW, listBoxH,
                 DialogProperties, DialogProcList, 0, 0);

    DialogCreate("OK", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH,
                 DialogProperties, DialogProcButton, 0, 0);

    DialogCreate("Model", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_BORDER,
                 listBoxW + DIALOG_PADDINGx2, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, modelW, modelH, DialogProperties,
                 DialogProcLWO, 0, 0);

    float labelX = listBoxW + DIALOG_PADDINGx2;
    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + modelH + DIALOG_PADDING;

    DialogCreate("Information:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0);

    labelX = listBoxW + DIALOG_PADDINGx2 + modelW + DIALOG_PADDING;
    labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    // 9
    DialogCreate("Player:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0);

    DialogCreate("Player 0", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW + DIALOG_PADDING, labelY, labelWideW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate("Tank Power:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0);

    DialogCreate("100", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW + DIALOG_PADDING, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate("Shield Power:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0);

    DialogCreate("100", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW + DIALOG_PADDING, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate("Shield Type:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0);

    DialogCreate("Medium", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW + DIALOG_PADDING, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate("State:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY,
                 buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0);

    DialogCreate("Deactivated", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW + DIALOG_PADDING, labelY, buttonW, buttonH, DialogProperties,
                 DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    // 19
    DialogCreate("Activate", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                 labelX + buttonW + DIALOG_PADDING, labelY, buttonW, buttonH,
                 DialogProperties, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcProperties()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcProperties(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_CREATE :
        return OnCreate(Dialog, Param1, Param2);

    case WM_DESTROY :
        return OnDelete(Dialog, Param1, Param2);

    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
            return OnSelectItem(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 1 ])
            return OnClickOK(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 19 ])
            return OnClickUseActivateDeactivate(Dialog, Param1, Param2);

        break;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: AutoDefenseSetFirstPlayer()
// Desc:
//-----------------------------------------------------------------------------

size_t AutoDefenseSetFirstPlayer(void)
{
    for (_AutoDefensePlayer = 0; _AutoDefensePlayer < GetPlayerCount(); _AutoDefensePlayer++)
    {
        if (-1 != BackpackGetAbsoluteIndex(GetPlayerBackpack(_AutoDefensePlayer), BPITEM_TYPEITEM, (void*)&ItemAutoDefense))
        {
            if (IsPlayerABot(_AutoDefensePlayer))
                AllowBotToSetupDefense(_AutoDefensePlayer);
            else
                return 1;
        }
    }

    _AutoDefensePlayer = -1;
    return 0;
}


//-----------------------------------------------------------------------------
// Name: AutoDefenseSetNextPlayer()
// Desc:
//-----------------------------------------------------------------------------

size_t AutoDefenseSetNextPlayer(void)
{
    for (; ;)
    {
        _AutoDefensePlayer++;
        if (_AutoDefensePlayer >= GetPlayerCount())
        {
            _AutoDefensePlayer = -1;
            return 0;
        }

        if (-1 != BackpackGetAbsoluteIndex(GetPlayerBackpack(_AutoDefensePlayer), BPITEM_TYPEITEM, (void*)&ItemAutoDefense))
        {
            if (IsPlayerABot(_AutoDefensePlayer))
                AllowBotToSetupDefense(_AutoDefensePlayer);
            else
                break;
        }
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: PropertiesUpdate()
// Desc:
//-----------------------------------------------------------------------------

void PropertiesUpdate(Dialog_t* Dialog)
{
    Item_t* Item;
    size_t PlayerIndex, Active;
    size_t SelectedIndex, Index;

    DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);
    SelectedIndex = DialogRetValue();

    if (-1 == _AutoDefensePlayer)
    {
        PlayerIndex = GetCurrentPlayer();
        Dialog->Name = "Properties";
    }
    else
    {
        PlayerIndex = _AutoDefensePlayer;
        Dialog->Name = "Automatic Defense";
    }

    auto PlayerName = GetPlayerName(PlayerIndex);
    DialogSend(Dialog->Children[ 10 ], WM_SETTEXT, (void*)&PlayerName, 0);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << GetPlayerTankPower(PlayerIndex);
    auto text = oss.str();
    DialogSend(Dialog->Children[ 12 ], WM_SETTEXT, (void*)&text, 0);

    oss.str("");
    oss << std::fixed << std::setprecision(2) << GetPlayerShieldPower(PlayerIndex);
    text = oss.str();
    DialogSend(Dialog->Children[ 14 ], WM_SETTEXT, (void*)&text, 0);

    auto ShieldName = GetShieldName(GetPlayerShieldType(PlayerIndex));
    DialogSend(Dialog->Children[ 16 ], WM_SETTEXT, (void*)&ShieldName, 0);

    Item = (Item_t*)BackpackGetItem(GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM, SelectedIndex);
    if (!Item)
        return;

    SetObjectModels(_DialogObject.ObjectID, Item->ModelID, -1);
    _DialogObject.Scale = Item->Scale;

    if (Item->ModelID == -1)
    {
        SetObjectModels(_DialogObject.ObjectID, g_ModelBuyFullVersion, -1);
        _DialogObject.Scale = 4.0f;
    }

    if (g_ModelShieldHeavy == Item->ModelID || g_ModelShieldMedium == Item->ModelID || g_ModelShieldLight == Item->ModelID)
        SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModelTrans);
    else
        SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModel);

    Dialog->Children[ 2 ]->Data = (void*)&_DialogObject;

    DialogSend(Dialog->Children[ 4 ], WM_SETTEXT, (void*)&Item->Line0, 0);
    DialogSend(Dialog->Children[ 5 ], WM_SETTEXT, (void*)&Item->Line1, 0);
    DialogSend(Dialog->Children[ 6 ], WM_SETTEXT, (void*)&Item->Line2, 0);
    DialogSend(Dialog->Children[ 7 ], WM_SETTEXT, (void*)&Item->Line3, 0);

    switch (Item->Type)
    {
    case ITEMTYPE_GUIDANCE :
    {
        Dialog->Children[17]->Flags |= DIALOG_VISIBLE;
        Dialog->Children[18]->Flags |= DIALOG_VISIBLE;
        Dialog->Children[19]->Flags |= DIALOG_VISIBLE;

        Index = BackpackGetAbsoluteIndex(GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM, Item);

        std::string Temp = IsBackpackItemActivated(GetPlayerBackpack(PlayerIndex), Index) ? "Activated" : "Deactivated";
        DialogSend(Dialog->Children[18], WM_SETTEXT, (void*)&Temp, 0);
        Temp = IsBackpackItemActivated(GetPlayerBackpack(PlayerIndex), Index) ? "Deactivate" : "Activate";
        DialogSend(Dialog->Children[19], WM_SETTEXT, (void*)&Temp, 0);
        DialogSend(Dialog->Children[19], WM_SIZE, 0, 0);

        break;
    }
    case ITEMTYPE_SHIELD :
    {
        Dialog->Children[17]->Flags |= DIALOG_VISIBLE;
        Dialog->Children[18]->Flags |= DIALOG_VISIBLE;
        Dialog->Children[19]->Flags |= DIALOG_VISIBLE;

        Index = BackpackGetAbsoluteIndex(GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM, Item);

        Active = IsObjectActivated(GetPlayerShield(PlayerIndex));

        std::string Temp = Active ? "Activated" : "Deactivated";
        DialogSend(Dialog->Children[18], WM_SETTEXT, (void*)&Temp, 0);
        Temp = Active ? "Deactivate" : "Activate";
        DialogSend(Dialog->Children[19], WM_SETTEXT, (void*)&Temp, 0);
        DialogSend(Dialog->Children[19], WM_SIZE, 0, 0);

        break;
    }
    case ITEMTYPE_OTHER :
    {
        std::string Temp;
        if (Item->Use)
        {
            Dialog->Children[17]->Flags &= ~DIALOG_VISIBLE;
            Dialog->Children[18]->Flags &= ~DIALOG_VISIBLE;
            Dialog->Children[19]->Flags |= DIALOG_VISIBLE;

            Temp = "Use";
            DialogSend(Dialog->Children[19], WM_SETTEXT, (void*)&Temp, 0);
        }
        else if (Item->Activate)
        {
            Dialog->Children[17]->Flags |= DIALOG_VISIBLE;
            Dialog->Children[18]->Flags |= DIALOG_VISIBLE;
            Dialog->Children[19]->Flags |= DIALOG_VISIBLE;

            Index = BackpackGetAbsoluteIndex(GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM, Item);

            Temp = IsBackpackItemActivated(GetPlayerBackpack(PlayerIndex), Index) ? "Activated" : "Deactivated";
            DialogSend(Dialog->Children[18], WM_SETTEXT, (void*)&Temp, 0);
            Temp = IsBackpackItemActivated(GetPlayerBackpack(PlayerIndex), Index) ? "Deactivate" : "Activate";
            DialogSend(Dialog->Children[19], WM_SETTEXT, (void*)&Temp, 0);
        }
        else
        {
            Dialog->Children[17]->Flags &= ~DIALOG_VISIBLE;
            Dialog->Children[18]->Flags &= ~DIALOG_VISIBLE;
            Dialog->Children[19]->Flags &= ~DIALOG_VISIBLE;
        }

        DialogSend(Dialog->Children[19], WM_SIZE, 0, 0);

        break;
    }
    case ITEMTYPE_POSSESS :
        Dialog->Children[ 17 ]->Flags &= ~DIALOG_VISIBLE;
        Dialog->Children[ 18 ]->Flags &= ~DIALOG_VISIBLE;
        Dialog->Children[ 19 ]->Flags &= ~DIALOG_VISIBLE;
        break;
    }
}


//-----------------------------------------------------------------------------
// Name: AddItemsToListBox()
// Desc:
//-----------------------------------------------------------------------------

void AddItemsToListBox(Dialog_t* Dialog)
{
    size_t PlayerIndex;
    size_t SelectedIndex;

    if (-1 == _AutoDefensePlayer)
        PlayerIndex = GetCurrentPlayer();
    else
        PlayerIndex = _AutoDefensePlayer;

    SelectedIndex = DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);

    AddBackpackItemsToListBox(Dialog->Children[ 0 ], GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM);

    DialogSend(Dialog->Children[ 0 ], LB_SETCURSEL, (void*)SelectedIndex, 0);
}


//-----------------------------------------------------------------------------
// Name: OnCreate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnCreate(Dialog_t* Dialog, void* Param1, void* Param2)
{
    Vector_t Position;

    _DialogObject.ObjectID = GetFreeObject();
    ResetObject(_DialogObject.ObjectID);

    SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModel);

    Position.X = Position.Z = 0.0f;
    Position.Y = TANK_BARRELLHEIGHT;
    SetObjectPosition(_DialogObject.ObjectID, 2, &Position);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnDelete()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnDelete(Dialog_t* Dialog, void* Param1, void* Param2)
{
    ReturnObject(_DialogObject.ObjectID);
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnActivate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2)
{
    const std::string Space = " ";

    if (WA_ACTIVE == (size_t)Param1)
    {
        Dialog->Children[ 2 ]->Data = nullptr;

        std::string Temp{ "No item selected." };
        DialogSend(Dialog->Children[ 4 ], WM_SETTEXT, (void*)&Temp, 0);
        DialogSend(Dialog->Children[ 5 ], WM_SETTEXT, (void*)&Space, 0);
        DialogSend(Dialog->Children[ 6 ], WM_SETTEXT, (void*)&Space, 0);
        DialogSend(Dialog->Children[ 7 ], WM_SETTEXT, (void*)&Space, 0);

        Dialog->Children[ 17 ]->Flags &= ~DIALOG_VISIBLE;
        Dialog->Children[ 18 ]->Flags &= ~DIALOG_VISIBLE;
        Dialog->Children[ 19 ]->Flags &= ~DIALOG_VISIBLE;

        AddItemsToListBox(Dialog);
        PropertiesUpdate(Dialog);

        DialogSend(Dialog->Children[ 0 ], LB_SETCURSEL, 0, 0);
    }

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnSelectItem()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnSelectItem(Dialog_t* Dialog, void* Param1, void* Param2)
{
    PropertiesUpdate(Dialog);
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickOK()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickOK(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogHide(Dialog);

    if (-1 != _AutoDefensePlayer)
    {
        if (!AutoDefenseSetNextPlayer())
        {
            _AutoDefensePlayer = -1;
            SetGameState(&g_StateBeginTurn);
        }
        else
            DialogShow(Dialog, 0);
    }

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickUseActivateDeactivate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickUseActivateDeactivate(Dialog_t* Dialog, void* Param1, void* Param2)
{
    size_t PlayerIndex;
    size_t SelectedIndex;
    Item_t* Item;

    DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);
    SelectedIndex = DialogRetValue();

    if (-1 == _AutoDefensePlayer)
        PlayerIndex = GetCurrentPlayer();
    else
        PlayerIndex = _AutoDefensePlayer;

    Item = (Item_t*)BackpackGetItem(GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM, SelectedIndex);
    if (Item)
    {
        if (Item->Use)
            Item->Use(Item, GetPlayerBackpack(PlayerIndex), PlayerIndex);

        else if (Item->Activate)
            Item->Activate(Item, GetPlayerBackpack(PlayerIndex), PlayerIndex);
    }

    AddBackpackItemsToListBox(Dialog->Children[ 0 ], GetPlayerBackpack(PlayerIndex), BPITEM_TYPEITEM);

    DialogSend(Dialog->Children[ 0 ], LB_SETCURSEL, (void*)SelectedIndex, 0);
    PropertiesUpdate(Dialog);

    DialogUpdateMyTank(DialogMyTank);

    return DIALOG_MSGHANDLED;
}
