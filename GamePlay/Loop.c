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

#include "Input.h"
#include "Timer.h"
#include "Game.h"
#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

size_t g_Active;

extern HDC g_hDC;


//----------------------------------------------------------------------------------------
// Name: RunGameLoop()
// Desc:
//----------------------------------------------------------------------------------------

void RunGameLoop(void)
{
    MSG Msg;

    LogWrite("RunGameLoop() beginning...\n");

    StampInitialTime();

    for (;;)
    {
        StampLoopTime();

        if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == Msg.message)
                break;

            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }

        if (g_Active)
        {
            if (IsTimeForFrame())
            {
                BeginFrame();
                PollInputDevices();

                PollGame();

                StoreInputData();
                SwapBuffers(g_hDC);
                EndFrame();
            }
            Sleep(0);
        }
        else
            WaitMessage();
    }

    LogWrite("RunGameLoop() exiting\n");
}
