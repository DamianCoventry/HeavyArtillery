//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "StateTypes.h"


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static State_t *_State = 0;


//----------------------------------------------------------------------------------------
// Name: SetGameState()
// Desc: 
//----------------------------------------------------------------------------------------

void SetGameState( State_t *State )
{
    if( _State && _State->Destructor )
        _State->Destructor();

    _State = State;

    if( _State && _State->Constructor )
        _State->Constructor();
}


//----------------------------------------------------------------------------------------
// Name: PollCurrentState()
// Desc: 
//----------------------------------------------------------------------------------------

void PollCurrentState( void )
{
    if( _State && _State->Poll )
        _State->Poll();
}
