//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 8 April 2002
//-----------------------------------------------------------------------------


#ifndef _BACKPACK_H_
#define _BACKPACK_H_

#include "Dialog.h"

#define BACKPACK_MAXITEMS       64

#define BPITEM_TYPEWEAPON       0
#define BPITEM_TYPEITEM         1

#define BACKPACK_NEWITEM        1
#define BACKPACK_ADDEDTOSTOCK   2

#define BACKPACK_MORELEFT       1
#define BACKPACK_NOMORELEFT     2

#define BACKPACK_ACTIVATED      0
#define BACKPACK_DEACTIVATED    1


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct BPItem_s
{
    size_t Activated;
    size_t Type;
    size_t AmountLeft;
    void *Item;
}
BPItem_t;

typedef struct Backpack_s
{
    size_t ItemCount;
    BPItem_t Items[ BACKPACK_MAXITEMS ];
}
Backpack_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

void AddBackpackItemsToListBox( Dialog_t *ListBox, Backpack_t *Backpack, size_t Type );

// returns either BACKPACK_NEWITEM or BACKPACK_ADDEDTOSTOCK
size_t BackpackAdd( Backpack_t *Backpack, size_t Type, size_t BundleSize, void *Item );

size_t BackpackRemove( Backpack_t *Backpack, size_t Type, void *Item );
size_t BackpackGetAbsoluteIndex( Backpack_t *Backpack, size_t Type, void *Item );
size_t BackpackGetRelativeIndex( Backpack_t *Backpack, size_t Type, void *Item );
void *BackpackGetItem( Backpack_t *Backpack, size_t Type, size_t Index );

// returns either BACKPACK_MORELEFT or BACKPACK_NOMORELEFT
size_t BackpackUse( Backpack_t *Backpack, size_t Type, void *Item );

size_t BackpackGetAmountLeft( Backpack_t *Backpack, size_t Type, void *Item );
size_t BackpackToggleActivate( Backpack_t *Backpack, size_t Index );

size_t BackpackIsOnlyWeaponLeft( Backpack_t *Backpack, void *Weapon );

size_t BackpackGetWeaponCount( Backpack_t *Backpack );
size_t BackpackGetItemCount( Backpack_t *Backpack );

void SetupBackpackForNewGame( Backpack_t *Backpack );
size_t IsBackpackItemActivated( Backpack_t *Backpack, size_t AbsoluteIndex );


#endif
