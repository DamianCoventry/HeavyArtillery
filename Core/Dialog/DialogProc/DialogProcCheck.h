//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGCHECK_H_
#define _DIALOGCHECK_H_

#include "Dialog.h"

#define CHECKBOX_SIZE               12
#define CHECKBOX_SIZEHALF           6
#define CHECKBOX_SIZEFOCUS          16
#define CHECKBOX_SIZEFOCUSHALF      8
#define CHECKBOX_CHECKHALFSIZE      4


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct CheckBox_s
{
    float OffsetX, OffsetY;
    size_t Group;
    size_t Checked;
}
CheckBox_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogDialogItemChecked( Dialog_t *Dialog );
void DialogCheckDialogItem( Dialog_t *Dialog, size_t Checked );

size_t DialogProcCheck( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
