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
#include <gl/glu.h>

#include "Hardware.h"
#include "OpenGL.h"
#include "Error.h"

#include "TextureManager.h"
#include "DialogStartupProgress.h"
#include "DialogScheme.h"

#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern HardwareConfig_t g_HWConfig;
extern HWND g_hMainWindow;
extern HWND g_hSplashWindow;


//----------------------------------------------------------------------------------------
// Name: SetDisplayMode()
// Desc: 
//----------------------------------------------------------------------------------------

long SetDisplayMode( long Width, long Height, long BPP, long RefreshRate )
{
    DEVMODE Mode;

    LogWrite( "SetDisplayMode() %zu %zu %zu %zu\n", Width, Height, BPP, RefreshRate );

    ZeroMemory( &Mode, sizeof( DEVMODE ) );

    Mode.dmSize             = sizeof( DEVMODE );
    Mode.dmPelsWidth        = static_cast<DWORD>(Width);
    Mode.dmPelsHeight       = static_cast<DWORD>(Height);
    Mode.dmBitsPerPel       = static_cast<DWORD>(BPP);
    Mode.dmDisplayFrequency = static_cast<DWORD>(RefreshRate);
    Mode.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

    if( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings( &Mode, CDS_FULLSCREEN ) )
        return SetError( "SetDisplayMode() failed to set the display mode to %zux%zu, %zu BPP, %zu Refresh Rate, GetLastError() returned %zu",
            Width, Height, BPP, RefreshRate, GetLastError() );

    g_HWConfig.GfxWidth         = Width;
    g_HWConfig.GfxHeight        = Height;
    g_HWConfig.GfxBPP           = BPP;
    g_HWConfig.GfxRefreshRate   = RefreshRate;

    LogWrite( "SetDisplayMode() succeeded\n" );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: PlaceWindow()
// Desc: 
//----------------------------------------------------------------------------------------

static void PlaceWindow( long Style, long X, long Y, long Width, long Height )
{
    RECT Rect;

    LogWrite( "PlaceWindow() %zu %zu %zu %zu\n", X, Y, Width, Height );

    Rect.left = Rect.top = 0;
    Rect.right = Width;
    Rect.bottom = Height;
    AdjustWindowRect( &Rect, Style, FALSE );

    SetWindowPos( g_hMainWindow, nullptr, X, Y, Rect.right - Rect.left,
        Rect.bottom - Rect.top, SWP_NOZORDER );

    ShowWindow( g_hMainWindow, SW_SHOW );
}


//----------------------------------------------------------------------------------------
// Name: SetWindowSize()
// Desc: 
//----------------------------------------------------------------------------------------

long SetWindowSize( long Width, long Height, long BPP, long RefreshRate )
{
    RECT Rect;
    long X, Y;

    LogWrite( "SetWindowSize() %zu %zu %zu %zu\n", Width, Height, BPP, RefreshRate );

    if( g_HWConfig.GfxFullScreen )
    {
        LogWrite( "SetWindowSize() app is currently fullscreen\n" );

        if( !SetDisplayMode( Width, Height, BPP, RefreshRate ) )
            return 0;

        MoveWindow( g_hMainWindow, 0, 0, Width, Height, TRUE );
        ShowWindow( g_hMainWindow, SW_SHOW );
    }
    else
    {
        LogWrite( "SetWindowSize() app is currently windowed\n" );

        X = ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - ( Width  / 2 );
        Y = ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - ( Height / 2 );

        PlaceWindow( WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, X, Y, Width, Height );
    }

    GetClientRect( g_hMainWindow, &Rect );
    ClientToScreen( g_hMainWindow, (POINT *)&Rect.left );
    ClientToScreen( g_hMainWindow, (POINT *)&Rect.right );
    ClipCursor( &Rect );

    glViewport( 0, 0, Width, Height );
    //glViewport( 100, 100, 300, 300 );

    g_HWConfig.GfxWidth         = Width;
    g_HWConfig.GfxHeight        = Height;
    g_HWConfig.GfxBPP           = BPP;
    g_HWConfig.GfxRefreshRate   = RefreshRate;

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StartGraphicsDevice()
// Desc: 
//----------------------------------------------------------------------------------------

long StartGraphicsDevice( void )
{
    LogWrite( "StartGraphicsDevice() starting...\n" );

    if( g_HWConfig.GfxFullScreen )
    {
        LogWrite( "StartGraphicsDevice() fullscreen startup\n" );

        if( !SetDisplayMode( static_cast<long>(g_HWConfig.GfxWidth), static_cast<long>(g_HWConfig.GfxHeight), static_cast<long>(g_HWConfig.GfxBPP), static_cast<long>(g_HWConfig.GfxRefreshRate) ) )
            return 0;

        MoveWindow( g_hMainWindow, 0, 0, static_cast<int>(g_HWConfig.GfxWidth), static_cast<int>(g_HWConfig.GfxHeight), TRUE );
        ShowWindow( g_hMainWindow, SW_SHOW );
    }
    else
        LogWrite( "StartGraphicsDevice() windowed startup\n" );

    if( !StartOpenGL() )
        return 0;

    glViewport( 0, 0, static_cast<GLsizei>(g_HWConfig.GfxWidth), static_cast<GLsizei>(g_HWConfig.GfxHeight) );
    //glViewport( 100, 100, 300, 300 );

    LogWrite( "StartGraphicsDevice() succeeded\n" );
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StartGraphicsDevice()
// Desc: 
//----------------------------------------------------------------------------------------

void StopGraphicsDevice( void )
{
    LogWrite( "StopGraphicsDevice() stopping...\n" );

    StopOpenGL();

    if( g_HWConfig.GfxFullScreen )
        ChangeDisplaySettings( nullptr, 0 );

    LogWrite( "StopGraphicsDevice() succeeded\n" );
}


//----------------------------------------------------------------------------------------
// Name: HideAllWindows()
// Desc: 
//----------------------------------------------------------------------------------------

void HideAllWindows( void )
{
    ShowWindow( g_hSplashWindow, SW_HIDE );
    ShowWindow( g_hMainWindow, SW_HIDE );
}


//----------------------------------------------------------------------------------------
// Name: AlterWindowStyle()
// Desc: 
//----------------------------------------------------------------------------------------

static void AlterWindowStyle( long NewStyle )
{
    LogWrite( "AlterWindowStyle()\n" );

    SetWindowLong( g_hMainWindow, GWL_STYLE, NewStyle );

    SetWindowPos( g_hMainWindow, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

    ShowWindow( g_hMainWindow, SW_SHOW );
}


//----------------------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: 
//----------------------------------------------------------------------------------------

void ToggleFullScreen( void )
{
    long Style;
    long X, Y;
    RECT Rect;

    LogWrite( "ToggleFullScreen() succeeded\n" );

    if( g_HWConfig.GfxFullScreen )
    {
        LogWrite( "ToggleFullScreen() app is currently fullscreen\n" );

        ChangeDisplaySettings( nullptr, 0 );
        Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

        X = ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - int( g_HWConfig.GfxWidth  / 2 );
        Y = ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - int( g_HWConfig.GfxHeight / 2 );

        LogWrite( "ToggleFullScreen() changed to windowed mode\n" );
    }
    else
    {
        LogWrite( "ToggleFullScreen() app is currently windowed\n" );

        Style = WS_POPUP;
        X = Y = 0;

        if( !SetDisplayMode(static_cast<long>(g_HWConfig.GfxWidth), static_cast<long>(g_HWConfig.GfxHeight), static_cast<long>(g_HWConfig.GfxBPP), static_cast<long>(g_HWConfig.GfxRefreshRate) ) )
        {
            Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
            X = ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - int( g_HWConfig.GfxWidth  / 2 );
            Y = ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - int( g_HWConfig.GfxHeight / 2 );
        }

        LogWrite( "ToggleFullScreen() changed to fullscreen mode\n" );
    }

    AlterWindowStyle( Style );
    PlaceWindow( Style, X, Y, static_cast<long>(g_HWConfig.GfxWidth), static_cast<long>(g_HWConfig.GfxHeight) );

    GetClientRect( g_hMainWindow, &Rect );
    ClientToScreen( g_hMainWindow, (POINT *)&Rect.left );
    ClientToScreen( g_hMainWindow, (POINT *)&Rect.right );
    ClipCursor( &Rect );

    g_HWConfig.GfxFullScreen = !g_HWConfig.GfxFullScreen;

    DialogSchemeReloadFonts();
    ReloadAllTextures();
    DialogHide( DialogStartupProgress );

    LogWrite( "ToggleFullScreen() succeeded\n" );
}


//----------------------------------------------------------------------------------------
// Name: Set3D()
// Desc: 
//----------------------------------------------------------------------------------------

void Set3D( void )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    gluPerspective( g_HWConfig.GLVFOV, (float)g_HWConfig.GfxWidth / (float)g_HWConfig.GfxHeight,
        g_HWConfig.GLNCP3D, g_HWConfig.GLFCP3D );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}


//----------------------------------------------------------------------------------------
// Name: Set2D()
// Desc: 
//----------------------------------------------------------------------------------------

void Set2D( void )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( 0.0f, static_cast<GLdouble>(g_HWConfig.GfxWidth), static_cast<GLdouble>(g_HWConfig.GfxHeight), 0.0f,
        static_cast<GLdouble>(g_HWConfig.GLNCP2D), static_cast<GLdouble>(g_HWConfig.GLFCP2D) );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}
