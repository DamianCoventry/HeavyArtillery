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
#include "LightSource.h"

#include "ShotManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "MeshManager.h"
#include "ExplosionManager.h"
#include "ParticleManager.h"
#include "SoundManager.h"

#include "Projectile.h"
#include "StateWaitForShot.h"
#include "Shoot.h"
#include "Utility.h"
#include "Sound.h"

#include "GameWeapons.h"
#include "GameModels.h"
#include "GameTypes.h"
#include "GameSounds.h"
#include "Camera.h"

#include "ParticleAirStrikeBeacon.h"
#include "ParticleDebrisGround.h"

#define MAXAEROPLANES               5
#define MAXAEROPLANEWAYPOINTS       2
#define AEROPLANESPACING            256.0f
#define WAYPOINTBBOXSIZE            32.0f
#define AEROPLANEANGLEDELTA         2.0f
#define AEROPLANEMISSILECOUNT       4
#define AEROPLANETIMEOUTTHRESHOLD   4000


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _AeroplaneObjIDs[ MAXAEROPLANES ];

static size_t _PlaneFinishedCount = 0;

static Vector_t _Waypoints[ MAXAEROPLANEWAYPOINTS ];

static size_t _ParticleBeacon = -1;


//----------------------------------------------------------------------------------------
// Name: AeroplaneThink()
// Desc: 
//----------------------------------------------------------------------------------------

static void AeroplaneThink( Object_t *Aeroplane, size_t IsHiRes )
{
    Vector_t Position;
    Vector_t Direction;
    Weapon_t *Weapon;
    Vector_t ShootVector;
    float ShotPower;
    float Heading, AngleDelta;
    size_t i, ShotID;

    // is the air plane close enough to the destination waypoint?
    GetObjectPosition( Aeroplane->ID, 0, &Position );
    if( Position.X > _Waypoints[ Aeroplane->ThinkCounter1 ].X - WAYPOINTBBOXSIZE && Position.X < _Waypoints[ Aeroplane->ThinkCounter1 ].X + WAYPOINTBBOXSIZE &&
        Position.Z > _Waypoints[ Aeroplane->ThinkCounter1 ].Z - WAYPOINTBBOXSIZE && Position.Z < _Waypoints[ Aeroplane->ThinkCounter1 ].Z + WAYPOINTBBOXSIZE )
    {
        if( ++Aeroplane->ThinkCounter1 >= MAXAEROPLANEWAYPOINTS )
        {
            ReturnObject( Aeroplane->ID );
            _PlaneFinishedCount++;
            return;
        }
        else // must have just reached the first waypoint, drop the payload!
        {
            Weapon = (Weapon_t *)BackpackGetItem( GetPlayerBackpack( GetCurrentPlayer() ), BPITEM_TYPEWEAPON,
                                                  GetPlayerCurrentWeapon( GetCurrentPlayer() ) );

            if( Weapon == &WeaponAirStrikeSmall )
                Weapon = &WeaponMissileSmall;
            else
                Weapon = &WeaponMissileLarge;

            SetSoundPosition( SoundAeroplaneEmit0, Position.X, Position.Y, Position.Z );
            PlaySound3D( SoundAeroplaneEmit0, 0 );

            for( i = 0; i < AEROPLANEMISSILECOUNT; i++ )
            {
                GetObjectPosition( Aeroplane->ID, 0, &Position );

                // choose an emit direction...
                ShootVector.X = ShootVector.Y = 0.0f; ShootVector.Z = 1.0f;
                VectorRotateX( DTOR * (float)( (size_t)rand() % (size_t)30 + (size_t)45 ), &ShootVector );
                VectorRotateY( DTOR * (float)( (size_t)rand() % (size_t)360 ), &ShootVector );

                // ...and power
                ShotPower = (float)( (size_t)rand() % (size_t)10 + (size_t)15 );

                ShotID = Weapon->Shoot( Weapon, &Position, &ShootVector, ShotPower, 0 );

                // let the camera follow the last shot out of the plane
                if( Aeroplane->ID == _AeroplaneObjIDs[ 0 ] && i == AEROPLANEMISSILECOUNT - 1 )
                {
                    UpdateCameraToFollowShot( ShotID );
                    StopEmittingParticles( _ParticleBeacon );
                    _ParticleBeacon = -1;
                }
            }
        }
    }

    // rotate plane towards waypoint
    VectorSubtract( _Waypoints[ Aeroplane->ThinkCounter1 ], Aeroplane->Positions[ 0 ], Direction );
    VectorNormalise( &Direction );
    VectorGetAnglesXY( &Direction, nullptr, &Heading );

    while( Heading < 0.0f ) Heading += 360.0f;
    while( Heading >= 360.0f ) Heading -= 360.0f;

    Aeroplane->Rotations[ 0 ].Y = -Aeroplane->Rotations[ 0 ].Y;

    AngleDelta = AngleDiff( Heading, Aeroplane->Rotations[ 0 ].Y );
    if( fabsf( AngleDelta ) <= AEROPLANEANGLEDELTA )
    {
        Aeroplane->Rotations[ 0 ].Y = Heading;

        // level the plane
        if( Aeroplane->Rotations[ 0 ].Z < 2.0f )
            Aeroplane->Rotations[ 0 ].Z += 2.0f;
        else if( Aeroplane->Rotations[ 0 ].Z > 2.0f )
            Aeroplane->Rotations[ 0 ].Z -= 2.0f;
        else
            Aeroplane->Rotations[ 0 ].Z = 0.0f;
    }
    else if( AngleDelta < 0.0f )
    {
        Aeroplane->Rotations[ 0 ].Y += AEROPLANEANGLEDELTA;
        if( Aeroplane->Rotations[ 0 ].Y >= 360.0f )
            Aeroplane->Rotations[ 0 ].Y -= 360.0f;

        // bank the plane
        Aeroplane->Rotations[ 0 ].Z += 2.0f;
        if( Aeroplane->Rotations[ 0 ].Z > 90.0f )
            Aeroplane->Rotations[ 0 ].Z = 90.0f;
    }
    else // it must be > 0.0f
    {
        Aeroplane->Rotations[ 0 ].Y -= AEROPLANEANGLEDELTA;
        if( Aeroplane->Rotations[ 0 ].Y < 0.0f )
            Aeroplane->Rotations[ 0 ].Y += 360.0f;

        // bank the plane
        Aeroplane->Rotations[ 0 ].Z -= 2.0f;
        if( Aeroplane->Rotations[ 0 ].Z < -90.0f )
            Aeroplane->Rotations[ 0 ].Z = -90.0f;
    }

    // set speed to maximum
    Aeroplane->Physics.Move.X = Aeroplane->Physics.Move.Y = 0.0f; Aeroplane->Physics.Move.Z = 1.0f;
    VectorRotateY( DTOR * Aeroplane->Rotations[ 0 ].Y, &Aeroplane->Physics.Move );
    VectorScale( Aeroplane->Physics.Move, TERMINALVELOCITY, Aeroplane->Physics.Move );

    UpdateShotShading( Aeroplane, IsHiRes );

    Aeroplane->Rotations[ 0 ].Y = -Aeroplane->Rotations[ 0 ].Y;

    // boilerplate checks, incase an airplane somehow misses its waypoint.

    if( Aeroplane->ThinkCounter1 > 1 )
    {
        if( ++Aeroplane->ThinkCounter2 > AEROPLANETIMEOUTTHRESHOLD )
        {
            ReturnObject( Aeroplane->ID );
            _PlaneFinishedCount++;
            return;
        }
    }

    if( Aeroplane->Positions[ 0 ].X < -5000.0f || Aeroplane->Positions[ 0 ].X > 15000.0f ||
        Aeroplane->Positions[ 0 ].Z < -5000.0f || Aeroplane->Positions[ 0 ].Z > 15000.0f )
    {
        ReturnObject( Aeroplane->ID );
        _PlaneFinishedCount++;
    }
}


//----------------------------------------------------------------------------------------
// Name: SetupWayPoints()
// Desc: 
//----------------------------------------------------------------------------------------

static void SetupWayPoints( Object_t *Projectile, Vector_t *TargetPoint )
{
    size_t i;
    size_t TilesWide, TilesDeep;
    float TileWidth, TileDepth;
    Vector_t Position;
    Vector_t Direction, Rotation;
    Weapon_t *Weapon;

    GetMeshSize( g_LandscapeMesh, &TilesWide, &TilesDeep );
    GetMeshTileSize( g_LandscapeMesh, &TileWidth, &TileDepth );

    Position.Y = GetMeshLargestHeight( g_LandscapeMesh ) + TileWidth;
    TargetPoint->Y = Position.Y;

    // choose initial starting point
    switch( rand() % 4 )
    {
        case 0 : // left
            Position.X = -TileWidth;
            Position.Z = (float)( (size_t)rand() % (size_t)( (size_t)TilesDeep * (size_t)TileDepth ) );
            break;
        case 1 : // right
            Position.X = ( TilesWide * TileWidth ) + TileWidth;
            Position.Z = (float)( (size_t)rand() % (size_t)( (size_t)TilesDeep * (size_t)TileDepth ) );
            break;
        case 2 : // back
            Position.X = (float)( (size_t)rand() % (size_t)( (size_t)TilesWide * (size_t)TileWidth ) );
            Position.Z = -TileDepth;
            break;
        case 3 : // front
            Position.X = (float)( (size_t)rand() % (size_t)( (size_t)TilesWide * (size_t)TileWidth ) );
            Position.Z = ( TilesDeep * TileDepth ) + TileDepth;
            break;
    }

    // propagate that point to the other planes
    VectorSubtract( (*TargetPoint), Position, Direction );
    VectorNormalise( &Direction );
    VectorScale( Direction, AEROPLANESPACING, Direction );

    Weapon = GetShotWeapon( Projectile->Data );

    for( i = 0; i < Weapon->AeroplaneCount; i++ )
    {
        SetObjectPosition( _AeroplaneObjIDs[ i ], 0, &Position );
        VectorSubtract( Position, Direction, Position );
    }

    // copy the target point across
    VectorCopy( (*TargetPoint), _Waypoints[ 0 ] );

    // choose ending point
    switch( rand() % 4 )
    {
        case 0 : // left
            Position.X = -TileWidth;
            Position.Z = (float)( (size_t)rand() % (size_t)( (size_t)TilesDeep * (size_t)TileDepth ) );
            break;
        case 1 : // right
            Position.X = ( TilesWide * TileWidth ) + TileWidth;
            Position.Z = (float)( (size_t)rand() % (size_t)( (size_t)TilesDeep * (size_t)TileDepth ) );
            break;
        case 2 : // back
            Position.X = (float)( (size_t)rand() % (size_t)( (size_t)TilesWide * (size_t)TileWidth ) );
            Position.Z = -TileDepth;
            break;
        case 3 : // front
            Position.X = (float)( (size_t)rand() % (size_t)( (size_t)TilesWide * (size_t)TileWidth ) );
            Position.Z = ( TilesDeep * TileDepth ) + TileDepth;
            break;
    }

    VectorCopy( Position, _Waypoints[ 1 ] );

    // aim planes at target point
    VectorNormalise( &Direction );
    VectorGetAnglesXY( &Direction, &Rotation.X, &Rotation.Y );
    Rotation.Z = 0.0f;

    while( Rotation.Y < 0.0f ) Rotation.Y += 360.0f;
    while( Rotation.Y >= 360.0f ) Rotation.Y -= 360.0f;

    Rotation.Y = -Rotation.Y;

    for( i = 0; i < Weapon->AeroplaneCount; i++ )
        SetObjectRotation( _AeroplaneObjIDs[ i ], 0, &Rotation );
}


//----------------------------------------------------------------------------------------
// Name: SetupAeroplanes()
// Desc: 
//----------------------------------------------------------------------------------------

static void SetupAeroplanes( Object_t *Projectile )
{
    size_t i;
    Weapon_t *Weapon;

    Weapon = GetShotWeapon( Projectile->Data );

    for( i = 0; i < Weapon->AeroplaneCount; i++ )
    {
        _AeroplaneObjIDs[ i ] = GetFreeObject();
        ResetObject( _AeroplaneObjIDs[ i ] );

        SetObjectModels( _AeroplaneObjIDs[ i ], g_ModelAirStrikePlaneLOD0, g_ModelAirStrikePlaneLOD1 );
        SetObjectDrawFunction( _AeroplaneObjIDs[ i ], DrawShadedModel );

        SetObjectOnThinkFunction( _AeroplaneObjIDs[ i ], AeroplaneThink );
        SetObjectRotateType( _AeroplaneObjIDs[ i ], MODELROTATE_YXZ );

        SetShieldParachutePhysics( _AeroplaneObjIDs[ i ] );

        ActivateObject( _AeroplaneObjIDs[ i ] );
        ShowObject( _AeroplaneObjIDs[ i ] );
    }

    _PlaneFinishedCount = 0;
}


//----------------------------------------------------------------------------------------
// Name: FreeAeroplanes()
// Desc: 
//----------------------------------------------------------------------------------------

static void FreeAeroplanes( Object_t *Projectile )
{
    size_t i;
    Weapon_t *Weapon;

    Weapon = GetShotWeapon( Projectile->Data );

    for( i = 0; i < Weapon->AeroplaneCount; i++ )
        ReturnObject( _AeroplaneObjIDs[ i ] );
}



//----------------------------------------------------------------------------------------
// Name: AirStrikeThink()
// Desc: 
//----------------------------------------------------------------------------------------

void AirStrikeThink( Object_t *Projectile, size_t IsHiRes )
{
    Weapon_t *Weapon;
    Vector_t Position;
    Vector_t Normal;
    size_t SoundID;

    switch( Projectile->ThinkCounter1 )
    {
        case 0 :            // intial projectile active
            UpdateShotShading( Projectile, IsHiRes );

            Projectile->Rotations[ 0 ].Y += 2.0f;
            Projectile->Rotations[ 0 ].Z += 1.0f;

            if( fabsf( VectorMagnitude( &Projectile->Physics.Move ) ) <= EPSILON )
            {
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

                SetSoundPosition( SoundAirRaid0, Position.X, Position.Y, Position.Z );
                PlaySound3D( SoundAirRaid0, 0 );

                GetMeshNormal( g_LandscapeMesh, Position.X, Position.Z, &Normal );
                StartDebrisGround( &Position, &Normal );

                HideObject( Projectile->ID );
                DeactivateObject( Projectile->ID );

                _ParticleBeacon = StartAirStrikeBeacon( &Position );

                Projectile->ThinkCounter1 = 1;

                GetCameraShakeLookAtPosition(Position);
                StopCameraTrackingObject(&Projectile->Positions[0], Position);
            }
            break;

        case 1 :        // setup air planes
            SetupAeroplanes( Projectile );
            SetupWayPoints( Projectile, &Projectile->Positions[ 0 ] );

            GetObjectPosition( _AeroplaneObjIDs[ 0 ], 0, &Position );
            SetSoundPosition( SoundAeroplaneEngine0, Position.X, Position.Y, Position.Z );
            PlaySound3D( SoundAeroplaneEngine0, 1 );

            // point the camera at the first plane
            TrackAeroplane( _AeroplaneObjIDs[ 0 ] );

            Projectile->ThinkCounter1 = 2;
            break;

        case 2 :        // air planes are active
            GetObjectPosition( _AeroplaneObjIDs[ 0 ], 0, &Position );
            SetSoundPosition( SoundAeroplaneEngine0, Position.X, Position.Y, Position.Z );

            Weapon = GetShotWeapon( Projectile->Data );
            if( _PlaneFinishedCount >= Weapon->AeroplaneCount )
                Projectile->ThinkCounter1 = 3;

            break;

        case 3 :        // all done
            StopSound( SoundAeroplaneEngine0 );

            FreeAeroplanes( Projectile );

            ReturnShot( Projectile->Data );
            KillObject( Projectile->ID );
            break;
    }

    SaveShotPosition(Projectile->Data, Projectile->Positions[0]);
}
