//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "ObjectTypes.h"
#include "LightSource.h"

#include "ModelManager.h"
#include "LevelManager.h"
#include "PlayerManager.h"
#include "MeshManager.h"
#include "ObjectManager.h"
#include "ParticleManager.h"
#include "SoundManager.h"

#include "GameItems.h"
#include "GameModels.h"
#include "GameSounds.h"

#include "Object.h"
#include "ParticleDamageSmoke.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void EmitDamageSmoke( Object_t *Object );


//-----------------------------------------------------------------------------
// Name: PlayerTankThink()
// Desc: 
//-----------------------------------------------------------------------------

void PlayerTankThink( Object_t *Object, size_t IsHiResModel )
{
    Vector_t LightDirection;
    float Ambient;
    size_t PlayerID, ShieldID, ParachuteID;
    size_t Index;
    Vector_t Rotation;
    Backpack_t *Backpack;

    // the tank's think data is the index of the player that controls this tank
    PlayerID    = Object->Data;
    ParachuteID = GetPlayerParachute( PlayerID );
    ShieldID    = GetPlayerShield( PlayerID );

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

            if( IsObjectOnGround( Object ) )
            {
                GetPolygonNormal( g_LandscapeMesh, Object->Physics.PolyNormal, &Rotation );
                Object->Rotations[ 0 ].X = -Rotation.Z * RTOD;
                Object->Rotations[ 0 ].Z =  Rotation.X * RTOD;

                // disable current parachute
                DeactivateObject( ParachuteID );
                HideObject( ParachuteID );
            }
            else
            {
                // activate a parachute if the player has one in stock and there's
                // not already one active.  also there's no need to open a parachute if
                // falling damage is off.
                if( !IsObjectActivated( ParachuteID ) && IsPlayerReceivingFallingDamage( PlayerID ) )
                {
                    Backpack = GetPlayerBackpack( PlayerID );

                    if( Backpack )
                    {
                        Index = BackpackGetAbsoluteIndex( Backpack, BPITEM_TYPEITEM, (void *)&ItemParachutes );
                        if( -1 != Index )
                        {
                            SetSoundPosition( SoundParachute0, Object->Positions[ 0 ].X, Object->Positions[ 0 ].Y, Object->Positions[ 0 ].Z );
                            PlaySound3D( SoundParachute0, 0 );

                            BackpackUse( Backpack, BPITEM_TYPEITEM, (void *)&ItemParachutes );

                            ActivateObject( ParachuteID );
                            ShowObject( ParachuteID );
                        }
                    }
                }
            }

            Object->Physics.Gravity = ( IsObjectActivated( ParachuteID ) ? DEFAULT_GRAVITY / 4.0f : DEFAULT_GRAVITY );

            EmitDamageSmoke( Object );

            break;
    }
}


//-----------------------------------------------------------------------------
// Name: OnPlayerMove()
// Desc: 
//-----------------------------------------------------------------------------

void OnPlayerMove( Object_t *Object )
{
    size_t PlayerID, ShieldID, ParachuteID;
    Vector_t Position;
    Vector_t Rotation;

    // the tank's think data is the index of the player that controls this tank
    PlayerID    = Object->Data;
    ParachuteID = GetPlayerParachute( PlayerID );
    ShieldID    = GetPlayerShield( PlayerID );

    if( IsObjectActivated( ParachuteID ) )
    {
        GetPlayerPosition( PlayerID, &Position );
        Position.Y += GetModelBSphereRadius( g_ModelTank0LOD0 );
        SetObjectPosition( ParachuteID, 0, &Position );

        VectorSet( Rotation, 0.0f );
        SetObjectRotation( GetPlayerTank( PlayerID ), 0, &Rotation );
    }

    if( IsObjectActivated( ShieldID ) )
    {
        GetPlayerPosition( PlayerID, &Position );
        SetObjectPosition( ShieldID, 0, &Position );
    }
}


//-----------------------------------------------------------------------------
// Name: OnPlayerCollideMesh()
// Desc: 
//-----------------------------------------------------------------------------

void OnPlayerCollideMesh( Object_t *Object )
{
}


//-----------------------------------------------------------------------------
// Name: OnPlayerCollideObject()
// Desc: 
//-----------------------------------------------------------------------------

void OnPlayerCollideObject( Object_t *PlayersTank, Object_t *OtherTank )
{
    Vector_t V;
    Vector_t Zero;
    float Mag, Damage;

    VectorSet( Zero, 0.0f );

    if( VectorFuzzyCompare( PlayersTank->Physics.Move, Zero, EPSILON ) )
    {
        if( VectorFuzzyCompare( OtherTank->Physics.Move, Zero, EPSILON ) )
        {
            // then both objects are stationary - do nothing
        }
        else
        {
            // playerstank is stationary but othertank is moving

            // create a vector from the other tank to the players tank
            VectorSubtract( OtherTank->Positions[ 0 ], PlayersTank->Positions[ 0 ], V );
            VectorNormalise( &V );
            Mag = fabsf( VectorMagnitude( &OtherTank->Physics.Move ) );

            if( !( OtherTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
            {
                // bounce othertank off playerstank
                VectorReflect( &OtherTank->Physics.Move, &V, &OtherTank->Physics.Move );
            }

            if( !( PlayersTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
            {
                // push playerstank away
                VectorInverse( V );
                VectorScale( V, Mag, V );
                VectorAdd( PlayersTank->Physics.Move, V, PlayersTank->Physics.Move );
            }

            Damage = Mag + fabsf( VectorMagnitude( &PlayersTank->Physics.Move ) );
        }
    }
    else if( VectorFuzzyCompare( OtherTank->Physics.Move, Zero, EPSILON ) )
    {
        // othertank is stationary but playerstank is moving

        // create a vector from the other tank to the players tank
        VectorSubtract( PlayersTank->Positions[ 0 ], OtherTank->Positions[ 0 ], V );
        VectorNormalise( &V );
        Mag = fabsf( VectorMagnitude( &PlayersTank->Physics.Move ) );

        if( !( PlayersTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // bounce playerstank off playerstank
            VectorReflect( &PlayersTank->Physics.Move, &V, &PlayersTank->Physics.Move );
        }

        if( !( OtherTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // push othertank away
            VectorInverse( V );
            VectorScale( V, Mag, V );
            VectorAdd( OtherTank->Physics.Move, V, OtherTank->Physics.Move );
        }

        Damage = Mag + fabsf( VectorMagnitude( &OtherTank->Physics.Move ) );
    }
    else
    {
        // both objects are moving

        // create a vector from the other tank to the players tank
        VectorSubtract( PlayersTank->Positions[ 0 ], OtherTank->Positions[ 0 ], V );
        VectorNormalise( &V );

        if( !( PlayersTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // bounce playerstank off playerstank
            VectorReflect( &PlayersTank->Physics.Move, &V, &PlayersTank->Physics.Move );
        }

        if( !( OtherTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
        {
            // bounce othertank off playerstank
            VectorInverse( V );
            VectorReflect( &OtherTank->Physics.Move, &V, &OtherTank->Physics.Move );
        }

        Damage = fabsf( VectorMagnitude( &PlayersTank->Physics.Move ) ) + fabsf( VectorMagnitude( &OtherTank->Physics.Move ) );
    }

    // take one step so the two objects are no longer colliding
    if( !( PlayersTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
    {
        VectorAdd( PlayersTank->Physics.Move, PlayersTank->Positions[ 0 ], PlayersTank->Positions[ 0 ] );
        PlayerTakeCollisionDamage( PlayersTank->ID, Damage );
    }

    if( !( OtherTank->Physics.Flags & PHYSICS_HASCOLLIDED ) )
    {
        VectorAdd( OtherTank->Physics.Move, OtherTank->Positions[ 0 ], OtherTank->Positions[ 0 ] );
        PlayerTakeCollisionDamage( OtherTank->ID, Damage );
    }

    PlayersTank->Physics.Flags |= PHYSICS_HASCOLLIDED;
    OtherTank->Physics.Flags |= PHYSICS_HASCOLLIDED;
}


//-----------------------------------------------------------------------------
// Name: OnPlayerFallingDamage()
// Desc: 
//-----------------------------------------------------------------------------

void OnPlayerFallingDamage( Object_t *Object )
{
    size_t i, SoundID;
    float DamageTaken;
    PlayerHistory_t *History;
    Vector_t Position;

    // find out which player this is

    for( i = 0; i < GetPlayerCount(); i++ )
    {
        if( Object->ID == GetPlayerTank( i ) )
        {
            DamageTaken = PlayerTakeDamage( i, fabsf( VectorMagnitude( &Object->Physics.Move ) ) );

            History = GetPlayerHistory( i );
            History->FallingDamage += DamageTaken;

            GetPlayerPosition( i, &Position );
            SoundID = 0 == rand() % 2 ? SoundTankImpact0 : SoundTankImpact1;
            SetSoundPosition( SoundID, Position.X, Position.Y, Position.Z );
            PlaySound3D( SoundID, 0 );
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: EmitDamageSmoke()
// Desc: 
//-----------------------------------------------------------------------------

static void EmitDamageSmoke( Object_t *Object )
{
    float TankPower;
    size_t ParticleID;

    ParticleID = GetPlayerDamageSmokeID( Object->Data );

    TankPower = GetPlayerTankPower( Object->Data );
    if( TankPower > 0.0f && TankPower < 30.0f )
    {
        if( -1 == ParticleID )
        {
            ParticleID = StartDamageSmoke( Object->ID );
            SetPlayerDamageSmokeID( Object->Data, ParticleID );
        }
    }
    else if( -1 != ParticleID )
    {
        StopEmittingParticles( ParticleID );
        SetPlayerDamageSmokeID( Object->Data, -1 );
    }
}
