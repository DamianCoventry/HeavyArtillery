//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 16 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGPLAYER_H_
#define _DIALOGPLAYER_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogPlayer;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogPlayerCreate( void );

void SetBotType( size_t BotType );


#endif
