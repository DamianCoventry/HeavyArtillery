//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>

#include "ParticleTypes.h"

#include "ParticleManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"

#include "GameExplosions.h"

#include "ParticleFireball.h"

#include "Object.h"


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _ExplSparksTemplate;

static ParticleEmitter_t _ExplSparksEmitterTiny;
static ParticleEmitter_t _ExplSparksEmitterSmall;
static ParticleEmitter_t _ExplSparksEmitterMedium;
static ParticleEmitter_t _ExplSparksEmitterLarge;
static ParticleEmitter_t _ExplSparksEmitterHuge;
static ParticleEmitter_t _ExplSparksEmitterPlayer;

//-----------------------------------------------------------------------------
// Name: CreateExplSparks()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateExplSparks( void )
{
    size_t Width, Height;

    // setup the template

    _ExplSparksTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _ExplSparksTemplate.TextureID )
        return 0;

    _ExplSparksTemplate.TextureUSize = 64.0f / Width;
    _ExplSparksTemplate.TextureVSize = 64.0f / Height;

    _ExplSparksTemplate.StartFrameWidth = 12.0f;
    _ExplSparksTemplate.StartFrameHeight = 12.0f;

    _ExplSparksTemplate.Type = PARTICLETYPE_BILLBOARD;
    _ExplSparksTemplate.BSphereSize = 16.0f;

    _ExplSparksTemplate.FrameDeltaWidth  = 0.0f;
    _ExplSparksTemplate.FrameDeltaHeight = 0.0f;

    _ExplSparksTemplate.StartTextureColorCount = 2;

    _ExplSparksTemplate.StartTextureColors[ 0 ].R = 1.00f;
    _ExplSparksTemplate.StartTextureColors[ 0 ].G = 1.00f;
    _ExplSparksTemplate.StartTextureColors[ 0 ].B = 0.00f;
    _ExplSparksTemplate.StartTextureColors[ 0 ].A = 0.90f;

    _ExplSparksTemplate.StartTextureColors[ 1 ].R = 1.00f;
    _ExplSparksTemplate.StartTextureColors[ 1 ].G = 0.50f;
    _ExplSparksTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _ExplSparksTemplate.StartTextureColors[ 1 ].A = 0.90f;

    _ExplSparksTemplate.FinishTextureColor.R = 0.0f;
    _ExplSparksTemplate.FinishTextureColor.G = 0.0f;
    _ExplSparksTemplate.FinishTextureColor.B = 0.0f;
    _ExplSparksTemplate.FinishTextureColor.A = 0.0f;

    _ExplSparksTemplate.TextureDeltaColor.R = 0.0f;
    _ExplSparksTemplate.TextureDeltaColor.G = 0.0f;
    _ExplSparksTemplate.TextureDeltaColor.B = 0.0f;
    _ExplSparksTemplate.TextureDeltaColor.A = 0.0f;

    _ExplSparksTemplate.StartTextureFrame  = 0;
    _ExplSparksTemplate.FinishTextureFrame = 0;
    _ExplSparksTemplate.FrameChangeDelay   = 0;
    _ExplSparksTemplate.NumFrames          = 1;
    _ExplSparksTemplate.AnimateFrames      = 0;
    _ExplSparksTemplate.AnimateColor       = 0;

    _ExplSparksTemplate.LifetimeMin   = 200;
    _ExplSparksTemplate.LifetimeMax   = 400;

    _ExplSparksTemplate.Gravity           = 0.050f;
    _ExplSparksTemplate.AirFriction       = 0.001f;
    _ExplSparksTemplate.GroundFriction    = 0.005f;

    _ExplSparksTemplate.CanCollideWithLandscape = 1;
    _ExplSparksTemplate.CanCollideWithPlayers   = 1;

    // setup the emitter

    _ExplSparksEmitterTiny.Shape = EMITTERSHAPE_AABOX;
    _ExplSparksEmitterTiny.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _ExplSparksEmitterTiny.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _ExplSparksEmitterTiny.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _ExplSparksEmitterTiny.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _ExplSparksEmitterTiny.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _ExplSparksEmitterTiny.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _ExplSparksEmitterTiny.SpeedChange = SPEEDCHANGE_ALL;

    _ExplSparksEmitterTiny.StartMoveVector.X = 0.0f;
    _ExplSparksEmitterTiny.StartMoveVector.Y = 1.0f;
    _ExplSparksEmitterTiny.StartMoveVector.Z = 0.0f;

    _ExplSparksEmitterTiny.StartSpeedMin = 150;
    _ExplSparksEmitterTiny.StartSpeedMax = 200;
    _ExplSparksEmitterTiny.SpeedDeltaMin = -100;
    _ExplSparksEmitterTiny.SpeedDeltaMax =  100;

    _ExplSparksEmitterTiny.GLBlendSrc = GL_SRC_ALPHA;
    _ExplSparksEmitterTiny.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _ExplSparksEmitterTiny.EmitLimit     = 2;
    _ExplSparksEmitterTiny.EmitAmountMin = 2;
    _ExplSparksEmitterTiny.EmitAmountMax = 2;

    _ExplSparksEmitterTiny.EmitDelayMin = 0;
    _ExplSparksEmitterTiny.EmitDelayMax = 0;

    memcpy( &_ExplSparksEmitterSmall, &_ExplSparksEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplSparksEmitterMedium, &_ExplSparksEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplSparksEmitterLarge, &_ExplSparksEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplSparksEmitterHuge, &_ExplSparksEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplSparksEmitterPlayer, &_ExplSparksEmitterTiny, sizeof( ParticleEmitter_t ) );

    _ExplSparksEmitterSmall.EmitLimit     = 4;
    _ExplSparksEmitterSmall.EmitAmountMin = 4;
    _ExplSparksEmitterSmall.EmitAmountMax = 4;
    _ExplSparksEmitterSmall.StartSpeedMin = 175;
    _ExplSparksEmitterSmall.StartSpeedMax = 225;
    _ExplSparksEmitterSmall.SpeedDeltaMin = -110;
    _ExplSparksEmitterSmall.SpeedDeltaMax =  110;

    _ExplSparksEmitterMedium.EmitLimit     = 6;
    _ExplSparksEmitterMedium.EmitAmountMin = 6;
    _ExplSparksEmitterMedium.EmitAmountMax = 6;
    _ExplSparksEmitterMedium.StartSpeedMin = 200;
    _ExplSparksEmitterMedium.StartSpeedMax = 250;
    _ExplSparksEmitterMedium.SpeedDeltaMin = -120;
    _ExplSparksEmitterMedium.SpeedDeltaMax =  120;

    _ExplSparksEmitterLarge.EmitLimit     = 8;
    _ExplSparksEmitterLarge.EmitAmountMin = 8;
    _ExplSparksEmitterLarge.EmitAmountMax = 8;
    _ExplSparksEmitterLarge.StartSpeedMin = 225;
    _ExplSparksEmitterLarge.StartSpeedMax = 275;
    _ExplSparksEmitterLarge.SpeedDeltaMin = -130;
    _ExplSparksEmitterLarge.SpeedDeltaMax =  130;

    _ExplSparksEmitterHuge.EmitLimit     = 10;
    _ExplSparksEmitterHuge.EmitAmountMin = 10;
    _ExplSparksEmitterHuge.EmitAmountMax = 10;
    _ExplSparksEmitterHuge.StartSpeedMin = 250;
    _ExplSparksEmitterHuge.StartSpeedMax = 300;
    _ExplSparksEmitterHuge.SpeedDeltaMin = -140;
    _ExplSparksEmitterHuge.SpeedDeltaMax =  140;

    _ExplSparksEmitterPlayer.EmitLimit     = 9;
    _ExplSparksEmitterPlayer.EmitAmountMin = 9;
    _ExplSparksEmitterPlayer.EmitAmountMax = 9;
    _ExplSparksEmitterPlayer.StartSpeedMin = 225;
    _ExplSparksEmitterPlayer.StartSpeedMax = 275;
    _ExplSparksEmitterPlayer.SpeedDeltaMin = -120;
    _ExplSparksEmitterPlayer.SpeedDeltaMax =  120;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteExplSparks()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteExplSparks( void )
{
    DeleteTexture( _ExplSparksTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartExplSparks()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartExplSparks( Vector_t *EmitPoint, Vector_t *Normal, ExplosionType_t *ExplType, void (*FinishCallback)( size_t ) )
{
    size_t ID = GetFreeParticleSystem();
    ParticleEmitter_t *Emitter;

    if( ExplType == &g_ExplosionTiny )
        Emitter = &_ExplSparksEmitterTiny;

    else if( ExplType == &g_ExplosionMedium )
        Emitter = &_ExplSparksEmitterMedium;

    else if( ExplType == &g_ExplosionLarge )
        Emitter = &_ExplSparksEmitterLarge;

    else if( ExplType == &g_ExplosionHuge )
        Emitter = &_ExplSparksEmitterHuge;

    else if( ExplType == &g_ExplosionPlayer )
        Emitter = &_ExplSparksEmitterPlayer;

    else
        Emitter = &_ExplSparksEmitterSmall;

    VectorCopy( (*Normal), Emitter->StartMoveVector );
    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_ExplSparksTemplate, Emitter, FinishCallback );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, NapalmParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
