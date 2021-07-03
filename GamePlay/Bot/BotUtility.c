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

#include "PlayerManager.h"
#include "MeshManager.h"

#include "GameTypes.h"
#include "PlayerHistory.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//-----------------------------------------------------------------------------
// Name: ChooseRandomPlayer()
// Desc: 
//-----------------------------------------------------------------------------

size_t ChooseRandomPlayer( size_t ExceptFromThisTeam )
{
    size_t i, j;
    size_t Players[ MAXPLAYERS ];

    j = 0;
    for( i = 0; i < GetPlayerCount(); i++ )
    {
        if( !IsPlayerDead( i ) && GetPlayerTeam( i ) != ExceptFromThisTeam )
            Players[ j++ ] = i;
    }

    if( j <= 0 )
        return -1;

    return Players[ rand() % j ];
}


//-----------------------------------------------------------------------------
// Name: SumPlayersPreviousDamage()
// Desc: 
//-----------------------------------------------------------------------------

float SumPlayersPreviousDamage( size_t SrcPlayer, size_t DstPlayer, size_t TurnCount )
{
    float Sum = 0.0f;
    PlayerTurn_t *PlayerTurn;
    size_t i;

    for( i = 0; i < TurnCount; i++ )
    {
        PlayerTurn = GetPreviousTurn( GetPlayerHistory( SrcPlayer ), i );
        Sum += PlayerTurn->DamageTaken[ DstPlayer ];
    }

    return Sum;
}


//-----------------------------------------------------------------------------
// Name: CanPlayerSeePlayer()
// Desc: 
//-----------------------------------------------------------------------------

size_t CanPlayerSeePlayer(size_t SrcPlayer, size_t DstPlayer)
{
    Vector_t LineStart, LineFinish, Position;
    Vector_t Normal;
    Polygon_t *Polygon;
    float Smallest;

    GetPlayerGroundNormal(SrcPlayer, &Normal);
    GetPlayerPosition(SrcPlayer, &Position);
    LineStart.X = Position.X + (Normal.X * TANK_BARRELLHEIGHT);
    LineStart.Y = Position.Y + (Normal.Y * TANK_BARRELLHEIGHT);
    LineStart.Z = Position.Z + (Normal.Z * TANK_BARRELLHEIGHT);

    GetPlayerGroundNormal(DstPlayer, &Normal);
    GetPlayerPosition(DstPlayer, &Position);
    LineFinish.X = Position.X + (Normal.X * TANK_BARRELLHEIGHT);
    LineFinish.Y = Position.Y + (Normal.Y * TANK_BARRELLHEIGHT);
    LineFinish.Z = Position.Z + (Normal.Z * TANK_BARRELLHEIGHT);

    return LineMeshIntersect(g_LandscapeMesh, &LineStart, &LineFinish, &Polygon, &Position, &Smallest) == IntersectionResult::ClearLineOfSight;
}
