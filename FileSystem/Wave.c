//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 1 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include "Wave.h"
#include "Error.h"


//-----------------------------------------------------------------------------
// Name: WaveLoad()
// Desc: 
//-----------------------------------------------------------------------------

Wave_t *WaveLoad( const std::string& FileName )
{
    HMMIO hFile;
    MMCKINFO hParent;
    MMCKINFO hChild;
    MMIOINFO hInfo;
    Wave_t *Wave;

    Wave = new Wave_t();

    memset( Wave, 0, sizeof( Wave_t ) );
    memset( &hInfo, 0, sizeof( MMIOINFO ) );

    hParent.ckid         = (FOURCC)0;
    hParent.cksize       = 0;
    hParent.fccType      = (FOURCC)0;
    hParent.dwFlags      = 0;
    hParent.dwDataOffset = 0;

    hChild = hParent;

    hFile = mmioOpen( (LPSTR)FileName.c_str(), &hInfo, MMIO_READ | MMIO_ALLOCBUF );
    if( !hFile )
    {
        SetError( "WaveLoad() failed to open/find the file %s", FileName.c_str() );
        delete Wave;
        return nullptr;
    }

    hParent.fccType = mmioFOURCC( 'W', 'A', 'V', 'E' );

    if( MMSYSERR_NOERROR != mmioDescend( hFile, &hParent, nullptr, MMIO_FINDRIFF ) )
    {
        SetError( "WaveLoad() the file %s is not a valid wave file", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    hChild.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );

    if( MMSYSERR_NOERROR != mmioDescend( hFile, &hChild, &hParent, 0 ) )
    {
        SetError( "WaveLoad() the file %s is not a valid wave file", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    if( sizeof( WAVEFORMATEX ) != mmioRead( hFile, (char *)&Wave->Format, sizeof( WAVEFORMATEX ) ) )
    {
        SetError( "WaveLoad() failed to read the wave data from the file %s", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    if( WAVE_FORMAT_PCM != Wave->Format.wFormatTag )
    {
        SetError( "WaveLoad() the file %s is not in the PCM wave format", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    if( MMSYSERR_NOERROR != mmioAscend( hFile, &hChild, 0 ) )
    {
        SetError( "WaveLoad() the file %s is not a valid wave file", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    hChild.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );

    if( MMSYSERR_NOERROR != mmioDescend( hFile, &hChild, &hParent, MMIO_FINDCHUNK ) )
    {
        SetError( "WaveLoad() the file %s is not a valid wave file", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    Wave->DataLength = hChild.cksize;

    Wave->Data = new unsigned char[hChild.cksize];
    if( nullptr == Wave->Data )
    {
        SetError( "WaveLoad() failed to allocate memory to hold the file %s", FileName.c_str() );
        delete Wave;
        mmioClose( hFile, 0 );
        return nullptr;
    }

    if( -1 == mmioRead( hFile, (char *)Wave->Data, hChild.cksize ) )
    {
        SetError( "WaveLoad() failed to read the wave data from the file %s", FileName.c_str() );
        WaveUnload( Wave );
        mmioClose( hFile, 0 );
        return nullptr;
    }

    mmioClose( hFile, 0 );

    return Wave;
}


//-----------------------------------------------------------------------------
// Name: WaveUnload()
// Desc: 
//-----------------------------------------------------------------------------

void WaveUnload( Wave_t *Wave )
{
    if( Wave )
    {
        if( Wave->Data )
            delete [] Wave->Data;

        delete Wave;
    }
}

