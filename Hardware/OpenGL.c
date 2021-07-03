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

#include "Hardware.h"
#include "Error.h"

#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

HDC g_hDC = nullptr;
HGLRC g_hRC = nullptr;

extern HardwareConfig_t g_HWConfig;
extern HWND g_hMainWindow;


//----------------------------------------------------------------------------------------
// Name: StartOpenGL()
// Desc: 
//----------------------------------------------------------------------------------------

size_t StartOpenGL( void )
{
    PIXELFORMATDESCRIPTOR Format;
    int Index;

    LogWrite( "StartOpenGL() starting, BPP=[%zu], DepthBits=[%zu]...\n", g_HWConfig.GfxBPP, g_HWConfig.GfxDepthBits );

    g_hDC = GetDC( g_hMainWindow );
    if( nullptr == g_hDC )
        return SetError( "StartOpenGL() failed to get a DC for the main window, GetLastError() returned %zu", GetLastError() );

    ZeroMemory( &Format, sizeof( PIXELFORMATDESCRIPTOR ) );

    Format.nSize         = sizeof( PIXELFORMATDESCRIPTOR );
    Format.nVersion      = 1;
    Format.dwFlags       = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    Format.iPixelType    = PFD_TYPE_RGBA;
    Format.cColorBits    = static_cast<BYTE>(g_HWConfig.GfxBPP);
    Format.cDepthBits    = static_cast<BYTE>(g_HWConfig.GfxDepthBits);
    Format.iLayerType    = PFD_MAIN_PLANE;

    Index = ChoosePixelFormat( g_hDC, &Format );
    if( !Index )
        return SetError( "StartOpenGL() failed to choose a pixel format for the main window, GetLastError() returned %zu", GetLastError() );

    if( !SetPixelFormat( g_hDC, Index, &Format ) )
        return SetError( "StartOpenGL() failed to set the pixel format for the main window, GetLastError() returned %zu", GetLastError() );

    g_hRC = wglCreateContext( g_hDC );
    if( nullptr == g_hRC )
        return SetError( "StartOpenGL() failed to create a rendering context for the main window, GetLastError() returned %zu", GetLastError() );

    if( !wglMakeCurrent( g_hDC, g_hRC ) )
        return SetError( "StartOpenGL() failed to make the rendering context the current one, GetLastError() returned %zu", GetLastError() );

    LogWrite( "StartOpenGL() succeeded\n" );
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StopOpenGL()
// Desc: 
//----------------------------------------------------------------------------------------

void StopOpenGL( void )
{
    LogWrite( "StopOpenGL() stopping...\n" );

    if( g_hRC && g_hDC && g_hMainWindow )
    {
        wglMakeCurrent( nullptr, nullptr );
        wglDeleteContext( g_hRC );

        ReleaseDC( g_hMainWindow, g_hDC );
    }

    LogWrite( "StopOpenGL() stopped\n" );
}
