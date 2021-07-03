//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 6 July 2002
//
//========================================================================================


#include "Vector.h"
#include "WeaponTypes.h"

#define SHOOT_POWERMODIFIER     0.25f

extern size_t ShootProjectile(Weapon_t* Weapon, Vector_t* StartPoint, Vector_t* ShootVector, float Power, size_t AllowGuidance);
extern size_t ShootHitScan(Weapon_t* Weapon, Vector_t* StartPoint, Vector_t* ShootVector, float Power, size_t AllowGuidance);

extern void EnableCrossHair(void);
extern void DisableCrossHair(void);
extern void DrawCrossHair(void);
