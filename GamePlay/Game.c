//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>

#include "Hardware.h"
#include "Graphics.h"
#include "Timer.h"
#include "View.h"
#include "Targa.h"
#include "Shoot.h"
#include "Game.h"

#include "Log.h"

#include "State.h"
#include "StateTypes.h"
#include "StateFlyBy.h"

#include "LoadingScreen.h"
#include "LightSource.h"
#include "Camera.h"
#include "View.h"

#include "Dialog.h"
#include "DialogScheme.h"
#include "DialogProcEdit.h"
#include "DialogStartupProgress.h"

#include "GameSounds.h"
#include "GameMusic.h"
#include "GameModels.h"
#include "GameParticles.h"
#include "GameDecals.h"
#include "GameWeapons.h"
#include "GameItems.h"
#include "GameDialogs.h"
#include "GameExplosions.h"

#include "ObjectSkySphere.h"
#include "ParticleSnowflake.h"

#include "MeshManager.h"
#include "ObjectManager.h"
#include "DecalManager.h"
#include "ParticleManager.h"
#include "ModelManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "MusicManager.h"
#include "SoundManager.h"

#include "BotBuy.h"
#include "CameraMouseLook.h"
#include "Input.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern HDC g_hDC;
extern HardwareConfig_t g_HWConfig;
extern HWND g_hSplashWindow;

size_t g_LandscapeMesh;
size_t g_SnowstormID = -1;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _SkySphere;


//----------------------------------------------------------------------------------------
// Name: CreateOnceOffObjects()
// Desc: 
//----------------------------------------------------------------------------------------

static void CreateOnceOffObjects( void )
{
    Vector_t Position;
    size_t MusicID;

    LogWrite( "CreateOnceOffObjects() creating...\n" );

    // create the one and only landscape
    g_LandscapeMesh = CreateMesh( TILESET_RANDOM, 64, 64, 150.0f, 150.0f );

    if( TILESET_SNOW == GetMeshTileSetType( g_LandscapeMesh ) )
        g_SnowstormID = StartSnowflake();

    GetMeshCenter( g_LandscapeMesh, &Position );
    SetCameraPosition( &Position );

    _SkySphere = GetFreeObject();
    ResetObject( _SkySphere );
    SetObjectModels( _SkySphere, g_ModelSkySphere, -1 );
    SetObjectDrawFunction( _SkySphere, DrawNormalModel );
    SetObjectOnThinkFunction( _SkySphere, SkySphereThink );

    ShowObject( _SkySphere );
    ActivateObject( _SkySphere );

    CreatePlayers();

    StopMusic( -1 );
    MusicID = ChooseRandomMusicTrack();
    if( MusicID != -1 )
    {
        SetMusicVolume( MusicID, -1000 );
        PlayMusic( MusicID );
    }

    LogWrite( "CreateOnceOffObjects() created\n" );
}


//----------------------------------------------------------------------------------------
// Name: ApplyOpenGLSettings()
// Desc: 
//----------------------------------------------------------------------------------------

static void ApplyOpenGLSettings( void )
{
    glAlphaFunc( GL_GREATER, 0.0f );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_CULL_FACE );

    glViewport( 0, 0, static_cast<GLsizei>(g_HWConfig.GfxWidth), static_cast<GLsizei>(g_HWConfig.GfxHeight) );
    //glViewport( 100, 100, 300, 300 );
}


//----------------------------------------------------------------------------------------
// Name: InitialiseGame()
// Desc: called ONCE before the game loop starts for the first time
//----------------------------------------------------------------------------------------

size_t InitialiseGame( void )
{
    LogWrite( "InitialiseGame() beginning...\n" );

    ApplyOpenGLSettings();

    DialogSetViewport( 0.0f, 0.0f, (float)g_HWConfig.GfxWidth, (float)g_HWConfig.GfxHeight );

    if( !DialogSchemeInitialise( g_hDC ) )
        return 0;

    DialogStartupProgressCreate();

    Set2D();

    // create all structures that have application scope
    CreateKeyASCIITable();
    CreateLoadingScreen();

    ShowWindow( g_hSplashWindow, SW_HIDE );

    if( !CreateSounds() )
        return 0;

    if( !CreateMusic() )
        return 0;

    if( !CreateModels() )
        return 0;

    if( !CreateParticles() )
        return 0;

    if( !CreateDecals() )
        return 0;

    if( !CreateWeapons() )
        return 0;

    if( !CreateItems() )
        return 0;

    if( !CreateDialogs() )
        return 0;

    if( !CreateExplosions() )
        return 0;

    SetupLightSource();
    SetupBotLookups();

    CreateOnceOffObjects();

    // default to the flyby state
    SetGameState( &g_StateFlyBy );

    LogWrite( "InitialiseGame() done\n" );
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: ShutdownGame()
// Desc: called after the game loop is exited and just before we exit the app
//----------------------------------------------------------------------------------------

void ShutdownGame( void )
{
    DeletePlayers();
    ReturnObject( _SkySphere );
    DeleteMesh( g_LandscapeMesh );

    // delete everything we built at app startup
    DeleteExplosions();
    DeleteDialogs();
    DeleteItems();
    DeleteWeapons();
    DeleteDecals();
    DeleteParticles();
    DeleteModels();
    DeleteMusic();
    DeleteSounds();
    DeleteLoadingScreen();

    DialogSend( DialogStartupProgress, WM_DESTROY, 0, 0 );
    DialogSchemeShutdown();
}


//----------------------------------------------------------------------------------------
// Name: DrawFPS()
// Desc: 
//----------------------------------------------------------------------------------------

static void DrawFPS( void )
{
    glEnable( GL_TEXTURE_2D );
    FontWrite( DialogScheme->Fonts.ControlNormal, 8.0f, 8.0f, 1.0f, 1.0f, 1.0f, GetFPS() );
    glDisable( GL_TEXTURE_2D );
}


//----------------------------------------------------------------------------------------
// Name: ShutdownGame()
// Desc: 
//----------------------------------------------------------------------------------------

void PollGame( void )
{
    Vector_t *Position;

    PollCurrentState();             // call the current input callback

    // temp
    if( IsKeyPressed( DIK_C ) && ( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) ) )
        g_ClipCamera = !g_ClipCamera;

    glClear( GL_DEPTH_BUFFER_BIT );

    Set3D();                        // setup the modelview matrix using the camera
    SetupViewTransform();
    ExtractViewFrustumPlanes();     // extract planes from the projection & modelview matrices

    UpdateSounds();
    UpdateAmbientSounds();

    CameraThink();                  // allow all subsystems to think
    MeshManagerThink();
    ObjectManagerThink();
    ParticleManagerThink();
    DecalMgrExpire();

    CameraMove();                   // move the camera, objects and particles around
    ObjectManagerMove();
    ParticleManagerMove();

    CameraCollide();                // collid the camera, objects and particles with the landscape & each other
    ObjectManagerCollide( g_LandscapeMesh );
    ParticleManagerCollide();

    Position = GetCameraPosition();
                                    // draw everything taking into consideration the viewfrustum and camera position
    MeshManagerDraw( GetViewFrustum(), Position->X, Position->Z );
    DecalMgrDraw( GetViewFrustum() );
    ObjectManagerDraw( GetViewFrustum() );
    ParticleManagerDraw( GetViewFrustum() );

    ObjectManagerRemoveDead();      // remove any objects flagged as dead

    Set2D();                        // change the projection matrix to 2D

    MoveMouseCursor();              // convert input from the mouse/keyboard into messages...
    DispatchDialogMessages();       // ...and dispatch them to the dialog procedures
 
    DrawActiveDialogs();            // draw all dialogs & the mouse cursor
    DrawMouseCursor();
    DrawCrossHair();

    DrawFPS();
}


//----------------------------------------------------------------------------------------
// Name: ScreenShot()
// Desc: 
//----------------------------------------------------------------------------------------

void ScreenShot( void )
{
    size_t i;
    FILE *File;
    errno_t Error;
    unsigned char *Pixels;

    CreateDirectory( "ScreenShots", nullptr );

    // find the next available screenshot filename
    std::ostringstream oss;
    for( i = 0; ; i++ )
    {
        oss.str("");
        oss << "ScreenShots/ScreeShot" << std::setw(4) << std::setfill('0') << i << ".tga";
        Error = fopen_s( &File, oss.str().c_str(), "rb" );
        if( Error != 0 )
            fclose( File );
        else
            break;
    }

    Pixels = new unsigned char[g_HWConfig.GfxWidth * g_HWConfig.GfxHeight * ( g_HWConfig.GfxBPP >> 3 )];
    if( nullptr == Pixels )
        return;

    glReadPixels( 0, 0, static_cast<GLsizei>(g_HWConfig.GfxWidth), static_cast<GLsizei>(g_HWConfig.GfxHeight), 24 == g_HWConfig.GfxBPP ? GL_BGR_EXT : GL_BGRA_EXT, GL_UNSIGNED_BYTE, Pixels );
    TargaSave(oss.str(), g_HWConfig.GfxWidth, g_HWConfig.GfxHeight, g_HWConfig.GfxBPP, Pixels );

    delete [] Pixels;
}


//----------------------------------------------------------------------------------------
// Name: ChooseRandomMusicTrack()
// Desc: 
//----------------------------------------------------------------------------------------

size_t ChooseRandomMusicTrack( void )
{
    size_t Tracks[ 3 ];
    size_t TrackCount = 0;

    Tracks[ TrackCount ] = MusicAmbient0;
    if( -1 != Tracks[ TrackCount ] )
        TrackCount++;

    Tracks[ TrackCount ] = MusicAmbient1;
    if( -1 != Tracks[ TrackCount ] )
        TrackCount++;

    Tracks[ TrackCount ] = MusicAmbient2;
    if( -1 != Tracks[ TrackCount ] )
        TrackCount++;

    if( TrackCount > 0 )
        return Tracks[ rand() % TrackCount ];

    return -1;
}
