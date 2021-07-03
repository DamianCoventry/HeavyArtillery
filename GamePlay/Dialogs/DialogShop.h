//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Sunday, 7 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGSHOP_H_
#define _DIALOGSHOP_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogShop;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogShopCreate( void );

size_t SetFirstShopPlayer( void );
size_t SetNextShopPlayer( void );


#endif
