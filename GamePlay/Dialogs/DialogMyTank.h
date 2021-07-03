//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 10 June 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGMYTANK_H_
#define _DIALOGMYTANK_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogMyTank;
extern size_t g_MyTankVisible;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogMyTankCreate( void );

void DialogUpdateMyTank( Dialog_t *Dialog );


#endif
