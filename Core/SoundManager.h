//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 26 June 2002
//
//========================================================================================


extern size_t CreateSound( const std::string& FileName, float Min, float Max );
extern void DeleteSound( size_t ID );

extern void PlaySound2D( size_t ID, size_t PlayLooped );
extern void PlaySound3D( size_t ID, size_t PlayLooped );

extern void StopSound( size_t ID );
extern void StopAllSounds( void );

extern size_t IsSoundPlaying( size_t ID );
extern size_t SetSoundPosition( size_t ID, float X, float Y, float Z );

extern void UpdateSounds( void );
