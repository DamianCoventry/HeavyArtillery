//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 10 June 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGPLAYORDER_H_
#define _DIALOGPLAYORDER_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogPlayOrder;
extern size_t g_PlayOrderVisible;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogPlayOrderCreate( void );

void DialogUpdatePlayOrder( Dialog_t *Dialog );


#endif
