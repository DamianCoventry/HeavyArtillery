//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Thursday, 13 June 2002
//-----------------------------------------------------------------------------


#ifndef _LOG_H_
#define _LOG_H_


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

void LogOpen( void );
void LogWrite( const char *String, ... );
void LogClose( void );


#endif
