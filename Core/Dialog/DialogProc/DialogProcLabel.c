//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcLabel.h"
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
// Name: DialogProcLabel()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcLabel( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    float X, Y;
    Button_t *Button;

    Button = (Button_t *)Dialog->Data;

    switch( Msg )
    {
        case WM_CREATE :
            Button = new Button_t();

            if( Dialog->Flags & DIALOG_CENTER )
                Button->OffsetX = ( Dialog->W / 2 ) - ( FontPixelWidth( DialogScheme->Fonts.ControlNormal, Dialog->Name, -1 ) / 2 );

            else if( Dialog->Flags & DIALOG_RIGHTALIGN )
                Button->OffsetX = Dialog->W - FontPixelWidth( DialogScheme->Fonts.ControlNormal, Dialog->Name, -1 );

            else
                Button->OffsetX = 0;

            Button->OffsetY = ( Dialog->H / 2 ) - ( DialogScheme->Fonts.ControlNormal->Height / 2 );

            Dialog->Data = (void *)Button;
            return DIALOG_MSGHANDLED;

        case WM_SIZE :
            if( Dialog->Flags & DIALOG_CENTER )
                Button->OffsetX = ( Dialog->W / 2 ) - ( FontPixelWidth( DialogScheme->Fonts.ControlNormal, Dialog->Name, -1 ) / 2 );

            else if( Dialog->Flags & DIALOG_RIGHTALIGN )
                Button->OffsetX = Dialog->W - FontPixelWidth( DialogScheme->Fonts.ControlNormal, Dialog->Name, -1 );

            else
                Button->OffsetX = 0;

            Button->OffsetY = ( Dialog->H / 2 ) - ( DialogScheme->Fonts.ControlNormal->Height / 2 );
            return DIALOG_MSGHANDLED;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );

            //DialogDrawRectControl( X, Y, Dialog->W, Dialog->H );

            if( Dialog->Flags & DIALOG_HIGHLIGHT )
                FontWrite( DialogScheme->Fonts.ControlHighlight, X + Button->OffsetX, Y + Button->OffsetY,
                    DialogScheme->Colors.FontControlHighlight, Dialog->Name );
            else
                FontWrite( DialogScheme->Fonts.ControlNormal, X + Button->OffsetX, Y + Button->OffsetY,
                    DialogScheme->Colors.FontControlNormal, Dialog->Name );
            return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
