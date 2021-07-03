//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Sunday, 30 June 2002
//
//========================================================================================


#ifndef _ITEM_H_
#define _ITEM_H_

#include "Backpack.h"

#include <string>


typedef struct Item_s
{
    std::string Name;
    size_t Type;
    size_t Price;
    size_t BundleSize;
    std::string Line0, Line1;
    std::string Line2, Line3;

    float Scale;
    size_t ModelID;

    void (*Use)(struct Item_s* Item, Backpack_t* Backpack, size_t PlayerID);
    void (*Activate)(struct Item_s* Item, Backpack_t* Backpack, size_t PlayerID);
}
Item_t;


#endif
