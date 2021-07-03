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

#include "State.h"
#include "StateWaitForShot.h"
#include "StateEndTurn.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"

#include "Dialog.h"
#include "Camera.h"

#include "BotMove.h"
#include "BotShoot.h"
#include "BotUtility.h"

#include "BotInept.h"
#include "BotVindictive.h"
#include "BotGreedy.h"
#include "BotCompetetive.h"
#include "BotCruel.h"

#include "StateGetShotInfo.h"


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _EndTurn = 0;


//----------------------------------------------------------------------------------------
// Name: ChooseTurn()
// Desc: 
//----------------------------------------------------------------------------------------

static void ChooseTurn( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    PlayerTurn_t *HistoryTurn;
    float Damage;
    size_t i;

    HistoryTurn = GetPreviousTurn( GetPlayerHistory( PlyrIndex ), 1 );

    // if the player is quite damaged and took damage on the previous turn then consider moving
    // instead of shooting for this turn.

    Damage = 0.0f;
    for( i = 0; i < GetPlayerCount(); i++ )
        Damage += SumPlayersPreviousDamage( PlyrIndex, i, 1 );

    if( GetPlayerTankPower( PlyrIndex ) < 50.0f && ( 0 == rand() % 2 ) && Damage > 0.0f )
    {
        BotInfo->State = BOTSTATE_POLLMOVE;
        _EndTurn = 0;

        StopPlayerReceivingFallingDamage( PlyrIndex );
        ChooseMoveDestination( BotInfo );

        SetPointToFollow( GetObjectPositionPtr( GetPlayerTank( PlyrIndex ), 0 ) );
        SetCameraState( CAMERASTATE_DISTANCEFOLLOW, CAMERATYPE_MOUSELOOK );
    }
    else
    {
        BotInfo->State      = BOTSTATE_POLLSHOOT;
        BotInfo->ShotState  = BOTSHOOTSTATE_CHOOSINGTARGET;
    }
}


//----------------------------------------------------------------------------------------
// Name: PollMove()
// Desc: 
//----------------------------------------------------------------------------------------

static void PollMove( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    if( !_EndTurn )
    {
        if( ReachedDestination( PlyrIndex, BotInfo ) )
        {
            _EndTurn = 1;
        }
        else
        {
            TurnTowardsDestination( PlyrIndex, BotInfo );

            if( IsPlayerOnGround( PlyrIndex ) )
                MovePlayerForward( PlyrIndex, 1.0f );

            if( !UpdateBotFuel( PlyrIndex ) )
                _EndTurn = 1;
        }
    }
    else
    {
        if( !IsPlayerMoving( PlyrIndex ) )
        {
            SetGameState( &g_StateEndTurn );
            AllowPlayerToReceiveFallingDamage( PlyrIndex );
        }
        else
            CameraLookAt( GetCameraPosition(), GetObjectPositionPtr( GetPlayerTank( PlyrIndex ), 0 ) );
    }
}


//----------------------------------------------------------------------------------------
// Name: PollShoot()
// Desc: 
//----------------------------------------------------------------------------------------

static void PollShoot( size_t PlyrIndex, BotInfo_t *BotInfo )
{
    switch( BotInfo->ShotState )
    {
        case BOTSHOOTSTATE_CHOOSINGTARGET :
            ChooseTarget( PlyrIndex, BotInfo );
            break;

        case BOTSHOOTSTATE_CALCULATINGSHOT :
            CalculateHeadingPitchPower( PlyrIndex, BotInfo );
            break;

        case BOTSHOOTSTATE_ROTATINGBARRELL :
            if( RotateTowardsTarget( PlyrIndex, BotInfo ) )
            {
                FireWeapon();
                SetGameState( &g_StateWaitForShot );
            }
            break;
    }
}


//----------------------------------------------------------------------------------------
// Name: AllowBotToThink()
// Desc: 
//----------------------------------------------------------------------------------------

void AllowBotToThink( size_t PlyrIndex )
{
    BotInfo_t *BotInfo;

    BotInfo = GetBotInfo( PlyrIndex );
    if( nullptr == BotInfo )
        return;

    switch( BotInfo->State )
    {
        case BOTSTATE_CHOOSINGTURN :
            ChooseTurn( PlyrIndex, BotInfo );
            break;

        case BOTSTATE_POLLMOVE :
            PollMove( PlyrIndex, BotInfo );
            break;

        case BOTSTATE_POLLSHOOT :
            PollShoot( PlyrIndex, BotInfo );
            break;
    }
}


//----------------------------------------------------------------------------------------
// Name: SetupBotCallbacks()
// Desc: 
//----------------------------------------------------------------------------------------

void SetupBotCallbacks( BotType_t Type, BotInfo_t *BotInfo )
{
    if( BOTTYPE_RANDOM == Type )
        Type = (BotType_t)( (size_t)rand() % 5 + 1 );

    switch( Type )
    {
        case BOTTYPE_INEPT :
            BotInfo->ChooseNewTarget    = BotIneptChooseNewTarget;
            BotInfo->ReconsiderTarget   = BotIneptReconsiderTarget;
            break;

        case BOTTYPE_VINDICTIVE :
            BotInfo->ChooseNewTarget    = BotVindictiveChooseNewTarget;
            BotInfo->ReconsiderTarget   = BotVindictiveReconsiderTarget;
            break;

        case BOTTYPE_COMPETETIVE :
            BotInfo->ChooseNewTarget    = BotCompetetiveChooseNewTarget;
            BotInfo->ReconsiderTarget   = BotCompetetiveReconsiderTarget;
            break;

        case BOTTYPE_GREEDY :
            BotInfo->ChooseNewTarget    = BotGreedyChooseNewTarget;
            BotInfo->ReconsiderTarget   = BotGreedyReconsiderTarget;
            break;

        case BOTTYPE_CRUEL :
            BotInfo->ChooseNewTarget    = BotCruelChooseNewTarget;
            BotInfo->ReconsiderTarget   = BotCruelReconsiderTarget;
            break;
    }
}
