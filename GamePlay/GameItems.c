//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "GameTypes.h"
#include "ItemTypes.h"

#include "LoadingScreen.h"
#include "GameModels.h"

#include "Guidance.h"
#include "Batteries.h"
#include "Parachutes.h"
#include "Shields.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Item_t ItemHeatSeekWeak;
Item_t ItemHeatSeekStrong;
Item_t ItemParachutes;
Item_t ItemBatteries;
Item_t ItemShieldsLight;
Item_t ItemShieldsMedium;
Item_t ItemShieldsHeavy;
Item_t ItemAutoDefense;
Item_t ItemFuel;


//-----------------------------------------------------------------------------
// Name: CreateItems()
// Desc:
//-----------------------------------------------------------------------------

size_t CreateItems(void)
{
    LogWrite("CreateItems() creating...\n");

    BeginLoadingScreen(TextureGame, "Creating Items...", 0, 9);

    ItemHeatSeekWeak.Name             = "Weak Heat Seeking";
    ItemHeatSeekWeak.Type             = ITEMTYPE_GUIDANCE;
    ItemHeatSeekWeak.Price            = 15000;
    ItemHeatSeekWeak.BundleSize       = 5;
    ItemHeatSeekWeak.Line0            = "When the player has activated this item,";
    ItemHeatSeekWeak.Line1            = "fired projectiles will move toward the";
    ItemHeatSeekWeak.Line2            = "closest enemy tank, but only after the";
    ItemHeatSeekWeak.Line3            = "projectile has reached apogee.";
    ItemHeatSeekWeak.Scale            = 10.0f;
    ItemHeatSeekWeak.ModelID          = g_ModelHeatSeekingWeak;
    ItemHeatSeekWeak.Use              = 0;
    ItemHeatSeekWeak.Activate         = ActivateGuidance;
    UpdateLoadingScreen(1);

    ItemHeatSeekStrong.Name          = "Strong Heat Seeking";
    ItemHeatSeekStrong.Type          = ITEMTYPE_GUIDANCE;
    ItemHeatSeekStrong.Price         = 25000;
    ItemHeatSeekStrong.BundleSize    = 3;
    ItemHeatSeekStrong.Line0         = "This item is the same as the Weak Heat";
    ItemHeatSeekStrong.Line1         = "Seeking except that the projectile moves";
    ItemHeatSeekStrong.Line2         = "toward the closest tank in bigger steps,";
    ItemHeatSeekStrong.Line3         = "and searches a larger radius for targets.";
    ItemHeatSeekStrong.Scale         = 10.0f;
    ItemHeatSeekStrong.ModelID       = g_ModelHeatSeekingStrong;
    ItemHeatSeekStrong.Use           = 0;
    ItemHeatSeekStrong.Activate      = ActivateGuidance;
    UpdateLoadingScreen(2);

    ItemParachutes.Name                = "Parachutes";
    ItemParachutes.Type                = ITEMTYPE_OTHER;
    ItemParachutes.Price               = 10000;
    ItemParachutes.BundleSize          = 8;
    ItemParachutes.Line0               = "Parachutes cause a falling tank not to";
    ItemParachutes.Line1               = "suffer any falling damage.  If the player";
    ItemParachutes.Line2               = "has activated this item a parachute will";
    ItemParachutes.Line3               = "be deployed when a tank is falling.";
    ItemParachutes.Scale               = 3.0;
    ItemParachutes.ModelID             = g_ModelParachuteLOD0;
    ItemParachutes.Use                 = 0;
    ItemParachutes.Activate            = ActivateParachutes;
    UpdateLoadingScreen(3);

    ItemBatteries.Name                 = "Batteries";
    ItemBatteries.Type                 = ITEMTYPE_OTHER;
    ItemBatteries.Price                = 7500;
    ItemBatteries.BundleSize           = 10;
    ItemBatteries.Line0                = "A player uses batteries to recharge the";
    ItemBatteries.Line1                = "tank and its shield when either has";
    ItemBatteries.Line2                = "suffered damage.";
    ItemBatteries.Line3                = " ";
    ItemBatteries.Scale                = 20.0f;
    ItemBatteries.ModelID              = g_ModelBatteries;
    ItemBatteries.Use                  = UseBatteries;
    ItemBatteries.Activate             = 0;
    UpdateLoadingScreen(4);

    ItemShieldsLight.Name              = "Light Shields";
    ItemShieldsLight.Type              = ITEMTYPE_SHIELD;
    ItemShieldsLight.Price             = 15000;
    ItemShieldsLight.BundleSize        = 4;
    ItemShieldsLight.Line0             = "When a player has activated this item all";
    ItemShieldsLight.Line1             = "damage otherwise suffered by the player's";
    ItemShieldsLight.Line2             = "tank will be suffered by their Light Shield";
    ItemShieldsLight.Line3             = "instead.";
    ItemShieldsLight.Scale             = 3.0f;
    ItemShieldsLight.ModelID           = g_ModelShieldLight;
    ItemShieldsLight.Use               = 0;
    ItemShieldsLight.Activate          = ActivateShields;
    UpdateLoadingScreen(5);

    ItemShieldsMedium.Name             = "Medium Shields";
    ItemShieldsMedium.Type             = ITEMTYPE_SHIELD;
    ItemShieldsMedium.Price            = 20000;
    ItemShieldsMedium.BundleSize       = 3;
    ItemShieldsMedium.Line0            = "This item is the same as the Light";
    ItemShieldsMedium.Line1            = "Shield except that this shield can absorb";
    ItemShieldsMedium.Line2            = "more damage.";
    ItemShieldsMedium.Line3            = " ";
    ItemShieldsMedium.Scale            = 3.0f;
    ItemShieldsMedium.ModelID          = g_ModelShieldMedium;
    ItemShieldsMedium.Use              = 0;
    ItemShieldsMedium.Activate         = ActivateShields;
    UpdateLoadingScreen(6);

    ItemShieldsHeavy.Name              = "Heavy Shields";
    ItemShieldsHeavy.Type              = ITEMTYPE_SHIELD;
    ItemShieldsHeavy.Price             = 30000;
    ItemShieldsHeavy.BundleSize        = 2;
    ItemShieldsHeavy.Line0             = "This item is the same as the Medium";
    ItemShieldsHeavy.Line1             = "Shield except that this shield can absorb";
    ItemShieldsHeavy.Line2             = "a lot more damage.";
    ItemShieldsHeavy.Line3             = " ";
    ItemShieldsHeavy.Scale             = 3.0f;
    ItemShieldsHeavy.ModelID           = g_ModelShieldHeavy;
    ItemShieldsHeavy.Use               = 0;
    ItemShieldsHeavy.Activate          = ActivateShields;
    UpdateLoadingScreen(7);

    ItemAutoDefense.Name               = "Automatic Defense";
    ItemAutoDefense.Type               = ITEMTYPE_POSSESS;
    ItemAutoDefense.Price              = 5000;
    ItemAutoDefense.BundleSize         = 1;
    ItemAutoDefense.Line0              = "Normally a player can only activate/use";
    ItemAutoDefense.Line1              = "items when it's their turn.  Auto Defense";
    ItemAutoDefense.Line2              = "allows a player to activate/use items before";
    ItemAutoDefense.Line3              = "the first shot is fired of each round.";
    ItemAutoDefense.Scale              = 30.0f;
    ItemAutoDefense.ModelID            = g_ModelAutoDefense;
    ItemAutoDefense.Use                = 0;
    ItemAutoDefense.Activate           = 0;
    UpdateLoadingScreen(8);

    ItemFuel.Name                      = "Fuel";
    ItemFuel.Type                      = ITEMTYPE_OTHER;
    ItemFuel.Price                     = 10000;
    ItemFuel.BundleSize                = 10;
    ItemFuel.Line0                     = "Fuel allows a player to move their tank. If";
    ItemFuel.Line1                     = "a player has fuel they'll be given the";
    ItemFuel.Line2                     = "option to either shoot at a player or move";
    ItemFuel.Line3                     = "their tank for their turn.";
    ItemFuel.Scale                     = 18.0f;
    ItemFuel.ModelID                   = g_ModelFuel;
    ItemFuel.Use                       = 0;
    ItemFuel.Activate                  = 0;

    EndLoadingScreen("Creating Items...Done", 9);
    LogWrite("CreateItems() created\n");

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteItems()
// Desc:
//-----------------------------------------------------------------------------

void DeleteItems(void)
{
    LogWrite("DeleteItems() deleting...\n");

    LogWrite("DeleteItems() deleted\n");

}


//-----------------------------------------------------------------------------
// Name: AddGameItemsToListBox()
// Desc:
//-----------------------------------------------------------------------------

void AddGameItemsToListBox(Dialog_t* ListBox)
{
    DialogSend(ListBox, LB_RESETCONTENT, 0, 0);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemHeatSeekWeak.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)0, &ItemHeatSeekWeak);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemHeatSeekStrong.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)1, &ItemHeatSeekStrong);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemParachutes.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)2, &ItemParachutes);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemBatteries.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)3, &ItemBatteries);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemShieldsLight.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)4, &ItemShieldsLight);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemShieldsMedium.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)5, &ItemShieldsMedium);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemShieldsHeavy.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)6, &ItemShieldsHeavy);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemAutoDefense.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)7, &ItemAutoDefense);

    DialogSend(ListBox, LB_ADDSTRING, (void*)&ItemFuel.Name, nullptr);
    DialogSend(ListBox, LB_SETITEMDATA, (void*)8, &ItemFuel);
}


//-----------------------------------------------------------------------------
// Name: GetShieldName()
// Desc:
//-----------------------------------------------------------------------------

std::string GetShieldName(float Type)
{
    if (SHIELDTYPE_LIGHT == Type)
        return "Light";

    if (SHIELDTYPE_MEDIUM == Type)
        return "Medium";

    if (SHIELDTYPE_HEAVY == Type)
        return "Heavy";

    return "None";
}
