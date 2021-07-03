//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>

#define MAXCHARACTERS           100
#define MAXSomethingS               128

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXSomethingS || !_SomethingPool[ _id ].Used )


//----------------------------------------------------------------------------------------
// private structures
//----------------------------------------------------------------------------------------

typedef struct Something_s
{
    size_t Used;
}
Something_t;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _SomethingIndex = 0;
static Something_t _SomethingPool[ MAXSomethingS ];


//----------------------------------------------------------------------------------------
// Name: GetFreeSomething()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t GetFreeSomething( void )
{
    size_t i;
    size_t ID;

    i = _SomethingIndex;

    while( _SomethingPool[ i ].Used )
    {
        i++;
        if( i >= MAXSomethingS )
            i = 0;

        if( i == _SomethingIndex )
            return -1;        // pool is full
    }

    _SomethingPool[ i ].Used = 1;
    ID = i;

    _SomethingIndex = i;
    if( ++_SomethingIndex >= MAXSomethingS )
        _SomethingIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnSomething()
// Desc: 
//----------------------------------------------------------------------------------------

static void ReturnSomething( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _SomethingPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: CreateSomething()
// Desc: 
//----------------------------------------------------------------------------------------

size_t CreateSomething( const std::string& FileName )
{
    size_t ID;

    ID = GetFreeSomething();
    if( -1 == ID )
        return -1;

    // ...load it

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: DeleteSomething()
// Desc: 
//----------------------------------------------------------------------------------------

void DeleteSomething( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    // .. delete it

    ReturnSomething( ID );
}


//----------------------------------------------------------------------------------------
// Name: ReloadSomethingSkin()
// Desc: 
//----------------------------------------------------------------------------------------

size_t ReloadSomethingSkin( size_t ID )
{
    if( INVALIDID( ID ) )
        return -1;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReloadAllSomethingSkins()
// Desc: 
//----------------------------------------------------------------------------------------

void ReloadAllSomethingSkins( void )
{
    size_t i;

    for( i = 0; i < MAXSomethingS; i++ )
    {
        if( _SomethingPool[ i ].Used )
            ReloadSomethingSkin( i );
    }
}
