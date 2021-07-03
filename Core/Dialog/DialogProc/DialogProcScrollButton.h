//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGSCROLLBUTTON_H_
#define _DIALOGSCROLLBUTTON_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct ScrollButton_s
{
    size_t Type;       // 0 = up, 1 = down
}
ScrollButton_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcScrollButton( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
