//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGSCROLLBAR_H_
#define _DIALOGSCROLLBAR_H_

#include "Dialog.h"

#define SCROLLBAR_SIZE              16.0f
#define SCROLLBAR_SIZEHALF          8.0f
#define SCROLLBUTTON_ARROWPADDING   5.0f


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcScrollBar( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
