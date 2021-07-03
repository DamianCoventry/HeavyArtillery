//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGLWO_H_
#define _DIALOGLWO_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct DialogObject_s
{
    float Scale;
    size_t ObjectID;
}
DialogObject_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcLWO( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );


#endif
