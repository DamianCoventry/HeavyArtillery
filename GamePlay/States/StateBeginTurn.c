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
#include "StateBotThink.h"
#include "StateFindTank.h"
#include "StateRunTurn.h"

#include "Input.h"
#include "Camera.h"
#include "Game.h"

#include "PlayerManager.h"
#include "LevelManager.h"

#include "DialogGameToolbar.h"
#include "DialogFindTank.h"
#include "DialogBeginTurn.h"

#include "PlayerHistory.h"


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    PlayerHistory_t *History;

    FindPlayer( GetCurrentPlayer() );
    DialogShow( DialogBeginTurn, 0 );

    History = GetPlayerHistory( GetCurrentPlayer() );
    History->HasHitThisTurn = 0;
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
    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateBeginTurn =
{
    Constructor, Destructor, Poll
};
