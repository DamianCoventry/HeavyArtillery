//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGEDIT_H_
#define _DIALOGEDIT_H_

#include "Dialog.h"

#define EDIT_BLINKTHRESHOLD      15


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct Edit_s
{
    Edit_s() { Reset(); }

    void Reset()
    {
        OffsetX = OffsetY = 0.0f;
        CursorPos = FirstChar = BlinkCounter = BlinkVisible = 0;
        Text.clear();
    }

    float OffsetX, OffsetY;
    size_t CursorPos;
    size_t FirstChar;
    size_t BlinkCounter;
    size_t BlinkVisible;
    std::string Text;
}
Edit_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

void CreateKeyASCIITable( void );
char KeyToASCII( size_t Index, size_t Shift );

size_t DialogProcEdit( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
