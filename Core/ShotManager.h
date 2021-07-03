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

#include <vector>

extern size_t GetFreeShot(void);
extern void ReturnShot(size_t ID);

extern void SetupShot(size_t ShotID, Weapon_t* Weapon, size_t ProjectileObjID, size_t (*Guidance)(size_t, Vector_t*, Vector_t*));

extern void UseGuidance(size_t ShotID);

extern size_t GetShotCount(void);

extern size_t GetShotObject(size_t ShotID);
extern Weapon_t* GetShotWeapon(size_t ShotID);

extern size_t UseShotProjectile(size_t ShotID);

extern size_t GetShotProjectileCount(size_t ShotID);
extern void SetShotProjectileCount(size_t ShotID, size_t Count);

extern void SetSmokeTrailID(size_t ShotID, size_t SmokeTrailID, size_t SparkTrailID);
extern void StopSmokeTrail(size_t ShotID);

extern void SaveShotPosition(size_t ShotID, const Vector_t& Position);
extern std::vector<Vector_t> GetSavedShotPositions(size_t ShotID);
