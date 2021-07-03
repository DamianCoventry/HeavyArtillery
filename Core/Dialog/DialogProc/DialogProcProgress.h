//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 9 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGPROGRESS_H_
#define _DIALOGPROGRESS_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct ProgressBar_s
{
    ProgressBar_s()
    {
        Low = High = Position = 0;
    }

    size_t Low, High;
    size_t Position;
}
ProgressBar_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcProgress( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
