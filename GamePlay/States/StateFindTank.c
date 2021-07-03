//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "StateTypes.h"
#include "State.h"
#include "StateBeginTurn.h"
#include "StateRunTurn.h"
#include "StateBotThink.h"

#include "Camera.h"
#include "Game.h"
#include "Input.h"

#include "PlayerManager.h"
#include "LevelManager.h"


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
}


//----------------------------------------------------------------------------------------
// Name: Destructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Destructor( void )
{
}


//----------------------------------------------------------------------------------------
// Name: Poll()
// Desc: 
//----------------------------------------------------------------------------------------

static void Poll( void )
{
    size_t LookFrom, LookAt;

    GetCameraDoneFlags( &LookFrom, &LookAt );

    if( LookFrom && LookAt )
    {
        if( IsPlayerABot( GetCurrentPlayer() ) )
            SetGameState( &g_StateBotThink );
        else
            SetGameState( &g_StateRunTurn );
    }

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateFindTank =
{
    Constructor, Destructor, Poll
};


//----------------------------------------------------------------------------------------
// Name: FindPlayer()
// Desc: 
//----------------------------------------------------------------------------------------

void FindPlayer( size_t PlayerID )
{
    Vector_t Position;
    Vector_t Normal;

    GetPlayerPosition( PlayerID, &Position );
    GetPlayerGroundNormal( PlayerID, &Normal );

    InitialiseFindTankPoints( &Position, &Normal );
    SetCameraState( CAMERASTATE_FINDTANK, CAMERATYPE_MOUSELOOK );

    SetGameState( &g_StateFindTank );
}
