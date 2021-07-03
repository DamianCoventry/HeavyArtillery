//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcStatic.h"
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
// Name: DialogProcStatic()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcStatic( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    float X, Y;

    switch( Msg )
    {
        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );

            if( Dialog->Flags & DIALOG_RAISED )
                DialogDrawRectRaised( X, Y, Dialog->W, Dialog->H );
            else
                DialogDrawRectSunken( X, Y, Dialog->W, Dialog->H );

            return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
