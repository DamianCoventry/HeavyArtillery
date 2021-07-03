//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 1 April 2002
//-----------------------------------------------------------------------------


#ifndef _WAVE_H_
#define _WAVE_H_


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string>


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct Wave_s
{
    WAVEFORMATEX Format;
    size_t DataLength;
    unsigned char *Data;
}
Wave_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

Wave_t *WaveLoad( const std::string& FileName );
void WaveUnload( Wave_t *Wave );


#endif
