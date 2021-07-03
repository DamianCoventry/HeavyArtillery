//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 8 July 2002
//
//========================================================================================


#include "Backpack.h"
#include "ItemTypes.h"
#include "Vector.h"


extern void ActivateGuidance( Item_t *Item, Backpack_t *Backpack, size_t PlayerID );
extern void InitialiseGuidance( size_t IsStrong );

extern size_t WeakGuidance( size_t ShootersTeam, Vector_t *ShotLocation, Vector_t *ShotVector );
extern size_t StrongGuidance( size_t ShootersTeam, Vector_t *ShotLocation, Vector_t *ShotVector );
