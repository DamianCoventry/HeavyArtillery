//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 9 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGCAMERA_H_
#define _DIALOGCAMERA_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t *DialogCamera;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogCameraCreate( void );

size_t DialogProcCameraMove( struct Dialog_s *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcCameraShoot( struct Dialog_s *Dialog, size_t Msg, void* Param1, void* Param2 );


#endif
