//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Log.h"
#include "Timer.h"


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static LARGE_INTEGER _ActualTicksPerSecond;
static LARGE_INTEGER _TicksRightNow;

static LARGE_INTEGER _LastFPSUpdateTime;
static long _FPSFrameCount;
static long _FPS;

static LONGLONG _DesiredFrameDeltaTicks;
static LONGLONG _LastFrameTicks;

float FrameScale;
LONGLONG FrameNowMs;


//----------------------------------------------------------------------------------------
// Name: InitialiseTimer()
// Desc:
//----------------------------------------------------------------------------------------

int InitialiseTimer(void)
{
    if (!QueryPerformanceFrequency(&_ActualTicksPerSecond))
    {
        LogWrite("InitialiseTimer() Unable to get to the HRPC frequency [%d]\n", GetLastError());
        return 0;
    }

    StampInitialTime();

    _DesiredFrameDeltaTicks = _ActualTicksPerSecond.QuadPart / 60;   // Desired FPS is 60

    _FPSFrameCount = 0;
    _FPS = 0;
    _LastFrameTicks = 0;
    FrameNowMs = 0;
    FrameScale = 0.0f;

    LogWrite("InitialiseTimer() timer frequency is [%I64d]\n", _ActualTicksPerSecond.QuadPart);
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StampInitialTime()
// Desc:
//----------------------------------------------------------------------------------------

void StampInitialTime(void)
{
    QueryPerformanceCounter(&_TicksRightNow);
    _LastFPSUpdateTime = _TicksRightNow;
}


//----------------------------------------------------------------------------------------
// Name: StampLoopTime()
// Desc:
//----------------------------------------------------------------------------------------

void StampLoopTime(void)
{
    QueryPerformanceCounter(&_TicksRightNow);

    FrameNowMs = _TicksRightNow.QuadPart / (_ActualTicksPerSecond.QuadPart / 1000);
}


//----------------------------------------------------------------------------------------
// Name: IsTimeForFrame()
// Desc:
//----------------------------------------------------------------------------------------

bool IsTimeForFrame()
{
    return _TicksRightNow.QuadPart - _LastFrameTicks >= _DesiredFrameDeltaTicks;
}


//----------------------------------------------------------------------------------------
// Name: BeginFrame()
// Desc:
//----------------------------------------------------------------------------------------

void BeginFrame(void)
{
    LONGLONG deltaTicks = _TicksRightNow.QuadPart - _LastFrameTicks;

    FrameScale = float(deltaTicks) / float(_ActualTicksPerSecond.QuadPart);
}


//----------------------------------------------------------------------------------------
// Name: EndFrame()
// Desc:
//----------------------------------------------------------------------------------------

void EndFrame(void)
{
    // Save when this frame happened.
    _LastFrameTicks = _TicksRightNow.QuadPart;

    ++_FPSFrameCount;

    // Save the FPS count and when we last updated it
    if (_TicksRightNow.QuadPart - _LastFPSUpdateTime.QuadPart > _ActualTicksPerSecond.QuadPart)
    {
        _LastFPSUpdateTime = _TicksRightNow;
        _FPS = _FPSFrameCount;
        _FPSFrameCount = 0;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetFPS()
// Desc:
//----------------------------------------------------------------------------------------

extern std::string GetFPS(void)
{
    return std::to_string(_FPS);
}


//----------------------------------------------------------------------------------------
// Name: GetNowMs()
// Desc:
//----------------------------------------------------------------------------------------

LONGLONG GetNowMs()
{
    LARGE_INTEGER nowTicks;
    QueryPerformanceCounter(&nowTicks);
    return nowTicks.QuadPart / (_ActualTicksPerSecond.QuadPart / 1000);
}
