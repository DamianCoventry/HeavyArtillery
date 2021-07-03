//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcScrollBox.h"
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
// Name: DialogProcScrollBox()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcScrollBox( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    ScrollBox_t *ScrollBox;
    float OffsetY;
    size_t Send;
    float Min, Max, Pos;
    size_t percent;
    float X, Y;
    int MouseY;

    ScrollBox = (ScrollBox_t *)Dialog->Data;

    switch( Msg )
    {
        case WM_CREATE :
            ScrollBox = new ScrollBox_t();
            ScrollBox->OffsetY = 0;
            Dialog->Data = (void *)ScrollBox;
            return DIALOG_MSGHANDLED;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );
            DialogDrawRectRaised( X, Y, Dialog->W, Dialog->H );
            return DIALOG_MSGHANDLED;

        case WM_LBUTTONDOWN :
        case WM_RBUTTONDOWN :
        case WM_MBUTTONDOWN :
            ScrollBox->OffsetY = CursorY;
            DialogScreenToClient( Dialog, nullptr, &ScrollBox->OffsetY );
            break;

        case WM_MOUSEMOVE :
            if( DialogCapture == Dialog )
            {
                Send = 0;

                if( CursorY > 0.0f && CursorY < DialogViewportY2 )
                {
                    GetMouseMovement( nullptr, &MouseY );
                    DialogClientToScreen( Dialog, nullptr, &OffsetY );

                    if( MouseY < 0 )
                    {
                        if( CursorY < OffsetY + ScrollBox->OffsetY )
                        {
                            Dialog->Y += (float)MouseY;
                            Send = 1;
                        }
                    }
                    else if( MouseY > 0 )
                    {
                        if( CursorY > OffsetY + ScrollBox->OffsetY )
                        {
                            Dialog->Y += (float)MouseY;
                            Send = 1;
                        }
                    }
                }

                if( Dialog->Y < Dialog->Parent->Children[ 0 ]->Y + SCROLLBAR_SIZE )
                    Dialog->Y = Dialog->Parent->Children[ 0 ]->Y + SCROLLBAR_SIZE;

                if( Dialog->Y > Dialog->Parent->Children[ 1 ]->Y - SCROLLBAR_SIZE )
                    Dialog->Y = Dialog->Parent->Children[ 1 ]->Y - SCROLLBAR_SIZE;

                if( Send )
                {
                    DialogClientToScreen( Dialog->Parent, nullptr, &Min );
                    DialogClientToScreen( Dialog, nullptr, &Pos );
                    Pos -= ( Min + SCROLLBAR_SIZE );
                    Max = Dialog->Parent->H - ( 3 * SCROLLBAR_SIZE );

                    percent = (size_t)( ( (float)Pos / (float)Max ) * 100.0f );

                    DialogSend( Dialog->Parent, WM_COMMAND, (void*)percent, Dialog );
                }
            }
            return DIALOG_MSGHANDLED;

        case WM_KEYDOWN :
            return DialogSend( Dialog->Parent, Msg, Param1, Param2 );
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
