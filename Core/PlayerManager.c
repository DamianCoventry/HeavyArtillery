//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include <string.h>
#include <string>
#include <stdlib.h>

#include "PlayerManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "MeshManager.h"
#include "ExplosionManager.h"
#include "LevelManager.h"
#include "ParticleManager.h"
#include "SoundManager.h"

#include "Object.h"
#include "WeaponTypes.h"
#include "ItemTypes.h"

#include "DialogMyTank.h"
#include "DialogPlayOrder.h"
#include "PlayerTank.h"

#include "PlayerCreate.h"
#include "Backpack.h"
#include "Projectile.h"

#include "BotThink.h"
#include "BotShoot.h"

#include "GameTypes.h"
#include "GameModels.h"
#include "GameConfig.h"
#include "GameItems.h"
#include "GameExplosions.h"
#include "GameSounds.h"

#include "Parachutes.h"
#include "Shields.h"

#include "ParticleTankSpark.h"
#include "ParticleShieldLightSpark.h"
#include "ParticleShieldMediumSpark.h"
#include "ParticleShieldHeavySpark.h"
#include "ParticleShieldHeavyDebris.h"
#include "ParticleShieldMediumDebris.h"
#include "ParticleShieldLightDebris.h"
#include "ParticleDebrisTank.h"
#include "Camera.h"


#define INVALIDINDEX( _idx )        ( _idx >= _PlayerCount )


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private structures
//----------------------------------------------------------------------------------------

typedef struct Player_s
{
    std::string Name;
    size_t Team;
    size_t Cash;
    size_t CurrentWeapon;

    float ShotPower;
    float TankPower;
    float ShieldPower;
    float ShieldType;

    size_t ParticleDamageSmoke;
    size_t ParticleDamageSparks;

    BotType_t BotType;
    BotInfo_t BotInfo;
    Backpack_t Backpack;
    PlayerHistory_t History;

    size_t TankObjectID;
    size_t ShieldObjectID;
    size_t ParachuteObjectID;
}
Player_t;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _PlayerCount;
static Player_t _Players[ MAXPLAYERS ];


//----------------------------------------------------------------------------------------
// private functions
//----------------------------------------------------------------------------------------

static void EmitDamageSparks(size_t PlyrIndex);
static void DamageSparksFinished(size_t ID);


//----------------------------------------------------------------------------------------
// Name: CreatePlayers()
// Desc:
//----------------------------------------------------------------------------------------

void CreatePlayers(void)
{
    size_t i;

    _PlayerCount = 0;
    memset(_Players, 0, sizeof(Player_t) * MAXPLAYERS);

    for (i = 0; i < MAXPLAYERS; i++)
    {
        // allocate an object for this player's tank
        _Players[ i ].TankObjectID = GetFreeObject();
        ResetObject(_Players[ i ].TankObjectID);

        if (i & 1)
            SetObjectModels(_Players[ i ].TankObjectID, g_ModelTank0LOD0, g_ModelTank0LOD1);
        else
            SetObjectModels(_Players[ i ].TankObjectID, g_ModelTank1LOD0, g_ModelTank1LOD1);

        SetObjectDrawFunction(_Players[ i ].TankObjectID, DrawShadedModel);
        SetObjectCollideMeshFunction(_Players[ i ].TankObjectID, CollideMeshSlide);

        SetObjectOnMoveFunction(_Players[ i ].TankObjectID, OnPlayerMove);
        SetObjectOnThinkFunction(_Players[ i ].TankObjectID, PlayerTankThink);
        SetObjectOnCollideMeshFunction(_Players[ i ].TankObjectID, OnPlayerCollideMesh);
        SetObjectOnCollideObjectFunction(_Players[ i ].TankObjectID, OnPlayerCollideObject);
        SetObjectOnFallingDamageFunction(_Players[ i ].TankObjectID, OnPlayerFallingDamage);

        AddObjectFlag(_Players[ i ].TankObjectID, OBJECT_CANCOLLIDEWITHTANK);
        AddObjectFlag(_Players[ i ].TankObjectID, OBJECT_ISTANK);
        DeactivateObject(_Players[ i ].TankObjectID);
        SetTankPhysics(_Players[ i ].TankObjectID);
        SetObjectRotateType(_Players[ i ].TankObjectID, MODELROTATE_XZY);

        // allocate an object for this player's shield
        _Players[ i ].ShieldObjectID = GetFreeObject();
        ResetObject(_Players[ i ].ShieldObjectID);
        SetObjectDrawFunction(_Players[ i ].ShieldObjectID, DrawShadedModelTrans);
        SetObjectOnThinkFunction(_Players[ i ].ShieldObjectID, ShieldsThink);
        DeactivateObject(_Players[ i ].ShieldObjectID);
        SetShieldParachutePhysics(_Players[ i ].ShieldObjectID);

        // allocate an object for this player's parachute
        _Players[ i ].ParachuteObjectID = GetFreeObject();
        ResetObject(_Players[ i ].ParachuteObjectID);
        SetObjectModels(_Players[ i ].ParachuteObjectID, g_ModelParachuteLOD0, g_ModelParachuteLOD1);
        SetObjectDrawFunction(_Players[ i ].ParachuteObjectID, DrawShadedModel);
        SetObjectOnThinkFunction(_Players[ i ].ParachuteObjectID, ParachutesThink);
        DeactivateObject(_Players[ i ].ParachuteObjectID);
        SetShieldParachutePhysics(_Players[ i ].ParachuteObjectID);

        // set the tank's data to the current player
        SetObjectData(_Players[ i ].TankObjectID, i);
    }
}


//----------------------------------------------------------------------------------------
// Name: DeletePlayers()
// Desc:
//----------------------------------------------------------------------------------------

void DeletePlayers(void)
{
    size_t i;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        ReturnObject(_Players[ i ].ParachuteObjectID);
        ReturnObject(_Players[ i ].ShieldObjectID);
        ReturnObject(_Players[ i ].TankObjectID);
    }
}


//----------------------------------------------------------------------------------------
// Name: PlacePlayersOnLandscape()
// Desc:
//----------------------------------------------------------------------------------------

void PlacePlayersOnLandscape(void)
{
    size_t i, j, Index;
    size_t Unique;
    size_t TilesX[ MAXPLAYERS ];
    size_t TilesZ[ MAXPLAYERS ];
    size_t TilesWide, TilesDeep;
    float TileWidth, TileDepth;
    Vector_t Position;

    memset(TilesX, 0, sizeof(size_t) * MAXPLAYERS);
    memset(TilesZ, 0, sizeof(size_t) * MAXPLAYERS);

    GetMeshSize(g_LandscapeMesh, &TilesWide, &TilesDeep);
    GetMeshTileSize(g_LandscapeMesh, &TileWidth, &TileDepth);

    // generate a unique starting tile for each player
    for (i = 0; i < _PlayerCount; i++)
    {
        do
        {
            TilesX[ i ] = (rand() % (TilesWide / 2)) + (TilesWide / 4);
            TilesZ[ i ] = (rand() % (TilesDeep / 2)) + (TilesDeep / 4);

            Unique = 1;
            for (j = 0; j < _PlayerCount; j++)
            {
                if (i == j)
                    continue;

                if (TilesX[ i ] == TilesX[ j ] && TilesZ[ i ] == TilesZ[ j ])
                {
                    Unique = 0;
                    break;
                }
            }
        }
        while (!Unique);

        Position.X = TileWidth * TilesX[ i ] + (TileWidth / 2.0f);
        Position.Z = TileDepth * TilesZ[ i ] + (TileDepth / 2.0f);
        Position.Y = GetMeshHeight(g_LandscapeMesh, Position.X, Position.Z, &Index);

        SetObjectPosition(_Players[ i ].TankObjectID, 0, &Position);
        SetObjectGroundPolygon(_Players[ i ].TankObjectID, Index);
    }
}


//----------------------------------------------------------------------------------------
// Name: SetupPlayersForNewGame()
// Desc:
//----------------------------------------------------------------------------------------

void SetupPlayersForNewGame(void)
{
    size_t i;
    Vector_t Position;
    Vector_t Rotation;

    _PlayerCount = GetPlayerCreateCount();

    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (i < _PlayerCount)
        {
            memset(&_Players[ i ].History, 0, sizeof(PlayerHistory_t));

            _Players[i].Name = GetPlayerCreateName(i);

            _Players[ i ].Team      = GetPlayerCreateTeam(i);
            _Players[ i ].BotType   = GetPlayerCreateBotType(i);
            _Players[ i ].Cash      = GetCashAtStart();

            _Players[ i ].TankPower     = 100.0f;
            _Players[ i ].ShieldPower   = 0.0f;
            _Players[ i ].ShieldType    = 0.0f;

            ReturnParticleSystem(_Players[ i ].ParticleDamageSmoke);
            _Players[ i ].ParticleDamageSmoke = -1;
            _Players[ i ].ParticleDamageSparks = -1;

            if (BOTTYPE_NOTABOT != _Players[ i ].BotType)
                SetupBotCallbacks(_Players[ i ].BotType, &_Players[ i ].BotInfo);

            DeactivateObject(_Players[ i ].TankObjectID);
            DeactivateObject(_Players[ i ].ShieldObjectID);
            DeactivateObject(_Players[ i ].ParachuteObjectID);

            Position.X = Position.Z = 0.0f;
            Position.Y = TANK_BARRELLHEIGHT;
            SetObjectPosition(_Players[ i ].TankObjectID, 2, &Position);

            _Players[ i ].ShotPower = 50.0f;

            VectorSet(Rotation, 0.0f);
            SetObjectRotation(_Players[ i ].TankObjectID, 0, &Rotation);
            SetObjectRotation(_Players[ i ].TankObjectID, 1, &Rotation);
            SetObjectRotation(_Players[ i ].TankObjectID, 2, &Rotation);

            ShowPlayer(i);
            SetupBackpackForNewGame(&_Players[ i ].Backpack);
        }
        else
        {
            DeactivateObject(_Players[ i ].TankObjectID);
            DeactivateObject(_Players[ i ].ShieldObjectID);
            DeactivateObject(_Players[ i ].ParachuteObjectID);
            HidePlayer(i);
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: SetupPlayersForNewLevel()
// Desc:
//----------------------------------------------------------------------------------------

void SetupPlayersForNewLevel(void)
{
    size_t i;
    Vector_t Rotation, Zero;

    VectorSet(Zero, 0.0f);

    for (i = 0; i < _PlayerCount; i++)
    {
        _Players[ i ].TankPower = 100.0f;
        _Players[ i ].ShotPower = 50.0f;
        _Players[ i ].CurrentWeapon = 0;

        ReturnParticleSystem(_Players[ i ].ParticleDamageSmoke);
        _Players[ i ].ParticleDamageSmoke = -1;
        _Players[ i ].ParticleDamageSparks = -1;

        _Players[ i ].BotInfo.TargetPlyrIndex   = -1;
        _Players[ i ].BotInfo.DestPower         = -1;

        SetupBotShotNoise(i, &_Players[ i ].BotInfo);
        SetObjectMovement(_Players[ i ].TankObjectID, &Zero);

        ActivateObject(_Players[ i ].TankObjectID);

        VectorSet(Rotation, 0.0f);
        SetObjectRotation(_Players[ i ].TankObjectID, 0, &Rotation);
        SetObjectRotation(_Players[ i ].TankObjectID, 1, &Rotation);
        SetObjectRotation(_Players[ i ].TankObjectID, 2, &Rotation);

        ShowPlayer(i);

        // clear the last level's history
        _Players[ i ].History.NumShotsFired     = 0;
        _Players[ i ].History.NumShotsHit       = 0;
        _Players[ i ].History.NumShotsMissed    = 0;

        _Players[ i ].History.DamageToEnemys    = 0.0f;
        _Players[ i ].History.DamageToSelf      = 0.0f;
        _Players[ i ].History.DamageToTeammates = 0.0f;

        _Players[ i ].History.FallingDamage     = 0;
        _Players[ i ].History.EnemyKills        = 0;
        _Players[ i ].History.SelfKills         = 0;
        _Players[ i ].History.TeammateKills     = 0;

        _Players[ i ].History.SurviveBonus      = 0;
        _Players[ i ].History.TeamWinBonus      = 0;
        _Players[ i ].History.CashWon           = 0;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCount()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerCount(void)
{
    return _PlayerCount;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerName()
// Desc:
//----------------------------------------------------------------------------------------

std::string GetPlayerName(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return {};

    return _Players[ PlyrIndex ].Name;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerTeam()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerTeam(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return -1;

    return _Players[ PlyrIndex ].Team;
}


//----------------------------------------------------------------------------------------
// Name: IsPlayerDead()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsPlayerDead(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0;

    //return IsObjectDead( _Players[ PlyrIndex ].TankObjectID );
    return !IsObjectActivated(_Players[PlyrIndex].TankObjectID);
}


//----------------------------------------------------------------------------------------
// Name: IsPlayerABot()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsPlayerABot(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0;

    return (BOTTYPE_NOTABOT != _Players[ PlyrIndex ].BotType);
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCash()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerCash(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0;

    return _Players[ PlyrIndex ].Cash;
}


//----------------------------------------------------------------------------------------
// Name: AddToPlayerCash()
// Desc:
//----------------------------------------------------------------------------------------

void AddToPlayerCash(size_t PlyrIndex, size_t Cash)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[PlyrIndex].Cash += Cash;
}


//----------------------------------------------------------------------------------------
// Name: SubtractFromPlayerCash()
// Desc:
//----------------------------------------------------------------------------------------

void SubtractFromPlayerCash(size_t PlyrIndex, size_t Cash)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    if (Cash >= _Players[PlyrIndex].Cash)
        _Players[PlyrIndex].Cash = 0;
    else
        _Players[PlyrIndex].Cash -= Cash;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerBackpack()
// Desc:
//----------------------------------------------------------------------------------------

Backpack_t* GetPlayerBackpack(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0;

    return &_Players[ PlyrIndex ].Backpack;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerTank()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerTank(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return -1;

    return _Players[ PlyrIndex ].TankObjectID;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerParachute()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerParachute(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return -1;

    return _Players[ PlyrIndex ].ParachuteObjectID;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerShield()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerShield(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return -1;

    return _Players[ PlyrIndex ].ShieldObjectID;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerTankPower()
// Desc:
//----------------------------------------------------------------------------------------

float GetPlayerTankPower(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0.0f;

    return _Players[ PlyrIndex ].TankPower;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerShieldPower()
// Desc:
//----------------------------------------------------------------------------------------

float GetPlayerShieldPower(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0.0f;

    return _Players[ PlyrIndex ].ShieldPower;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerShieldType()
// Desc:
//----------------------------------------------------------------------------------------

float GetPlayerShieldType(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0.0f;

    return _Players[ PlyrIndex ].ShieldType;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCurrentWeapon()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerCurrentWeapon(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0;

    return _Players[ PlyrIndex ].CurrentWeapon;
}


//----------------------------------------------------------------------------------------
// Name: SetPlayerCurrentWeapon()
// Desc:
//----------------------------------------------------------------------------------------

void SetPlayerCurrentWeapon(size_t PlyrIndex, size_t Weapon)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].CurrentWeapon = Weapon;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerShotDetails()
// Desc:
//----------------------------------------------------------------------------------------

void GetPlayerShotDetails(size_t PlyrIndex, float* Heading, float* Pitch, float* Power)
{
    Vector_t V;

    if (INVALIDINDEX(PlyrIndex))
        return;

    if (Heading)
    {
        GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 1, &V);
        (*Heading) = V.Y;
    }

    if (Pitch)
    {
        GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 2, &V);
        (*Pitch) = V.X;
    }

    if (Power)
        (*Power) = _Players[ PlyrIndex ].ShotPower;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerShotDetails()
// Desc:
//----------------------------------------------------------------------------------------

void StopPlayerReceivingFallingDamage(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    SetObjectOnFallingDamageFunction(GetPlayerTank(PlyrIndex), nullptr);
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerShotDetails()
// Desc:
//----------------------------------------------------------------------------------------

void AllowPlayerToReceiveFallingDamage(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    SetObjectOnFallingDamageFunction(GetPlayerTank(PlyrIndex), OnPlayerFallingDamage);
}


//----------------------------------------------------------------------------------------
// Name: IsPlayerReceivingFallingDamage()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsPlayerReceivingFallingDamage(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return 0;

    return DoesObjectHaveFallingDamageFunction(GetPlayerTank(PlyrIndex));
}


//----------------------------------------------------------------------------------------
// Name: RepairPlayersTank()
// Desc:
//----------------------------------------------------------------------------------------

void RepairPlayersTank(size_t PlyrIndex)
{
    Item_t* Item;
    size_t Index;

    if (INVALIDINDEX(PlyrIndex))
        return;

    // is the tank damaged?
    if (_Players[ PlyrIndex ].TankPower < 100.0f || (_Players[ PlyrIndex ].ShieldType != SHIELDTYPE_NONE && _Players[ PlyrIndex ].ShieldPower < 100.0f))
    {
        Index = BackpackGetRelativeIndex(&_Players[ PlyrIndex ].Backpack, BPITEM_TYPEITEM, (void*)&ItemBatteries);
        if (-1 == Index)
            DialogMessageBox("Heavy Artillery", "You don't have any batteries to repair your tank with.", MB_OK, nullptr);
        else
        {
            Item = (Item_t*)BackpackGetItem(&_Players[ PlyrIndex ].Backpack, BPITEM_TYPEITEM, Index);
            if (Item && Item->Use)
            {
                Item->Use(Item, GetPlayerBackpack(PlyrIndex), PlyrIndex);
                DialogUpdateMyTank(DialogMyTank);
            }
        }
    }
    else
        DialogMessageBox("Heavy Artillery", "Your tank's at full power.", MB_OK, nullptr);
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerPosition()
// Desc:
//----------------------------------------------------------------------------------------

void GetPlayerPosition(size_t PlyrIndex, Vector_t* Position)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    GetObjectPosition(_Players[ PlyrIndex ].TankObjectID, 0, Position);
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerGroundNormal()
// Desc:
//----------------------------------------------------------------------------------------

void GetPlayerGroundNormal(size_t PlyrIndex, Vector_t* Normal)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    GetPolygonNormal(g_LandscapeMesh, GetObjectGroundPolygon(_Players[ PlyrIndex ].TankObjectID), Normal);
}


//----------------------------------------------------------------------------------------
// Name: RotateTankBarrell()
// Desc:
//----------------------------------------------------------------------------------------

void ModifyTankBarrell(size_t PlyrIndex, float HeadingDelta, float PitchDelta)
{
    Vector_t Rotation;

    if (INVALIDINDEX(PlyrIndex))
        return;

    GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 1, &Rotation);

    Rotation.Y += HeadingDelta;
    if (Rotation.Y < 0.0f)
        Rotation.Y += 360.0f;
    else if (Rotation.Y >= 360.0f)
        Rotation.Y -= 360.0f;

    SetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 1, &Rotation);

    GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 2, &Rotation);

    Rotation.X += PitchDelta;
    if (Rotation.X < 0.0f)
        Rotation.X = 0.0f;
    else if (Rotation.X >= 90.0f)
        Rotation.X = 90.0f;

    SetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 2, &Rotation);
}


//----------------------------------------------------------------------------------------
// Name: ModifyTankShotPower()
// Desc:
//----------------------------------------------------------------------------------------

void ModifyTankShotPower(size_t PlyrIndex, float PowerDelta)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].ShotPower += PowerDelta;

    if (_Players[ PlyrIndex ].ShotPower < 0.0f)
        _Players[ PlyrIndex ].ShotPower = 0.0f;
    else if (_Players[ PlyrIndex ].ShotPower > 100.0f)
        _Players[ PlyrIndex ].ShotPower = 100.0f;
}


//----------------------------------------------------------------------------------------
// Name: ModifyTankRotation()
// Desc:
//----------------------------------------------------------------------------------------

void ModifyTankRotation(size_t PlyrIndex, float HeadingDelta)
{
    Vector_t Rotation;

    if (INVALIDINDEX(PlyrIndex))
        return;

    GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 0, &Rotation);

    Rotation.Y += HeadingDelta;
    if (Rotation.Y < 0.0f)
        Rotation.Y += 360.0f;
    else if (Rotation.Y >= 360.0f)
        Rotation.Y -= 360.0f;

    SetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 0, &Rotation);
}


//----------------------------------------------------------------------------------------
// Name: SetTankShotPower()
// Desc:
//----------------------------------------------------------------------------------------

void SetTankShotPower(size_t PlyrIndex, float Power)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].ShotPower = Power;
}


//----------------------------------------------------------------------------------------
// Name: HidePlayer()
// Desc:
//----------------------------------------------------------------------------------------

void HidePlayer(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    HideObject(_Players[ PlyrIndex ].TankObjectID);
    HideObject(_Players[ PlyrIndex ].ShieldObjectID);
    HideObject(_Players[ PlyrIndex ].ParachuteObjectID);
}


//----------------------------------------------------------------------------------------
// Name: ShowPlayer()
// Desc:
//----------------------------------------------------------------------------------------

void ShowPlayer(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    ShowObject(_Players[ PlyrIndex ].TankObjectID);
    ShowObject(_Players[ PlyrIndex ].ShieldObjectID);
    ShowObject(_Players[ PlyrIndex ].ParachuteObjectID);
}


//----------------------------------------------------------------------------------------
// Name: MovePlayerForward()
// Desc:
//----------------------------------------------------------------------------------------

void MovePlayerForward(size_t PlyrIndex, float MoveScale)
{
    Vector_t Move;
    Vector_t Rotation;

    if (INVALIDINDEX(PlyrIndex))
        return;

    GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 0, &Rotation);
    VectorGetMoveXZY(-Rotation.X * DTOR, -Rotation.Z * DTOR, -Rotation.Y * DTOR, &Move);
    VectorScale(Move, MoveScale, Move);

    ApplyForceToObject(_Players[ PlyrIndex ].TankObjectID, &Move);
    CapObjectToTermVelo(_Players[ PlyrIndex ].TankObjectID);
}


//----------------------------------------------------------------------------------------
// Name: MovePlayerBackward()
// Desc:
//----------------------------------------------------------------------------------------

void MovePlayerBackward(size_t PlyrIndex, float MoveScale)
{
    Vector_t Move;
    Vector_t Rotation;

    if (INVALIDINDEX(PlyrIndex))
        return;

    GetObjectRotation(_Players[ PlyrIndex ].TankObjectID, 0, &Rotation);
    VectorGetMoveXZY(-Rotation.X * DTOR, -Rotation.Z * DTOR, -Rotation.Y * DTOR, &Move);
    VectorScale(Move, MoveScale, Move);

    VectorInverse(Move);
    ApplyForceToObject(_Players[ PlyrIndex ].TankObjectID, &Move);
    CapObjectToTermVelo(_Players[ PlyrIndex ].TankObjectID);
}


//----------------------------------------------------------------------------------------
// Name: IsPlayerMoving()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsPlayerMoving(size_t PlyrIndex)
{
    Vector_t Zero;
    Vector_t* Move;

    if (INVALIDINDEX(PlyrIndex))
        return 0;

    VectorSet(Zero, 0.0f);
    Move = GetObjectMovement(_Players[ PlyrIndex ].TankObjectID);

    return !VectorFuzzyCompare(Zero, (*Move), EPSILON);
}


//----------------------------------------------------------------------------------------
// Name: IsPlayerOnTheGround()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsPlayerOnGround(size_t PlyrIndex)
{
    Vector_t Position;

    if (INVALIDINDEX(PlyrIndex))
        return 0;

    GetObjectPosition(_Players[ PlyrIndex ].TankObjectID, 0, &Position);

    return (Position.Y <= GetMeshHeight(g_LandscapeMesh, Position.X, Position.Z, nullptr) + 16.0f);
}


//----------------------------------------------------------------------------------------
// Name: ActivatePlayerShield()
// Desc:
//----------------------------------------------------------------------------------------

void ActivatePlayerShield(size_t PlyrIndex, float ShieldType)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    ActivateObject(_Players[ PlyrIndex ].ShieldObjectID);
    ShowObject(_Players[ PlyrIndex ].ShieldObjectID);

    if (ShieldType == SHIELDTYPE_LIGHT)
        SetObjectModels(_Players[ PlyrIndex ].ShieldObjectID, g_ModelShieldLight, -1);
    else if (ShieldType == SHIELDTYPE_MEDIUM)
        SetObjectModels(_Players[ PlyrIndex ].ShieldObjectID, g_ModelShieldMedium, -1);
    else
        SetObjectModels(_Players[ PlyrIndex ].ShieldObjectID, g_ModelShieldHeavy, -1);

    _Players[ PlyrIndex ].ShieldPower   = 100.0f;
    _Players[ PlyrIndex ].ShieldType    = ShieldType;
}


//----------------------------------------------------------------------------------------
// Name: DeactivatePlayerShield()
// Desc:
//----------------------------------------------------------------------------------------

void DeactivatePlayerShield(size_t PlyrIndex)
{
    size_t Index;
    Item_t* Item;

    if (INVALIDINDEX(PlyrIndex))
        return;

    if (_Players[ PlyrIndex ].ShieldType == SHIELDTYPE_LIGHT)
        Item = &ItemShieldsLight;
    else if (_Players[ PlyrIndex ].ShieldType == SHIELDTYPE_MEDIUM)
        Item = &ItemShieldsMedium;
    else
        Item = &ItemShieldsHeavy;

    Index = BackpackGetAbsoluteIndex(&_Players[ PlyrIndex ].Backpack, BPITEM_TYPEITEM, (void*)Item);
    BackpackToggleActivate(&_Players[ PlyrIndex ].Backpack, Index);

    DeactivateObject(_Players[ PlyrIndex ].ShieldObjectID);
    HideObject(_Players[ PlyrIndex ].ShieldObjectID);

    _Players[ PlyrIndex ].ShieldPower   = 0.0f;
    _Players[ PlyrIndex ].ShieldType    = SHIELDTYPE_NONE;
}


//----------------------------------------------------------------------------------------
// Name: PlayerTakeDamage()
// Desc:
//----------------------------------------------------------------------------------------

float PlayerTakeDamage(size_t PlyrIndex, float Damage)
{
    float Remaining, DamageTaken;
    Vector_t Position;
    PlayerTurn_t* HistoryTurn;
    PlayerHistory_t* History;

    if (INVALIDINDEX(PlyrIndex))
        return 0.0f;

    if (IsObjectDead(_Players[ PlyrIndex ].TankObjectID) || !IsObjectActivated(_Players[ PlyrIndex ].TankObjectID))
        return 0.0f;

    // record this damage in the player's history
    HistoryTurn = GetCurrentTurn(&_Players[ PlyrIndex ].History);
    if (HistoryTurn)
        HistoryTurn->DamageTaken[ GetCurrentPlayer() ] += Damage;

    DamageTaken = 0.0f;

    // get the shield to absorb some (or all) of the damage
    if (_Players[ PlyrIndex ].ShieldPower > 0.0f)
    {
        DamageTaken = (Damage * _Players[ PlyrIndex ].ShieldType);
        _Players[ PlyrIndex ].ShieldPower -= DamageTaken;
        if (_Players[ PlyrIndex ].ShieldPower <= 0.0f)
        {
            GetPlayerPosition(PlyrIndex, &Position);
            if (_Players[ PlyrIndex ].ShieldType == SHIELDTYPE_LIGHT)
            {
                StartShieldLightDebris(&Position);
                StartShieldLightSpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);
            }
            else if (_Players[ PlyrIndex ].ShieldType == SHIELDTYPE_MEDIUM)
            {
                StartShieldMediumDebris(&Position);
                StartShieldMediumSpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);
            }
            else
            {
                StartShieldHeavyDebris(&Position);
                StartShieldHeavySpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);
            }

            Remaining = -_Players[ PlyrIndex ].ShieldPower;
            DeactivatePlayerShield(PlyrIndex);

            SetSoundPosition(SoundShieldDown0, Position.X, Position.Y, Position.Z);
            PlaySound3D(SoundShieldDown0, 0);

            SetSoundPosition(SoundShieldShatter0, Position.X, Position.Y, Position.Z);
            PlaySound3D(SoundShieldShatter0, 0);
        }
        else
        {
            // all damage absorbed by shield
            EmitDamageSparks(PlyrIndex);
            return 0.0f;
        }
    }
    else
        Remaining = Damage;

    // shield didn't absord all damage, reduce the tank's power
    DamageTaken += Remaining;
    _Players[ PlyrIndex ].TankPower -= Remaining;
    if (_Players[ PlyrIndex ].TankPower <= 0.0f)
    {
        ExplodePlayer(PlyrIndex);

        // update the play order dialog
        DialogUpdatePlayOrder(DialogPlayOrder);
    }

    EmitDamageSparks(PlyrIndex);

    // adjust the current player's stats with the damage taken.
    History = GetPlayerHistory(GetCurrentPlayer());

    if (GetCurrentPlayer() == PlyrIndex)
        // then the current player has damaged themself
        History->DamageToSelf += DamageTaken;
    else if (GetPlayerTeam(GetCurrentPlayer()) == GetPlayerTeam(PlyrIndex))
        // then the current player has damaged a teammate
        History->DamageToTeammates += DamageTaken;
    else
        // then the current player has damaged an enemy
        History->DamageToEnemys += DamageTaken;

    return DamageTaken;
}


//----------------------------------------------------------------------------------------
// Name: ExplodePlayer()
// Desc:
//----------------------------------------------------------------------------------------

void ExplodePlayer(size_t PlyrIndex)
{
    Vector_t Position;
    Vector_t Normal;
    size_t ModelID, SoundID;
    PlayerHistory_t* History;

    if (INVALIDINDEX(PlyrIndex))
        return;

    ReturnParticleSystem(_Players[ PlyrIndex ].ParticleDamageSmoke);
    _Players[ PlyrIndex ].ParticleDamageSmoke = -1;

    if (IsObjectDead(_Players[ PlyrIndex ].TankObjectID))
        return;

    ClearObjectPhysics(_Players[ PlyrIndex ].TankObjectID);
    GetObjectPosition(_Players[ PlyrIndex ].TankObjectID, 0, &Position);

    if (!IsObjectDead(_Players[ PlyrIndex ].ShieldObjectID))
    {
        DeactivatePlayerShield(PlyrIndex);

        SetSoundPosition(SoundShieldDown0, Position.X, Position.Y, Position.Z);
        PlaySound3D(SoundShieldDown0, 0);

        SetSoundPosition(SoundShieldShatter0, Position.X, Position.Y, Position.Z);
        PlaySound3D(SoundShieldShatter0, 0);

        DeactivateObject(_Players[ PlyrIndex ].ShieldObjectID);
        HideObject(_Players[ PlyrIndex ].ShieldObjectID);
    }

    if (!IsObjectDead(_Players[ PlyrIndex ].ParachuteObjectID))
    {
        DeactivateObject(_Players[ PlyrIndex ].ParachuteObjectID);
        HideObject(_Players[ PlyrIndex ].ParachuteObjectID);
    }

    GetPlayerGroundNormal(PlyrIndex, &Normal);
    GetObjectModels(_Players[ PlyrIndex ].TankObjectID, &ModelID, nullptr);
    StartDebrisTank(&Position, &Normal, ModelID);

    DeactivateObject(_Players[ PlyrIndex ].TankObjectID);
    HideObject(_Players[ PlyrIndex ].TankObjectID);

    StartExplosion(&Position, PlyrIndex, &g_ExplosionPlayer);

    switch (rand() % 3)
    {
    case 0 : SoundID = SoundTankExplode0; break;
    case 1 : SoundID = SoundTankExplode1; break;
    case 2 : SoundID = SoundTankExplode2; break;
    }
    SetSoundPosition(SoundID, Position.X, Position.Y, Position.Z);
    PlaySound3D(SoundID, 0);

    // adjust the current player's stats with the kill
    History = GetPlayerHistory(GetCurrentPlayer());

    if (GetCurrentPlayer() == PlyrIndex)
        // then the current player has killed themself
        History->SelfKills++;
    else if (GetPlayerTeam(GetCurrentPlayer()) == GetPlayerTeam(PlyrIndex))
        // then the current player has killed a teammate
        History->TeammateKills++;
    else
        // then the current player has killed an enemy
        History->EnemyKills++;

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(GetObjectPositionPtr(_Players[PlyrIndex].TankObjectID, 0) , Position);
}


//-----------------------------------------------------------------------------
// Name: PlayerTakeCollisionDamage()
// Desc:
//-----------------------------------------------------------------------------

void PlayerTakeCollisionDamage(size_t ObjectID, float Damage)
{
    size_t i;

    // find out which player this is, if any

    for (i = 0; i < GetPlayerCount(); i++)
    {
        if (ObjectID == GetPlayerTank(i))
        {
            PlayerTakeDamage(i, Damage);
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: GetPlayerBotType()
// Desc:
//-----------------------------------------------------------------------------

BotType_t GetPlayerBotType(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return BOTTYPE_NOTABOT;

    return _Players[ PlyrIndex ].BotType;
}


//-----------------------------------------------------------------------------
// Name: GetBotInfo()
// Desc:
//-----------------------------------------------------------------------------

BotInfo_t* GetBotInfo(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return nullptr;

    return &_Players[ PlyrIndex ].BotInfo;
}


//-----------------------------------------------------------------------------
// Name: InitaliseBotTurn()
// Desc:
//-----------------------------------------------------------------------------

void InitaliseBotTurn(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].BotInfo.State = BOTSTATE_CHOOSINGTURN;
}


//-----------------------------------------------------------------------------
// Name: GetPlayerHistory()
// Desc:
//-----------------------------------------------------------------------------

PlayerHistory_t* GetPlayerHistory(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return nullptr;

    return &_Players[ PlyrIndex ].History;
}


//-----------------------------------------------------------------------------
// Name: SetPlayerTankPower()
// Desc:
//-----------------------------------------------------------------------------

void SetPlayerTankPower(size_t PlyrIndex, float TankPower)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].TankPower = TankPower;
}


//-----------------------------------------------------------------------------
// Name: SetPlayerShieldPower()
// Desc:
//-----------------------------------------------------------------------------

void SetPlayerShieldPower(size_t PlyrIndex, float ShieldPower)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].ShieldPower = ShieldPower;
}


//-----------------------------------------------------------------------------
// Name: SetPlayerDamageSmokeID()
// Desc:
//-----------------------------------------------------------------------------

void SetPlayerDamageSmokeID(size_t PlyrIndex, size_t ParticleID)
{
    if (INVALIDINDEX(PlyrIndex))
        return;

    _Players[ PlyrIndex ].ParticleDamageSmoke = ParticleID;
}


//-----------------------------------------------------------------------------
// Name: GetPlayerDamageSmokeID()
// Desc:
//-----------------------------------------------------------------------------

size_t GetPlayerDamageSmokeID(size_t PlyrIndex)
{
    if (INVALIDINDEX(PlyrIndex))
        return -1;

    return _Players[ PlyrIndex ].ParticleDamageSmoke;
}


//-----------------------------------------------------------------------------
// Name: EmitDamageSparks()
// Desc:
//-----------------------------------------------------------------------------

static void DamageSparksFinished(size_t ID)
{
    size_t i;

    // find out which player this is for
    for (i = 0; i < GetPlayerCount(); i++)
    {
        if (ID == _Players[ i ].ParticleDamageSparks)
        {
            _Players[ i ].ParticleDamageSparks = -1;
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: EmitDamageSparks()
// Desc:
//-----------------------------------------------------------------------------

static void EmitDamageSparks(size_t PlyrIndex)
{
    size_t SoundID;
    Vector_t Position;

    if (INVALIDINDEX(PlyrIndex))
        return;

    if (-1 != _Players[ PlyrIndex ].ParticleDamageSparks)
        return;

    if (SHIELDTYPE_NONE == GetPlayerShieldType(PlyrIndex))
    {
        _Players[ PlyrIndex ].ParticleDamageSparks = StartTankSpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);
        SoundID = 0 == rand() % 2 ? SoundTankHit0 : SoundTankHit1;
    }
    else
    {
        switch (rand() % 3)
        {
        case 0 : SoundID = SoundShieldHit0; break;
        case 1 : SoundID = SoundShieldHit1; break;
        case 2 : SoundID = SoundShieldHit2; break;
        }

        if (SHIELDTYPE_LIGHT == GetPlayerShieldType(PlyrIndex))
            _Players[ PlyrIndex ].ParticleDamageSparks = StartShieldLightSpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);

        else if (SHIELDTYPE_MEDIUM == GetPlayerShieldType(PlyrIndex))
            _Players[ PlyrIndex ].ParticleDamageSparks = StartShieldMediumSpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);

        else if (SHIELDTYPE_HEAVY == GetPlayerShieldType(PlyrIndex))
            _Players[ PlyrIndex ].ParticleDamageSparks = StartShieldHeavySpark(GetPlayerTank(PlyrIndex), DamageSparksFinished);
    }

    GetObjectPosition(_Players[ PlyrIndex ].TankObjectID, 0, &Position);
    SetSoundPosition(SoundID, Position.X, Position.Y, Position.Z);
    PlaySound3D(SoundID, 0);
}
