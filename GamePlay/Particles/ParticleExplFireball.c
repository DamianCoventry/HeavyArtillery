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

#define NAPALM_DAMAGE       0.1f


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _ExplFireballTemplate;

static ParticleEmitter_t _ExplFireballEmitterTiny;
static ParticleEmitter_t _ExplFireballEmitterSmall;
static ParticleEmitter_t _ExplFireballEmitterMedium;
static ParticleEmitter_t _ExplFireballEmitterLarge;
static ParticleEmitter_t _ExplFireballEmitterHuge;
static ParticleEmitter_t _ExplFireballEmitterPlayer;

//-----------------------------------------------------------------------------
// Name: CreateExplFireball()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateExplFireball( void )
{
    size_t Width, Height;

    // setup the template

    _ExplFireballTemplate.TextureID = CreateTexture( "TGA/Fireball.tga", &Width, &Height, 1, 0 );
    if( -1 == _ExplFireballTemplate.TextureID )
        return 0;

    _ExplFireballTemplate.TextureUSize = 32.0f / Width;
    _ExplFireballTemplate.TextureVSize = 32.0f / Height;

    _ExplFireballTemplate.StartFrameWidth = 64.0f;
    _ExplFireballTemplate.StartFrameHeight = 64.0f;

    _ExplFireballTemplate.Type = PARTICLETYPE_BILLBOARD;
    _ExplFireballTemplate.BSphereSize = 32.0f;

    _ExplFireballTemplate.FrameDeltaWidth  = 0.0f;
    _ExplFireballTemplate.FrameDeltaHeight = 0.0f;

    _ExplFireballTemplate.StartTextureColorCount = 4;

    _ExplFireballTemplate.StartTextureColors[ 0 ].R = 0.90f;
    _ExplFireballTemplate.StartTextureColors[ 0 ].G = 0.90f;
    _ExplFireballTemplate.StartTextureColors[ 0 ].B = 0.00f;
    _ExplFireballTemplate.StartTextureColors[ 0 ].A = 0.90f;

    _ExplFireballTemplate.StartTextureColors[ 1 ].R = 0.85f;
    _ExplFireballTemplate.StartTextureColors[ 1 ].G = 0.80f;
    _ExplFireballTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _ExplFireballTemplate.StartTextureColors[ 1 ].A = 0.90f;

    _ExplFireballTemplate.StartTextureColors[ 2 ].R = 0.80f;
    _ExplFireballTemplate.StartTextureColors[ 2 ].G = 0.50f;
    _ExplFireballTemplate.StartTextureColors[ 2 ].B = 0.00f;
    _ExplFireballTemplate.StartTextureColors[ 2 ].A = 0.90f;

    _ExplFireballTemplate.StartTextureColors[ 3 ].R = 0.75f;
    _ExplFireballTemplate.StartTextureColors[ 3 ].G = 0.45f;
    _ExplFireballTemplate.StartTextureColors[ 3 ].B = 0.00f;
    _ExplFireballTemplate.StartTextureColors[ 3 ].A = 0.90f;

    _ExplFireballTemplate.FinishTextureColor.R = 0.0f;
    _ExplFireballTemplate.FinishTextureColor.G = 0.0f;
    _ExplFireballTemplate.FinishTextureColor.B = 0.0f;
    _ExplFireballTemplate.FinishTextureColor.A = 0.0f;

    _ExplFireballTemplate.TextureDeltaColor.R = 0.0f;
    _ExplFireballTemplate.TextureDeltaColor.G = 0.0f;
    _ExplFireballTemplate.TextureDeltaColor.B = 0.0f;
    _ExplFireballTemplate.TextureDeltaColor.A = 0.0f;

    _ExplFireballTemplate.StartTextureFrame  = -1;
    _ExplFireballTemplate.FinishTextureFrame = 0;
    _ExplFireballTemplate.FrameChangeDelay   = 2;
    _ExplFireballTemplate.NumFrames          = 16;
    _ExplFireballTemplate.AnimateFrames      = 1;
    _ExplFireballTemplate.AnimateColor       = 0;

    _ExplFireballTemplate.LifetimeMin   = 200;
    _ExplFireballTemplate.LifetimeMax   = 400;

    _ExplFireballTemplate.Gravity           = 0.050f;
    _ExplFireballTemplate.AirFriction       = 0.001f;
    _ExplFireballTemplate.GroundFriction    = 0.005f;

    _ExplFireballTemplate.CanCollideWithLandscape = 1;
    _ExplFireballTemplate.CanCollideWithPlayers   = 1;

    // setup the emitter

    _ExplFireballEmitterTiny.Shape = EMITTERSHAPE_AABOX;
    _ExplFireballEmitterTiny.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _ExplFireballEmitterTiny.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _ExplFireballEmitterTiny.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _ExplFireballEmitterTiny.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _ExplFireballEmitterTiny.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _ExplFireballEmitterTiny.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _ExplFireballEmitterTiny.SpeedChange = SPEEDCHANGE_ALL;

    _ExplFireballEmitterTiny.StartMoveVector.X = 0.0f;
    _ExplFireballEmitterTiny.StartMoveVector.Y = 1.0f;
    _ExplFireballEmitterTiny.StartMoveVector.Z = 0.0f;

    _ExplFireballEmitterTiny.StartSpeedMin = 150;
    _ExplFireballEmitterTiny.StartSpeedMax = 200;
    _ExplFireballEmitterTiny.SpeedDeltaMin = -100;
    _ExplFireballEmitterTiny.SpeedDeltaMax =  100;

    _ExplFireballEmitterTiny.GLBlendSrc = GL_SRC_ALPHA;
    _ExplFireballEmitterTiny.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _ExplFireballEmitterTiny.EmitLimit     = 2;
    _ExplFireballEmitterTiny.EmitAmountMin = 2;
    _ExplFireballEmitterTiny.EmitAmountMax = 2;

    _ExplFireballEmitterTiny.EmitDelayMin = 0;
    _ExplFireballEmitterTiny.EmitDelayMax = 0;

    memcpy( &_ExplFireballEmitterSmall, &_ExplFireballEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplFireballEmitterMedium, &_ExplFireballEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplFireballEmitterLarge, &_ExplFireballEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplFireballEmitterHuge, &_ExplFireballEmitterTiny, sizeof( ParticleEmitter_t ) );
    memcpy( &_ExplFireballEmitterPlayer, &_ExplFireballEmitterTiny, sizeof( ParticleEmitter_t ) );

    _ExplFireballEmitterSmall.EmitLimit     = 4;
    _ExplFireballEmitterSmall.EmitAmountMin = 4;
    _ExplFireballEmitterSmall.EmitAmountMax = 4;
    _ExplFireballEmitterSmall.StartSpeedMin = 175;
    _ExplFireballEmitterSmall.StartSpeedMax = 225;
    _ExplFireballEmitterSmall.SpeedDeltaMin = -110;
    _ExplFireballEmitterSmall.SpeedDeltaMax =  110;

    _ExplFireballEmitterMedium.EmitLimit     = 6;
    _ExplFireballEmitterMedium.EmitAmountMin = 6;
    _ExplFireballEmitterMedium.EmitAmountMax = 6;
    _ExplFireballEmitterMedium.StartSpeedMin = 200;
    _ExplFireballEmitterMedium.StartSpeedMax = 250;
    _ExplFireballEmitterMedium.SpeedDeltaMin = -120;
    _ExplFireballEmitterMedium.SpeedDeltaMax =  120;

    _ExplFireballEmitterLarge.EmitLimit     = 8;
    _ExplFireballEmitterLarge.EmitAmountMin = 8;
    _ExplFireballEmitterLarge.EmitAmountMax = 8;
    _ExplFireballEmitterLarge.StartSpeedMin = 225;
    _ExplFireballEmitterLarge.StartSpeedMax = 275;
    _ExplFireballEmitterLarge.SpeedDeltaMin = -130;
    _ExplFireballEmitterLarge.SpeedDeltaMax =  130;

    _ExplFireballEmitterHuge.EmitLimit     = 10;
    _ExplFireballEmitterHuge.EmitAmountMin = 10;
    _ExplFireballEmitterHuge.EmitAmountMax = 10;
    _ExplFireballEmitterHuge.StartSpeedMin = 250;
    _ExplFireballEmitterHuge.StartSpeedMax = 275;
    _ExplFireballEmitterHuge.SpeedDeltaMin = -140;
    _ExplFireballEmitterHuge.SpeedDeltaMax =  140;

    _ExplFireballEmitterPlayer.EmitLimit     = 9;
    _ExplFireballEmitterPlayer.EmitAmountMin = 9;
    _ExplFireballEmitterPlayer.EmitAmountMax = 9;
    _ExplFireballEmitterPlayer.StartSpeedMin = 225;
    _ExplFireballEmitterPlayer.StartSpeedMax = 275;
    _ExplFireballEmitterPlayer.SpeedDeltaMin = -120;
    _ExplFireballEmitterPlayer.SpeedDeltaMax =  120;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteExplFireball()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteExplFireball( void )
{
    DeleteTexture( _ExplFireballTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartExplFireball()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartExplFireball( Vector_t *EmitPoint, Vector_t *Normal, ExplosionType_t *ExplType, void (*FinishCallback)( size_t ) )
{
    size_t ID = GetFreeParticleSystem();
    ParticleEmitter_t *Emitter;

    if( ExplType == &g_ExplosionTiny )
        Emitter = &_ExplFireballEmitterTiny;

    else if( ExplType == &g_ExplosionMedium )
        Emitter = &_ExplFireballEmitterMedium;

    else if( ExplType == &g_ExplosionLarge )
        Emitter = &_ExplFireballEmitterLarge;

    else if( ExplType == &g_ExplosionHuge )
        Emitter = &_ExplFireballEmitterHuge;

    else if( ExplType == &g_ExplosionPlayer )
        Emitter = &_ExplFireballEmitterPlayer;

    else
        Emitter = &_ExplFireballEmitterSmall;

    VectorCopy( (*Normal), Emitter->StartMoveVector );
    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_ExplFireballTemplate, Emitter, FinishCallback );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, NapalmParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
