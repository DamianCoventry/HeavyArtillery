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

#include "Projectile.h"
#include "StateWaitForShot.h"
#include "Shoot.h"
#include "FunkyBomb.h"
#include "ParticleDebrisGround.h"

#include "GameWeapons.h"
#include "GameSounds.h"
#include "Camera.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _State = 0;


//----------------------------------------------------------------------------------------
// Name: FunkyBombThink()
// Desc: 
//----------------------------------------------------------------------------------------

void FunkyBombThink( Object_t *Projectile, size_t IsHiResModel )
{
    Vector_t Position;
    Vector_t ShootVector;
    float ShotPower;
    size_t Modulus, ShotID;
    size_t EmitFunky;

    switch( Projectile->ThinkCounter1 )
    {
        case 0 :
            Projectile->Rotations[ 0 ].X += 2.0f;
            Projectile->Rotations[ 0 ].Y -= 1.0f;

            UpdateShotShading( Projectile, IsHiResModel );
            break;

        case 1 :
            // is it time to emit a new shot?
            if( ++Projectile->ThinkCounter2 > FUNKYBOMB_EMITTHRESHOLD )
            {
                Projectile->ThinkCounter2 = 0;

                // have we emitted the total amount of shots yet?
                if( ++Projectile->ThinkCounter3 > FUNKYBOMB_MISSILECOUNT )
                {
                    // yep, return the shot to the pool and the kill the object
                    Projectile->ThinkCounter3 = 0;

                    ReturnShot( Projectile->Data );
                    KillObject( Projectile->ID );
                }
                else
                {
                    // no, choose a point to emit a new shot from
                    VectorCopy( Projectile->Positions[ 0 ], Position );
                    Position.Y += 64.0f;

                    // choose an emit direction...
                    ShootVector.X = ShootVector.Y = 0.0f; ShootVector.Z = 1.0f;
                    VectorRotateX( -DTOR * (float)( (size_t)rand() % (size_t)20 + (size_t)60 ), &ShootVector );
                    VectorRotateY( -DTOR * (float)( (size_t)rand() % (size_t)360 ), &ShootVector );

                    // ...and power
                    ShotPower = (float)( (size_t)rand() % (size_t)20 + (size_t)20 );

                    EmitFunky = 0;

                    // decide whether or not to emit another funky bomb

                    // have we already emitted a funkybomb from this explosion or is this the last
                    // funky bomb in the chain?
                    if( Projectile->ThinkCounter4 || GetShotProjectileCount( Projectile->Data ) <= 0 )
                    {
                        // yes, so only emit a sm or lg missile
                        Modulus = 2;
                    }
                    else
                    {
                        // no we haven't emitted one yet

                        // if this is the last shot from this explosion then emit one
                        if( Projectile->ThinkCounter3 == FUNKYBOMB_MISSILECOUNT )
                            EmitFunky = 1;
                        else
                            Modulus = 3;
                    }

                    if( EmitFunky )
                    {
                        Projectile->ThinkCounter4 = 1;

                        ShotID = WeaponFunkyBomb.Shoot( &WeaponFunkyBomb, &Position, &ShootVector, ShotPower, 0 );

                        SetSoundPosition( SoundShotEmit0, Position.X, Position.Y, Position.Z );
                        PlaySound3D( SoundShotEmit0, 0 );

                        // make the camera follow this new funky bomb
                        UpdateCameraToFollowShot( ShotID );

                        // make the new funky bomb 'inherit' this funky bomb's projectile count
                        UseShotProjectile( Projectile->Data );
                        SetShotProjectileCount( ShotID, GetShotProjectileCount( Projectile->Data ) );
                    }
                    else
                    {
                        // emit the new shot
                        switch( rand() % Modulus )
                        {
                            case 0 :
                                WeaponMissileSmall.Shoot( &WeaponMissileSmall, &Position, &ShootVector, ShotPower, 0 );

                                SetSoundPosition( SoundShotEmit0, Position.X, Position.Y, Position.Z );
                                PlaySound3D( SoundShotEmit0, 0 );
                                break;

                            case 1 :
                                WeaponMissileLarge.Shoot( &WeaponMissileLarge, &Position, &ShootVector, ShotPower, 0 );

                                SetSoundPosition( SoundShotEmit0, Position.X, Position.Y, Position.Z );
                                PlaySound3D( SoundShotEmit0, 0 );
                                break;

                            case 2 :
                                Projectile->ThinkCounter4 = 1;

                                ShotID = WeaponFunkyBomb.Shoot( &WeaponFunkyBomb, &Position, &ShootVector, ShotPower, 0 );

                                SetSoundPosition( SoundShotEmit0, Position.X, Position.Y, Position.Z );
                                PlaySound3D( SoundShotEmit0, 0 );

                                // make the camera follow this new funky bomb
                                UpdateCameraToFollowShot( ShotID );

                                // make the new funky bomb 'inherit' this funky bomb's projectile count
                                UseShotProjectile( Projectile->Data );
                                SetShotProjectileCount( ShotID, GetShotProjectileCount( Projectile->Data ) );

                                break;
                        }
                    }
                }
            }
            break;
    }

    SaveShotPosition(Projectile->Data, Projectile->Positions[0]);
}


//----------------------------------------------------------------------------------------
// Name: FunkyBombMeshExplode()
// Desc: 
//----------------------------------------------------------------------------------------

void FunkyBombMeshExplode( Object_t *Object )
{
    Weapon_t *Weapon;
    Vector_t Position;
    Vector_t Normal;
    size_t SoundID;

    GetObjectPosition( Object->ID, 0, &Position );

    Weapon = GetShotWeapon( Object->Data );
    StartExplosion( &Position, GetCurrentPlayer(), Weapon->ExplType );

    switch( rand() % 3 )
    {
        case 0 : SoundID = SoundBombExplode0; break;
        case 1 : SoundID = SoundBombExplode1; break;
        case 2 : SoundID = SoundBombExplode2; break;
    }
    SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
    PlaySound3D( SoundID, 0 );

    GetMeshNormal( g_LandscapeMesh, Position.X, Position.Z, &Normal );
    StartDebrisGround( &Position, &Normal );

    HideObject( Object->ID );
    DeactivateObject( Object->ID );

    Object->ThinkCounter1 = 1;

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(&Object->Positions[0], Position);
}


//----------------------------------------------------------------------------------------
// Name: FunkyBombObjectExplode()
// Desc: 
//----------------------------------------------------------------------------------------

void FunkyBombObjectExplode( Object_t *Projectile, Object_t *Object )
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

    switch( rand() % 3 )
    {
        case 0 : SoundID = SoundBombExplode0; break;
        case 1 : SoundID = SoundBombExplode1; break;
        case 2 : SoundID = SoundBombExplode2; break;
    }
    SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
    PlaySound3D( SoundID, 0 );

    HideObject( Projectile->ID );
    DeactivateObject( Projectile->ID );

    Object->ThinkCounter1 = 1;

    GetCameraShakeLookAtPosition(Position);
    StopCameraTrackingObject(&Projectile->Positions[0], Position);
}
