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
#include <math.h>

#include "Vector.h"
#include "Explosion.h"
#include "ExplosionTypes.h"
#include "Camera.h"

#include "PlayerManager.h"
#include "ObjectManager.h"
#include "ParticleManager.h"
#include "MeshManager.h"
#include "DecalManager.h"
#include "ModelManager.h"
#include "LevelManager.h"
#include "SoundManager.h"

#include "GameTypes.h"
#include "GameModels.h"
#include "GameSounds.h"

#include "PlayerHistory.h"

#include "ParticleDecalSmoke.h"
#include "ParticleCentralExplosion.h"
#include "ParticleExplFireball.h"
#include "ParticleExplSparks.h"

#define MAXEXPLOSIONS           128

#define EXPLOSION_FADESPEED     0.01f

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXEXPLOSIONS || !_ExplosionPool[ _id ].Used )


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private structures
//----------------------------------------------------------------------------------------

typedef struct Explosion_s
{
    size_t Used;
    size_t FrameCounter;

    size_t ParticleID;
    size_t CentralExplosionID;
    size_t ShockwaveID;
    size_t FireballID;
    size_t SparksID;

    size_t IsFireballsFinished;
    size_t IsSparksFinished;

    size_t ObjectID;

    ExplosionType_t *ExplType;
}
Explosion_t;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _ExplosionIndex = 0;
static size_t _ExplosionCount = 0;
static Explosion_t _ExplosionPool[ MAXEXPLOSIONS ];


//----------------------------------------------------------------------------------------
// private functions
//----------------------------------------------------------------------------------------

static void FireballsFinished( size_t ID );
static void SparksFinished( size_t ID );


//----------------------------------------------------------------------------------------
// Name: GetFreeExplosion()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t GetFreeExplosion( void )
{
    size_t i;
    size_t ID;

    i = _ExplosionIndex;

    while( _ExplosionPool[ i ].Used )
    {
        i++;
        if( i >= MAXEXPLOSIONS )
            i = 0;

        if( i == _ExplosionIndex )
            return -1;        // pool is full
    }

    _ExplosionPool[ i ].Used = 1;
    ID = i;

    _ExplosionIndex = i;
    if( ++_ExplosionIndex >= MAXEXPLOSIONS )
        _ExplosionIndex = 0;

    _ExplosionCount++;
    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnExplosion()
// Desc: 
//----------------------------------------------------------------------------------------

void ReturnExplosion( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ExplosionCount--;
    _ExplosionPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: GetExplosionCount()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetExplosionCount( void )
{
    return _ExplosionCount;
}


//----------------------------------------------------------------------------------------
// Name: DispatchDamageToPlayers()
// Desc: 
//----------------------------------------------------------------------------------------

static void DispatchDamageToPlayers( Vector_t *Position, size_t Initiator, ExplosionType_t *ExplType )
{
    size_t PlyrIndex;
    Vector_t PlyrPosition;
    Vector_t Force;
    PlayerHistory_t *PlayerHistory;
    float DistanceFromExpl;
    float PercentageOfDamage;
    float TotalDamageIssued = 0.0f;

    for( PlyrIndex = 0; PlyrIndex < GetPlayerCount(); PlyrIndex++ )
    {
        GetPlayerPosition( PlyrIndex, &PlyrPosition );
        DistanceFromExpl = fabsf( VectorMagnitude2( Position, &PlyrPosition ) );

        // is the player within the blast radius?
        if( DistanceFromExpl <= ExplType->Radius )
        {
            PercentageOfDamage = 1.0f - ( DistanceFromExpl / ExplType->Radius );

            // propel the player away from centre of explosion
            VectorSubtract( PlyrPosition, (*Position), Force );
            VectorNormalise( &Force );
            VectorScale( Force, PercentageOfDamage * ( 5.0f * TERMINALVELOCITY ), Force );
            ApplyForceToObject( GetPlayerTank( PlyrIndex ), &Force );

            // issue damage to this player
            TotalDamageIssued += PlayerTakeDamage( PlyrIndex, PercentageOfDamage * ExplType->Damage );
        }
        // else too far away, no damage issued to this player
    }

    // if no damage was issued by this explosion, then add a
    // miss to the player's stats, otherwise add a hit

    PlayerHistory = GetPlayerHistory( GetCurrentPlayer() );
    if( !PlayerHistory->HasHitThisTurn )
    {
        if( TotalDamageIssued > 0.0f )
            PlayerHistory->NumShotsHit++;
        else
            PlayerHistory->NumShotsMissed++;

        PlayerHistory->HasHitThisTurn = 1;
    }
}


//----------------------------------------------------------------------------------------
// Name: CreateObjectsAndParticles()
// Desc: 
//----------------------------------------------------------------------------------------

static void CreateObjectsAndParticles( size_t ID, Vector_t *Position, ExplosionType_t *ExplType )
{
    Vector_t V;
    Vector_t Scale;
    Color_t Color;

    // create the mushroom cloud
    _ExplosionPool[ ID ].ObjectID = GetFreeObject();
    ResetObject( _ExplosionPool[ ID ].ObjectID );

    SetObjectData( _ExplosionPool[ ID ].ObjectID, ID );
    SetObjectBlendFactor( _ExplosionPool[ ID ].ObjectID, 1.0f );

    SetObjectOnThinkFunction( _ExplosionPool[ ID ].ObjectID, ExplosionThink );
    SetObjectDrawFunction2( _ExplosionPool[ ID ].ObjectID, DrawExplosionModel );
    SetObjectRotateType( _ExplosionPool[ ID ].ObjectID, MODELROTATE_XZY );

    Color.R = Color.G = Color.B = Color.A = 1.0f;
    SetObjectBaseColor( _ExplosionPool[ ID ].ObjectID, &Color );

    SetObjectModels( _ExplosionPool[ ID ].ObjectID, g_ModelExplosion, -1 );
    SetObjectPosition( _ExplosionPool[ ID ].ObjectID, 0, Position );

    VectorSet( Scale, ExplType->BaseScale );
    SetObjectScale( _ExplosionPool[ ID ].ObjectID, 0, &Scale );

    V.Y = 0.0f;
    GetMeshRotations( g_LandscapeMesh, Position->X, Position->Z, &V.X, &V.Z );
    SetObjectRotation( _ExplosionPool[ ID ].ObjectID, 0, &V );

    ActivateObject( _ExplosionPool[ ID ].ObjectID );
    ShowObject( _ExplosionPool[ ID ].ObjectID );

    // create the shockwave
    _ExplosionPool[ ID ].ShockwaveID = GetFreeObject();
    ResetObject( _ExplosionPool[ ID ].ShockwaveID );

    SetObjectBlendFactor( _ExplosionPool[ ID ].ShockwaveID, 0.75f );

    SetObjectOnThinkFunction( _ExplosionPool[ ID ].ShockwaveID, ShockwaveThink );
    SetObjectDrawFunction2( _ExplosionPool[ ID ].ShockwaveID, DrawExplosionModel );
    SetObjectRotateType( _ExplosionPool[ ID ].ShockwaveID, MODELROTATE_XZY );

    SetObjectModels( _ExplosionPool[ ID ].ShockwaveID, g_ModelShockwave, -1 );
    SetObjectPosition( _ExplosionPool[ ID ].ShockwaveID, 0, Position );

    Color.R = Color.G = Color.A = 1.0f; Color.B = 0.0f;
    SetObjectBaseColor( _ExplosionPool[ ID ].ShockwaveID, &Color );

    VectorSet( Scale, ExplType->BaseScale );
    Scale.Y = 1.0f;
    SetObjectScale( _ExplosionPool[ ID ].ShockwaveID, 0, &Scale );

    V.Y = 0.0f;
    GetMeshRotations( g_LandscapeMesh, Position->X, Position->Z, &V.X, &V.Z );
    SetObjectRotation( _ExplosionPool[ ID ].ShockwaveID, 0, &V );

    ActivateObject( _ExplosionPool[ ID ].ShockwaveID );
    ShowObject( _ExplosionPool[ ID ].ShockwaveID );

    // start particle systems
    _ExplosionPool[ ID ].CentralExplosionID = StartCentralExplosion( Position, ExplType );

    GetMeshNormal( g_LandscapeMesh, Position->X, Position->Z, &V );

    _ExplosionPool[ ID ].FireballID = StartExplFireball( Position, &V, ExplType, FireballsFinished );
    _ExplosionPool[ ID ].IsFireballsFinished = 0;
    SetParticleData( _ExplosionPool[ ID ].FireballID, ID );

    _ExplosionPool[ ID ].SparksID = StartExplSparks( Position, &V, ExplType, SparksFinished );
    _ExplosionPool[ ID ].IsSparksFinished = 0;
    SetParticleData( _ExplosionPool[ ID ].SparksID, ID );
}


//----------------------------------------------------------------------------------------
// Name: PlaceDecal()
// Desc: 
//----------------------------------------------------------------------------------------

static void PlaceDecal( Vector_t *Position, ExplosionType_t *ExplType )
{
    DecalMgrCreate( GetMeshStruct( g_LandscapeMesh ), ExplType->DecalTexture, Position->X, Position->Z );

    // start smoke rising from the explosion point
    StartDecalSmoke( Position );
}


//----------------------------------------------------------------------------------------
// Name: StartExplosion()
// Desc: 
//----------------------------------------------------------------------------------------

size_t StartExplosion( Vector_t *Position, size_t Initiator, ExplosionType_t *ExplType )
{
    size_t ID = GetFreeExplosion();

    if( ID == -1 )
        return -1;

    _ExplosionPool[ ID ].FrameCounter   = 0;
    _ExplosionPool[ ID ].ExplType       = ExplType;

    DispatchDamageToPlayers( Position, Initiator, ExplType );
    CreateObjectsAndParticles( ID, Position, ExplType );
    PlaceDecal( Position, ExplType );

    Vector_t SlightlyAbovePosition{ *Position };
    SlightlyAbovePosition.Y += 5.0f;
    SetCameraShakeLookAtPosition(SlightlyAbovePosition);

    AddCameraShake( ExplType->CameraShakeFrameCount, -ExplType->CameraShakeAmount, ExplType->CameraShakeAmount );

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: StartNapalmExplosion()
// Desc: 
//----------------------------------------------------------------------------------------

size_t StartNapalmExplosion( Vector_t *Position, size_t Initiator, ExplosionType_t *ExplType, size_t ParticleID )
{
    size_t ID = GetFreeExplosion();

    if( ID == -1 )
        return -1;

    _ExplosionPool[ ID ].FrameCounter   = 0;
    _ExplosionPool[ ID ].ExplType       = ExplType;
    _ExplosionPool[ ID ].ParticleID     = ParticleID;

    DispatchDamageToPlayers( Position, Initiator, ExplType );
    CreateObjectsAndParticles( ID, Position, ExplType );
    PlaceDecal( Position, ExplType );

    Vector_t SlightlyAbovePosition{ *Position };
    SlightlyAbovePosition.Y += 5.0f;
    SetCameraShakeLookAtPosition(SlightlyAbovePosition);

    AddCameraShake( ExplType->CameraShakeFrameCount, -ExplType->CameraShakeAmount, ExplType->CameraShakeAmount );

    SetObjectOnThinkFunction( _ExplosionPool[ ID ].ObjectID, ExplosionNapalmThink );

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: IncrementExplosion()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IncrementExplosion( size_t ID )
{
    float BlendFactor;

    if( INVALIDID( ID ) )
        return 0;

    BlendFactor = GetObjectBlendFactor( _ExplosionPool[ ID ].ObjectID );
    BlendFactor -= EXPLOSION_FADESPEED;

    if( BlendFactor < 0.0f )
        BlendFactor = 0.0f;

    SetObjectBlendFactor( _ExplosionPool[ ID ].ObjectID, BlendFactor );

    if( BlendFactor <= 0.0f && _ExplosionPool[ ID ].IsFireballsFinished && _ExplosionPool[ ID ].IsSparksFinished )
    {
        ReturnParticleSystem( _ExplosionPool[ ID ].CentralExplosionID );
        ReturnObject( _ExplosionPool[ ID ].ShockwaveID );
        ReturnObject( _ExplosionPool[ ID ].ObjectID );
        ReturnExplosion( ID );
    }

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: IncrementNapalmExplosion()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IncrementNapalmExplosion( size_t ID )
{
    float BlendFactor;

    if( INVALIDID( ID ) )
        return 0;

    BlendFactor = GetObjectBlendFactor( _ExplosionPool[ ID ].ObjectID );
    BlendFactor -= EXPLOSION_FADESPEED;

    if( BlendFactor < 0.0f )
        BlendFactor = 0.0f;

    SetObjectBlendFactor( _ExplosionPool[ ID ].ObjectID, BlendFactor );

    if( BlendFactor <= 0.0f )
    {
        if( GetActiveParticleCount( _ExplosionPool[ ID ].ParticleID ) <= 0 )
        {
            ReturnParticleSystem( _ExplosionPool[ ID ].CentralExplosionID );

            ReturnObject( _ExplosionPool[ ID ].ObjectID );
            ReturnExplosion( ID );
        }
    }

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: GetExplosionType()
// Desc: 
//----------------------------------------------------------------------------------------

ExplosionType_t *GetExplosionType( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    return _ExplosionPool[ ID ].ExplType;
}


//----------------------------------------------------------------------------------------
// Name: FireballsFinished()
// Desc: 
//----------------------------------------------------------------------------------------

static void FireballsFinished( size_t ID )
{
    _ExplosionPool[ GetParticleData( ID ) ].IsFireballsFinished = 1;
}


//----------------------------------------------------------------------------------------
// Name: SparksFinished()
// Desc: 
//----------------------------------------------------------------------------------------

static void SparksFinished( size_t ID )
{
    _ExplosionPool[ GetParticleData( ID ) ].IsSparksFinished = 1;
}
