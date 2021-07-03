//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 9 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcProgress.h"
#include "DialogProcDefault.h"
#include "Dialog.h"
#include "DialogScheme.h"
#include "DialogDraw.h"
#include "Input.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------
// Name: DialogProcProgress()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcProgress( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    float W, X, Y;
    ProgressBar_t *ProgressBar;

    ProgressBar = (ProgressBar_t *)Dialog->Data;

    switch( Msg )
    {
        case WM_CREATE :
            ProgressBar = new ProgressBar_t();

            Dialog->Data = (void *)ProgressBar;
            return DIALOG_MSGHANDLED;

        case PBM_SETRANGE :
            ProgressBar->Low  = (size_t)Param1;
            ProgressBar->High = (size_t)Param2;
            ProgressBar->Position = ProgressBar->Low;
            return DIALOG_MSGHANDLED;

        case PBM_SETPOS :
            ProgressBar->Position = (size_t)Param1;
            return DIALOG_MSGHANDLED;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );
            DialogDrawRectSunken( X, Y, Dialog->W, Dialog->H );

            W = ( Dialog->W / ( ProgressBar->High - ProgressBar->Low ) ) * ( ProgressBar->Position - ProgressBar->Low );

            DialogDrawRect( X, Y, W, Dialog->H-1.0f,
                DialogScheme->Colors.TitleFocus.R, DialogScheme->Colors.TitleFocus.G,
                DialogScheme->Colors.TitleFocus.B );

            return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
