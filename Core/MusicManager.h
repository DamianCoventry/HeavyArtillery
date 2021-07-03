//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 9 August 2002
//
//========================================================================================


// define a custom window message for graph events
#define WM_GRAPHEVENT	    WM_USER

#include <string>

extern size_t LoadMusic( const std::string& FileName );
extern void UnloadMusic( size_t ID );

extern void PlayMusic( size_t ID );
extern void StopMusic( size_t ID );

extern void PauseMusic( size_t ID );
extern void ResumeMusic( size_t ID );

extern void SetMusicVolume( size_t ID, long Volume );

extern void EnableMusic( size_t Enable );

extern void OnMusicEvent( void );
