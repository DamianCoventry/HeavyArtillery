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
#include "Vector.h"

#include "LevelManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"

#include "BotUtility.h"


//----------------------------------------------------------------------------------------
// Name: BotCompetetiveChooseNewTarget()
// Desc: 
//----------------------------------------------------------------------------------------

void BotCompetetiveChooseNewTarget( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    Vector_t SrcPosition, DstPosition;
    float ClosestMag, Mag;
    size_t i;

    // choose the closest target

    ClosestMag = 1000000.0f;
    BotInfo->TargetPlyrIndex = -1;

    GetPlayerPosition( PlyrIndex, &SrcPosition );

    for( i = 0; i < GetPlayerCount(); i++ )
    {
        if( !IsPlayerDead( i ) && GetPlayerTeam( i ) != GetPlayerTeam( PlyrIndex ) )
        {
            GetPlayerPosition( i, &DstPosition );

            Mag = fabsf( VectorMagnitude2( &SrcPosition, &DstPosition ) );

            if( Mag < ClosestMag )
            {
                ClosestMag = Mag;
                BotInfo->TargetPlyrIndex = i;
            }
        }
    }

    // did we find a suitable target?
    if( -1 == BotInfo->TargetPlyrIndex )
    {
        // no, then choose a random target
        BotInfo->TargetPlyrIndex = ChooseRandomPlayer( GetPlayerTeam( PlyrIndex ) );
    }
}


//----------------------------------------------------------------------------------------
// Name: BotCompetetiveReconsiderTarget()
// Desc: 
//----------------------------------------------------------------------------------------

void BotCompetetiveReconsiderTarget( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    size_t i, Count;
    size_t Players[ MAXPLAYERS ];
    PlayerTurn_t *PlayerTurn;

    // is our target still alive?
    if( IsPlayerDead( BotInfo->TargetPlyrIndex ) )
    {
        BotInfo->ChooseNewTarget( PlyrIndex, BotInfo );
        return;
    }

    // if a different tank than the one we're shooting at has caused us damage in the last 3 turns
    // then shoot at that tank instead.

    Count = 0;
    for( i = 0; i < GetPlayerCount(); i++ )
    {
        if( !IsPlayerDead( i ) && GetPlayerTeam( i ) != GetPlayerTeam( PlyrIndex ) && i != BotInfo->TargetPlyrIndex )
        {
            PlayerTurn = GetPreviousTurn( GetPlayerHistory( PlyrIndex ), 1 );
            if( PlayerTurn->DamageTaken[ i ] > 0.0f )
            {
                PlayerTurn = GetPreviousTurn( GetPlayerHistory( PlyrIndex ), 2 );
                if( PlayerTurn->DamageTaken[ i ] > 0.0f )
                {
                    PlayerTurn = GetPreviousTurn( GetPlayerHistory( PlyrIndex ), 3 );
                    if( PlayerTurn->DamageTaken[ i ] > 0.0f )
                        Players[ Count++ ] = i;
                }
            }
        }
    }

    // if there was at least one player then randomly choose of them
    if( Count > 0 )
        BotInfo->TargetPlyrIndex = Players[ rand() % Count ];
}
