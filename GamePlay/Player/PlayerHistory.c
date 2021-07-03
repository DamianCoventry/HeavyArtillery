//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "PlayerHistory.h"


//-----------------------------------------------------------------------------
// Name: GetCurrentTurn()
// Desc: 
//-----------------------------------------------------------------------------

PlayerTurn_t *GetCurrentTurn( PlayerHistory_t *History )
{
    return &History->Turns[ History->Index ];
}


//-----------------------------------------------------------------------------
// Name: GetPreviousTurn()
// Desc: 
//-----------------------------------------------------------------------------

PlayerTurn_t *GetPreviousTurn( PlayerHistory_t *History, size_t TurnsAgo )
{
    size_t Index;
    size_t i;

    Index = History->Index;

    for( i = 0; i < TurnsAgo && i < MAXHISTORYENTRIES; i++ )
    {
        if( --Index < 0 )
            Index = MAXHISTORYENTRIES - 1;
    }

    return &History->Turns[ Index ];
}


//-----------------------------------------------------------------------------
// Name: IncrementHistory()
// Desc: 
//-----------------------------------------------------------------------------

void IncrementHistory( PlayerHistory_t *History )
{
    if( ++History->Index >= MAXHISTORYENTRIES - 1 )
        History->Index = 0;
}
