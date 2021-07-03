//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 15 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "GameTypes.h"
#include "GameItems.h"

#include "Vector.h"
#include "Utility.h"

#include "MeshManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"

#define TANK_TURNSPEED      2.0f


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _FuelCounter;


//----------------------------------------------------------------------------------------
// Name: ReachedDestination()
// Desc: 
//----------------------------------------------------------------------------------------

size_t ReachedDestination( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    Vector_t Position;
    size_t TilesWide, TilesDeep;
    size_t TileX, TileZ;

    GetPlayerPosition( PlyrIndex, &Position );
    GetMeshSize( g_LandscapeMesh, &TilesWide, &TilesDeep );

    TileX = (size_t)Position.X / TilesWide;
    TileZ = (size_t)Position.Z / TilesDeep;

    if( TileX >= BotInfo->DestTileX - 2 && TileX <= BotInfo->DestTileX + 2 &&
        TileZ >= BotInfo->DestTileZ - 2 && TileZ <= BotInfo->DestTileZ + 2 )
        return 1;

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: TurnTowardsDestination()
// Desc: 
//----------------------------------------------------------------------------------------

void TurnTowardsDestination( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    Vector_t SrcPoint, DstPoint;
    Vector_t Rotation, Direction;
    float TileWidth, TileDepth;
    float Heading, AngleDelta;

    GetMeshTileSize( g_LandscapeMesh, &TileWidth, &TileDepth );
    DstPoint.X = ( BotInfo->DestTileX * TileWidth ) + ( TileWidth / 2.0f );
    DstPoint.Z = ( BotInfo->DestTileZ * TileDepth ) + ( TileDepth / 2.0f );
    DstPoint.Y = 0.0f;

    GetPlayerPosition( PlyrIndex, &SrcPoint );
    SrcPoint.Y = 0.0f;

    VectorSubtract( DstPoint, SrcPoint, Direction );
    VectorNormalise( &Direction );
    VectorGetAnglesXY( &Direction, nullptr, &Heading );

    while( Heading < 0.0f ) Heading += 360.0f;
    while( Heading >= 360.0f ) Heading -= 360.0f;

    GetObjectRotation( GetPlayerTank( PlyrIndex ), 0, &Rotation );
    Rotation.Y = -Rotation.Y;

    AngleDelta = AngleDiff( Heading, Rotation.Y );
    if( fabsf( AngleDelta ) <= TANK_TURNSPEED )
    {
        Rotation.Y = Heading;
    }
    else if( AngleDelta < 0.0f )
    {
        Rotation.Y += TANK_TURNSPEED;
        if( Rotation.Y >= 360.0f )
            Rotation.Y -= 360.0f;
    }
    else // it must be > 0.0f
    {
        Rotation.Y -= TANK_TURNSPEED;
        if( Rotation.Y < 0.0f )
            Rotation.Y += 360.0f;
    }

    Rotation.Y = -Rotation.Y;
    SetObjectRotation( GetPlayerTank( PlyrIndex ), 0, &Rotation );
}


//----------------------------------------------------------------------------------------
// Name: ChooseMoveDestination()
// Desc: 
//----------------------------------------------------------------------------------------

void ChooseMoveDestination( BotInfo_t *BotInfo )
{
    size_t TilesWide, TilesDeep;

    GetMeshSize( g_LandscapeMesh, &TilesWide, &TilesDeep );

    BotInfo->DestTileX = ( rand() % ( TilesWide / 2 ) ) + ( TilesWide / 4 );
    BotInfo->DestTileZ = ( rand() % ( TilesDeep / 2 ) ) + ( TilesDeep / 4 );

    _FuelCounter = 0;
}


//----------------------------------------------------------------------------------------
// Name: UpdateBotFuel()
// Desc: 
//----------------------------------------------------------------------------------------

size_t UpdateBotFuel( size_t PlyrIndex )
{
    size_t FuelLeft;

    FuelLeft = BackpackGetAmountLeft( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, (void *)&ItemFuel );

    if( IsPlayerMoving( PlyrIndex ) )
        _FuelCounter++;

    if( ++_FuelCounter > NUMFRAMESPERLITRE )
    {
        _FuelCounter = 0;

        if( BACKPACK_NOMORELEFT == BackpackUse( GetPlayerBackpack( PlyrIndex ), BPITEM_TYPEITEM, (void *)&ItemFuel ) )
            return 0;
    }

    return 1;
}
