//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "WeaponTypes.h"
#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Weapon_t WeaponMissileSmall;
extern Weapon_t WeaponMissileLarge;
extern Weapon_t WeaponNukeSmall;
extern Weapon_t WeaponNukeLarge;
extern Weapon_t WeaponLeapFrogSmall;
extern Weapon_t WeaponLeapFrogLarge;
extern Weapon_t WeaponFunkyBomb;
extern Weapon_t WeaponMIRV;
extern Weapon_t WeaponDeathsHead;
extern Weapon_t WeaponRailgun;
extern Weapon_t WeaponNapalmSmall;
extern Weapon_t WeaponNapalmLarge;
extern Weapon_t WeaponRollerSmall;
extern Weapon_t WeaponRollerLarge;
extern Weapon_t WeaponBouncerSmall;
extern Weapon_t WeaponBouncerLarge;
extern Weapon_t WeaponAirStrikeSmall;
extern Weapon_t WeaponAirStrikeLarge;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

extern size_t CreateWeapons( void );
extern void DeleteWeapons( void );

extern void AddGameWeaponsToListBox( Dialog_t *ListBox );
