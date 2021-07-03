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
#include <objbase.h>
#include <stdio.h>
#include <time.h>
#include <sstream>

#include "Application.h"
#include "Hardware.h"
#include "Graphics.h"
#include "Game.h"
#include "Loop.h"
#include "Error.h"
#include "Log.h"
#include "GameConfig.h"

extern HINSTANCE g_hInstance;


//----------------------------------------------------------------------------------------
// Name: ExitWithError()
// Desc:
//----------------------------------------------------------------------------------------

int ExitWithError(void)
{
    ShutdownHardware();
    HideAllWindows();
    ShowCursor(TRUE);

    std::ostringstream oss;
    oss << "The game was unable to start due to a runtime error.  The "
        << "exact error message reported by the game is as follows:\r\n"
        << "\r\n"
        << GetError() << "\r\n"
        << "\r\n"
        << "Please make sure you have the latest Windows service packs "
        << "installed, are running at least DirectX 8.0 and have installed the lastest "
        << "video card drivers from your hardware manufacture.";

    MessageBox(nullptr, oss.str().c_str(), nullptr, MB_OK | MB_ICONINFORMATION);

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: WinMain()
// Desc:
//----------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    ShowCursor(FALSE);
    LogOpen();

    if (FAILED(CoInitialize(nullptr)))
        return 1;

    srand((unsigned int)time(nullptr));
    g_hInstance = hInstance;

    if (!LoadHardwareConfig())
        SetHardwareDefaults();

    if (!InitialiseApplication())
        return ExitWithError();

    if (!InitialiseHardware())
        return ExitWithError();

    if (!LoadGameConfig())
        SetGameDefaults();

    if (!InitialiseGame())
        return ExitWithError();

    RunGameLoop();

    ShutdownGame();
    ShutdownHardware();
    ShutdownApplication();

    SaveGameConfig();
    SaveHardwareConfig();

    CoUninitialize();
    ShowCursor(TRUE);

    LogWrite("Leaving WinMain()\n");
    LogClose();
    return 1;
}
