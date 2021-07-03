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
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/glut.h>

#include "StateTypes.h"
#include "TileSet.h"
#include "MeshManager.h"
#include "Hardware.h"

#include "Input.h"
#include "Camera.h"
#include "Dialog.h"
#include "Game.h"

#include "DialogFlyBy.h"
#include "DialogMain.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;
extern HardwareConfig_t g_HWConfig;


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    Vector_t Position;
    Vector_t Rotation;

    DialogHideAll( nullptr );

    GetMeshCenter( g_LandscapeMesh, &Position );
    SetCameraPosition( &Position );

    SetCameraState( CAMERASTATE_FLYBY, CAMERATYPE_MOUSELOOK );
    InitialiseFlyByPoints();

    Rotation.Y = Rotation.Z = 0.0f;
    Rotation.X = -45.0f;
    SetCameraRotation( &Rotation );

    CursorX = g_HWConfig.GfxWidth  / 2;
    CursorY = g_HWConfig.GfxHeight / 2;

    DialogShow( DialogFlyBy, 0 );
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
    if( IsDialogActive( DialogFlyBy ) )
    {
        if( IsKeyPressed( DIK_ESCAPE ) )
        {
            DialogHide( DialogFlyBy );
            DialogShow( DialogMain, 0 );
        }
    }

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateFlyBy =
{
    Constructor, Destructor, Poll
};
