//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcScrollButton.h"
#include "DialogProcScrollBar.h"
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
// Name: DialogProcScrollButton()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcScrollButton( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    ScrollButton_t *ScrollButton;
    float X, Y;

    ScrollButton = (ScrollButton_t *)Dialog->Data;

    switch( Msg )
    {
        case WM_CREATE :
            ScrollButton = new ScrollButton_t();
            ScrollButton->Type = (size_t)Param1;
            Dialog->Data = (void *)ScrollButton;
            return DIALOG_MSGHANDLED;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );

            if( DialogCapture == Dialog )
            {
                if( DialogControlAtCoords( Dialog->Parent, CursorX, CursorY ) == Dialog )
                    DialogDrawRectSunken( X, Y, Dialog->W, Dialog->H );
                else
                    DialogDrawRectRaised( X, Y, Dialog->W, Dialog->H );
            }
            else
            {
                DialogDrawRectRaised( X, Y, Dialog->W, Dialog->H );
            }

            if( ScrollButton->Type )
            {
                if( X >= 0.0f && X+SCROLLBAR_SIZE <= DialogViewportX2 &&
                    Y >= 0.0f && Y+SCROLLBAR_SIZE <= DialogViewportY2 )
                {
                    DialogDrawLine( X+SCROLLBUTTON_ARROWPADDING, Y+SCROLLBAR_SIZE-SCROLLBUTTON_ARROWPADDING,
                             X+SCROLLBAR_SIZEHALF, Y+SCROLLBUTTON_ARROWPADDING,
                             DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );

                    DialogDrawLine( X+SCROLLBAR_SIZEHALF, Y+SCROLLBUTTON_ARROWPADDING,
                             X+SCROLLBAR_SIZE-SCROLLBUTTON_ARROWPADDING, Y+SCROLLBAR_SIZE-SCROLLBUTTON_ARROWPADDING,
                             DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );
                }
            }
            else
            {
                if( X >= 0.0f && X+SCROLLBAR_SIZE <= DialogViewportX2 &&
                    Y >= 0.0f && Y+SCROLLBAR_SIZE <= DialogViewportY2 )
                {
                    DialogDrawLine( X+SCROLLBUTTON_ARROWPADDING, Y+SCROLLBUTTON_ARROWPADDING,
                             X+SCROLLBAR_SIZEHALF, Y+SCROLLBAR_SIZE-SCROLLBUTTON_ARROWPADDING,
                             DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );

                    DialogDrawLine( X+SCROLLBAR_SIZEHALF, Y+SCROLLBAR_SIZE-SCROLLBUTTON_ARROWPADDING,
                             X+SCROLLBAR_SIZE-SCROLLBUTTON_ARROWPADDING, Y+SCROLLBUTTON_ARROWPADDING,
                             DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );
                }
            }

            return DIALOG_MSGHANDLED;

        case WM_LBUTTONUP :
        case WM_RBUTTONUP :
        case WM_MBUTTONUP :
            if( Dialog->Parent->Focus == Dialog )
            {
                //snd_play( snd_click1, 0 );
                return DialogSend( Dialog->Parent, WM_COMMAND, (void*)BN_CLICKED, Dialog );
            }
            break;

        case WM_KEYDOWN :
            return DialogSend( Dialog->Parent, Msg, Param1, Param2 );
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
