//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 26 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "Sound.h"
#include "SoundManager.h"
#include "Wave.h"

#define MAXSOUNDBUFFERS         8
#define MAXSOUNDS               64

#define MAXACTIVESOUNDS         32

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXSOUNDS || !_SoundPool[ _id ].Used )


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct Sound_s
{
    size_t Used;
    size_t CurrentSound;
    IDirectSoundBuffer *DSBuffers[ MAXSOUNDBUFFERS ];
    IDirectSound3DBuffer *DS3DBuffers[ MAXSOUNDBUFFERS ];
}
Sound_t;

typedef struct ActiveSound_s
{
    size_t Used;
    size_t PoolID;
    size_t SoundID;
    size_t BufferIndex;

    struct ActiveSound_s *Next;
    struct ActiveSound_s *Prev;
}
ActiveSound_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t _SoundIndex = 0;
static Sound_t _SoundPool[ MAXSOUNDS ];

static size_t _ActiveSoundIndex = 0;
static ActiveSound_t _ActiveSoundPool[ MAXACTIVESOUNDS ];

static ActiveSound_t *_ActiveSoundHead = nullptr;
static ActiveSound_t *_ActiveSoundTail = nullptr;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t AppendSound( size_t SoundID, size_t BufferIndex );
static void RemoveSound( ActiveSound_t *ActiveSound );


//----------------------------------------------------------------------------------------
// Name: GetFreeSound()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t GetFreeSound( void )
{
    size_t i;
    size_t ID;

    i = _SoundIndex;

    while( _SoundPool[ i ].Used )
    {
        i++;
        if( i >= MAXSOUNDS )
            i = 0;

        if( i == _SoundIndex )
            return -1;        // pool is full
    }

    _SoundPool[ i ].Used = 1;
    ID = i;

    _SoundIndex = i;
    if( ++_SoundIndex >= MAXSOUNDS )
        _SoundIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnSound()
// Desc: 
//----------------------------------------------------------------------------------------

static void ReturnSound( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _SoundPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: GetFreeActiveSound()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t GetFreeActiveSound( void )
{
    size_t i;
    size_t ID;

    i = _ActiveSoundIndex;

    while( _ActiveSoundPool[ i ].Used )
    {
        i++;
        if( i >= MAXACTIVESOUNDS )
            i = 0;

        if( i == _ActiveSoundIndex )
            return -1;        // pool is full
    }

    _ActiveSoundPool[ i ].Used = 1;
    ID = i;

    _ActiveSoundIndex = i;
    if( ++_ActiveSoundIndex >= MAXACTIVESOUNDS )
        _ActiveSoundIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnActiveSound()
// Desc: 
//----------------------------------------------------------------------------------------

static void ReturnActiveSound( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ActiveSoundPool[ ID ].Used = 0;
}

//----------------------------------------------------------------------------------------
// Name: CreateSound()
// Desc: 
//----------------------------------------------------------------------------------------

size_t CreateSound( const std::string& FileName, float Min, float Max )
{
    Wave_t *Wave;
    size_t ID;
    size_t i;

    if( !IsSoundAvailable() )
        return -1;

    Wave = WaveLoad( FileName );
    if( nullptr == Wave )
        return -1;

    // should really check the wave's format here!

    ID = GetFreeSound();
    if( -1 == ID )
        return -1;

    _SoundPool[ ID ].DS3DBuffers[ 0 ] = CreateSoundBuffer( &Wave->Format, Wave->DataLength, Wave->Data, &_SoundPool[ ID ].DSBuffers[ 0 ] );

    WaveUnload( Wave );

    if( nullptr == _SoundPool[ ID ].DSBuffers[ 0 ] )
    {
        ReturnSound( ID );
        return -1;
    }

    if( !DuplicateSoundBuffers( _SoundPool[ ID ].DSBuffers, _SoundPool[ ID ].DS3DBuffers, MAXSOUNDBUFFERS ) )
    {
        DeleteSound( ID );
        return -1;
    }

    for( i = 0; i < MAXSOUNDBUFFERS; i++ )
    {
        _SoundPool[ ID ].DS3DBuffers[ i ]->SetMinDistance( Min, DS3D_IMMEDIATE );
        _SoundPool[ ID ].DS3DBuffers[ i ]->SetMaxDistance( Max, DS3D_IMMEDIATE );
        _SoundPool[ ID ].DS3DBuffers[ i ]->SetPosition( 0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE );

        // we'll add sounds to an active list, and set the mode to DS3DMODE_NORMAL while the sound is in the list.  we'll disable 3D
        // processing on the sound when it's not being played (ie. not in the active list) 
        _SoundPool[ ID ].DS3DBuffers[ i ]->SetMode( DS3DMODE_DISABLE, DS3D_IMMEDIATE );
    }

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: DeleteSound()
// Desc: 
//----------------------------------------------------------------------------------------

void DeleteSound( size_t ID )
{
    size_t i;

    if( INVALIDID( ID ) )
        return;

    for( i = 0; i < MAXSOUNDBUFFERS; i++ )
    {
        if( _SoundPool[ ID ].DS3DBuffers[ i ] )
            _SoundPool[ ID ].DS3DBuffers[ i ]->Release();

        if( _SoundPool[ ID ].DSBuffers[ i ] )
            _SoundPool[ ID ].DSBuffers[ i ]->Release();
    }

    ReturnSound( ID );
}


//----------------------------------------------------------------------------------------
// Name: PlaySound2D()
// Desc: 
//----------------------------------------------------------------------------------------

void PlaySound2D( size_t ID, size_t PlayLooped )
{
    if( !IsSoundAvailable() || INVALIDID( ID ) )
        return;

    // disable 3D sound processing for this sound
    _SoundPool[ ID ].DS3DBuffers[ _SoundPool[ ID ].CurrentSound ]->SetMode( DS3DMODE_DISABLE, DS3D_IMMEDIATE );

    _SoundPool[ ID ].DSBuffers[ _SoundPool[ ID ].CurrentSound ]->Play( 0, 0, PlayLooped ? DSBPLAY_LOOPING : 0 );

    if( ++_SoundPool[ ID ].CurrentSound >= MAXSOUNDBUFFERS )
        _SoundPool[ ID ].CurrentSound = 0;
}


//----------------------------------------------------------------------------------------
// Name: PlaySound3D()
// Desc: 
//----------------------------------------------------------------------------------------

void PlaySound3D( size_t ID, size_t PlayLooped )
{
    if( !IsSoundAvailable() || INVALIDID( ID ) )
        return;

    // add this sound to the active list.  when this sound is finished playing it will be removed
    // from the list and have its 3D sound processing disabled.
    if( -1 != AppendSound( ID, _SoundPool[ ID ].CurrentSound ) )
    {
        // enable 3D sound processing for this sound
        _SoundPool[ ID ].DS3DBuffers[ _SoundPool[ ID ].CurrentSound ]->SetMode( DS3DMODE_NORMAL, DS3D_IMMEDIATE );

        _SoundPool[ ID ].DSBuffers[ _SoundPool[ ID ].CurrentSound ]->Play( 0, 0, PlayLooped ? DSBPLAY_LOOPING : 0 );

        if( ++_SoundPool[ ID ].CurrentSound >= MAXSOUNDBUFFERS )
            _SoundPool[ ID ].CurrentSound = 0;
    }
}


//----------------------------------------------------------------------------------------
// Name: StopSound()
// Desc: 
//----------------------------------------------------------------------------------------

void StopSound( size_t ID )
{
    size_t i;

    if( !IsSoundAvailable() || INVALIDID( ID ) )
        return;

    for( i = 0; i < MAXSOUNDBUFFERS; i++ )
        _SoundPool[ ID ].DSBuffers[ i ]->Stop();
}


//----------------------------------------------------------------------------------------
// Name: StopAllSounds()
// Desc: 
//----------------------------------------------------------------------------------------

void StopAllSounds( void )
{
    size_t i, SndIdx;

    for( SndIdx = 0; SndIdx < MAXSOUNDS; SndIdx++ )
    {
        if( _SoundPool[ SndIdx ].Used )
        {
            for( i = 0; i < MAXSOUNDBUFFERS; i++ )
                _SoundPool[ SndIdx ].DSBuffers[ i ]->Stop();
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: IsSoundPlaying()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IsSoundPlaying( size_t ID )
{
    size_t i;
    DWORD Status;

    if( !IsSoundAvailable() || INVALIDID( ID ) )
        return 0;

    for( i = 0; i < MAXSOUNDBUFFERS; i++ )
    {
        _SoundPool[ ID ].DSBuffers[ i ]->GetStatus( &Status );
        if( Status & DSBSTATUS_PLAYING )
            return 1;
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: SetSoundPosition()
// Desc: 
//----------------------------------------------------------------------------------------

size_t SetSoundPosition( size_t ID, float X, float Y, float Z )
{
    size_t i;

    if( !IsSoundAvailable() || INVALIDID( ID ) )
        return 0;

    for( i = 0; i < MAXSOUNDBUFFERS; i++ )
        _SoundPool[ ID ].DS3DBuffers[ i ]->SetPosition( X, Y, -Z        // flip it coz ogl is right handed
                                                               , DS3D_IMMEDIATE );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: AppendSound()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t AppendSound( size_t SoundID, size_t BufferIndex )
{
    size_t ID;
    ActiveSound_t *NewSound;

    ID = GetFreeActiveSound();
    if( -1 == ID )
        return -1;

    NewSound = &_ActiveSoundPool[ ID ];

    NewSound->PoolID        = ID;
    NewSound->SoundID       = SoundID;
    NewSound->BufferIndex   = BufferIndex;
    NewSound->Next = NewSound->Prev = nullptr;

    if( nullptr == _ActiveSoundHead )
    {
        _ActiveSoundHead = NewSound;
        _ActiveSoundTail = NewSound;
    }
    else
    {
        _ActiveSoundTail->Next = NewSound;
        NewSound->Prev = _ActiveSoundTail;

        _ActiveSoundTail = NewSound;
    }

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: RemoveSound()
// Desc: 
//----------------------------------------------------------------------------------------

static void RemoveSound( ActiveSound_t *ActiveSound )
{
    Sound_t *Sound;

    if( ActiveSound ==_ActiveSoundHead )
    {
        if( ActiveSound ==_ActiveSoundTail )
        {
            _ActiveSoundHead = nullptr;
            _ActiveSoundTail = nullptr;
        }
        else
        {
            _ActiveSoundHead =_ActiveSoundHead->Next;
            _ActiveSoundHead->Prev = nullptr;
        }
    }
    else if( ActiveSound ==_ActiveSoundTail )
    {
        _ActiveSoundTail =_ActiveSoundTail->Prev;
        _ActiveSoundTail->Next = nullptr;
    }
    else
    {
        ActiveSound->Next->Prev = ActiveSound->Prev;
        ActiveSound->Prev->Next = ActiveSound->Next;
    }

    ActiveSound->Next = nullptr;
    ActiveSound->Prev = nullptr;
    ReturnActiveSound( ActiveSound->PoolID );

    Sound = &_SoundPool[ ActiveSound->SoundID ];

    // disable 3D sound processing for this sound
    Sound->DS3DBuffers[ ActiveSound->BufferIndex ]->SetMode( DS3DMODE_DISABLE, DS3D_IMMEDIATE );
}


//----------------------------------------------------------------------------------------
// Name: UpdateSounds()
// Desc: 
//----------------------------------------------------------------------------------------

void UpdateSounds( void )
{
    ActiveSound_t *Current;
    ActiveSound_t *Previous;
    Sound_t *Sound;
    DWORD Status;

    Current = _ActiveSoundHead;
    Previous = nullptr;

    while( Current )
    {
        Sound = &_SoundPool[ Current->SoundID ];

        Sound->DSBuffers[ Current->BufferIndex ]->GetStatus( &Status );

        // if this sound has finished playing then remove it from the list and disable its 3D sound processing
        if( !( Status & DSBSTATUS_PLAYING ) )
        {
            RemoveSound( Current );

            if( Previous )
                Current = Previous->Next;
            else
                Current = _ActiveSoundHead;
        }
        else
        {
            Previous = Current;
            Current = Current->Next;
        }
    }
}
