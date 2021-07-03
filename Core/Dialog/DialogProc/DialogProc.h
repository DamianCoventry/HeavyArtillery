//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGPROC_H_
#define _DIALOGPROC_H_

#include "Dialog.h"


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcButton( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcCheck( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcDefault( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcEdit( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcLabel( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcList( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcScrollBar( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcScrollBox( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcScrollButton( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );
size_t DialogProcStatic( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );


#endif
