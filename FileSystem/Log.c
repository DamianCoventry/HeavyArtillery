//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Thursday, 13 June 2002
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static FILE *m_File;


//-----------------------------------------------------------------------------
// Name: LogOpen()
// Desc: 
//-----------------------------------------------------------------------------

void LogOpen( void )
{
    time_t Time;
    struct tm Today;
    errno_t Error;

    Error = fopen_s( &m_File, "Debug.txt", "wt" );    // this'll overwrite any previous logs
    if( Error == 0 )
    {
        time( &Time );
        localtime_s( &Today, &Time );

        char Buffer[128];
        strftime( Buffer, 128, "%A, %d %B %Y, %H:%M:%S\n", &Today );
        fprintf( m_File, Buffer );

        fclose( m_File );
        fopen_s( &m_File, "Debug.txt", "at" );
    }
}


//-----------------------------------------------------------------------------
// Name: LogWrite()
// Desc: 
//-----------------------------------------------------------------------------

void LogWrite( const char *String, ... )
{
    va_list List;

    if( m_File )
    {
        va_start( List, String );
        vfprintf( m_File, String, List );
        va_end( List );

        fflush( m_File );
    }
}


//-----------------------------------------------------------------------------
// Name: LogClose()
// Desc: 
//-----------------------------------------------------------------------------

void LogClose( void )
{
    if( m_File )
    {
        fflush( m_File );
        fclose( m_File );
        m_File = nullptr;
    }
}
