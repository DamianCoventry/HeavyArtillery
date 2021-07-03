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
#include "Camera.h"
#include "Input.h"
#include "Game.h"

#include "KeyList.h"
#include "DialogKeys.h"

#include "StateTypes.h"
#include "State.h"
#include "StateRunTurn.h"


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    DialogHideAll( 0 );

    DialogShow( DialogKeys, &g_MouseLookKeys );

    SetCameraState( CAMERASTATE_MOUSELOOK, CAMERATYPE_MOUSELOOK );
}


//----------------------------------------------------------------------------------------
// Name: Destructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Destructor( void )
{
    SetCameraState( CAMERASTATE_IDLE, CAMERATYPE_MOUSELOOK );
}


//----------------------------------------------------------------------------------------
// Name: Poll()
// Desc: 
//----------------------------------------------------------------------------------------

static void Poll( void )
{
    if( IsKeyPressed( DIK_ESCAPE ) || IsKeyPressed( DIK_RETURN ) )
        SetGameState( &g_StateRunTurn );

    if (IsKeyPressed(DIK_F8))
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateMouseLook =
{
    Constructor, Destructor, Poll
};
