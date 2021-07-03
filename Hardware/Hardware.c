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

#define HARDWARECONFIGFILE          "Hardware.bin"

#include <stdio.h>

#include "Hardware.h"
#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "Timer.h"

#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

HardwareConfig_t g_HWConfig;
HINSTANCE g_hInstance;


//----------------------------------------------------------------------------------------
// Name: LoadHardwareConfig()
// Desc:
//----------------------------------------------------------------------------------------

size_t LoadHardwareConfig(void)
{
    FILE* File;
    errno_t Error;

    LogWrite("LoadHardwareConfig() loading [%s]...\n", HARDWARECONFIGFILE);

    Error = fopen_s(&File, HARDWARECONFIGFILE, "rb");
    if (Error != 0)
    {
        LogWrite("LoadHardwareConfig() failed to open/find [%s], will create it instead\n", HARDWARECONFIGFILE);
        return 0;
    }

    if (1 != fread(&g_HWConfig, sizeof(HardwareConfig_t), 1, File))
    {
        fclose(File);
        return 0;
    }

    fclose(File);

    LogWrite("LoadHardwareConfig() succeeded\n");
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: SaveHardwareConfig()
// Desc:
//----------------------------------------------------------------------------------------

void SaveHardwareConfig(void)
{
    FILE* File;
    errno_t Error;

    LogWrite("SaveHardwareConfig() saving...\n");

    Error = fopen_s(&File, HARDWARECONFIGFILE, "wb");
    if (Error == 0)
    {
        fwrite(&g_HWConfig, sizeof(HardwareConfig_t), 1, File);
        fclose(File);
    }

    LogWrite("SaveHardwareConfig() succeeded\n");
}


//----------------------------------------------------------------------------------------
// Name: SetHardwareDefaults()
// Desc:
//----------------------------------------------------------------------------------------

void SetHardwareDefaults(void)
{
    LogWrite("SetHardwareDefaults() setting defaults...\n");

    g_HWConfig.GfxWidth    = 640;
    g_HWConfig.GfxHeight   = 480;
    g_HWConfig.GfxBPP      = 32;

    g_HWConfig.GfxDepthBits    = 24;
    g_HWConfig.GfxRefreshRate  = 60;
    g_HWConfig.GfxFullScreen   = 0;

    g_HWConfig.SndChannels         = 1;
    g_HWConfig.SndBitsPerSample    = 8;
    g_HWConfig.SndSamplesPerSecond = 11025;

    g_HWConfig.GLVFOV   =    60.0f;
    g_HWConfig.GLNCP2D  = -1000.0f;
    g_HWConfig.GLFCP2D  =  1000.0f;
    g_HWConfig.GLNCP3D  =     1.0f;
    g_HWConfig.GLFCP3D  = 40960.0f;

    LogWrite("SetHardwareDefaults() succeeded\n");
}


//----------------------------------------------------------------------------------------
// Name: InitialiseHardware()
// Desc:
//----------------------------------------------------------------------------------------

size_t InitialiseHardware(void)
{
    LogWrite("InitialiseHardware() initialising...\n");

    if (!StartGraphicsDevice())
        return 0;

    if (!StartInputDevices())
        return 0;

    StartSoundDevice();
    if (!InitialiseTimer())
        return 0;

    LogWrite("InitialiseHardware() succeeded\n");
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: ShutdownHardware()
// Desc:
//----------------------------------------------------------------------------------------

void ShutdownHardware(void)
{
    LogWrite("ShutdownHardware() shutting down...\n");

    StopSoundDevice();
    StopInputDevices();
    StopGraphicsDevice();

    LogWrite("ShutdownHardware() succeeded\n");
}
