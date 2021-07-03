//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#include "StateTypes.h"
#include "WeaponTypes.h"

extern State_t g_StateWaitForShot;

extern void SetShotID(size_t ID);
extern size_t GetShotID();
extern void UpdateCameraToFollowShot(size_t ID);
extern void TrackAeroplane(size_t ID);
extern void SetHitScanShot(void);
extern void LookAtCurrentPlayer(void);
extern void ChooseShotCameraState(size_t ID, Weapon_t* Weapon);
