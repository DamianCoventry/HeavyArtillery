//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 8 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <math.h>
#include "Backpack.h"
#include "ItemTypes.h"
#include "Vector.h"
#include "Camera.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "SoundManager.h"

#include "GameItems.h"
#include "GameSounds.h"


#define WEAKGUIDANCE_SCANRADIUS         1024.0f
#define WEAKGUIDANCE_STEPSIZE           32.0f
#define WEAKGUIDANCE_THRESHOLD          30

#define STRONGGUIDANCE_SCANRADIUS       2048.0f
#define STRONGGUIDANCE_STEPSIZE         64.0f
#define STRONGGUIDANCE_THRESHOLD        15


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _GuidanceCounter;
static size_t _GuidanceThreshold;


//----------------------------------------------------------------------------------------
// Name: ActivateGuidance()
// Desc: 
//----------------------------------------------------------------------------------------

void ActivateGuidance( Item_t *Item, Backpack_t *Backpack, size_t PlayerID )
{
    size_t SoundID;
    Vector_t *Position;
    size_t Index = BackpackGetAbsoluteIndex( Backpack, BPITEM_TYPEITEM, (void *)Item );

    if( BACKPACK_DEACTIVATED == BackpackToggleActivate( Backpack, Index ) )
        SoundID = SoundItemDeactivated0;
    else
        SoundID = SoundItemActivated0;

    Position = GetCameraPosition();
    SetSoundPosition( SoundID, Position->X, Position->Y, Position->Z );
    PlaySound3D( SoundID, 0 );
}


//----------------------------------------------------------------------------------------
// Name: InitialiseGuidance()
// Desc: 
//----------------------------------------------------------------------------------------

void InitialiseGuidance( size_t IsStrong )
{
    _GuidanceCounter = 0;
    _GuidanceThreshold = ( IsStrong ? STRONGGUIDANCE_THRESHOLD : WEAKGUIDANCE_THRESHOLD );
}


//----------------------------------------------------------------------------------------
// Name: FindClosestTank()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t FindClosestTank( size_t ShootersTeam, Vector_t *ShotLocation, Vector_t *ClosestTank, float ScanRadius )
{
    size_t i, Found;
    float Mag, Closest;
    Vector_t PlayerPosition;

    Closest = 1000000.0f;
    Found = 0;

    for( i = 0; i < GetPlayerCount(); i++ )
    {
        if( GetPlayerTeam( i ) != ShootersTeam )
        {
            GetPlayerPosition( i, &PlayerPosition );

            Mag = fabsf( VectorMagnitude2( &PlayerPosition, ShotLocation ) );
            if( Mag <= ScanRadius && Mag < Closest )
            {
                Found = 1;
                Closest = Mag;
                VectorCopy( PlayerPosition, (*ClosestTank) );
            }
        }
    }

    return Found;
}


//----------------------------------------------------------------------------------------
// Name: ModifyShotVector()
// Desc: 
//----------------------------------------------------------------------------------------

static void ModifyShotVector( Vector_t *ShotLocation, Vector_t *ClosestTank, Vector_t *ShotVector, float StepSize )
{
    Vector_t V1, V2;
    float Mag;

    VectorSubtract( (*ClosestTank), (*ShotLocation), V1 );      // create a vector that points at the target from the shot's location
    VectorSubtract( V1, (*ShotVector), V2 );                    // create a vector from the shot vector's tip to the target vector's tip

    VectorNormalise( &V2 );                                     // set it to size of the given step
    VectorScale( V2, StepSize, V2 );

    Mag = fabsf( VectorMagnitude( ShotVector ) );               // save the original shot vector's length (ie. speed)
    VectorAdd( (*ShotVector), V2, (*ShotVector) );              // apply the new vector to the shot vector

    VectorNormalise( ShotVector );                              // set it to saved length (ie. speed )
    VectorScale( (*ShotVector), Mag, (*ShotVector) );
}


//----------------------------------------------------------------------------------------
// Name: WeakGuidance()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t Guidance( size_t ShootersTeam, Vector_t *ShotLocation, Vector_t *ShotVector, float ScanRadius, float StepSize )
{
    Vector_t ClosestTank;

    if( ShotVector->Y <= 0.0f )
    {
        if( ++_GuidanceCounter >= _GuidanceThreshold )
        {
            _GuidanceCounter = 0;

            if( FindClosestTank( ShootersTeam, ShotLocation, &ClosestTank, ScanRadius ) )
            {
                ModifyShotVector( ShotLocation, &ClosestTank, ShotVector, StepSize );

                SetSoundPosition( SoundGuidance0, ShotLocation->X, ShotLocation->Y, ShotLocation->Z );
                PlaySound3D( SoundGuidance0, 0 );
                return 1;
            }
        }
    }

    return 0;
}

//----------------------------------------------------------------------------------------
// Name: WeakGuidance()
// Desc: 
//----------------------------------------------------------------------------------------

size_t WeakGuidance( size_t ShootersTeam, Vector_t *ShotLocation, Vector_t *ShotVector )
{
    _GuidanceThreshold = WEAKGUIDANCE_THRESHOLD;
    if( Guidance( ShootersTeam, ShotLocation, ShotVector, WEAKGUIDANCE_SCANRADIUS, WEAKGUIDANCE_STEPSIZE ) )
    {
        BackpackUse( GetPlayerBackpack( GetCurrentPlayer() ), BPITEM_TYPEITEM, (void *)&ItemHeatSeekWeak );
        return 1;
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: StrongGuidance()
// Desc: 
//----------------------------------------------------------------------------------------

size_t StrongGuidance( size_t ShootersTeam, Vector_t *ShotLocation, Vector_t *ShotVector )
{
    _GuidanceThreshold = STRONGGUIDANCE_THRESHOLD;
    if( Guidance( ShootersTeam, ShotLocation, ShotVector, STRONGGUIDANCE_SCANRADIUS, STRONGGUIDANCE_STEPSIZE ) )
    {
        BackpackUse( GetPlayerBackpack( GetCurrentPlayer() ), BPITEM_TYPEITEM, (void *)&ItemHeatSeekStrong );
        return 1;
    }

    return 0;
}
