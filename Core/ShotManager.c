//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 6 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>

#include "Vector.h"
#include "WeaponTypes.h"

#include "ObjectManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ParticleManager.h"
#include "ShotManager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAXSHOTS                128

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXSHOTS || !_ShotPool[_id].Used )

extern LONGLONG FrameNowMs;

//----------------------------------------------------------------------------------------
// private structures
//----------------------------------------------------------------------------------------

typedef struct Shot_s
{
    size_t Used;
    Weapon_t* Weapon;
    size_t ProjectileObjID;

    size_t ProjectileCount;
    size_t SmokeTrailID;
    size_t SparkTrailID;

    size_t UsedGuidance;
    size_t (*Guidance)(size_t ShootersTeam, Vector_t* ShotLocation, Vector_t* ShotVector);

    LONGLONG LastSavedShotTime;
    std::vector<Vector_t> SavedPositions;
}
Shot_t;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _ShotIndex = 0;
static size_t _ShotCount = 0;
static Shot_t _ShotPool[MAXSHOTS];


//----------------------------------------------------------------------------------------
// Name: GetFreeShot()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetFreeShot(void)
{
    size_t i;
    size_t ID;

    i = _ShotIndex;

    while (_ShotPool[i].Used)
    {
        i++;
        if (i >= MAXSHOTS)
            i = 0;

        if (i == _ShotIndex)
            return -1;        // pool is full
    }

    _ShotPool[i].Used = 1;
    ID = i;

    _ShotIndex = i;
    if (++_ShotIndex >= MAXSHOTS)
        _ShotIndex = 0;

    _ShotCount++;
    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnShot()
// Desc:
//----------------------------------------------------------------------------------------

void ReturnShot(size_t ID)
{
    if (INVALIDID(ID))
        return;

    _ShotCount--;
    _ShotPool[ID].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: SetupShot()
// Desc:
//----------------------------------------------------------------------------------------

void SetupShot(size_t ShotID, Weapon_t* Weapon, size_t ProjectileObjID, size_t (*Guidance)(size_t, Vector_t*, Vector_t*))
{
    if (INVALIDID(ShotID))
        return;

    _ShotPool[ShotID].Weapon          = Weapon;
    _ShotPool[ShotID].ProjectileObjID = ProjectileObjID;

    _ShotPool[ShotID].UsedGuidance    = 0;
    _ShotPool[ShotID].Guidance        = Guidance;

    _ShotPool[ShotID].ProjectileCount = Weapon->ProjectileCount;

    _ShotPool[ShotID].SavedPositions.clear();
    _ShotPool[ShotID].LastSavedShotTime = FrameNowMs;
}


//----------------------------------------------------------------------------------------
// Name: GetShotCount()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetShotCount(void)
{
    return _ShotCount;
}


//----------------------------------------------------------------------------------------
// Name: UseGuidance()
// Desc:
//----------------------------------------------------------------------------------------

void UseGuidance(size_t ShotID)
{
    Vector_t ShotLocation;
    Vector_t* ShotVector;

    if (INVALIDID(ShotID))
        return;

    if (!_ShotPool[ShotID].UsedGuidance && _ShotPool[ShotID].Guidance)
    {
        GetObjectPosition(_ShotPool[ShotID].ProjectileObjID, 0, &ShotLocation);
        ShotVector = GetObjectMovement(_ShotPool[ShotID].ProjectileObjID);

        _ShotPool[ShotID].UsedGuidance = _ShotPool[ShotID].Guidance(GetPlayerTeam(GetCurrentPlayer()), &ShotLocation, ShotVector);
    }
}


//----------------------------------------------------------------------------------------
// Name: GetShotObject()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetShotObject(size_t ShotID)
{
    if (INVALIDID(ShotID))
        return -1;

    return _ShotPool[ShotID].ProjectileObjID;
}


//----------------------------------------------------------------------------------------
// Name: GetShotWeapon()
// Desc:
//----------------------------------------------------------------------------------------

Weapon_t* GetShotWeapon(size_t ShotID)
{
    if (INVALIDID(ShotID))
        return nullptr;

    return _ShotPool[ShotID].Weapon;
}


//----------------------------------------------------------------------------------------
// Name: UseShotProjectile()
// Desc:
//----------------------------------------------------------------------------------------

size_t UseShotProjectile(size_t ShotID)
{
    if (INVALIDID(ShotID))
        return 0;

    _ShotPool[ShotID].ProjectileCount--;

    return (_ShotPool[ShotID].ProjectileCount > 0);
}


//----------------------------------------------------------------------------------------
// Name: SetShotProjectileCount()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetShotProjectileCount(size_t ShotID)
{
    if (INVALIDID(ShotID))
        return 0;

    return _ShotPool[ShotID].ProjectileCount;
}


//----------------------------------------------------------------------------------------
// Name: SetShotProjectileCount()
// Desc:
//----------------------------------------------------------------------------------------

void SetShotProjectileCount(size_t ShotID, size_t Count)
{
    if (INVALIDID(ShotID))
        return;

    _ShotPool[ShotID].ProjectileCount = Count;
}


//----------------------------------------------------------------------------------------
// Name: SetSmokeTrailID()
// Desc:
//----------------------------------------------------------------------------------------

void SetSmokeTrailID(size_t ShotID, size_t SmokeTrailID, size_t SparkTrailID)
{
    if (INVALIDID(ShotID))
        return;

    _ShotPool[ShotID].SmokeTrailID = SmokeTrailID;
    _ShotPool[ShotID].SparkTrailID = SparkTrailID;
}


//----------------------------------------------------------------------------------------
// Name: StopSmokeTrail()
// Desc:
//----------------------------------------------------------------------------------------

void StopSmokeTrail(size_t ShotID)
{
    if (INVALIDID(ShotID))
        return;

    StopEmittingParticles(_ShotPool[ShotID].SmokeTrailID);
    StopEmittingParticles(_ShotPool[ShotID].SparkTrailID);
}


//----------------------------------------------------------------------------------------
// Name: SaveShotPosition()
// Desc:
//----------------------------------------------------------------------------------------

void SaveShotPosition(size_t ShotID, const Vector_t& Position)
{
    if (INVALIDID(ShotID))
        return;

    if (FrameNowMs - _ShotPool[ShotID].LastSavedShotTime >= 500)
    {
        _ShotPool[ShotID].LastSavedShotTime = FrameNowMs;
        _ShotPool[ShotID].SavedPositions.push_back(Position);
    }
}


//----------------------------------------------------------------------------------------
// Name: GetSavedShotPositions()
// Desc:
//----------------------------------------------------------------------------------------

std::vector<Vector_t> GetSavedShotPositions(size_t ShotID)
{
    if (INVALIDID(ShotID))
        return std::vector<Vector_t>();
    return _ShotPool[ShotID].SavedPositions;
}
