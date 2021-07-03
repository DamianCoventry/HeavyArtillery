//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


extern long StartGraphicsDevice( void );
extern void StopGraphicsDevice( void );

extern long SetWindowSize( long Width, long Height, long BPP, long RefreshRate );
extern long SetDisplayMode( long Width, long Height, long BPP, long RefreshRate );
extern void HideAllWindows( void );

extern void ToggleFullScreen( void );

extern void Set3D( void );
extern void Set2D( void );
