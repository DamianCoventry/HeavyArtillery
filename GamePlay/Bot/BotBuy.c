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
#include "Backpack.h"

#include "GameItems.h"
#include "GameWeapons.h"

#include "ItemTypes.h"

#define CHEAPEST_WEAPON         3500            // this is the large missile


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Weapon_t *_WeaponsSortedByPrice[] = 
{
    &WeaponDeathsHead,
    &WeaponLeapFrogLarge,
    &WeaponMIRV,
    &WeaponAirStrikeLarge,
    &WeaponNukeLarge,
    &WeaponBouncerLarge,
    &WeaponNukeSmall,
    &WeaponLeapFrogSmall,
    &WeaponAirStrikeSmall,
    &WeaponRailgun,
    &WeaponNapalmLarge,
    &WeaponRollerLarge,
    &WeaponFunkyBomb,
    &WeaponBouncerSmall,
    &WeaponNapalmSmall,
    &WeaponRollerSmall,
    &WeaponMissileLarge,
    nullptr
};

Weapon_t *_WeaponsSortedByDamage[] = 
{
    &WeaponDeathsHead,
    &WeaponNapalmLarge,
    &WeaponAirStrikeLarge,
    &WeaponAirStrikeSmall,
    &WeaponNukeLarge,
    &WeaponNukeSmall,
    &WeaponRailgun,
    &WeaponLeapFrogLarge,
    &WeaponNapalmSmall,
    &WeaponFunkyBomb,
    &WeaponMIRV,
    &WeaponLeapFrogSmall,
    &WeaponMissileLarge,
    &WeaponRollerLarge,
    &WeaponBouncerLarge,
    &WeaponRollerSmall,
    &WeaponBouncerSmall,
    nullptr
};

Weapon_t *_WeaponsSortedByProjectile[] = 
{
    &WeaponAirStrikeLarge,
    &WeaponFunkyBomb,
    &WeaponDeathsHead,
    &WeaponAirStrikeSmall,
    &WeaponNapalmLarge,
    &WeaponLeapFrogLarge,
    &WeaponNapalmSmall,
    &WeaponMIRV,
    &WeaponLeapFrogSmall,
    &WeaponNukeLarge,
    &WeaponNukeSmall,
    &WeaponRailgun,
    &WeaponMissileLarge,
    &WeaponRollerLarge,
    &WeaponBouncerLarge,
    &WeaponRollerSmall,
    &WeaponBouncerSmall,
    nullptr
};


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static Item_t *_ItemsSortedByUsefulness[] = 
{
    &ItemBatteries,
    &ItemShieldsHeavy,
    &ItemShieldsMedium,
    &ItemShieldsLight,
    &ItemHeatSeekStrong,
    &ItemHeatSeekWeak,
    &ItemAutoDefense,
    &ItemParachutes,
    &ItemFuel,
    nullptr
};

static Item_t *_ItemsSortedByBestDefense[] = 
{
    &ItemShieldsHeavy,
    &ItemShieldsMedium,
    &ItemShieldsLight,
    &ItemAutoDefense,
    &ItemBatteries,
    &ItemParachutes,
    &ItemFuel,
    &ItemHeatSeekStrong,
    &ItemHeatSeekWeak,
    nullptr
};


//-----------------------------------------------------------------------------
// Name: SetupBotLookups()
// Desc: 
//-----------------------------------------------------------------------------

void SetupBotLookups( void )
{
}


//----------------------------------------------------------------------------------------
// Name: BuyWeapon()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t BuyWeapon( size_t PlyrIndex, Weapon_t *Weapon, size_t Amount )
{
    if( Weapon->ProjectileModelIDLOD0 == -1 )
        return 0;

    if( GetPlayerCash( PlyrIndex ) >= Weapon->Price )
    {
        BackpackAdd( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEWEAPON, Weapon->BundleSize, (void *)Weapon );
        SubtractFromPlayerCash( PlyrIndex, Weapon->Price );

        return 1;
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: BuyItem()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t BuyItem( size_t PlyrIndex, Item_t *Item, size_t Amount )
{
    if( Item->ModelID == -1 )
        return 0;

    if( GetPlayerCash( PlyrIndex ) >= Item->Price )
    {
        BackpackAdd( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, Item->BundleSize, (void *)Item );
        SubtractFromPlayerCash( PlyrIndex, Item->Price );

        return 1;
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: BuyBaseItems()
// Desc: 
//----------------------------------------------------------------------------------------

static void BuyBaseItems( size_t PlyrIndex )
{
    BuyItem( PlyrIndex, &ItemAutoDefense, 1 );
    BuyItem( PlyrIndex, &ItemBatteries, 1 );
    BuyItem( PlyrIndex, &ItemShieldsHeavy, 1 );
    BuyItem( PlyrIndex, &ItemShieldsMedium, 1 );
    BuyItem( PlyrIndex, &ItemShieldsLight, 1 );
    BuyItem( PlyrIndex, &ItemParachutes, 1 );
}


//----------------------------------------------------------------------------------------
// Name: BuyBaseWeapons()
// Desc: 
//----------------------------------------------------------------------------------------

static void BuyBaseWeapons( size_t PlyrIndex )
{
    BuyWeapon( PlyrIndex, &WeaponNukeLarge, 1 );
    BuyWeapon( PlyrIndex, &WeaponNukeSmall, 1 );
    BuyWeapon( PlyrIndex, &WeaponMissileLarge, 1 );
    BuyWeapon( PlyrIndex, &WeaponMissileSmall, 1 );
    BuyWeapon( PlyrIndex, &WeaponRailgun, 1 );
    BuyWeapon( PlyrIndex, &WeaponNapalmSmall, 1 );
}


//----------------------------------------------------------------------------------------
// Name: BuyWeapons()
// Desc: 
//----------------------------------------------------------------------------------------

static void BuyWeapons( size_t PlyrIndex, size_t Allowance, Weapon_t **WeaponList )
{
    size_t i;
    size_t Total = 0;

    for( i = 0; WeaponList[ i ]; i++ )
    {
        if( BuyWeapon( PlyrIndex, WeaponList[ i ], rand() % 3 + 1 ) )
        {
            Total += WeaponList[ i ]->Price;
            if( Total >= Allowance )
                break;
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: BuyItems()
// Desc: 
//----------------------------------------------------------------------------------------

static void BuyItems( size_t PlyrIndex, size_t Allowance, Item_t **ItemList )
{
    size_t i;
    size_t Total = 0;

    for( i = 0; ItemList[ i ]; i++ )
    {
        if( BuyItem( PlyrIndex, ItemList[ i ], rand() % 3 + 1 ) )
        {
            Total += ItemList[ i ]->Price;
            if( Total >= Allowance )
                break;
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: AllowBotToBuyFromShop()
// Desc: 
//----------------------------------------------------------------------------------------

void AllowBotToBuyFromShop( size_t PlyrIndex )
{
    float WeaponPercent, ItemPercent;
    Weapon_t **WeaponList;
    Item_t **ItemList;

    // buy the base items
    if( rand() % 2 == 0 )
    {
        BuyBaseItems( PlyrIndex );
        BuyBaseWeapons( PlyrIndex );
    }
    else
    {
        BuyBaseWeapons( PlyrIndex );
        BuyBaseItems( PlyrIndex );
    }

    // randomly choose how much to spend on weapons and items
    switch( rand() % 3 )
    {
        case 0 : WeaponPercent = 0.5f; ItemPercent = 0.5f; break;
        case 1 : WeaponPercent = 0.7f; ItemPercent = 0.3f; break;
        case 2 : WeaponPercent = 0.3f; ItemPercent = 0.7f; break;
    }

    // randomly choose a weapon and item list
    switch( rand() % 2 )
    {
        case 0 : WeaponList = _WeaponsSortedByPrice; break;
        case 1 : WeaponList = _WeaponsSortedByDamage; break;
    }

    switch( rand() % 2 )
    {
        case 0 : ItemList = _ItemsSortedByUsefulness; break;
        case 1 : ItemList = _ItemsSortedByBestDefense; break;
    }

    BuyWeapons( PlyrIndex, (size_t)( (float)GetPlayerCash( PlyrIndex ) * WeaponPercent ), WeaponList );
    BuyItems( PlyrIndex, (size_t)( (float)GetPlayerCash( PlyrIndex ) * ItemPercent ), ItemList );

    // spend the rest of it
    while( GetPlayerCash( PlyrIndex ) >= CHEAPEST_WEAPON )
    {
        BuyWeapons( PlyrIndex, GetPlayerCash( PlyrIndex ), WeaponList );
        BuyItems( PlyrIndex, GetPlayerCash( PlyrIndex ), ItemList );
    }
}
