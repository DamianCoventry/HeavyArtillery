//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 10 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "DialogBotToolbar.h"
#include "DialogPopupGame.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"

#include "Hardware.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t *DialogBotToolbar;

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcBotToolbar()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcBotToolbar( struct Dialog_s *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    float X, Y;

    switch( Msg )
    {
        case WM_CREATE :
            Dialog->X = ( (float)g_HWConfig.GfxWidth / 2.0f ) - ( Dialog->W / 2.0f );
            Dialog->Y = 8.0f;
            break;

        case WM_DISPLAYCHANGE :
            Dialog->X = ( (float)g_HWConfig.GfxWidth / 2.0f ) - ( Dialog->W / 2.0f );
            Dialog->Y = 8.0f;
            return DIALOG_MSGHANDLED;

        case WM_ACTIVATE :
            DialogShowCursor( 1 );
            return DIALOG_MSGHANDLED;

        case WM_COMMAND :
            DialogClientToScreen( Dialog, &X, &Y );

            if( Param2 == Dialog->Children[ 0 ] )
            {
                DialogTrackPopup( DialogPopupGame, X + Dialog->Children[ 0 ]->X,
                    Y + Dialog->Children[ 0 ]->Y + Dialog->Children[ 0 ]->H, 0 );
            }
            return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}


//-----------------------------------------------------------------------------
// Name: DialogBotToolbarCreate()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogBotToolbarCreate( void )
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        TOOLDIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING;

    DialogBotToolbar = DialogCreate( "Game Toolbar", DIALOG_MOVEABLE | DIALOG_TOOLBAR | DIALOG_TITLE,
                               0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcBotToolbar, 0, 0 );

    DialogCreate( "Game", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM | DIALOG_CENTER,
        DIALOG_PADDING, TOOLDIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW, buttonH, DialogBotToolbar, DialogProcButton, 0, 0 );

    return 1;
}
