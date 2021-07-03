//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Sunday, 7 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogProcDefault.h"
#include "DialogProcStatic.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "WeaponTypes.h"
#include "ItemTypes.h"

#include "PlayerManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"

#include "DialogShop.h"
#include "DialogProperties.h"

#include "GameWeapons.h"
#include "GameItems.h"
#include "GameTypes.h"
#include "GameModels.h"

#include "Utility.h"
#include "State.h"
#include "StateBeginTurn.h"

#include "BotBuy.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogShop;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static DialogObject_t _DialogObject;
static size_t _ShopPlayer;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void ShopBuyWeapon(Dialog_t* Dialog, Weapon_t* Weapon);
static void ShopBuyItem(Dialog_t* Dialog, Item_t* Item);

static bool IsTheWeaponTabSelected(Dialog_t* Dialog);

static size_t CanPlayerBuyItem(Dialog_t* Dialog);
static size_t CanPlayerSellItem(Dialog_t* Dialog);

static size_t DialogProcShop(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t OnCreate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnDestroy(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickWeapons(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickItems(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnSelectWeaponItem(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickBuy(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickSell(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnSelectBackpackItem(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickOK(Dialog_t* Dialog, void* Param1, void* Param2);


//-----------------------------------------------------------------------------
// Name: DialogShopCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogShopCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float modelW = 200.0f;
    const float modelH = 200.0f;

    const float listBoxShopW = 200.0f;
    const float listBoxShopH = 500.0f;

    const float labelH = buttonH / 2;

    const float dialogW =
        listBoxShopW + DIALOG_PADDING +
        700.0f;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        listBoxShopH + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING;

    DialogShop = DialogCreate("Shop", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                              0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcShop, 0, 0);

    const float staticFrameY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + buttonH;

    DialogCreate("Static", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_RAISED,
                 DIALOG_PADDING, staticFrameY,
                 listBoxShopW + DIALOG_PADDINGx2, listBoxShopH + DIALOG_PADDINGx2,
                 DialogShop, DialogProcStatic, 0, 0);

    const float tabButtonY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Weapons", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_TABSELECTED | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, tabButtonY, buttonW, buttonH, DialogShop, DialogProcButton, 0, 0);

    DialogCreate("Items", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_TABUNSELECTED | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING + buttonW, tabButtonY, buttonW, buttonH, DialogShop, DialogProcButton, 0, 0);

    const float listBoxShopY = tabButtonY + buttonH + DIALOG_PADDING;

    DialogCreate("Weapon/Item ListBox", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDINGx2, listBoxShopY,
                 listBoxShopW, listBoxShopH,
                 DialogShop, DialogProcList, 0, 0);

    const float controlX = DIALOG_PADDINGx2 + listBoxShopW + DIALOG_PADDINGx2;

    const float listBoxBackpackY = listBoxShopY + modelH + DIALOG_PADDINGx2 + buttonH;
    const float listBoxBackpackW = dialogW - controlX - DIALOG_PADDING;
    const float listBoxBackpackH = dialogH - listBoxBackpackY - buttonH - DIALOG_PADDINGx2;

    const float buttonY = listBoxBackpackY - buttonH - DIALOG_PADDING;

    DialogCreate("Buy", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - BUTTON_PADDING - buttonW - BUTTON_PADDING, buttonY, buttonW, buttonH,
                 DialogShop, DialogProcButton, 0, 0);

    DialogCreate("Sell", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - BUTTON_PADDING, buttonY, buttonW, buttonH,
                 DialogShop, DialogProcButton, 0, 0);

    DialogCreate("Backpack ListBox", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 controlX, listBoxBackpackY, listBoxBackpackW, listBoxBackpackH,
                 DialogShop, DialogProcList, 0, 0);

    DialogCreate("OK", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_OK | DIALOG_CANCEL | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH,
                 DialogShop, DialogProcButton, 0, 0);

    DialogCreate("Model", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_BORDER,
                 controlX, listBoxShopY, modelW, modelH, DialogShop,
                 DialogProcLWO, 0, 0);

    float labelX = controlX;

    DialogCreate("Player:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW / 2, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelX += (buttonW / 2) + DIALOG_PADDINGx2;

    DialogCreate("XXXXXX", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelX += 2 * buttonW + DIALOG_PADDINGx2 + buttonW / 2;

    DialogCreate("Cash:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW / 2, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelX += (buttonW / 2) + DIALOG_PADDINGx2;

    DialogCreate("XXXXXX", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0);

    labelX = controlX + modelW + DIALOG_PADDINGx2;
    float labelY = listBoxShopY;

    DialogCreate("Name:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0);

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Bundle Size:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0);

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Price:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0);

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX + buttonW, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Information:", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 labelX, labelY, buttonW, buttonH, DialogShop,
                 DialogProcLabel, 0, 0); labelY += labelH;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcShop()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcShop(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_CREATE :
        return OnCreate(Dialog, Param1, Param2);

    case WM_DESTROY :
        return OnDestroy(Dialog, Param1, Param2);

    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 1 ])
            return OnClickWeapons(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 2 ])
            return OnClickItems(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 3 ])
            return OnSelectWeaponItem(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 4 ])
            return OnClickBuy(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 5 ])
            return OnClickSell(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 6 ])
            return OnSelectBackpackItem(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 7 ])
            return OnClickOK(Dialog, Param1, Param2);

        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: ShopSetFirstPlayer()
// Desc:
//-----------------------------------------------------------------------------

size_t SetFirstShopPlayer(void)
{
    _ShopPlayer = 0;

    while (GetPlayerCash(_ShopPlayer) <= 0 || IsPlayerABot(_ShopPlayer))
    {
        if (IsPlayerABot(_ShopPlayer))
            AllowBotToBuyFromShop(_ShopPlayer);

        if (++_ShopPlayer >= GetPlayerCount())
            return 0;
    }

    auto PlayerName = GetPlayerName(_ShopPlayer);
    DialogSend(DialogShop->Children[ 10 ], WM_SETTEXT, (void*)&PlayerName, 0);

    auto PlayerCash = std::to_string(GetPlayerCash(_ShopPlayer));
    DialogSend(DialogShop->Children[ 12 ], WM_SETTEXT, (void*)&PlayerCash, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: SetNextShopPlayer()
// Desc:
//-----------------------------------------------------------------------------

size_t SetNextShopPlayer(void)
{
    if (++_ShopPlayer >= GetPlayerCount())
        return 0;

    while (GetPlayerCash(_ShopPlayer) <= 0 || IsPlayerABot(_ShopPlayer))
    {
        if (IsPlayerABot(_ShopPlayer))
            AllowBotToBuyFromShop(_ShopPlayer);

        if (++_ShopPlayer >= GetPlayerCount())
            return 0;
    }

    auto PlayerName = GetPlayerName(_ShopPlayer);
    DialogSend(DialogShop->Children[ 10 ], WM_SETTEXT, (void*)&PlayerName, 0);

    auto PlayerCash = std::to_string(GetPlayerCash(_ShopPlayer));
    DialogSend(DialogShop->Children[ 12 ], WM_SETTEXT, (void*)&PlayerCash, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: ShopBuyWeapon()
// Desc:
//-----------------------------------------------------------------------------

static void ShopBuyWeapon(Dialog_t* Dialog, Weapon_t* Weapon)
{
    BackpackAdd(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON, Weapon->BundleSize, (void*)Weapon);
    AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON);

    SubtractFromPlayerCash(_ShopPlayer, Weapon->Price);
    DialogEnable(Dialog->Children[ 4 ], GetPlayerCash(_ShopPlayer) >= Weapon->Price);

    auto PlayerCash = std::to_string(GetPlayerCash(_ShopPlayer));
    DialogSend(DialogShop->Children[ 12 ], WM_SETTEXT, (void*)&PlayerCash, 0);
}


//-----------------------------------------------------------------------------
// Name: ShopBuyItem()
// Desc:
//-----------------------------------------------------------------------------

static void ShopBuyItem(Dialog_t* Dialog, Item_t* Item)
{
    BackpackAdd(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM, Item->BundleSize, (void*)Item);
    AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM);

    SubtractFromPlayerCash(_ShopPlayer, Item->Price);
    DialogEnable(Dialog->Children[ 4 ], GetPlayerCash(_ShopPlayer) >= Item->Price);

    auto PlayerCash = std::to_string(GetPlayerCash(_ShopPlayer));
    DialogSend(DialogShop->Children[ 12 ], WM_SETTEXT, (void*)&PlayerCash, 0);
}


//-----------------------------------------------------------------------------
// Name: IsTheWeaponTabSelected()
// Desc:
//-----------------------------------------------------------------------------

static bool IsTheWeaponTabSelected(Dialog_t* Dialog)
{
    return (Dialog->Children[1]->Flags & DIALOG_TABSELECTED) != 0;
}


//-----------------------------------------------------------------------------
// Name: CanPlayerBuyItem()
// Desc:
//-----------------------------------------------------------------------------

static size_t CanPlayerBuyItem(Dialog_t* Dialog)
{
    Weapon_t* Weapon;
    Item_t* Item;
    size_t ModelID;

    GetObjectModels(_DialogObject.ObjectID, &ModelID, nullptr);
    if (ModelID == g_ModelBuyFullVersion)
        return 0;

    DialogSend(Dialog->Children[ 3 ], LB_GETCURSEL, 0, 0);
    auto CurSel = DialogRetValue();
    if (CurSel == LB_ERR)
        return 0;

    DialogSend(Dialog->Children[ 3 ], LB_GETITEMDATA, (void*)CurSel, 0);

    if (IsTheWeaponTabSelected(Dialog))
    {
        Weapon = (Weapon_t*)DialogRetValue();
        return (GetPlayerCash(_ShopPlayer) >= Weapon->Price);
    }

    Item = (Item_t*)DialogRetValue();

    if (ITEMTYPE_POSSESS == Item->Type)
    {
        return (GetPlayerCash(_ShopPlayer) >= Item->Price &&
                (-1 == BackpackGetAbsoluteIndex(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM, (void*)Item)));
    }

    return (GetPlayerCash(_ShopPlayer) >= Item->Price);
}


//-----------------------------------------------------------------------------
// Name: CanPlayerSellItem()
// Desc:
//-----------------------------------------------------------------------------

static size_t CanPlayerSellItem(Dialog_t* Dialog)
{
    DialogSend(Dialog->Children[ 6 ], LB_GETCURSEL, 0, 0);
    auto CurSel = DialogRetValue();
    if (CurSel == LB_ERR)
        return 0;

    if (IsTheWeaponTabSelected(Dialog))
        return (-1 != CurSel) && (CurSel > 0);  // The item at index 0 is always the unsellable Small Missiles

    return (-1 != CurSel);
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
// Name: OnDestroy()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnDestroy(Dialog_t* Dialog, void* Param1, void* Param2)
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
    if (WA_ACTIVE == (size_t)Param1)
    {
        DialogShowCursor(1);

        DialogUpdateFlags(Dialog->Children[1], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);
        DialogUpdateFlags(Dialog->Children[2], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);

        AddGameWeaponsToListBox(Dialog->Children[ 3 ]);
        AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON);

        DialogSend(Dialog->Children[ 3 ], LB_SETCURSEL, 0, 0);
        DialogEnable(Dialog->Children[ 4 ], CanPlayerBuyItem(Dialog));
        DialogEnable(Dialog->Children[ 5 ], CanPlayerSellItem(Dialog));
    }

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickWeapons()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickWeapons(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogUpdateFlags(Dialog->Children[1], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);
    DialogUpdateFlags(Dialog->Children[2], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);

    AddGameWeaponsToListBox(Dialog->Children[ 3 ]);
    AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON);

    DialogSend(Dialog->Children[ 3 ], LB_SETCURSEL, 0, 0);

    DialogEnable(Dialog->Children[ 4 ], CanPlayerBuyItem(Dialog));
    DialogEnable(Dialog->Children[ 5 ], CanPlayerSellItem(Dialog));

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickItems()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickItems(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogUpdateFlags(Dialog->Children[1], DIALOG_TABUNSELECTED, DIALOG_TABSELECTED);
    DialogUpdateFlags(Dialog->Children[2], DIALOG_TABSELECTED, DIALOG_TABUNSELECTED);

    AddGameItemsToListBox(Dialog->Children[ 3 ]);
    AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM);

    DialogSend(Dialog->Children[ 3 ], LB_SETCURSEL, 0, 0);

    DialogEnable(Dialog->Children[ 4 ], CanPlayerBuyItem(Dialog));
    DialogEnable(Dialog->Children[ 5 ], CanPlayerSellItem(Dialog));

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnSelectWeaponItem()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnSelectWeaponItem(Dialog_t* Dialog, void* Param1, void* Param2)
{
    Weapon_t* Weapon;
    Item_t* Item;

    DialogSend(Dialog->Children[ 3 ], LB_GETCURSEL, 0, 0);
    auto CurSel = DialogRetValue();
    if (CurSel == LB_ERR)
        return 0;

    DialogSend(Dialog->Children[ 3 ], LB_GETITEMDATA, (void*)CurSel, 0);

    if (IsTheWeaponTabSelected(Dialog))
    {
        Weapon = (Weapon_t*)DialogRetValue();

        SetObjectModels(_DialogObject.ObjectID, Weapon->ProjectileModelIDLOD0, -1);
        _DialogObject.Scale = Weapon->Scale;

        if (Weapon->ProjectileModelIDLOD0 == -1)
        {
            Dialog->Children[4]->Name = "Not In Demo";
            DialogSend(Dialog->Children[ 4 ], WM_SIZE, 0, 0);

            SetObjectModels(_DialogObject.ObjectID, g_ModelBuyFullVersion, -1);
            _DialogObject.Scale = 4.0f;
        }
        else
        {
            Dialog->Children[4]->Name = "Buy";
            DialogSend(Dialog->Children[ 4 ], WM_SIZE, 0, 0);
        }

        SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModel);

        Dialog->Children[ 8 ]->Data = (void*)&_DialogObject;

        DialogSend(Dialog->Children[ 14 ], WM_SETTEXT, (void*)&Weapon->Name, 0);

        auto BundleSize = std::to_string(Weapon->BundleSize);
        DialogSend(Dialog->Children[ 16 ], WM_SETTEXT, (void*)&BundleSize, 0);

        auto Price = std::to_string(Weapon->Price);
        DialogSend(Dialog->Children[ 18 ], WM_SETTEXT, (void*)&Price, 0);

        DialogSend(Dialog->Children[ 20 ], WM_SETTEXT, (void*)&Weapon->Line0, 0);
        DialogSend(Dialog->Children[ 21 ], WM_SETTEXT, (void*)&Weapon->Line1, 0);
        DialogSend(Dialog->Children[ 22 ], WM_SETTEXT, (void*)&Weapon->Line2, 0);
        DialogSend(Dialog->Children[ 23 ], WM_SETTEXT, (void*)&Weapon->Line3, 0);
    }
    else
    {
        Item = (Item_t*)DialogRetValue();

        SetObjectModels(_DialogObject.ObjectID, Item->ModelID, -1);
        _DialogObject.Scale = Item->Scale;

        if (Item->ModelID == -1)
        {
            Dialog->Children[4]->Name = "Not In Demo";
            DialogSend(Dialog->Children[ 4 ], WM_SIZE, 0, 0);

            SetObjectModels(_DialogObject.ObjectID, g_ModelBuyFullVersion, -1);
            _DialogObject.Scale = 4.0f;
        }
        else
        {
            Dialog->Children[4]->Name = "Buy";
            DialogSend(Dialog->Children[ 4 ], WM_SIZE, 0, 0);
        }

        if (g_ModelShieldHeavy == Item->ModelID || g_ModelShieldMedium == Item->ModelID || g_ModelShieldLight == Item->ModelID)
            SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModelTrans);
        else
            SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModel);

        Dialog->Children[ 8 ]->Data = (void*)&_DialogObject;

        DialogSend(Dialog->Children[ 14 ], WM_SETTEXT, (void*)&Item->Name, 0);

        auto BundleSize = std::to_string(Item->BundleSize);
        DialogSend(Dialog->Children[ 16 ], WM_SETTEXT, (void*)&BundleSize, 0);

        auto Price = std::to_string(Item->Price);
        DialogSend(Dialog->Children[ 18 ], WM_SETTEXT, (void*)&Price, 0);

        DialogSend(Dialog->Children[ 20 ], WM_SETTEXT, (void*)&Item->Line0, 0);
        DialogSend(Dialog->Children[ 21 ], WM_SETTEXT, (void*)&Item->Line1, 0);
        DialogSend(Dialog->Children[ 22 ], WM_SETTEXT, (void*)&Item->Line2, 0);
        DialogSend(Dialog->Children[ 23 ], WM_SETTEXT, (void*)&Item->Line3, 0);
    }

    DialogEnable(Dialog->Children[ 4 ], CanPlayerBuyItem(Dialog));

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickBuy()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickBuy(Dialog_t* Dialog, void* Param1, void* Param2)
{
    Weapon_t* Weapon;
    Item_t* Item;

    if (_DialogObject.ObjectID == g_ModelBuyFullVersion)        // don't buy it coz this must be a demo build
        return DIALOG_MSGHANDLED;

    DialogSend(Dialog->Children[ 3 ], LB_GETCURSEL, 0, 0);
    auto CurSel = DialogRetValue();
    if (CurSel == LB_ERR)
        return DIALOG_MSGHANDLED;

    DialogSend(Dialog->Children[ 3 ], LB_GETITEMDATA, (void*)CurSel, 0);

    if (IsTheWeaponTabSelected(Dialog))
    {
        Weapon = (Weapon_t*)DialogRetValue();
        ShopBuyWeapon(Dialog, Weapon);
    }
    else
    {
        Item = (Item_t*)DialogRetValue();
        ShopBuyItem(Dialog, Item);
    }

    DialogEnable(Dialog->Children[ 4 ], CanPlayerBuyItem(Dialog));
    DialogEnable(Dialog->Children[ 5 ], CanPlayerSellItem(Dialog));

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickSell()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickSell(Dialog_t* Dialog, void* Param1, void* Param2)
{
    Weapon_t* Weapon;
    Item_t* Item;
    size_t Amount;

    DialogSend(Dialog->Children[ 6 ], LB_GETCURSEL, 0, 0);
    auto CurSel = DialogRetValue();
    if (CurSel == LB_ERR)
        return DIALOG_MSGHANDLED;

    if (IsTheWeaponTabSelected(Dialog))
    {
        Weapon = (Weapon_t*)BackpackGetItem(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON, CurSel);
        Amount = BackpackGetAmountLeft(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON, (void*)Weapon);

        AddToPlayerCash(_ShopPlayer, (Amount * Weapon->Price) / Weapon->BundleSize);

        BackpackRemove(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON, (void*)Weapon);
        AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEWEAPON);
    }
    else
    {
        Item   = (Item_t*)BackpackGetItem(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM, CurSel);
        Amount = BackpackGetAmountLeft(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM, (void*)Item);

        AddToPlayerCash(_ShopPlayer, (Amount * Item->Price) / Item->BundleSize);

        BackpackRemove(GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM, (void*)Item);
        AddBackpackItemsToListBox(Dialog->Children[ 6 ], GetPlayerBackpack(_ShopPlayer), BPITEM_TYPEITEM);
    }

    auto PlayerCash = GetPlayerCash(_ShopPlayer);
    DialogSend(DialogShop->Children[ 12 ], WM_SETTEXT, (void*)&PlayerCash, 0);

    DialogEnable(Dialog->Children[ 4 ], CanPlayerBuyItem(Dialog));
    DialogEnable(Dialog->Children[ 5 ], CanPlayerSellItem(Dialog));

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnSelectBackpackItem()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnSelectBackpackItem(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogEnable(Dialog->Children[ 5 ], CanPlayerSellItem(Dialog));
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickOK()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickOK(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogHide(Dialog);

    if (!SetNextShopPlayer())
    {
        if (AutoDefenseSetFirstPlayer())
            DialogShow(DialogProperties, 0);
        else
            SetGameState(&g_StateBeginTurn);
    }
    else
        DialogShow(Dialog, 0);

    return DIALOG_MSGHANDLED;
}
