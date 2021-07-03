//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#ifndef _HARDWARETYPES_H_
#define _HARDWARETYPES_H_

//----------------------------------------------------------------------------------------
// public structures
//----------------------------------------------------------------------------------------

typedef struct HardwareConfig_s
{
    size_t GfxWidth, GfxHeight;
    size_t GfxBPP, GfxDepthBits;
    size_t GfxFullScreen;
    size_t GfxRefreshRate;

    size_t SndChannels;
    size_t SndBitsPerSample;
    size_t SndSamplesPerSecond;

    float GLVFOV;
    float GLNCP2D, GLFCP2D;
    float GLNCP3D, GLFCP3D;
}
HardwareConfig_t;


#endif
