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
#include "State.h"
#include "StateTypes.h"
#include "StateBeginTurn.h"
#include "Input.h"

#include "Dialog.h"
#include "DialogShop.h"
#include "DialogProperties.h"

#include "LevelManager.h"
#include "PlayerManager.h"
#include "MusicManager.h"

#include "GameMusic.h"

#include "Camera.h"
#include "Game.h"


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    size_t MusicID;

    BeginNewLevel();

    GenerateNewPlayOrder();
    SetupPlayersForNewLevel();
    PlacePlayersOnLandscape();

    DialogHideAll( nullptr );

    if( SetFirstShopPlayer() )
    {
        DialogShow( DialogShop, 0 );
        SetCameraState( CAMERASTATE_IDLE, CAMERATYPE_MOUSELOOK );
    }
    else
    {
        if( AutoDefenseSetFirstPlayer() )
            DialogShow( DialogProperties, 0 );
        else
            SetGameState( &g_StateBeginTurn );
    }

    StopMusic( -1 );
    MusicID = ChooseRandomMusicTrack();
    if( MusicID != -1 )
    {
        SetMusicVolume( MusicID, -1000 );
        PlayMusic( MusicID );
    }
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

State_t g_StateBeginLevel =
{
    Constructor, Destructor, Poll
};
