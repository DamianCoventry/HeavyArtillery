//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "ExplosionTypes.h"
#include "GameDecals.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

ExplosionType_t g_ExplosionTiny;
ExplosionType_t g_ExplosionSmall;
ExplosionType_t g_ExplosionMedium;
ExplosionType_t g_ExplosionLarge;
ExplosionType_t g_ExplosionHuge;
ExplosionType_t g_ExplosionPlayer;


//-----------------------------------------------------------------------------
// Name: CreateExplosions()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateExplosions( void )
{
    LogWrite( "CreateExplosions() creating...\n" );

    g_ExplosionTiny.Damage          = 10.0f;
    g_ExplosionTiny.Radius          = 50.0f;
    g_ExplosionTiny.FrameThreshold  = 100;
    g_ExplosionTiny.BaseScale       = 0.1f;
    g_ExplosionTiny.FrameWidth      = 128.0f;
    g_ExplosionTiny.FrameHeight     = 128.0f;
    g_ExplosionTiny.DecalTexture    = &g_DecalBlastTiny;
    g_ExplosionTiny.CameraShakeAmount       = 10;
    g_ExplosionTiny.CameraShakeFrameCount   = 10;

    g_ExplosionSmall.Damage         =  20.0f;
    g_ExplosionSmall.Radius         = 100.0f;
    g_ExplosionSmall.FrameThreshold = 100;
    g_ExplosionSmall.BaseScale      = 2.0f;
    g_ExplosionSmall.FrameWidth     = 256.0f;
    g_ExplosionSmall.FrameHeight    = 256.0f;
    g_ExplosionSmall.DecalTexture   = &g_DecalBlastSmall;
    g_ExplosionSmall.CameraShakeAmount       = 15;
    g_ExplosionSmall.CameraShakeFrameCount   = 15;

    g_ExplosionMedium.Damage            =  40.0f;
    g_ExplosionMedium.Radius            = 250.0f;
    g_ExplosionMedium.FrameThreshold    = 100;
    g_ExplosionMedium.BaseScale         = 5.0f;
    g_ExplosionMedium.FrameWidth        = 512.0f;
    g_ExplosionMedium.FrameHeight       = 512.0f;
    g_ExplosionMedium.DecalTexture      = &g_DecalBlastMedium;
    g_ExplosionMedium.CameraShakeAmount       = 20;
    g_ExplosionMedium.CameraShakeFrameCount   = 20;

    g_ExplosionLarge.Damage         = 60.0f;
    g_ExplosionLarge.Radius         = 400.0f;
    g_ExplosionLarge.FrameThreshold = 100;
    g_ExplosionLarge.BaseScale      = 10.0f;
    g_ExplosionLarge.FrameWidth     = 1024.0f;
    g_ExplosionLarge.FrameHeight    = 1024.0f;
    g_ExplosionLarge.DecalTexture   = &g_DecalBlastLarge;
    g_ExplosionLarge.CameraShakeAmount       = 25;
    g_ExplosionLarge.CameraShakeFrameCount   = 25;

    g_ExplosionHuge.Damage          = 80.0f;
    g_ExplosionHuge.Radius          = 550.0f;
    g_ExplosionHuge.FrameThreshold  = 100;
    g_ExplosionHuge.BaseScale       = 20.0f;
    g_ExplosionHuge.FrameWidth      = 2048.0f;
    g_ExplosionHuge.FrameHeight     = 2048.0f;
    g_ExplosionHuge.DecalTexture    = &g_DecalBlastHuge;
    g_ExplosionHuge.CameraShakeAmount       = 30;
    g_ExplosionHuge.CameraShakeFrameCount   = 30;

    g_ExplosionPlayer.Damage            = 70.0f;
    g_ExplosionPlayer.Radius            = 300.0f;
    g_ExplosionPlayer.FrameThreshold    = 100;
    g_ExplosionPlayer.BaseScale         = 15.0f;
    g_ExplosionPlayer.FrameWidth        = 1536.0f;
    g_ExplosionPlayer.FrameHeight       = 1536.0f;
    g_ExplosionPlayer.DecalTexture      = &g_DecalBlastLarge;
    g_ExplosionTiny.CameraShakeAmount       = 22;
    g_ExplosionTiny.CameraShakeFrameCount   = 22;

    LogWrite( "CreateExplosions() created\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteExplosions()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteExplosions( void )
{
    LogWrite( "DeleteExplosions() deleting...\n" );

    LogWrite( "DeleteExplosions() deleted\n" );
}
