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
#include "GameConfig.h"
#include "Camera.h"
#include "Input.h"

#include "Dialog.h"
#include "DialogLevelStart.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "MeshManager.h"
#include "ParticleManager.h"

#include "ParticleSnowflake.h"
#include "Game.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;
extern size_t g_SnowstormID;


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    Vector_t Position;

    BeginNewGame();
    SetupPlayersForNewGame();

    ChooseNewTileSet( g_LandscapeMesh, (TileSetType_t)GetTileSetType() );
    GenerateNewMesh( g_LandscapeMesh );
    AssignNewTileSet( g_LandscapeMesh );

    if( TILESET_SNOW == GetMeshTileSetType( g_LandscapeMesh ) )
    {
        g_SnowstormID = StartSnowflake();
    }
    else
    {
        ReturnParticleSystem( g_SnowstormID );
        g_SnowstormID = -1;
    }

    GetMeshCenter( g_LandscapeMesh, &Position );
    SetCameraPosition( &Position );

    DialogHideAll( 0 );
    DialogShow( DialogLevelStart, 0 );

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
    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateBeginGame =
{
    Constructor, Destructor, Poll
};
