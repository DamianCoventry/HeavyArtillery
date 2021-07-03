//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 2 July 2002
//
//========================================================================================


#include "stdafx.h"
#include "StateTypes.h"
#include "State.h"
#include "StateBotThink.h"
#include "StateFindTank.h"

#include "Input.h"
#include "Camera.h"

#include "PlayerManager.h"
#include "LevelManager.h"

#include "DialogGameToolbar.h"
#include "DialogFindTank.h"
#include "Game.h"


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    if( !IsDialogActive( DialogGameToolbar ) )
        DialogShow( DialogGameToolbar, 0 );
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
    if( IsKeyPressed( DIK_F3 ) )
        DialogShow( DialogFindTank, 0 );

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateRunTurn =
{
    Constructor, Destructor, Poll
};
