//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 11 July 2002
//
//========================================================================================


#ifndef _EXPLOSIONTYPES_H_
#define _EXPLOSIONTYPES_H_

#include "DecalStructs.h"

typedef struct ExplosionType_s
{
    float Damage;
    float Radius;

    float FrameWidth, FrameHeight;

    float BaseScale;
    size_t FrameThreshold;

    long CameraShakeFrameCount;
    long CameraShakeAmount;

    DecalTexture_t *DecalTexture;
}
ExplosionType_t;


#endif
