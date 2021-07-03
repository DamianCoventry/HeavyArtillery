//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 15 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>

#include "PlayerManager.h"

#include "GameItems.h"


//----------------------------------------------------------------------------------------
// Name: RepairBotsTank()
// Desc: 
//----------------------------------------------------------------------------------------

static void RepairBotsTank( size_t PlyrIndex )
{
    Item_t *Item;
    size_t Index;

    while( GetPlayerTankPower( PlyrIndex ) < 100.0f )
    {
        Index = BackpackGetRelativeIndex( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, (void *)&ItemBatteries );
        if( -1 == Index )
            break;

        Item = (Item_t*)BackpackGetItem( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Index );
        if( Item && Item->Use )
            Item->Use( Item, GetPlayerBackpack( PlyrIndex ), PlyrIndex );
    }

    if( GetPlayerShieldType( PlyrIndex ) != SHIELDTYPE_NONE )
    {
        while( GetPlayerShieldPower( PlyrIndex ) < 100.0f )
        {
            Index = BackpackGetRelativeIndex( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, (void *)&ItemBatteries );
            if( -1 == Index )
                break;

            Item = (Item_t*)BackpackGetItem( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Index );
            if( Item && Item->Use )
                Item->Use( Item, GetPlayerBackpack( PlyrIndex ), PlyrIndex );
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: EnableBestShield()
// Desc: 
//----------------------------------------------------------------------------------------

static void EnableBestShield( size_t PlyrIndex )
{
    Item_t *Item;
    Item_t *Shields[] = { &ItemShieldsHeavy, &ItemShieldsMedium, &ItemShieldsLight, nullptr };
    size_t Index, i;

    for( i = 0; Shields[ i ]; i++ )
    {
        Index = BackpackGetRelativeIndex( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Shields[ i ] );
        if( -1 == Index )
            continue;

        Item = (Item_t*)BackpackGetItem( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Index );
        if( Item && Item->Activate )
        {
            Item->Activate( Item, GetPlayerBackpack( PlyrIndex ), PlyrIndex );
            break;
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: EnableBackpackItem()
// Desc: 
//----------------------------------------------------------------------------------------

static void EnableBackpackItem( size_t PlyrIndex, Item_t *Item )
{
    size_t Index;

    Index = BackpackGetRelativeIndex( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Item );
    if( -1 == Index )
        return;

    Item = (Item_t*)BackpackGetItem( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Index );
    if( Item && Item->Activate )
        Item->Activate( Item, GetPlayerBackpack( PlyrIndex ), PlyrIndex );
}


//----------------------------------------------------------------------------------------
// Name: EnableBestGuidance()
// Desc: 
//----------------------------------------------------------------------------------------

static void EnableBestGuidance( size_t PlyrIndex )
{
    Item_t *Item;
    Item_t *Guidances[] = { &ItemHeatSeekStrong, &ItemHeatSeekWeak, nullptr };
    size_t Index, i;

    for( i = 0; Guidances[ i ]; i++ )
    {
        Index = BackpackGetRelativeIndex( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Guidances[ i ] );
        if( -1 == Index )
            continue;

        Item = (Item_t*)BackpackGetItem( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Index );
        if( Item && Item->Activate )
        {
            Item->Activate( Item, GetPlayerBackpack( PlyrIndex ), PlyrIndex );
            break;
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: AllowBotToBuyFromShop()
// Desc: 
//----------------------------------------------------------------------------------------

void AllowBotToSetupDefense( size_t PlyrIndex )
{
    // use batteries repair to tank
    RepairBotsTank( PlyrIndex );

    // if there's no current shield enable the best shield
    if( GetPlayerShieldType( PlyrIndex ) == SHIELDTYPE_NONE )
        EnableBestShield( PlyrIndex );

    // activate parachutes if they're not activated
    EnableBackpackItem( PlyrIndex, &ItemParachutes );

    // activate guidance if it's not activated
    EnableBestGuidance( PlyrIndex );
}
