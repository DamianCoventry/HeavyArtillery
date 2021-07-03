//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "StateTypes.h"
#include "State.h"
#include "StateBeginTurn.h"
#include "StateEndTurn.h"
#include "StateEndLevel.h"
#include "StateWaitForShot.h"
#include "StateFlyBy.h"

#include "KeyList.h"
#include "DialogKeys.h"

#include "Input.h"
#include "LevelManager.h"
#include "ShotManager.h"
#include "ExplosionManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"
#include "GameWeapons.h"

#include "Camera.h"
#include "Game.h"


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _LookingAtPlayer = -1;
static size_t _FollowingAeroplane = -1;
static size_t _ShotID;
static size_t _IsMouseLook = 0;
static size_t _IsHitScanShot = 0;
static size_t _IsMessageBoxActive = 0;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static void CallbackEndGame(size_t RetValue);


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc:
//----------------------------------------------------------------------------------------

static void Constructor(void)
{
    //size_t ProjectileObjID;

    DialogHideAll(0);

    //if (_LookingAtPlayer != -1)
    //{
    //    DialogShow(DialogKeys, &g_WaitForShotKeys);
    //    SetCameraState(CAMERASTATE_LOOKINGATPOINT, CAMERATYPE_MOUSELOOK);
    //}
    //else if (_IsHitScanShot)
    //{
    //    DialogShow(DialogKeys, &g_WaitForShotKeys);
    //    SetCameraState(CAMERASTATE_DISTANCEFOLLOW, CAMERATYPE_MOUSELOOK);
    //}
    //else if (_IsMouseLook)
    //{
    //    DialogShow(DialogKeys, &g_MouseLookWaitKeys);
    //    SetCameraState(CAMERASTATE_MOUSELOOK, CAMERATYPE_MOUSELOOK);
    //}
    //else
    //{
    //    ProjectileObjID = GetShotObject(_ShotID);
    //    SetPointToFollow(GetObjectPositionPtr(ProjectileObjID, 0));

    //    DialogShow(DialogKeys, &g_WaitForShotKeys);
    //    SetCameraState(CAMERASTATE_FOLLOWINGPOINT, CAMERATYPE_MOUSELOOK);
    //}
}


//----------------------------------------------------------------------------------------
// Name: Destructor()
// Desc:
//----------------------------------------------------------------------------------------

static void Destructor(void)
{
    _IsMouseLook = 0;

    ClearAllCameraShakeLookAtInfo();

    SetCameraState(CAMERASTATE_MOUSELOOK, CAMERATYPE_MOUSELOOK);
}


//----------------------------------------------------------------------------------------
// Name: NoTanksAreMoving()
// Desc:
//----------------------------------------------------------------------------------------

static size_t NoTanksAreMoving(void)
{
    size_t i;
    Vector_t Zero;
    Vector_t* Move;

    VectorSet(Zero, 0.0f);

    for (i = 0; i < GetPlayerCount(); i++)
    {
        if (!IsPlayerDead(i))
        {
            Move = GetObjectMovement(GetPlayerTank(i));

            if (!VectorFuzzyCompare((*Move), Zero, EPSILON))
            {
                // this tank is moving, point the camera at it
                //if (!_IsMouseLook)
                //{
                //    SetPointToFollow(GetObjectPositionPtr(GetPlayerTank(i), 0));
                //    SetCameraState(CAMERASTATE_LOOKINGATPOINT, CAMERATYPE_MOUSELOOK);
                //}
                return 0;
            }
        }
    }

    return 1;   // true, no tanks are moving
}


//----------------------------------------------------------------------------------------
// Name: EndMouseLookMode()
// Desc:
//----------------------------------------------------------------------------------------

static void EndMouseLookMode(void)
{
    //if (_LookingAtPlayer != -1)
    //{
    //    LookAtCurrentPlayer();
    //    SetCameraState(CAMERASTATE_LOOKINGATPOINT, CAMERATYPE_MOUSELOOK);
    //}
    //else if (-1 == _FollowingAeroplane)
    //{
    //    SetCameraState(CAMERASTATE_FOLLOWINGPOINT, CAMERATYPE_MOUSELOOK);

    //    size_t ProjectileObjID = GetShotObject(_ShotID);
    //    if (-1 != ProjectileObjID)
    //        SetPointToFollow(GetObjectPositionPtr(ProjectileObjID, 0));
    //}
    //else
    //{
    //    SetPointToFollow(GetObjectPositionPtr(_FollowingAeroplane, 0));
    //    SetCameraState(CAMERASTATE_LOOKINGATPOINT, CAMERATYPE_MOUSELOOK);
    //}

    DialogShow(DialogKeys, &g_WaitForShotKeys);
}


//----------------------------------------------------------------------------------------
// Name: Poll()
// Desc:
//----------------------------------------------------------------------------------------

static void Poll(void)
{
    if (IsKeyPressed(DIK_F8))
        ScreenShot();

    if (_IsMessageBoxActive)
        return;

    if (NoTanksAreMoving() && GetShotCount() <= 0 && GetExplosionCount() <= 0)
    {
        if (IsEndOfLevel())
            SetGameState(&g_StateEndLevel);
        else
            SetGameState(&g_StateEndTurn);

        return;
    }

    //if (IsKeyPressed(DIK_ESCAPE))
    //{
    //    _IsMouseLook = !_IsMouseLook;

    //    if (_IsMouseLook)
    //    {
    //        DialogShow(DialogKeys, &g_MouseLookWaitKeys);
    //        SetCameraState(CAMERASTATE_MOUSELOOK, CAMERATYPE_MOUSELOOK);
    //    }
    //    else
    //        EndMouseLookMode();
    //}

    if (IsKeyPressed(DIK_F9))
    {
        _IsMessageBoxActive = 1;

        DialogShowCursor(1);

        SetCameraState(CAMERASTATE_IDLE, CAMERATYPE_MOUSELOOK);

        DialogMessageBox("Heavy Artillery", "Are you sure you want to end the game?", MB_YESNO, CallbackEndGame);
    }
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateWaitForShot =
{
    Constructor, Destructor, Poll
};


//----------------------------------------------------------------------------------------
// Name: SetShotID()
// Desc:
//----------------------------------------------------------------------------------------

void SetShotID(size_t ID)
{
    _FollowingAeroplane = -1;
    _IsHitScanShot = 0;
    _LookingAtPlayer = -1;

    _ShotID = ID;
}


//----------------------------------------------------------------------------------------
// Name: GetShotID()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetShotID()
{
    return _ShotID;
}


//----------------------------------------------------------------------------------------
// Name: UpdateCameraToFollowShot()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateCameraToFollowShot(size_t ID)
{
    _FollowingAeroplane = -1;
    _IsHitScanShot = 0;
    _LookingAtPlayer = -1;

    _ShotID = ID;
    SetPointToFollow(GetObjectPositionPtr(GetShotObject(_ShotID), 0));
}


//----------------------------------------------------------------------------------------
// Name: TrackAeroplane()
// Desc:
//----------------------------------------------------------------------------------------

void TrackAeroplane(size_t ID)
{
    _FollowingAeroplane = ID;
    SetPointToFollow(GetObjectPositionPtr(ID, 0));
    SetCameraState(CAMERASTATE_LOOKINGATPOINT, CAMERATYPE_MOUSELOOK);
}


//----------------------------------------------------------------------------------------
// Name: SetHitScanShot()
// Desc:
//----------------------------------------------------------------------------------------

void SetHitScanShot(void)
{
    _IsHitScanShot = 1;
}


//----------------------------------------------------------------------------------------
// Name: LookAtCurrentPlayer()
// Desc:
//----------------------------------------------------------------------------------------

void LookAtCurrentPlayer(void)
{
    _LookingAtPlayer = GetCurrentPlayer();
    SetPointToFollow(GetObjectPositionPtr(GetPlayerTank(_LookingAtPlayer), 0));
}


//----------------------------------------------------------------------------------------
// Name: ChooseShotCameraState()
// Desc:
//----------------------------------------------------------------------------------------

void ChooseShotCameraState(size_t ID, Weapon_t* Weapon)
{
    _ShotID = ID;

    if (Weapon == &WeaponFunkyBomb)
        SetCameraState(CAMERASTATE_TRACK_FUNKY_BOMB, CAMERATYPE_MOUSELOOK);
    else if (Weapon == &WeaponMIRV || Weapon == &WeaponDeathsHead)
        SetCameraState(CAMERASTATE_TRACK_MULTI_PROJECTILE, CAMERATYPE_MOUSELOOK);
    else if (Weapon == &WeaponRailgun)
    {
        if (rand() % 2 == 0)
            SetCameraState(CAMERASTATE_RAILGUN_PERPENDICULAR_ACROSS_PATH, CAMERATYPE_MOUSELOOK);
        else
            SetCameraState(CAMERASTATE_RAILGUN_CONCAVE_ARC_ACROSS_PATH, CAMERATYPE_MOUSELOOK);
    }
    else if (Weapon == &WeaponNapalmSmall || Weapon == &WeaponNapalmLarge)
        SetCameraState(CAMERASTATE_TRACK_NAPALM, CAMERATYPE_MOUSELOOK);
    else if (Weapon == &WeaponAirStrikeSmall || Weapon == &WeaponAirStrikeLarge)
        SetCameraState(CAMERASTATE_TRACK_AIR_STRIKE, CAMERATYPE_MOUSELOOK);
    else
    {
        switch (rand() % 17)
        {
        case 0:
            SetCameraStateAndShotID(CAMERASTATE_PERPENDICULAR_STATIONARY, _ShotID);
            break;
        case 1:
            SetCameraStateAndShotID(CAMERASTATE_PERPENDICULAR_PARTIAL_FOLLOW, _ShotID);
            break;
        case 2:
            SetCameraStateAndShotID(CAMERASTATE_PERPENDICULAR_COMPLETE_FOLLOW, _ShotID);
            break;
        case 3:
            SetCameraStateAndShotID(CAMERASTATE_CORNER_STATIONARY, _ShotID);
            break;
        case 4:
            SetCameraStateAndShotID(CAMERASTATE_CORNER_PARTIAL_FOLLOW, _ShotID);
            break;
        case 5:
            SetCameraStateAndShotID(CAMERASTATE_CORNER_COMPLETE_FOLLOW, _ShotID);
            break;
        case 6:
            SetCameraStateAndShotID(CAMERASTATE_FOLLOW_SHOT_DIRECTLY, _ShotID);
            break;
        case 7:
            SetCameraStateAndShotID(CAMERASTATE_FOLLOW_SHOT_WAVILY, _ShotID);
            break;
        case 8:
            SetCameraStateAndShotID(CAMERASTATE_DOLLY_TRACK_PERPENDICULAR_ACROSS_PATH, _ShotID);
            break;
        case 9:
            SetCameraStateAndShotID(CAMERASTATE_DOLLY_TRACK_CONVEX_ARC_ACROSS_PATH, _ShotID);
            break;
        case 10:
            SetCameraStateAndShotID(CAMERASTATE_DOLLY_TRACK_CONCAVE_ARC_ACROSS_PATH, _ShotID);
            break;
        case 11:
            SetCameraStateAndShotID(CAMERASTATE_BEGIN_STATIONARY_END_STATIONARY, _ShotID);
            break;
        case 12:
            SetCameraStateAndShotID(CAMERASTATE_BEGIN_STATIONARY_END_PERPENDICULAR_ACROSS_PATH, _ShotID);
            break;
        case 13:
            SetCameraStateAndShotID(CAMERASTATE_BEGIN_STATIONARY_END_CONCEX_ARC_ACROSS_PATH, _ShotID);
            break;
        case 14:
            SetCameraStateAndShotID(CAMERASTATE_TRACK_TANK_STATIONARY, _ShotID);
            break;
        case 15:
            SetCameraStateAndShotID(CAMERASTATE_TRACK_TANK_CONCEX_ARC, _ShotID);
            break;
        case 16:
            SetCameraStateAndShotID(CAMERASTATE_TRACK_TANK_FOLLOW, _ShotID);
            break;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: CallbackEndGame()
// Desc:
//-----------------------------------------------------------------------------

static void CallbackEndGame(size_t RetValue)
{
    _IsMessageBoxActive = 0;

    DialogShowCursor(0);

    if (RetValue == IDYES)
        SetGameState(&g_StateFlyBy);
    else
        EndMouseLookMode();
}
