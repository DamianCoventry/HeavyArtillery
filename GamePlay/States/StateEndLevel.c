//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "State.h"
#include "StateTypes.h"
#include "StateFlyBy.h"
#include "Input.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "MeshManager.h"
#include "ParticleManager.h"

#include "ParticleSnowflake.h"

#include "Dialog.h"
#include "DialogLevelStart.h"
#include "DialogStats.h"

#include "Camera.h"
#include "Game.h"
#include "GameConfig.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;
extern size_t g_SnowstormID;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _EndLevelDone;


//----------------------------------------------------------------------------------------
// Name: CallbackGameOver()
// Desc: 
//----------------------------------------------------------------------------------------

void CallbackGameOver( size_t RetValue )
{
    SetGameState( &g_StateFlyBy );
}


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    AwardEndOfLevelCash();

    DialogHideAll( nullptr );
    DialogShow( DialogStats, 0 );

    SetCameraState( CAMERASTATE_IDLE, CAMERATYPE_MOUSELOOK );

    _EndLevelDone = 0;
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
    Vector_t Position;

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();

    if( !_EndLevelDone && !IsDialogActive( DialogStats ) )
    {
        _EndLevelDone = 1;

        if( !EndLevel() )
        {
            DialogMessageBox( "Heavy Artillery", "Game Over", MB_OK, CallbackGameOver );
        }
        else
        {
            ReturnAllParticleSystems();

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

            DialogHideAll( nullptr );
            DialogShow( DialogLevelStart, 0 );
        }
    }
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateEndLevel =
{
    Constructor, Destructor, Poll
};
