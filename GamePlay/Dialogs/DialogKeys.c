//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 11 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogKeys.h"
#include "Hardware.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "KeyList.h"

#define DIALOGKEYS_LINEHEIGHT       25


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t *DialogKeys;

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcKeys()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcKeys( struct Dialog_s *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    KeyList_t *KeyList;
    size_t i;

    switch( Msg )
    {
        case WM_DISPLAYCHANGE :
        case WM_CREATE :
            Dialog->Y = 8.0f;
            Dialog->X = (float)g_HWConfig.GfxWidth - Dialog->W - 8.0f;
            return DIALOG_MSGHANDLED;

        case WM_ACTIVATE :
            if( WA_ACTIVE == (size_t)Param1 && 0 != Param2 )
            {
                DialogShowCursor( 0 );

                KeyList = (KeyList_t *)Param2;

                for( i = 0; i < MAXKEYSINLIST/2; i++ )
                {
                    if( i < KeyList->KeyCount )
                    {
                        DialogSend( Dialog->Children[ i ], WM_SETTEXT, (void*)&KeyList->Keys[ i ].KeyName, 0 );
                        DialogSend( Dialog->Children[ i+(MAXKEYSINLIST/2) ], WM_SETTEXT, (void*)&KeyList->Keys[ i ].Description, 0 );

                        Dialog->Children[ i ]->Flags |= DIALOG_VISIBLE;
                        Dialog->Children[ i+(MAXKEYSINLIST/2) ]->Flags |= DIALOG_VISIBLE;
                    }
                    else
                    {
                        Dialog->Children[ i ]->Flags &= ~DIALOG_VISIBLE;
                        Dialog->Children[ i+(MAXKEYSINLIST/2) ]->Flags &= ~DIALOG_VISIBLE;
                    }
                }

                Dialog->H = DIALOG_TITLEBARHEIGHT + DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + ( DIALOGKEYS_LINEHEIGHT * (float)KeyList->KeyCount );
            }
            return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}


//-----------------------------------------------------------------------------
// Name: DialogKeysCreate()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogKeysCreate( void )
{
    const float labelThinW = MESSAGEBOX_BUTTONWIDTH;
    const float labelWideW = MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float labelH = MESSAGEBOX_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + labelThinW + DIALOG_PADDING + labelWideW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        (labelH + DIALOG_PADDING) * DIALOGKEYS_LINEHEIGHT;

    size_t i;

    DialogKeys = DialogCreate( "Keys", DIALOG_MOVEABLE | DIALOG_TITLE,
        0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcKeys, 0, 0 );

    for (i = 0; i < MAXKEYSINLIST / 2; i++)
    {
        DialogCreate("aaa", DIALOG_CHILD | DIALOG_HIGHLIGHT, DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + (i*DIALOGKEYS_LINEHEIGHT),
            labelThinW, labelH, DialogKeys, DialogProcLabel, 0, 0);
    }

    for (i = 0; i < MAXKEYSINLIST / 2; i++)
    {
        DialogCreate("bbb", DIALOG_CHILD, dialogW - labelWideW - DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + (i*DIALOGKEYS_LINEHEIGHT),
            labelWideW, labelH, DialogKeys, DialogProcLabel, 0, 0);
    }

    return 1;
}
