//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcScrollBar.h"
#include "DialogProcScrollBox.h"
#include "DialogProcScrollButton.h"
#include "DialogProcDefault.h"
#include "Dialog.h"
#include "DialogScheme.h"
#include "DialogDraw.h"
#include "Input.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// Name: DialogProcScrollBar()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcScrollBar( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    Dialog_t *Child;
    float ScrollBoxY;
    float X, Y;

    switch( Msg )
    {
        case WM_CREATE :
            // create 3 Children: up & down buttons, and a scroll box
            Dialog->Children[ 0 ] = DialogCreate( "sbar up",   DIALOG_CHILD | DIALOG_VISIBLE,
                                             0, 0, SCROLLBAR_SIZE, SCROLLBAR_SIZE, Dialog,
                                             DialogProcScrollButton, (void*)1, 0 );

            Dialog->Children[ 1 ] = DialogCreate( "sbar down", DIALOG_CHILD | DIALOG_VISIBLE,
                                             0, Dialog->H - SCROLLBAR_SIZE, SCROLLBAR_SIZE, SCROLLBAR_SIZE, Dialog,
                                             DialogProcScrollButton, 0, 0 );

            Dialog->Children[ 2 ] = DialogCreate( "sbar box",  DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MOVEABLE,
                                             0, SCROLLBAR_SIZE, SCROLLBAR_SIZE, SCROLLBAR_SIZE, Dialog,
                                             DialogProcScrollBox, 0, 0 );
            return DIALOG_MSGHANDLED;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );
            DialogDrawRectRaised( X, Y, Dialog->W, Dialog->H );
            // paint Children too
            DialogSend( Dialog->Children[ 0 ], Msg, Param1, Param2 );
            DialogSend( Dialog->Children[ 1 ], Msg, Param1, Param2 );
            DialogSend( Dialog->Children[ 2 ], Msg, Param1, Param2 );
            return DIALOG_MSGHANDLED;

        case WM_COMMAND :
            if( Param2 == Dialog->Children[ 0 ] )
            {
                DialogSend( Dialog->Parent, WM_VSCROLL, (void*)SB_LINEUP, 0 );
                return DIALOG_MSGHANDLED;
            }
            if( Param2 == Dialog->Children[ 1 ] )
            {
                DialogSend( Dialog->Parent, WM_VSCROLL, (void*)SB_LINEDOWN, 0 );
                return DIALOG_MSGHANDLED;
            }
            if( Param2 == Dialog->Children[ 2 ] )
            {
                DialogSend( Dialog->Parent, WM_VSCROLL, (void*)SB_THUMBTRACK, Param1 ); // Param1 is position
                return DIALOG_MSGHANDLED;
            }
            break;

        case WM_LBUTTONDOWN :
        case WM_RBUTTONDOWN :
        case WM_MBUTTONDOWN :
            Child = DialogControlAtCoords( Dialog, CursorX, CursorY );
            if( Child && ( Child->Flags & DIALOG_VISIBLE ) && ( Child->Flags & DIALOG_ENABLED ) )
            {
                DialogSetFocus( Dialog, Child );
                return DialogSend( Child, Msg, Param1, Param2 );
            }
            else if( !( Dialog->Flags & DIALOG_CHILD ) )
                DialogSetFocus( Dialog, nullptr );

            DialogCapture = Dialog;

            // work out if cursor is above or below scrollbox
            DialogClientToScreen( Dialog->Children[ 2 ], nullptr, &ScrollBoxY );
            if( CursorY < ScrollBoxY )
                DialogSend( Dialog->Parent, WM_VSCROLL, (void*)SB_PAGEUP, 0 );
            else
                DialogSend( Dialog->Parent, WM_VSCROLL, (void*)SB_PAGEDOWN, 0 );

            return DIALOG_MSGHANDLED;

        case WM_KEYDOWN :
            return DialogSend( Dialog->Parent, Msg, Param1, Param2 );
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
