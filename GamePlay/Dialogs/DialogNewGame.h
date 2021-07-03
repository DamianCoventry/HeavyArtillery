//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 16 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGNEWGAME_H_
#define _DIALOGNEWGAME_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogNewGame;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogNewGameCreate( void );

void EnableDisableButtons( Dialog_t *Dialog );


#endif
