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

#include "ObjectTypes.h"
#include "GameTypes.h"
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

#include "ParticleDecalSmoke.h"
#include "ParticleDebrisGround.h"

#include "PlayerHistory.h"
#include "Camera.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _RailgunCounter = 0;


//----------------------------------------------------------------------------------------
// Name: CollideMeshExplode()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideMeshExplode( Object_t *Object )
{
    Weapon_t *Weapon;
    Vector_t Position;
    Vector_t Normal;
    size_t SoundID;

    GetObjectPosition( Object->ID, 0, &Position );

    Weapon = GetShotWeapon( Object->Data );
    StartExplosion( &Position, GetCurrentPlayer(), Weapon->ExplType );
    StopSmokeTrail( Object->Data );

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

    ReturnShot( Object->Data );
    KillObject( Object->ID );

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(&Object->Positions[0], Position);
}


//----------------------------------------------------------------------------------------
// Name: CollideObjectBounce()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideObjectBounce( Object_t *Projectile, Object_t *TankObject )
{
    Vector_t V;
    Vector_t Zero;
    float Mag, Damage;
    PlayerHistory_t *PlayerHistory;

    // this projectile has hit a tank, add a hit to the player's stats

    PlayerHistory = GetPlayerHistory( GetCurrentPlayer() );
    if( !PlayerHistory->HasHitThisTurn )
    {
        PlayerHistory->NumShotsHit++;
        PlayerHistory->HasHitThisTurn = 1;
    }

    VectorSet( Zero, 0.0f );

    if( VectorFuzzyCompare( Projectile->Physics.Move, Zero, EPSILON ) )
    {
        if( VectorFuzzyCompare( TankObject->Physics.Move, Zero, EPSILON ) )
        {
            // then both objects are stationary - do nothing
        }
        else
        {
            // Projectile is stationary but TankObject is moving

            // create a vector from the TankObject to the Projectile
            VectorSubtract( TankObject->Positions[ 0 ], Projectile->Positions[ 0 ], V );
            VectorNormalise( &V );
            Mag = fabsf( VectorMagnitude( &TankObject->Physics.Move ) );

            if( !( TankObject->Physics.Flags & PHYSICS_HASCOLLIDED ) )
            {
                // bounce TankObject off Projectile
                VectorReflect( &TankObject->Physics.Move, &V, &TankObject->Physics.Move );
            }

            if( !( Projectile->Physics.Flags & PHYSICS_HASCOLLIDED ) )
            {
                // push Projectile away
                VectorInverse( V );
                VectorScale( V, Mag, V );
                VectorAdd( Projectile->Physics.Move, V, Projectile->Physics.Move );
            }

            Damage = Mag + fabsf( VectorMagnitude( &Projectile->Physics.Move ) );
        }
    }
    else if( VectorFuzzyCompare( TankObject->Physics.Move, Zero, EPSILON ) )
    {
        // TankObject is stationary but Projectile is moving

        // create a vector from the TankObject to the Projectile
        VectorSubtract( Projectile->Positions[ 0 ], TankObject->Positions[ 0 ], V );
        VectorNormalise( &V );
        Mag = fabsf( VectorMagnitude( &Projectile->Physics.Move ) );

        if( !( Projectile->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // bounce Projectile off Projectile
            VectorReflect( &Projectile->Physics.Move, &V, &Projectile->Physics.Move );
        }

        if( !( TankObject->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // push TankObject away
            VectorInverse( V );
            VectorScale( V, Mag, V );
            VectorAdd( TankObject->Physics.Move, V, TankObject->Physics.Move );
        }

        Damage = Mag + fabsf( VectorMagnitude( &TankObject->Physics.Move ) );
    }
    else
    {
        // both objects are moving

        // create a vector from the TankObject to the Projectile
        VectorSubtract( Projectile->Positions[ 0 ], TankObject->Positions[ 0 ], V );
        VectorNormalise( &V );

        if( !( Projectile->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // bounce Projectile off Projectile
            VectorReflect( &Projectile->Physics.Move, &V, &Projectile->Physics.Move );
        }

        if( !( TankObject->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // bounce TankObject off Projectile
            VectorInverse( V );
            VectorReflect( &TankObject->Physics.Move, &V, &TankObject->Physics.Move );
        }

        Damage = fabsf( VectorMagnitude( &Projectile->Physics.Move ) ) + fabsf( VectorMagnitude( &TankObject->Physics.Move ) );
    }

    // take one step so the two objects are no longer colliding
    if( !( Projectile->Physics.Flags & PHYSICS_HASCOLLIDED ) )
    {
        VectorAdd( Projectile->Physics.Move, Projectile->Positions[ 0 ], Projectile->Positions[ 0 ] );
        PlayerTakeCollisionDamage( Projectile->ID, Damage );
    }

    if( !( TankObject->Physics.Flags & PHYSICS_HASCOLLIDED ) )
    {
        VectorAdd( TankObject->Physics.Move, TankObject->Positions[ 0 ], TankObject->Positions[ 0 ] );
        PlayerTakeCollisionDamage( TankObject->ID, Damage );
    }

    Projectile->Physics.Flags |= PHYSICS_HASCOLLIDED;
    TankObject->Physics.Flags |= PHYSICS_HASCOLLIDED;
}


//----------------------------------------------------------------------------------------
// Name: CollideObjectExplode()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideObjectExplode( Object_t *Projectile, Object_t *Object )
{
    Weapon_t *Weapon;
    Vector_t Position;
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
    StopSmokeTrail( Projectile->Data );

    switch( rand() % 3 )
    {
        case 0 : SoundID = SoundMissileExplode0; break;
        case 1 : SoundID = SoundMissileExplode1; break;
        case 2 : SoundID = SoundMissileExplode2; break;
    }
    SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
    PlaySound3D( SoundID, 0 );

    ReturnShot( Projectile->Data );
    KillObject( Projectile->ID );

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(&Projectile->Positions[0], Position);
}


//----------------------------------------------------------------------------------------
// Name: UpdateShotShading()
// Desc: 
//----------------------------------------------------------------------------------------

void UpdateShotShading( Object_t *Object, size_t IsHiResModel )
{
    Vector_t LightDirection;
    float Ambient;

    GetLightSourceDirection( &LightDirection.X, &LightDirection.Y, &LightDirection.Z );
    GetLightSourceAmbient( &Ambient );

    switch( Object->State )
    {
        case OBJECTSTATE_DEAD :
            break;

        case OBJECTSTATE_DYING :
            break;

        case OBJECTSTATE_ALIVE :
            if( IsHiResModel )
                CalcObjectPointColors( Object->ModelIDHiRes, &LightDirection, Ambient,
                                       Object->Positions, Object->Rotations, Object->PointColorsHiRes );
            else
                CalcObjectPointColors( Object->ModelIDLoRes, &LightDirection, Ambient,
                                       Object->Positions, Object->Rotations, Object->PointColorsLoRes );

            break;
    }

    UseGuidance( Object->Data );
}


//----------------------------------------------------------------------------------------
// Name: FaceDirection()
// Desc: 
//----------------------------------------------------------------------------------------

void FaceDirection( Object_t *Object, size_t IsHiResModel )
{
    Vector_t V1, V2;
    float Dot;

    // calc the heading based on the xz movement
    V1.X = V1.Y = 0.0f; V1.Z = -1.0f;
    VectorCopy( Object->Physics.Move, V2 );
    V2.Y = 0.0f;
    VectorNormalise( &V2 );

    Dot = VectorDotProd( V2, V1 );
    Object->Rotations[ 0 ].Y = RTOD * (float)acos( Dot );

    V1.X = 1.0f; V1.Y = V1.Z = 0.0f;
    Dot = VectorDotProd( V2, V1 );
    if( Dot < 0.0f )
        Object->Rotations[ 0 ].Y = 360.0f - Object->Rotations[ 0 ].Y;

    Object->Rotations[ 0 ].Y -= 180.0f;     // adjust for open being right handed

    // calc the pitch based on the yz or xy movement
    V2.X = 0.0f;
    V2.Y = Object->Physics.Move.Y;
    V2.Z = Max( fabsf( Object->Physics.Move.X ), fabsf( Object->Physics.Move.Z ) );
    V2.Z = -V2.Z;
    VectorNormalise( &V2 );

    V1.X = V1.Y = 0.0f; V1.Z = -1.0f;
    Dot = VectorDotProd( V2, V1 );
    Object->Rotations[ 0 ].X = RTOD * (float)acos( Dot );

    V1.X = V1.Z = 0.0f; V1.Y = 1.0f;
    Dot = VectorDotProd( V2, V1 );
    if( Dot < 0.0f )
        Object->Rotations[ 0 ].X = -Object->Rotations[ 0 ].X;

    Object->Rotations[ 0 ].Z += 1.0f;

    SaveShotPosition(Object->Data, Object->Positions[0]);

    // shade the object
    UpdateShotShading( Object, IsHiResModel );
}


//----------------------------------------------------------------------------------------
// Name: RollerBouncerThink()
// Desc: 
//----------------------------------------------------------------------------------------

void RollerBouncerThink( Object_t *Projectile, size_t IsHiRes )
{
    Weapon_t *Weapon;
    Vector_t Position;
    size_t SoundID;

    UpdateShotShading( Projectile, IsHiRes );

    Projectile->Rotations[ 0 ].Y += 2.0f;
    Projectile->Rotations[ 0 ].Z += 1.0f;

    if( fabsf( VectorMagnitude( &Projectile->Physics.Move ) ) <= EPSILON )
    {
        GetObjectPosition( Projectile->ID, 0, &Position );

        Weapon = GetShotWeapon( Projectile->Data );
        StartExplosion( &Position, GetCurrentPlayer(), Weapon->ExplType );
        StopSmokeTrail( Projectile->Data );

        switch( rand() % 3 )
        {
            case 0 : SoundID = SoundMissileExplode0; break;
            case 1 : SoundID = SoundMissileExplode1; break;
            case 2 : SoundID = SoundMissileExplode2; break;
        }
        SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
        PlaySound3D( SoundID, 0 );

        ReturnShot( Projectile->Data );
        KillObject( Projectile->ID );

        GetCameraShakeLookAtPosition(Position);
        StopCameraTrackingObject(&Projectile->Positions[0], Position);
    }

    SaveShotPosition(Projectile->Data, Projectile->Positions[0]);
}


//----------------------------------------------------------------------------------------
// Name: RailgunThink()
// Desc: 
//----------------------------------------------------------------------------------------

void RailgunThink( Object_t *Projectile, size_t IsHisRes )
{
    if( ++_RailgunCounter > RAILGUN_THRESHOLD )
    {
        _RailgunCounter = 0;

        ReturnShot( Projectile->Data );
        KillObject( Projectile->ID );
    }
}
