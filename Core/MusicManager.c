//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 9 August 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include <string.h>
#include <stdio.h>

#include <strmif.h>
#include <uuids.h>
#include <control.h>
#include <evcode.h>

#include "MusicManager.h"
#include "Error.h"

#define MAXMUSIC                8

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXMUSIC || !_MusicPool[ _id ].Used )


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern HWND g_hMainWindow;


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct Music_s
{
    size_t Used;
    IGraphBuilder* GraphBuilder;
    IMediaControl* MediaControl;
    IMediaEventEx* MediaEvent;
    IMediaPosition* MediaPosition;
    IBasicAudio* BasicAudio;
}
Music_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t _IsMusicEnabled = 1;

static size_t _CurrentMusicTrack = -1;

static size_t _MusicIndex = 0;
static Music_t _MusicPool[ MAXMUSIC ];


//----------------------------------------------------------------------------------------
// Name: GetFreeMusic()
// Desc:
//----------------------------------------------------------------------------------------

static size_t GetFreeMusic(void)
{
    size_t i;
    size_t ID;

    i = _MusicIndex;

    while (_MusicPool[ i ].Used)
    {
        i++;
        if (i >= MAXMUSIC)
            i = 0;

        if (i == _MusicIndex)
            return -1;        // pool is full
    }

    _MusicPool[ i ].Used = 1;
    ID = i;

    _MusicIndex = i;
    if (++_MusicIndex >= MAXMUSIC)
        _MusicIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnMusic()
// Desc:
//----------------------------------------------------------------------------------------

static void ReturnMusic(size_t ID)
{
    if (INVALIDID(ID))
        return;

    _MusicPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: LoadMusic()
// Desc:
//----------------------------------------------------------------------------------------

size_t LoadMusic(const std::string& FileName)
{
    size_t ID;
    WCHAR wFilename[ FILENAME_MAX ];

    if (FileName.empty())
        return -1;

    ID = GetFreeMusic();
    if (-1 == ID)
        return -1;

    // try to create a graphbuilder object
    if (FAILED(CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&_MusicPool[ ID ].GraphBuilder)))
    {
        SetError("LoadMusic() failed to create a DirectShow GraphBuilder object, continuing without music");
        ReturnMusic(ID);
        return -1;
    }

    // get pointers to some useful interfaces
    _MusicPool[ ID ].GraphBuilder->QueryInterface(IID_IMediaControl, (void**)&_MusicPool[ ID ].MediaControl);
    _MusicPool[ ID ].GraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&_MusicPool[ ID ].MediaEvent);
    _MusicPool[ ID ].GraphBuilder->QueryInterface(IID_IMediaPosition, (void**)&_MusicPool[ ID ].MediaPosition);
    _MusicPool[ ID ].GraphBuilder->QueryInterface(IID_IBasicAudio, (void**)&_MusicPool[ ID ].BasicAudio);

    _MusicPool[ ID ].MediaEvent->SetNotifyWindow((OAHWND)g_hMainWindow, WM_GRAPHEVENT, 0);
    _MusicPool[ ID ].MediaEvent->SetNotifyFlags(0);        // turn on notifications

    // load the file
    MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), -1, wFilename, FILENAME_MAX);

    if (FAILED(_MusicPool[ ID ].GraphBuilder->RenderFile(wFilename, nullptr)))
    {
        SetError("LoadMusic() failed to open/read the file [%s], this track will be ignored", FileName);
        UnloadMusic(ID);
        return -1;
    }

    _MusicPool[ ID ].MediaPosition->put_CurrentPosition(0);

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: UnloadMusic()
// Desc:
//----------------------------------------------------------------------------------------

void UnloadMusic(size_t ID)
{
    if (INVALIDID(ID))
        return;

    if (_MusicPool[ ID ].GraphBuilder)
    {
        if (_MusicPool[ ID ].BasicAudio)
        {
            _MusicPool[ ID ].BasicAudio->Release();
            _MusicPool[ ID ].BasicAudio = nullptr;
        }

        if (_MusicPool[ ID ].MediaPosition)
        {
            _MusicPool[ ID ].MediaPosition->Release();
            _MusicPool[ ID ].MediaPosition = nullptr;
        }

        if (_MusicPool[ ID ].MediaEvent)
        {
            _MusicPool[ ID ].MediaEvent->Release();
            _MusicPool[ ID ].MediaEvent = nullptr;
        }

        if (_MusicPool[ ID ].MediaControl)
        {
            _MusicPool[ ID ].MediaControl->Release();
            _MusicPool[ ID ].MediaControl = nullptr;
        }

        _MusicPool[ ID ].GraphBuilder->Release();
        _MusicPool[ ID ].GraphBuilder = nullptr;
    }

    ReturnMusic(ID);
}


//----------------------------------------------------------------------------------------
// Name: PlayMusic()
// Desc:
//----------------------------------------------------------------------------------------

void PlayMusic(size_t ID)
{
    if (INVALIDID(ID))
        return;

    if (!_IsMusicEnabled)
        return;

    if (-1 != _CurrentMusicTrack)
        StopMusic(_CurrentMusicTrack);

    _MusicPool[ ID ].MediaControl->Run();
    _CurrentMusicTrack = ID;
}


//----------------------------------------------------------------------------------------
// Name: StopMusic()
// Desc:
//----------------------------------------------------------------------------------------

void StopMusic(size_t ID)
{
    if (INVALIDID(ID))
    {
        ID = _CurrentMusicTrack;
        if (INVALIDID(ID))
            return;
    }

    _MusicPool[ ID ].MediaControl->Stop();
    _MusicPool[ ID ].MediaPosition->put_CurrentPosition(0);

    _CurrentMusicTrack = -1;
}


//----------------------------------------------------------------------------------------
// Name: PauseMusic()
// Desc:
//----------------------------------------------------------------------------------------

void PauseMusic(size_t ID)
{
    if (INVALIDID(ID))
    {
        ID = _CurrentMusicTrack;
        if (INVALIDID(ID))
            return;
    }

    _MusicPool[ ID ].MediaControl->Pause();
}


//----------------------------------------------------------------------------------------
// Name: ResumeMusic()
// Desc:
//----------------------------------------------------------------------------------------

void ResumeMusic(size_t ID)
{
    if (INVALIDID(ID))
    {
        ID = _CurrentMusicTrack;
        if (INVALIDID(ID))
            return;
    }

    _MusicPool[ ID ].MediaControl->Run();
}


//----------------------------------------------------------------------------------------
// Name: SetMusicVolume()
// Desc:
//----------------------------------------------------------------------------------------

void SetMusicVolume(size_t ID, long Volume)
{
    if (INVALIDID(ID))
        return;

    _MusicPool[ ID ].BasicAudio->put_Volume(Volume);
}


//----------------------------------------------------------------------------------------
// Name: EnableMusic()
// Desc:
//----------------------------------------------------------------------------------------

void EnableMusic(size_t Enable)
{
    _IsMusicEnabled = Enable;
}


//----------------------------------------------------------------------------------------
// Name: OnMusicEvent()
// Desc:
//----------------------------------------------------------------------------------------

void OnMusicEvent(void)
{
    long EventCode;
    LONG_PTR Param1;
    LONG_PTR Param2;

    if (INVALIDID(_CurrentMusicTrack))
        return;

    while (E_ABORT != _MusicPool[ _CurrentMusicTrack ].MediaEvent->GetEvent(&EventCode, &Param1, &Param2, 0))
    {
        switch (EventCode)
        {
        case EC_COMPLETE:
            // then the current track has finished.  we won't actually stop the track,
            // we'll just move the current position back to the start
            _MusicPool[ _CurrentMusicTrack ].MediaPosition->put_CurrentPosition(0);
            break;

        default:
            break;
        }

        _MusicPool[ _CurrentMusicTrack ].MediaEvent->FreeEventParams(EventCode, Param1, Param2);
    }
}
