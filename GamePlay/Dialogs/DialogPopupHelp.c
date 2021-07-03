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

#include "DialogHelp.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t *DialogPopupHelp;


//-----------------------------------------------------------------------------
// Name: DialogProcPopupHelp()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcPopupHelp( struct Dialog_s *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    switch( Msg )
    {
        case WM_COMMAND :
            DialogHide( Dialog );
            if( Param2 == Dialog->Children[ 0 ] )
                DialogShow( DialogHelp, 0 );
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}


//-----------------------------------------------------------------------------
// Name: DialogPopupHelpCreate()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogPopupHelpCreate( void )
{
    const float buttonW = MENUITEM_BUTTONWIDTH;
    const float buttonH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        buttonH + DIALOG_PADDINGx2;

    DialogPopupHelp = DialogCreate( "PopupHelp", DIALOG_POPUPMENU,
                               0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPopupHelp, 0, 0 );

    DialogCreate( "View", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_MENUITEM,
        DIALOG_PADDING, DIALOG_PADDING, buttonW, buttonH, DialogPopupHelp, DialogProcButton, 0, 0 );

    return 1;
}
