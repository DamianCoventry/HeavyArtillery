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

static ParticleTemplate_t _ShieldLightSparkTemplate;
static ParticleEmitter_t _ShieldLightSparkEmitter;


//-----------------------------------------------------------------------------
// Name: CreateShieldLightSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateShieldLightSpark( void )
{
    size_t Width, Height;

    // setup the template

    _ShieldLightSparkTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _ShieldLightSparkTemplate.TextureID )
        return 0;

    _ShieldLightSparkTemplate.TextureUSize = 64.0f / Width;
    _ShieldLightSparkTemplate.TextureVSize = 64.0f / Height;

    _ShieldLightSparkTemplate.StartFrameWidth = 8.0f;
    _ShieldLightSparkTemplate.StartFrameHeight = 8.0f;

    _ShieldLightSparkTemplate.Type = PARTICLETYPE_BILLBOARD;
    _ShieldLightSparkTemplate.BSphereSize = 64.0f;

    _ShieldLightSparkTemplate.FrameDeltaWidth  = 0.0f;
    _ShieldLightSparkTemplate.FrameDeltaHeight = 0.0f;

    _ShieldLightSparkTemplate.StartTextureColorCount = 2;

    _ShieldLightSparkTemplate.StartTextureColors[ 0 ].R = 1.00f;
    _ShieldLightSparkTemplate.StartTextureColors[ 0 ].G = 0.00f;
    _ShieldLightSparkTemplate.StartTextureColors[ 0 ].B = 0.10f;
    _ShieldLightSparkTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _ShieldLightSparkTemplate.StartTextureColors[ 1 ].R = 0.85f;
    _ShieldLightSparkTemplate.StartTextureColors[ 1 ].G = 0.10f;
    _ShieldLightSparkTemplate.StartTextureColors[ 1 ].B = 0.10f;
    _ShieldLightSparkTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _ShieldLightSparkTemplate.FinishTextureColor.R = 0.0f;
    _ShieldLightSparkTemplate.FinishTextureColor.G = 0.0f;
    _ShieldLightSparkTemplate.FinishTextureColor.B = 0.0f;
    _ShieldLightSparkTemplate.FinishTextureColor.A = 0.0f;

    _ShieldLightSparkTemplate.TextureDeltaColor.R =  0.00f;
    _ShieldLightSparkTemplate.TextureDeltaColor.G =  0.00f;
    _ShieldLightSparkTemplate.TextureDeltaColor.B =  0.00f;
    _ShieldLightSparkTemplate.TextureDeltaColor.A = -0.01f;

    _ShieldLightSparkTemplate.StartTextureFrame  = 0;
    _ShieldLightSparkTemplate.FinishTextureFrame = 0;
    _ShieldLightSparkTemplate.FrameChangeDelay   = 0;
    _ShieldLightSparkTemplate.NumFrames          = 1;
    _ShieldLightSparkTemplate.AnimateFrames      = 0;
    _ShieldLightSparkTemplate.AnimateColor       = 0;

    _ShieldLightSparkTemplate.LifetimeMin   = 100;
    _ShieldLightSparkTemplate.LifetimeMax   = 200;

    _ShieldLightSparkTemplate.Gravity            = 0.05f;
    _ShieldLightSparkTemplate.AirFriction        = 0.0f;
    _ShieldLightSparkTemplate.GroundFriction     = 0.0f;

    _ShieldLightSparkTemplate.CanCollideWithLandscape = 1;
    _ShieldLightSparkTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _ShieldLightSparkEmitter.Shape = EMITTERSHAPE_AABOX;
    _ShieldLightSparkEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -750;
    _ShieldLightSparkEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  750;
    _ShieldLightSparkEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  750;
    _ShieldLightSparkEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -750;
    _ShieldLightSparkEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  750;
    _ShieldLightSparkEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -750;

    _ShieldLightSparkEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _ShieldLightSparkEmitter.StartMoveVector.X = 0.0f;
    _ShieldLightSparkEmitter.StartMoveVector.Y = 1.0f;
    _ShieldLightSparkEmitter.StartMoveVector.Z = 0.0f;

    _ShieldLightSparkEmitter.StartSpeedMin = 150;
    _ShieldLightSparkEmitter.StartSpeedMax = 300;
    _ShieldLightSparkEmitter.SpeedDeltaMin = -50;
    _ShieldLightSparkEmitter.SpeedDeltaMax =  50;

    _ShieldLightSparkEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _ShieldLightSparkEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _ShieldLightSparkEmitter.EmitAmountMin = 3;
    _ShieldLightSparkEmitter.EmitAmountMax = 8;

    _ShieldLightSparkEmitter.EmitDelayMin = 1;
    _ShieldLightSparkEmitter.EmitDelayMax = 3;

    _ShieldLightSparkEmitter.EmitLimit = 16;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteShieldLightSpark()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteShieldLightSpark( void )
{
    DeleteTexture( _ShieldLightSparkTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartShieldLightSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartShieldLightSpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_ShieldLightSparkTemplate, &_ShieldLightSparkEmitter, FinishCallback );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
