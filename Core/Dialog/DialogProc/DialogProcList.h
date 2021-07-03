//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGLIST_H_
#define _DIALOGLIST_H_

#include "Dialog.h"

#include <string>
#include <vector>


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct List_s
{
    List_s()
    {
        SelectedIndex = FirstIndex = MaxVertStrings = 0;
    }

    size_t SelectedIndex;
    size_t FirstIndex;
    size_t MaxVertStrings;
    std::vector<std::string> Strings;
    std::vector<size_t> Values;
}
List_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogProcList( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 );

#endif
