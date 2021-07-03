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
#include "StateEndTurn.h"
#include "StateEndLevel.h"
#include "StateFlyBy.h"

#include "Game.h"
#include "Camera.h"
#include "Input.h"

#include "KeyList.h"
#include "DialogKeys.h"
#include "DialogBotToolbar.h"

#include "PlayerManager.h"
#include "LevelManager.h"


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static void CallbackEndGame( size_t RetValue );


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    DialogHideAll( 0 );

    DialogShow( DialogKeys, &g_WaitForShotKeys );
    DialogShow( DialogBotToolbar, 0 );

    InitaliseBotTurn( GetCurrentPlayer() );

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
    AllowBotToThink( GetCurrentPlayer() );

    // temp
    if( IsKeyPressed( DIK_ESCAPE ) )
    {
        SetGameState( &g_StateEndTurn );
        return;
    }

    if( IsEndOfLevel() )
    {
        SetGameState( &g_StateEndLevel );
        return;
    }

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();

    if( IsKeyPressed( DIK_F10 ) )
    {
        DialogShowCursor( 1 );
        DialogMessageBox( "Heavy Artillery", "Are you sure you want to end the game?", MB_YESNO, CallbackEndGame );
    }
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateBotThink =
{
    Constructor, Destructor, Poll
};


//-----------------------------------------------------------------------------
// Name: CallbackEndGame()
// Desc: 
//-----------------------------------------------------------------------------

static void CallbackEndGame( size_t RetValue )
{
    if( IDYES == RetValue )
        SetGameState( &g_StateFlyBy );

    DialogShowCursor( 0 );
    //GamePopDialogState();
}
