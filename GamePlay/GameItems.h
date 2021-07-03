//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "ItemTypes.h"
#include "Dialog.h"

#include <string>


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Item_t ItemHeatSeekWeak;
extern Item_t ItemHeatSeekStrong;
extern Item_t ItemParachutes;
extern Item_t ItemBatteries;
extern Item_t ItemShieldsLight;
extern Item_t ItemShieldsMedium;
extern Item_t ItemShieldsHeavy;
extern Item_t ItemAutoDefense;
extern Item_t ItemFuel;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

extern size_t CreateItems(void);
extern void DeleteItems(void);

extern void AddGameItemsToListBox(Dialog_t* ListBox);

extern std::string GetShieldName(float Type);
