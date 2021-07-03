//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 8 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "Backpack.h"
#include "ItemTypes.h"
#include "WeaponTypes.h"

#include "GameWeapons.h"
#include "GameTypes.h"


//-----------------------------------------------------------------------------
// Name: BackpackAdd()
// Desc:
//-----------------------------------------------------------------------------

void AddBackpackItemsToListBox(Dialog_t* ListBox, Backpack_t* Backpack, size_t Type)
{
    size_t i;
    Item_t* Item;
    Weapon_t* Weapon;

    DialogSend(ListBox, LB_RESETCONTENT, 0, 0);

    for (i = 0; i < Backpack->ItemCount; i++)
    {
        if (Type != Backpack->Items[i].Type)
            continue;

        std::ostringstream oss;
        if (Type == BPITEM_TYPEWEAPON)
        {
            Weapon = (Weapon_t*)Backpack->Items[i].Item;
            if (!Weapon->Price)
                oss << Weapon->Name << " (Unlimited)";
            else
                oss << Weapon->Name << " (" << Backpack->Items[i].AmountLeft << ")";
        }
        else
        {
            Item = (Item_t*)Backpack->Items[i].Item;
            if (!Item->Price)
                oss << Item->Name << " (Unlimited)";
            else
                oss << Item->Name << " (" << Backpack->Items[i].AmountLeft << ")";
        }

        auto text = oss.str();
        DialogSend(ListBox, LB_ADDSTRING, (void*)&text, 0);
    }
}


//-----------------------------------------------------------------------------
// Name: BackpackAdd()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackAdd(Backpack_t* Backpack, size_t Type, size_t BundleSize, void* Item)
{
    size_t Index;

    // first, see if there's already an item in the backpack of the same type
    Index = BackpackGetAbsoluteIndex(Backpack, Type, Item);

    // second, if we did find an item of this type already in stock, then simply
    // increase the amount left
    if (-1 != Index)
    {
        Backpack->Items[ Index ].AmountLeft += BundleSize;
        return BACKPACK_ADDEDTOSTOCK;
    }

    // didn't find it in the backpack, add it
    Backpack->Items[ Backpack->ItemCount ].AmountLeft   = BundleSize;
    Backpack->Items[ Backpack->ItemCount ].Item         = Item;
    Backpack->Items[ Backpack->ItemCount ].Type         = Type;

    Backpack->ItemCount++;
    return BACKPACK_NEWITEM;
}


//-----------------------------------------------------------------------------
// Name: BackpackRemove()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackRemove(Backpack_t* Backpack, size_t Type, void* Item)
{
    size_t Index;

    // first, find this item in the backpack
    Index = BackpackGetAbsoluteIndex(Backpack, Type, Item);
    if (-1 == Index)
        return 0;

    // second, remove it by shifting everything down the array
    memcpy(&Backpack->Items[ Index ]
           , &Backpack->Items[ Index + 1 ]
           , sizeof(BPItem_t) * (Backpack->ItemCount - (Index + 1)));

    Backpack->ItemCount--;

    // erase the old last item
    memset(&Backpack->Items[ Backpack->ItemCount ], 0, sizeof(BPItem_t));

    return 1;
}


//-----------------------------------------------------------------------------
// Name: BackpackGetAbsoluteIndex()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackGetAbsoluteIndex(Backpack_t* Backpack, size_t Type, void* Item)
{
    size_t i;

    for (i = 0; i < Backpack->ItemCount; i++)
    {
        if (Type == Backpack->Items[ i ].Type && Item == Backpack->Items[ i ].Item)
            return i;
    }

    return -1;
}


//-----------------------------------------------------------------------------
// Name: BackpackGetItem()
// Desc:
//-----------------------------------------------------------------------------

void* BackpackGetItem(Backpack_t* Backpack, size_t Type, size_t ItemIndex)
{
    size_t i, Index;

    Index = 0;
    for (i = 0; i < Backpack->ItemCount; i++)
    {
        if (Type == Backpack->Items[ i ].Type)
        {
            if (Index == ItemIndex)
                return (void*)Backpack->Items[ i ].Item;

            Index++;
        }
    }

    return nullptr;
}


//-----------------------------------------------------------------------------
// Name: BackpackUse()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackUse(Backpack_t* Backpack, size_t Type, void* Item)
{
    size_t Index;

    Index = BackpackGetAbsoluteIndex(Backpack, Type, Item);

    if (-1 == Index)
        return BACKPACK_NOMORELEFT;

    if (BPITEM_TYPEWEAPON == Type)              // no point subtracting an unlimited weapon/item
    {
        if (((Weapon_t*)Item)->Price == 0)
            return 0;
    }
    else
    {
        if (((Item_t*)Item)->Price == 0)
            return 0;
    }

    if (--Backpack->Items[ Index ].AmountLeft <= 0)
    {
        BackpackRemove(Backpack, Type, Item);
        return BACKPACK_NOMORELEFT;
    }

    return BACKPACK_MORELEFT;
}


//-----------------------------------------------------------------------------
// Name: BackpackGetAmountLeft()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackGetAmountLeft(Backpack_t* Backpack, size_t Type, void* Item)
{
    size_t Index;

    Index = BackpackGetAbsoluteIndex(Backpack, Type, Item);
    if (-1 == Index)
        return -1;

    return Backpack->Items[ Index ].AmountLeft;
}


//-----------------------------------------------------------------------------
// Name: BackpackToggleActivate()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackToggleActivate(Backpack_t* Backpack, size_t Index)
{
    size_t i;
    Item_t* Item1, *Item2;

    if (Index < 0 || Index >= Backpack->ItemCount)
        return BACKPACK_DEACTIVATED;

    if (Backpack->Items[ Index ].Activated)
    {
        Backpack->Items[ Index ].Activated = 0;
        return BACKPACK_DEACTIVATED;
    }

    // items of the same type form a mutually exclusive group.  therefore if
    // we activate an item we must deactivate the other items of the same type

    for (i = 0; i < Backpack->ItemCount; i++)
    {
        if (i != Index && BPITEM_TYPEITEM == Backpack->Items[ i ].Type)
        {
            Item1 = (Item_t*)Backpack->Items[ i ].Item;
            Item2 = (Item_t*)Backpack->Items[ Index ].Item;
            if (Item1->Type == Item2->Type)
                Backpack->Items[ i ].Activated = 0;
        }
    }

    Backpack->Items[ Index ].Activated = 1;
    return BACKPACK_ACTIVATED;
}


//-----------------------------------------------------------------------------
// Name: BackpackIsOnlyWeaponLeft()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackIsOnlyWeaponLeft(Backpack_t* Backpack, void* Item)
{
    size_t Count;
    size_t i;
    Weapon_t* Weapon;

    // is this weapon in the backpack?
    if (-1 == BackpackGetAbsoluteIndex(Backpack, BPITEM_TYPEWEAPON, Item))
        return 0;       // nope, it's not

    // if it is in here then check to see if it's the only
    // limited weapon left
    for (i = 0, Count = 0; i < Backpack->ItemCount; i++)
    {
        if (BPITEM_TYPEWEAPON == Backpack->Items[ i ].Type)
        {
            Weapon = (Weapon_t*)Backpack->Items[ i ].Item;
            if (Weapon->Price)
                Count++;
        }
    }

    return (1 == Count);
}


//-----------------------------------------------------------------------------
// Name: BackpackGetRelativeIndex()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackGetRelativeIndex(Backpack_t* Backpack, size_t Type, void* Item)
{
    size_t i;
    size_t Index;

    for (i = 0, Index = 0; i < Backpack->ItemCount; i++)
    {
        if (Backpack->Items[ i ].Type == Type)
        {
            if (Backpack->Items[ i ].Item == Item)
                return Index;

            Index++;
        }
    }

    return -1;
}


//-----------------------------------------------------------------------------
// Name: BackpackGetRelativeIndex()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackGetWeaponCount(Backpack_t* Backpack)
{
    size_t i;
    size_t Count;

    Count = 0;
    for (i = 0; i < Backpack->ItemCount; i++)
    {
        if (Backpack->Items[ i ].Type == BPITEM_TYPEWEAPON)
            Count++;
    }

    return Count;
}


//-----------------------------------------------------------------------------
// Name: BackpackGetRelativeIndex()
// Desc:
//-----------------------------------------------------------------------------

size_t BackpackGetItemCount(Backpack_t* Backpack)
{
    size_t i;
    size_t Count;

    Count = 0;
    for (i = 0; i < Backpack->ItemCount; i++)
    {
        if (Backpack->Items[ i ].Type == BPITEM_TYPEITEM)
            Count++;
    }

    return Count;
}

//-----------------------------------------------------------------------------
// Name: BackpackGetRelativeIndex()
// Desc:
//-----------------------------------------------------------------------------

void SetupBackpackForNewGame(Backpack_t* Backpack)
{
    if (Backpack)
    {
        memset(Backpack, 0, sizeof(Backpack_t));
        BackpackAdd(Backpack, BPITEM_TYPEWEAPON, WeaponMissileSmall.BundleSize, &WeaponMissileSmall);
    }
}


//-----------------------------------------------------------------------------
// Name: IsBackpackItemActivated()
// Desc:
//-----------------------------------------------------------------------------

size_t IsBackpackItemActivated(Backpack_t* Backpack, size_t AbsoluteIndex)
{
    if (nullptr == Backpack)
        return 0;

    if (AbsoluteIndex < 0 || AbsoluteIndex >= Backpack->ItemCount)
        return 0;

    return Backpack->Items[ AbsoluteIndex ].Activated;
}
