//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>


extern size_t StartSoundDevice( void );
extern void StopSoundDevice( void );

extern size_t IsSoundAvailable( void );

extern IDirectSound3DBuffer *CreateSoundBuffer( WAVEFORMATEX *Format, size_t DataLength,
                                                unsigned char *Data, IDirectSoundBuffer **DSBuffer );

extern size_t DuplicateSoundBuffers( IDirectSoundBuffer **DSBuffers, IDirectSound3DBuffer **DS3DBuffers,
                                  size_t BufferCount );

extern void SetSoundListenerPosition( float PositionX, float PositionY, float PositionZ
                                    , float ForwardX, float ForwardY, float ForwardZ
                                    , float UpX, float UpY, float UpZ );
