//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdio.h>
#include "WindowProc.h"
#include "Input.h"
#include "Graphics.h"

#include "TextureManager.h"
#include "MusicManager.h"

#include "DialogScheme.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_Active;


//----------------------------------------------------------------------------------------
// Name: RegisterWindowClass()
// Desc: 
//----------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    RECT Rect;

    switch( uMsg )
    {
        case WM_ACTIVATE :
            g_Active = ( LOWORD( wParam ) != WA_INACTIVE );

            if( g_Active )
            {
                AcquireInputDevices();

                GetClientRect( hWnd, &Rect );
                ClientToScreen( hWnd, (POINT *)&Rect.left );
                ClientToScreen( hWnd, (POINT *)&Rect.right );
                ClipCursor( &Rect );

                ResumeMusic( -1 );
            }
            else
            {
                UnacquireInputDevices();
                ClipCursor( nullptr );

                PauseMusic( -1 );
            }
            return 0;

        case WM_SYSCOMMAND :
            switch( wParam )
            {
                case SC_CONTEXTHELP :
                case SC_MONITORPOWER :
                case SC_HOTKEY :
                case SC_SCREENSAVE :
                case SC_TASKLIST :
                    return 0;
            }
            break;

        case WM_SETCURSOR :
            SetCursor( nullptr );
            return 0;

        case WM_CLOSE :
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
