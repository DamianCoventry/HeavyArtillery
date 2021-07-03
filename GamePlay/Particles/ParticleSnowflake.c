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

#include "Object.h"


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _SnowflakeTemplate;
static ParticleEmitter_t _SnowflakeEmitter;


//-----------------------------------------------------------------------------
// Name: CreateSnowflake()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateSnowflake( void )
{
    size_t Width, Height;

    // setup the template

    _SnowflakeTemplate.TextureID = CreateTexture( "TGA/Snowflake.tga", &Width, &Height, 1, 0 );
    if( -1 == _SnowflakeTemplate.TextureID )
        return 0;

    _SnowflakeTemplate.TextureUSize = 64.0f / Width;
    _SnowflakeTemplate.TextureVSize = 64.0f / Height;

    _SnowflakeTemplate.StartFrameWidth = 8.0f;
    _SnowflakeTemplate.StartFrameHeight = 8.0f;

    _SnowflakeTemplate.Type = PARTICLETYPE_BILLBOARD;
    _SnowflakeTemplate.BSphereSize = 32.0f;

    _SnowflakeTemplate.FrameDeltaWidth  = 0.0f;
    _SnowflakeTemplate.FrameDeltaHeight = 0.0f;

    _SnowflakeTemplate.StartTextureColorCount = 2;

    _SnowflakeTemplate.StartTextureColors[ 0 ].R = 0.90f;
    _SnowflakeTemplate.StartTextureColors[ 0 ].G = 0.90f;
    _SnowflakeTemplate.StartTextureColors[ 0 ].B = 0.90f;
    _SnowflakeTemplate.StartTextureColors[ 0 ].A = 0.75f;

    _SnowflakeTemplate.StartTextureColors[ 1 ].R = 0.85f;
    _SnowflakeTemplate.StartTextureColors[ 1 ].G = 0.85f;
    _SnowflakeTemplate.StartTextureColors[ 1 ].B = 0.95f;
    _SnowflakeTemplate.StartTextureColors[ 1 ].A = 0.75f;

    _SnowflakeTemplate.FinishTextureColor.R = 0.0f;
    _SnowflakeTemplate.FinishTextureColor.G = 0.0f;
    _SnowflakeTemplate.FinishTextureColor.B = 0.0f;
    _SnowflakeTemplate.FinishTextureColor.A = 0.0f;

    _SnowflakeTemplate.TextureDeltaColor.R = 0.0f;
    _SnowflakeTemplate.TextureDeltaColor.G = 0.0f;
    _SnowflakeTemplate.TextureDeltaColor.B = 0.0f;
    _SnowflakeTemplate.TextureDeltaColor.A = 0.0f;

    _SnowflakeTemplate.AttachedToCamera = 1;

    _SnowflakeTemplate.StartTextureFrame  = 0;
    _SnowflakeTemplate.FinishTextureFrame = 0;
    _SnowflakeTemplate.FrameChangeDelay   = 0;
    _SnowflakeTemplate.NumFrames          = 1;
    _SnowflakeTemplate.AnimateFrames      = 0;
    _SnowflakeTemplate.AnimateColor       = 0;

    _SnowflakeTemplate.LifetimeMin   = -1;
    _SnowflakeTemplate.LifetimeMax   = -1;

    _SnowflakeTemplate.Gravity            = 0.05f;
    _SnowflakeTemplate.AirFriction        = 0.0f;
    _SnowflakeTemplate.GroundFriction     = 0.0f;

    _SnowflakeTemplate.CanCollideWithLandscape = 0;
    _SnowflakeTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _SnowflakeEmitter.Shape = EMITTERSHAPE_AABOX;
    _SnowflakeEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =   512;
    _SnowflakeEmitter.Dimensions[ EMITTERSIDE_LEFT   ] =  -512;
    _SnowflakeEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  2000;
    _SnowflakeEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -2000;
    _SnowflakeEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =   512;
    _SnowflakeEmitter.Dimensions[ EMITTERSIDE_BACK   ] =  -512;

    _SnowflakeEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _SnowflakeEmitter.StartMoveVector.X =  0.0f;
    _SnowflakeEmitter.StartMoveVector.Y = -1.0f;
    _SnowflakeEmitter.StartMoveVector.Z =  0.0f;

    _SnowflakeEmitter.StartSpeedMin =  70;
    _SnowflakeEmitter.StartSpeedMax = 100;
    _SnowflakeEmitter.SpeedDeltaMin = -50;
    _SnowflakeEmitter.SpeedDeltaMax =  50;

    _SnowflakeEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _SnowflakeEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _SnowflakeEmitter.EmitAmountMin = 0;
    _SnowflakeEmitter.EmitAmountMax = 0;

    _SnowflakeEmitter.EmitDelayMin = 0;
    _SnowflakeEmitter.EmitDelayMax = 0;

    _SnowflakeEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteSnowflake()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteSnowflake( void )
{
    DeleteTexture( _SnowflakeTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartSnowflake()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartSnowflake( void )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, nullptr, &_SnowflakeTemplate, &_SnowflakeEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, SnowflakeParticleCollide );
    SetDrawFunction( ID, StandardBillboardDraw );

    CreateSnowStorm( ID, (size_t *)_SnowflakeEmitter.Dimensions, &_SnowflakeTemplate );

    return ID;
}
