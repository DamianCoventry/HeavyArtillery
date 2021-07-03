//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#ifndef _STATETYPES_H_
#define _STATETYPES_H_


//----------------------------------------------------------------------------------------
// public structures
//----------------------------------------------------------------------------------------

typedef struct State_s
{
    void (*Constructor)( void );
    void (*Destructor)( void );
    void (*Poll)( void );
}
State_t;


#endif
