//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGSCROLLBOX_H_
#define _DIALOGSCROLLBOX_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct ScrollBox_s
{
    float OffsetY;
}
ScrollBox_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcScrollBox( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
