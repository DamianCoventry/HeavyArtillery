//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 9 August 2002
//
//========================================================================================


#include "stdafx.h"
#include "GameMusic.h"
#include "MusicManager.h"
#include "LoadingScreen.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

size_t MusicAmbient0;
size_t MusicAmbient1;
size_t MusicAmbient2;


//-----------------------------------------------------------------------------
// Name: CreateMusic()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateMusic( void )
{
    LogWrite( "CreateMusic() loading...\n" );
    BeginLoadingScreen( TextureGame, "Loading Music...", 0, 3 );

    // it's ok if the music fails to load, the game can still be played

    MusicAmbient0 = LoadMusic( "MP3/GameMusic0.mp3" ); UpdateLoadingScreen( 1 );
    MusicAmbient1 = LoadMusic( "MP3/GameMusic1.mp3" ); UpdateLoadingScreen( 2 );
    MusicAmbient2 = LoadMusic( "MP3/GameMusic2.mp3" );

    EndLoadingScreen( "Loading Music...done", 3 );
    LogWrite( "CreateMusic() loaded\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteMusic()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteMusic( void )
{
    LogWrite( "DeleteMusic() unloading...\n" );

    UnloadMusic( MusicAmbient2 );
    UnloadMusic( MusicAmbient1 );
    UnloadMusic( MusicAmbient0 );

    LogWrite( "DeleteMusic() unloaded\n" );
}
