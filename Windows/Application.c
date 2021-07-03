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

#define CLASSNAME                   "GameClass"
#define APPLICATIONNAME             "Heavy Artillery v1.0"

#define GDISPLASHBITMAP_WIDTH       400
#define GDISPLASHBITMAP_HEIGHT      100

#include "Resource.h"
#include "Hardware.h"
#include "WindowProc.h"

#include "Error.h"
#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

HWND g_hMainWindow = nullptr;
HWND g_hSplashWindow = nullptr;

extern HINSTANCE g_hInstance;
extern HardwareConfig_t g_HWConfig;


//----------------------------------------------------------------------------------------
// Name: RegisterWindowClass()
// Desc: 
//----------------------------------------------------------------------------------------

size_t RegisterWindowClass( void )
{
    WNDCLASS Class;

    LogWrite( "RegisterWindowClass() registering...\n" );

    Class.style         = CS_OWNDC;
    Class.lpfnWndProc   = WindowProc;
    Class.cbClsExtra    = 0;
    Class.cbWndExtra    = 0;
    Class.hInstance     = g_hInstance;
    Class.hIcon         = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_HEAVY_ARTILLERY));
    Class.hCursor       = LoadCursor( nullptr, IDC_ARROW );
    Class.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    Class.lpszMenuName  = nullptr;
    Class.lpszClassName = CLASSNAME;

    if( !RegisterClass( &Class ) )
        return SetError( "RegisterWindowClass() failed, GetLastError() returned %zu", GetLastError() );

    LogWrite( "RegisterWindowClass() succeeded\n" );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: CreateMainWindow()
// Desc: 
//----------------------------------------------------------------------------------------

size_t CreateMainWindow( void )
{
    DWORD Style;
    int X, Y;
    int Width, Height;
    RECT Rect;

    LogWrite( "CreateMainWindow() creating...\n" );

    if( g_HWConfig.GfxFullScreen )
    {
        LogWrite( "CreateMainWindow() fullscreen startup\n" );

        Style = WS_POPUP;
        X = Y = 0;
        Width = GetSystemMetrics( SM_CXSCREEN );
        Height = GetSystemMetrics( SM_CYSCREEN );
    }
    else
    {
        LogWrite( "CreateMainWindow() windowed startup\n" );

        Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

        Rect.left = Rect.top = 0;
        Rect.right = static_cast<LONG>(g_HWConfig.GfxWidth);
        Rect.bottom = static_cast<LONG>(g_HWConfig.GfxHeight);
        AdjustWindowRect( &Rect, Style, FALSE );

        Width = Rect.right - Rect.left;
        Height = Rect.bottom - Rect.top;

        X = ( GetSystemMetrics( SM_CXSCREEN ) / 2 ) - ( Width / 2 );
        Y = ( GetSystemMetrics( SM_CYSCREEN ) / 2 ) - ( Height / 2 );
    }

    g_hMainWindow = CreateWindowEx( 0, CLASSNAME, APPLICATIONNAME, Style | WS_VISIBLE, X, Y,
        Width, Height, nullptr, nullptr, g_hInstance, 0 );

    if( nullptr == g_hMainWindow )
        return SetError( "CreateMainWindow() failed, GetLastError() returned %zu", GetLastError() );

    LogWrite( "CreateMainWindow() succeeded\n" );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: InitialiseApplication()
// Desc: 
//----------------------------------------------------------------------------------------

size_t InitialiseApplication( void )
{
    RECT Rect;
    HBITMAP hBitmap;

    if( !RegisterWindowClass() )
        return 0;

    if( !CreateMainWindow() )
        return 0;

    LogWrite( "InitialiseApplication() GDI splash screen\n" );

    // show the main window with a bitmap in
    // it so that the user knows the app is starting
    GetClientRect( g_hMainWindow, &Rect );

    hBitmap = static_cast<HBITMAP>(LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_GDI_SPLASH), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR));

    g_hSplashWindow = CreateWindowEx( 0, "STATIC", "TheBitmap", WS_CHILD | WS_VISIBLE | SS_BITMAP,
                                ( Rect.right / 2 ) - ( GDISPLASHBITMAP_WIDTH / 2 ),
                                ( Rect.bottom / 2 ) - ( GDISPLASHBITMAP_HEIGHT / 2 ),
                                0, 0, g_hMainWindow, nullptr, g_hInstance, nullptr );

    SendMessage( g_hSplashWindow, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap );

    // force windows to draw this child window, which has a bitmap in it
    RedrawWindow( g_hMainWindow, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: ShutdownApplication()
// Desc: 
//----------------------------------------------------------------------------------------

void ShutdownApplication( void )
{
    LogWrite( "ShutdownApplication() shutting down...\n" );

    if( g_hMainWindow )
    {
        DestroyWindow( g_hMainWindow );
        g_hMainWindow = nullptr;
    }

    UnregisterClass( CLASSNAME, g_hInstance );

    LogWrite( "ShutdownApplication() succeeded\n" );
}
