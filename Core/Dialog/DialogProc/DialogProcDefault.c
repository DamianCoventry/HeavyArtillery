//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
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
// Name: DialogProcDefault()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcDefault( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    size_t i;
    float X, Y;
    Dialog_t *Child;
    //Dialog_t *current;
    int MouseX, MouseY;

    switch( Msg )
    {
        case WM_ACTIVATE :
            if( WA_ACTIVE == (size_t)Param1 )
            {
                //Dialog_Focus = Dialog->Focus;
                //if( Dialog->Hint )
                //    DialogSend( Dialog_statusbar->Children[ 1 ], WM_SETTEXT, Dialog->Hint, 0 );
            }
            return DIALOG_MSGHANDLED;

        case WM_DESTROY :
            // propagate to Children
            for( i = 0; i < Dialog->ChildCount; i++ )
                DialogSend( Dialog->Children[ i ], Msg, Param1, Param2 );

            if( Dialog->Data )
            {
                delete Dialog->Data;
                Dialog->Data = nullptr;
            }
            return 1;

        case WM_PAINT :
            if( Dialog->Flags & DIALOG_CHILD )
            {
                if( !( Dialog->Flags & DIALOG_PROPPAGE ) )
                {
                    DialogClientToScreen( Dialog, &X, &Y );

                    if( Dialog->Flags & DIALOG_TABSELECTED )
                    {
                        DialogDrawRectTabTop( X, Y, Dialog->W, Dialog->H );
                    }
                    else if( Dialog->Flags & DIALOG_TABUNSELECTED )
                    {
                        DialogDrawRectTabBottom( X, Y, Dialog->W, Dialog->H );
                    }
                    else
                    {
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

                        if( Dialog->Parent->Focus == Dialog )
                            DialogDrawRectFocus( X+4, Y+4, Dialog->W-8, Dialog->H-8 );
                    }
                }
            }
            else
            {
                DialogDrawRectRaised( Dialog->X, Dialog->Y, Dialog->W, Dialog->H );

                if( Dialog->Flags & DIALOG_TITLE )
                {
                    if( DialogHead == Dialog )
                    {
                        if( Dialog->Flags & DIALOG_TOOLBAR )
                        {
                            DialogDrawRect( Dialog->X+8, Dialog->Y+8, Dialog->W-16, TOOLDIALOG_TITLEBARHEIGHT,
                                DialogScheme->Colors.TitleFocus.R, DialogScheme->Colors.TitleFocus.G,
                                DialogScheme->Colors.TitleFocus.B );

                            FontWrite( DialogScheme->Fonts.ControlHighlight, Dialog->X+12, Dialog->Y+8,
                                DialogScheme->Colors.FontTitleFocus, Dialog->Name );
                        }
                        else
                        {
                            DialogDrawRect( Dialog->X+8, Dialog->Y+8, Dialog->W-16, DIALOG_TITLEBARHEIGHT,
                                DialogScheme->Colors.TitleFocus.R, DialogScheme->Colors.TitleFocus.G,
                                DialogScheme->Colors.TitleFocus.B );

                            FontWrite( DialogScheme->Fonts.TitleFocus, Dialog->X+12, Dialog->Y+10,
                                DialogScheme->Colors.FontTitleFocus, Dialog->Name );
                        }
                    }
                    else
                    {
                        if( Dialog->Flags & DIALOG_TOOLBAR )
                        {
                            DialogDrawRect( Dialog->X+8, Dialog->Y+8, Dialog->W-16, TOOLDIALOG_TITLEBARHEIGHT,
                                DialogScheme->Colors.TitleNoFocus.R, DialogScheme->Colors.TitleNoFocus.G,
                                DialogScheme->Colors.TitleNoFocus.B );

                            FontWrite( DialogScheme->Fonts.ControlHighlight, Dialog->X+12, Dialog->Y+8,
                                DialogScheme->Colors.FontTitleNoFocus, Dialog->Name );
                        }
                        else
                        {
                            DialogDrawRect( Dialog->X+8, Dialog->Y+8, Dialog->W-16, DIALOG_TITLEBARHEIGHT,
                                DialogScheme->Colors.TitleNoFocus.R, DialogScheme->Colors.TitleNoFocus.G,
                                DialogScheme->Colors.TitleNoFocus.B );

                            FontWrite( DialogScheme->Fonts.TitleNoFocus, Dialog->X+12, Dialog->Y+10,
                                DialogScheme->Colors.FontTitleNoFocus, Dialog->Name );
                        }
                    }
                }
            }

            for( i = 0; i < Dialog->ChildCount; i++ )
            {
                if( Dialog->Children[ i ]->Flags & DIALOG_VISIBLE )
                    DialogSend( Dialog->Children[ i ], Msg, Param1, Param2 );
            }

            return DIALOG_MSGHANDLED;

        case WM_SETTEXT :
            Dialog->Name = *(std::string*)Param1;
            return DIALOG_MSGHANDLED;

        case WM_LBUTTONDOWN :
        case WM_RBUTTONDOWN :
        case WM_MBUTTONDOWN :
        {
            Child = DialogControlAtCoords( Dialog, CursorX, CursorY );
            if( Child && ( Child->Flags & DIALOG_VISIBLE ) && ( Child->Flags & DIALOG_ENABLED ) )
            {
                DialogSetFocus( Dialog, Child );
                return DialogSend( Child, Msg, Param1, Param2 );
            }
            else if( !( Dialog->Flags & DIALOG_CHILD ) )
                DialogSetFocus( Dialog, nullptr );

            DialogCapture = Dialog;

            return DIALOG_MSGHANDLED;
        }
        case WM_LBUTTONUP :
        case WM_RBUTTONUP :
        case WM_MBUTTONUP :
            Child = DialogControlAtCoords( Dialog, CursorX, CursorY );
            if( Child )
                return DialogSend( Child, Msg, Param1, Param2 );

            return DIALOG_MSGHANDLED;

        case WM_MOUSEMOVE :
            GetMouseMovement( &MouseX, &MouseY );
            if( DialogCapture == Dialog && !( Dialog->Flags & DIALOG_CHILD ) && ( Dialog->Flags & DIALOG_MOVEABLE ) )
            {
                if( CursorX > 0.0f && CursorX < DialogViewportX2 )
                    Dialog->X += MouseX;
                if( CursorY > 0.0f && CursorY < DialogViewportY2 )
                    Dialog->Y += MouseY;
            }
            else if( !DialogCapture )
                Dialog->Hover = DialogControlAtCoords( Dialog, CursorX, CursorY );

            return DIALOG_MSGHANDLED;

        case WM_KEYDOWN :
            // send the key to the ctrl With the Focus
            if( DIALOG_MSGHANDLED == DialogSend( Dialog->Focus, Msg, Param1, Param2 ) )
                return DIALOG_MSGHANDLED;

            if( IsKeyPressed( DIK_TAB ) )
            {
                if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
                    DialogFocusPrev( Dialog->Parent ? Dialog->Parent : Dialog );
                else
                    DialogFocusNext( Dialog->Parent ? Dialog->Parent : Dialog );

                return 1;
            }

            if( IsKeyPressed( DIK_RETURN ) )
            {
                for( i = 0; i < Dialog->ChildCount; i++ )
                {
                    if( ( Dialog->Children[ i ]->Flags & DIALOG_OK ) && 
                        ( Dialog->Children[ i ]->Flags & DIALOG_ENABLED ) )
                        return DialogSend( Dialog, WM_COMMAND, (void*)BN_CLICKED, Dialog->Children[ i ] );
                }
            }

            if( IsKeyPressed( DIK_ESCAPE ) )
            {
                for( i = 0; i < Dialog->ChildCount; i++ )
                {
                    if( ( Dialog->Children[ i ]->Flags & DIALOG_CANCEL ) && 
                        ( Dialog->Children[ i ]->Flags & DIALOG_ENABLED ) )
                        return DialogSend( Dialog, WM_COMMAND, (void*)BN_CLICKED, Dialog->Children[ i ] );
                }
            }
            break;
    }

    return DIALOG_MSGNOTHANDLED;
}
