//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcButton.h"
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
// Name: DialogProcButton()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcButton( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    Button_t *Button;
    float X, Y;

    Button = (Button_t *)Dialog->Data;

    switch( Msg )
    {
        case WM_CREATE :
            Button = new Button_t();

            Button->StringLength = Dialog->Name.length();

            if( Dialog->Flags & DIALOG_CENTER )
                Button->OffsetX = ( Dialog->W / 2.0f ) - ( FontPixelWidth( DialogScheme->Fonts.ControlNormal, Dialog->Name, -1 ) / 2.0f );
            else
                Button->OffsetX = 0;

            Button->OffsetY = ( Dialog->H / 2.0f ) - ( DialogScheme->Fonts.ControlNormal->Height / 2.0f );

            Dialog->Data = (void *)Button;

            return DIALOG_MSGHANDLED;

        case WM_SIZE :
            if( Dialog->Flags & DIALOG_CENTER )
                Button->OffsetX = ( Dialog->W / 2.0f ) - ( FontPixelWidth( DialogScheme->Fonts.ControlNormal, Dialog->Name, -1 ) / 2.0f );
            else
                Button->OffsetX = 8;

            Button->OffsetY = ( Dialog->H / 2.0f ) - ( DialogScheme->Fonts.ControlNormal->Height / 2.0f );
            return DIALOG_MSGHANDLED;

        case WM_LBUTTONUP :
        case WM_RBUTTONUP :
        case WM_MBUTTONUP :
            if( Dialog->Parent->Focus == Dialog && Dialog->Flags & DIALOG_ENABLED )
            {
                //snd_play( snd_click0, 0 );
                return DialogSend( Dialog->Parent, WM_COMMAND, (void*)BN_CLICKED, (void*)Dialog );
            }
            break;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );

            if( Dialog->Flags & DIALOG_MENUITEM )
            {
                if( Dialog->Parent->Hover == Dialog )
                {
                    DialogDrawRect( X-4.0f, Y, Dialog->W+8.0f, Dialog->H,
                             DialogScheme->Colors.TitleFocus.R,
                             DialogScheme->Colors.TitleFocus.G,
                             DialogScheme->Colors.TitleFocus.B );

                    if( Dialog->Flags & DIALOG_ENABLED )
                    {
                        FontWrite( DialogScheme->Fonts.ControlHighlight, X + Button->OffsetX, Y + Button->OffsetY,
                            DialogScheme->Colors.FontControlHighlight, Dialog->Name );

                        if( Dialog->Flags & DIALOG_HASSUBMENU )
                            FontWrite( DialogScheme->Fonts.ControlHighlight, X + Dialog->W - 12, Y + Button->OffsetY,
                                DialogScheme->Colors.FontControlHighlight, ">" );
                    }
                    else
                    {
                        FontWrite( DialogScheme->Fonts.ControlDisabled, X + Button->OffsetX, Y + Button->OffsetY,
                            DialogScheme->Colors.FontControlDisabled, Dialog->Name );

                        if( Dialog->Flags & DIALOG_HASSUBMENU )
                            FontWrite( DialogScheme->Fonts.ControlDisabled, X + Dialog->W - 12, Y + Button->OffsetY,
                                DialogScheme->Colors.FontControlDisabled, ">" );
                    }
                }
                else
                {
                    DialogDrawRect( X, Y, Dialog->W, Dialog->H,
                             DialogScheme->Colors.DialogBack.R,
                             DialogScheme->Colors.DialogBack.G,
                             DialogScheme->Colors.DialogBack.B );

                    if( Dialog->Flags & DIALOG_ENABLED )
                    {
                        FontWrite( DialogScheme->Fonts.ControlNormal, X + Button->OffsetX, Y + Button->OffsetY,
                            DialogScheme->Colors.FontControlNormal, Dialog->Name );

                        if( Dialog->Flags & DIALOG_HASSUBMENU )
                            FontWrite( DialogScheme->Fonts.ControlNormal, X + Dialog->W - 12, Y + Button->OffsetY,
                                DialogScheme->Colors.FontControlNormal, ">" );
                    }
                    else
                    {
                        FontWrite( DialogScheme->Fonts.ControlDisabled, X + Button->OffsetX, Y + Button->OffsetY,
                            DialogScheme->Colors.FontControlDisabled, Dialog->Name );

                        if( Dialog->Flags & DIALOG_HASSUBMENU )
                            FontWrite( DialogScheme->Fonts.ControlDisabled, X + Dialog->W - 12, Y + Button->OffsetY,
                                DialogScheme->Colors.FontControlDisabled, ">" );
                    }
                }
            }
            else
            {
                DialogProcDefault( Dialog, Msg, Param1, Param2 );

                if (Dialog->Flags & DIALOG_TABUNSELECTED)
                    X += 1.0f, Y += 1.0f;

                if( Dialog->Flags & DIALOG_ENABLED )
                    FontWrite( DialogScheme->Fonts.ControlNormal, X + Button->OffsetX, Y + Button->OffsetY,
                        DialogScheme->Colors.FontControlNormal, Dialog->Name );
                else
                    FontWrite( DialogScheme->Fonts.ControlDisabled, X + Button->OffsetX, Y + Button->OffsetY,
                        DialogScheme->Colors.FontControlDisabled, Dialog->Name );
            }

            return DIALOG_MSGHANDLED;

        case WM_KEYDOWN :
            if( IsKeyPressed( DIK_SPACE ) && Dialog->Parent->Focus == Dialog && ( Dialog->Flags & DIALOG_ENABLED ) )
                return DialogSend( Dialog->Parent, WM_COMMAND, (void*)BN_CLICKED, (void*)Dialog );
            break;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
