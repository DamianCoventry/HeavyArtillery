//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 10 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>

#include "ObjectTypes.h"
#include "LightSource.h"

#include "ShotManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "ExplosionManager.h"
#include "MeshManager.h"
#include "SoundManager.h"

#include "GameExplosions.h"
#include "GameSounds.h"
#include "ParticleDebrisGround.h"
#include "Camera.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// Name: CollideMeshLeapFrog()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideMeshLeapFrog( Object_t *Object )
{
    Vector_t Position;
    Vector_t *Move;
    Weapon_t *Weapon;
    Vector_t Normal;
    size_t SoundID;

    GetObjectPosition( Object->ID, 0, &Position );

    Weapon = GetShotWeapon( Object->Data );
    StartExplosion( &Position, GetCurrentPlayer(), Weapon->ExplType );

    switch( rand() % 3 )
    {
        case 0 : SoundID = SoundMissileExplode0; break;
        case 1 : SoundID = SoundMissileExplode1; break;
        case 2 : SoundID = SoundMissileExplode2; break;
    }
    SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
    PlaySound3D( SoundID, 0 );

    GetMeshNormal( g_LandscapeMesh, Position.X, Position.Z, &Normal );
    StartDebrisGround( &Position, &Normal );

    if( !UseShotProjectile( Object->Data ) )
    {
        StopSmokeTrail( Object->Data );

        ReturnShot( Object->Data );
        KillObject( Object->ID );
        return;
    }

    Position.Y += 32.0f;
    SetObjectPosition( Object->ID, 0, &Position );

    Move = GetObjectMovement( Object->ID );
    Move->Y = -Move->Y;
    VectorScale( (*Move), Weapon->BounceReduction, (*Move) );

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(&Object->Positions[0], Position);
}


//----------------------------------------------------------------------------------------
// Name: CollideObjectLeapFrog()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideObjectLeapFrog( Object_t *Projectile, Object_t *Object )
{
    Vector_t Position;
    Weapon_t *Weapon;
    Vector_t *Move;
    PlayerHistory_t *PlayerHistory;
    size_t SoundID;

    // this projectile has hit a tank, add a hit to the player's stats

    PlayerHistory = GetPlayerHistory( GetCurrentPlayer() );
    if( !PlayerHistory->HasHitThisTurn )
    {
        PlayerHistory->NumShotsHit++;
        PlayerHistory->HasHitThisTurn = 1;
    }

    GetObjectPosition( Projectile->ID, 0, &Position );

    Weapon = GetShotWeapon( Projectile->Data );
    StartExplosion( &Position, GetCurrentPlayer(), Weapon->ExplType );

    switch( rand() % 3 )
    {
        case 0 : SoundID = SoundMissileExplode0; break;
        case 1 : SoundID = SoundMissileExplode1; break;
        case 2 : SoundID = SoundMissileExplode2; break;
    }
    SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
    PlaySound3D( SoundID, 0 );

    if( !UseShotProjectile( Projectile->Data ) )
    {
        StopSmokeTrail( Projectile->Data );

        ReturnShot( Projectile->Data );
        KillObject( Projectile->ID );
        return;
    }

    Position.Y += 32.0f;
    SetObjectPosition( Projectile->ID, 0, &Position );

    Move = GetObjectMovement( Object->ID );
    Move->Y = -Move->Y;
    VectorScale( (*Move), Weapon->BounceReduction, (*Move) );

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(&Projectile->Positions[0], Position);
}
