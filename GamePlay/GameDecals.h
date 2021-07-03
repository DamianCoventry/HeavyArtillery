//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#ifndef _GAMEDECALS_H_
#define _GAMEDECALS_H_


#include "DecalStructs.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern DecalTexture_t g_DecalBlastTiny;
extern DecalTexture_t g_DecalBlastSmall;
extern DecalTexture_t g_DecalBlastMedium;
extern DecalTexture_t g_DecalBlastLarge;
extern DecalTexture_t g_DecalBlastHuge;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

extern size_t CreateDecals( void );
extern void DeleteDecals( void );


#endif
