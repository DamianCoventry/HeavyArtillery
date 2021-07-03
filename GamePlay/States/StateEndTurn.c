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
#include "StateEndLevel.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "PlayerHistory.h"

#include "Dialog.h"
#include "Camera.h"
#include "Game.h"
#include "Input.h"


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    SetCameraState( CAMERASTATE_IDLE, CAMERATYPE_MOUSELOOK );
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
    IncrementHistory( GetPlayerHistory( GetCurrentPlayer() ) );

    DialogHideAll( 0 );
    SetNextPlayer();

    if( IsEndOfLevel() )
        SetGameState( &g_StateEndLevel );
    else
        SetGameState( &g_StateBeginTurn );

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateEndTurn =
{
    Constructor, Destructor, Poll
};
