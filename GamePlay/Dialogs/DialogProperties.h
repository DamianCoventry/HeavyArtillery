//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 13 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGPROPERTIES_H_
#define _DIALOGPROPERTIES_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogProperties;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogPropertiesCreate( void );

void PropertiesUpdate( Dialog_t *Dialog );

size_t AutoDefenseSetFirstPlayer( void );
size_t AutoDefenseSetNextPlayer( void );

#endif
