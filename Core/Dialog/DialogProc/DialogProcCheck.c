//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcCheck.h"
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
// Name: DialogProcCheck()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcCheck( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    CheckBox_t *CheckBox;
    float X, Y;
    float CenterX, CenterY;
    size_t i;

    CheckBox = (CheckBox_t *)Dialog->Data;

    switch( Msg )
    {
        case WM_CREATE :
            CheckBox = new CheckBox_t();

            CheckBox->Checked = 0;
            CheckBox->Group = (size_t)Param1;

            CheckBox->OffsetX = CHECKBOX_SIZE + CHECKBOX_SIZE;
            CheckBox->OffsetY = ( Dialog->H / 2 ) - ( DialogScheme->Fonts.ControlNormal->Height / 2 );

            Dialog->Data = (void *)CheckBox;

            return DIALOG_MSGHANDLED;

        case WM_LBUTTONUP :
        case WM_RBUTTONUP :
        case WM_MBUTTONUP :
            if( Dialog->Parent->Focus == Dialog && ( Dialog->Flags & DIALOG_ENABLED ) )
            {
                CheckBox->Checked = !CheckBox->Checked;
                //snd_play( snd_click1, 0 );

                // this following bit of code makes mutually exclusive
                // Check boxes, aka RadioButtons
                if( CheckBox->Group )
                {
                    for( i = 0; i < Dialog->Parent->ChildCount; i++ )
                    {
                        if( Dialog != Dialog->Parent->Children[ i ] &&
                            Dialog->Parent->Children[ i ]->DialogProc == DialogProcCheck &&
                            ((CheckBox_t *)Dialog->Parent->Children[ i ]->Data)->Group == CheckBox->Group )
                            ((CheckBox_t *)Dialog->Parent->Children[ i ]->Data)->Checked = 0;
                    }
                }

                return DialogSend( Dialog->Parent, WM_COMMAND, (void*)BN_CLICKED, (void*)Dialog );
            }
            break;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );
            CenterX = X + CHECKBOX_SIZEFOCUSHALF;
            CenterY = Y + ( Dialog->H / 2 );

            if( Dialog->Flags & DIALOG_ENABLED )
            {
                DialogDrawRectControl( ( X + CHECKBOX_SIZEFOCUSHALF ) - CHECKBOX_SIZEHALF,
                                  Y + ( ( Dialog->H / 2 ) - CHECKBOX_SIZEHALF ),
                                  CHECKBOX_SIZE, CHECKBOX_SIZE );
            }
            else
            {
                DialogDrawRectSunken( ( X + CHECKBOX_SIZEFOCUSHALF ) - CHECKBOX_SIZEHALF,
                                 Y + ( ( Dialog->H / 2 ) - CHECKBOX_SIZEHALF ),
                                 CHECKBOX_SIZE, CHECKBOX_SIZE );
            }

            if( Dialog->Parent->Focus == Dialog )
                DialogDrawRectFocus( X, CenterY - CHECKBOX_SIZEFOCUSHALF, Dialog->W, CHECKBOX_SIZEFOCUS );

            if( CheckBox->Checked )
            {
                DialogDrawLine( CenterX-CHECKBOX_CHECKHALFSIZE+1, CenterY-CHECKBOX_CHECKHALFSIZE, CenterX+CHECKBOX_CHECKHALFSIZE, CenterY+CHECKBOX_CHECKHALFSIZE-1,
                    DialogScheme->Colors.FontControlNormal.R, DialogScheme->Colors.FontControlNormal.G, DialogScheme->Colors.FontControlNormal.B );

                DialogDrawLine( CenterX-CHECKBOX_CHECKHALFSIZE+1, CenterY+CHECKBOX_CHECKHALFSIZE-1, CenterX+CHECKBOX_CHECKHALFSIZE, CenterY-CHECKBOX_CHECKHALFSIZE,
                    DialogScheme->Colors.FontControlNormal.R, DialogScheme->Colors.FontControlNormal.G, DialogScheme->Colors.FontControlNormal.B );
            }

            if( Dialog->Flags & DIALOG_ENABLED )
                FontWrite( DialogScheme->Fonts.ControlNormal, X + CheckBox->OffsetX, Y + CheckBox->OffsetY,
                    DialogScheme->Colors.FontControlNormal, Dialog->Name );
            else
                FontWrite( DialogScheme->Fonts.ControlDisabled, X + CheckBox->OffsetX, Y + CheckBox->OffsetY,
                    DialogScheme->Colors.FontControlNormal, Dialog->Name );

            return DIALOG_MSGHANDLED;

        case WM_KEYDOWN :
            if( IsKeyPressed( DIK_SPACE ) && Dialog->Parent->Focus == Dialog && ( Dialog->Flags & DIALOG_ENABLED ) )
            {
                CheckBox->Checked = !CheckBox->Checked;

                // this following bit of code makes mutually exclusive
                // Check boxes, aka RadioButtons
                if( CheckBox->Group )
                {
                    for( i = 0; i < Dialog->Parent->ChildCount; i++ )
                    {
                        if( Dialog != Dialog->Parent->Children[ i ] &&
                            Dialog->Parent->Children[ i ]->DialogProc == DialogProcCheck &&
                            ((CheckBox_t *)Dialog->Parent->Children[ i ]->Data)->Group == CheckBox->Group )
                            ((CheckBox_t *)Dialog->Parent->Children[ i ]->Data)->Checked = 0;
                    }
                }
                return DialogSend( Dialog->Parent, WM_COMMAND, (void*)BN_CLICKED, (void*)Dialog );
            }
            break;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogDialogItemChecked( Dialog_t *Dialog )
{
    return( ((CheckBox_t *)Dialog->Data)->Checked );
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------

void DialogCheckDialogItem( Dialog_t *Dialog, size_t Checked )
{
    ((CheckBox_t *)Dialog->Data)->Checked = Checked;
}

