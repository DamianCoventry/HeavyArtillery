//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGBUTTON_H_
#define _DIALOGBUTTON_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct Button_s
{
    float OffsetX;
    float OffsetY;
    size_t StringLength;
}
Button_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcButton( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
