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
#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

#include "Hardware.h"
#include "Error.h"

#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern HWND g_hMainWindow;
extern HINSTANCE g_hInstance;
extern HardwareConfig_t g_HWConfig;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static IDirectSound* _DSound;
static IDirectSoundBuffer* _PrimaryBuffer;
static IDirectSound3DListener* _Listener;


//----------------------------------------------------------------------------------------
// Name: CreatePrimaryBuffer()
// Desc:
//----------------------------------------------------------------------------------------

static size_t CreatePrimaryBuffer(void)
{
    DSBUFFERDESC BufDesc;

    ZeroMemory(&BufDesc, sizeof(DSBUFFERDESC));
    BufDesc.dwSize  = sizeof(DSBUFFERDESC);
    BufDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;

    LogWrite("CreatePrimaryBuffer() creating...\n");

    if (FAILED(_DSound->CreateSoundBuffer(&BufDesc, &_PrimaryBuffer, nullptr)))
        return SetError("CreatePrimaryBuffer() failed to create the primary sound buffer");

    _PrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

    LogWrite("CreatePrimaryBuffer() succeeded\n");

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: CreateListener()
// Desc:
//----------------------------------------------------------------------------------------

static size_t CreateListener(void)
{
    DS3DLISTENER Params;

    LogWrite("CreateListener() calling QueryInterface()...\n");

    if (FAILED(_PrimaryBuffer->QueryInterface(IID_IDirectSound3DListener, (void**)&_Listener)))
        return SetError("CreateListener() failed to an interface to a 3D sound listener");

    Params.dwSize = sizeof(DS3DLISTENER);
    _Listener->GetAllParameters(&Params);

    Params.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;
    Params.flRolloffFactor = DS3D_DEFAULTROLLOFFFACTOR;
    _Listener->SetAllParameters(&Params, DS3D_IMMEDIATE);

    LogWrite("CreateListener() succeeded\n");

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: SetFormat()
// Desc:
//----------------------------------------------------------------------------------------

static size_t SetFormat(void)
{
    WAVEFORMATEX Format;

    LogWrite("SetFormat() setting format....\n");

    ZeroMemory(&Format, sizeof(WAVEFORMATEX));
    Format.wFormatTag = WAVE_FORMAT_PCM;
    Format.nChannels = static_cast<WORD>(g_HWConfig.SndChannels);
    Format.nSamplesPerSec = static_cast<DWORD>(g_HWConfig.SndSamplesPerSecond);
    Format.wBitsPerSample = static_cast<WORD>(g_HWConfig.SndBitsPerSample);
    Format.nBlockAlign = (Format.wBitsPerSample / 8) * Format.nChannels;
    Format.nAvgBytesPerSec = Format.nSamplesPerSec * Format.nBlockAlign;

    if (FAILED(_PrimaryBuffer->SetFormat(&Format)))
        return SetError("SetFormat() failed to set the primary sound buffer's format to %zu, %zu, %zu",
            g_HWConfig.SndChannels, g_HWConfig.SndSamplesPerSecond, g_HWConfig.SndBitsPerSample);

    LogWrite("SetFormat() succeeded\n");

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StartSoundDevice()
// Desc:
//----------------------------------------------------------------------------------------

size_t StartSoundDevice(void)
{
    LogWrite("StartSoundDevice() starting...\n");

    if (FAILED(CoCreateInstance(CLSID_DirectSound, nullptr, CLSCTX_INPROC, IID_IDirectSound, (void**)&_DSound)))
        return SetError("StartSoundDevice() the call to CoCreateInstance() failed, running game with no sound");

    if (FAILED(_DSound->Initialize(nullptr)))
    {
        _DSound->Release();
        _DSound = nullptr;
        return SetError("StartSoundDevice() failed to initialise the main directsound object, running game with no sound");
    }

    if (FAILED(_DSound->SetCooperativeLevel(g_hMainWindow, DSSCL_PRIORITY)))
    {
        _DSound->Release();
        _DSound = nullptr;
        return SetError("StartSoundDevice() failed to set the cooperative level, running game with no sound");
    }

    if (!CreatePrimaryBuffer())
        return 0;

    if (!SetFormat())
        return 0;

    if (!CreateListener())
        return 0;

    LogWrite("StartSoundDevice() succeeded\n");

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StopSoundDevice()
// Desc:
//----------------------------------------------------------------------------------------

void StopSoundDevice(void)
{
    LogWrite("StopSoundDevice() stopping...\n");

    if (_DSound)
    {
        if (_Listener)
        {
            _Listener->Release();
            _Listener = nullptr;
        }

        if (_PrimaryBuffer)
        {
            _PrimaryBuffer->Release();
            _PrimaryBuffer = nullptr;
        }

        _DSound->Release();
        _DSound = nullptr;
    }

    LogWrite("StopSoundDevice() stopped\n");
}


//----------------------------------------------------------------------------------------
// Name: CreateSoundBuffer()
// Desc:
//----------------------------------------------------------------------------------------

IDirectSound3DBuffer* CreateSoundBuffer(WAVEFORMATEX* Format, size_t DataLength, unsigned char* Data, IDirectSoundBuffer** DSBuffer)
{
    DSBUFFERDESC BufDesc;
    IDirectSound3DBuffer* DS3DBuffer;
    unsigned char* DataTemp1;
    unsigned char* DataTemp2;
    unsigned long DataLen1;
    unsigned long DataLen2;

    // setup a buffer desc struct
    memset((DSBUFFERDESC*)&BufDesc, 0, sizeof(DSBUFFERDESC));

    BufDesc.dwSize          = sizeof(DSBUFFERDESC);
    BufDesc.dwFlags         = DSBCAPS_LOCSOFTWARE | DSBCAPS_STATIC | DSBCAPS_CTRL3D;
    BufDesc.dwBufferBytes   = static_cast<DWORD>(DataLength);
    BufDesc.lpwfxFormat     = Format;

    // create the buffer
    if (FAILED(_DSound->CreateSoundBuffer(&BufDesc, DSBuffer, nullptr)))
        return nullptr;

    if (FAILED((*DSBuffer)->QueryInterface(IID_IDirectSound3DBuffer, (void**)&DS3DBuffer)))
    {
        (*DSBuffer)->Release();
        return nullptr;
    }

    DataTemp1 = DataTemp2 = nullptr;
    DataLen1  = DataLen2  = 0;

    // write the bytes to the buffer
    if (FAILED((*DSBuffer)->Lock(0, static_cast<DWORD>(DataLength), (void**)&DataTemp1, &DataLen1,
                                 (void**)&DataTemp2, &DataLen2, DSBLOCK_FROMWRITECURSOR)))
    {
        DS3DBuffer->Release();
        (*DSBuffer)->Release();
        return nullptr;
    }

    memcpy(DataTemp1, Data, DataLen1);
    memcpy(DataTemp2, (Data + DataLen1), DataLen2);

    if (FAILED((*DSBuffer)->Unlock(DataTemp1, DataLen1, DataTemp2, DataLen2)))
    {
        DS3DBuffer->Release();
        (*DSBuffer)->Release();
        return nullptr;
    }

    return DS3DBuffer;
}


//----------------------------------------------------------------------------------------
// Name: DuplicateSoundBuffers()
// Desc:
//----------------------------------------------------------------------------------------

size_t DuplicateSoundBuffers(IDirectSoundBuffer** DSBuffers, IDirectSound3DBuffer** DS3DBuffers, size_t BufferCount)
{
    size_t i;

    for (i = 1; i < BufferCount; i++)
    {
        if (FAILED(_DSound->DuplicateSoundBuffer(DSBuffers[ 0 ], &DSBuffers[ i ])))
            return 0;

        if (FAILED(DSBuffers[ i ]->QueryInterface(IID_IDirectSound3DBuffer, (void**)&DS3DBuffers[ i ])))
            return 0;
    }

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: IsSoundAvailable()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsSoundAvailable(void)
{
    return (nullptr != _DSound);
}


//----------------------------------------------------------------------------------------
// Name: SetSoundListenerPosition()
// Desc:
//----------------------------------------------------------------------------------------

void SetSoundListenerPosition(float PositionX, float PositionY, float PositionZ
                              , float ForwardX, float ForwardY, float ForwardZ
                              , float UpX, float UpY, float UpZ)
{
    DS3DLISTENER Params;

    if (_DSound && _Listener)
    {
        Params.dwSize = sizeof(DS3DLISTENER);
        _Listener->GetAllParameters(&Params);

        Params.vPosition.x = PositionX;
        Params.vPosition.y = PositionY;
        Params.vPosition.z = -PositionZ;        // flip it coz ogl is right handed

        Params.vOrientFront.x = ForwardX;
        Params.vOrientFront.y = ForwardY;
        Params.vOrientFront.z = -ForwardZ;

        Params.vOrientTop.x = UpX;
        Params.vOrientTop.y = UpY;
        Params.vOrientTop.z = -UpZ;

        _Listener->SetAllParameters(&Params, DS3D_IMMEDIATE);
    }
}
