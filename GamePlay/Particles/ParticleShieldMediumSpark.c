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

static ParticleTemplate_t _ShieldMediumSparkTemplate;
static ParticleEmitter_t _ShieldMediumSparkEmitter;


//-----------------------------------------------------------------------------
// Name: CreateShieldMediumSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateShieldMediumSpark( void )
{
    size_t Width, Height;

    // setup the template

    _ShieldMediumSparkTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _ShieldMediumSparkTemplate.TextureID )
        return 0;

    _ShieldMediumSparkTemplate.TextureUSize = 64.0f / Width;
    _ShieldMediumSparkTemplate.TextureVSize = 64.0f / Height;

    _ShieldMediumSparkTemplate.StartFrameWidth = 8.0f;
    _ShieldMediumSparkTemplate.StartFrameHeight = 8.0f;

    _ShieldMediumSparkTemplate.Type = PARTICLETYPE_BILLBOARD;
    _ShieldMediumSparkTemplate.BSphereSize = 64.0f;

    _ShieldMediumSparkTemplate.FrameDeltaWidth  = 0.0f;
    _ShieldMediumSparkTemplate.FrameDeltaHeight = 0.0f;

    _ShieldMediumSparkTemplate.StartTextureColorCount = 2;

    _ShieldMediumSparkTemplate.StartTextureColors[ 0 ].R = 0.00f;
    _ShieldMediumSparkTemplate.StartTextureColors[ 0 ].G = 1.00f;
    _ShieldMediumSparkTemplate.StartTextureColors[ 0 ].B = 0.00f;
    _ShieldMediumSparkTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _ShieldMediumSparkTemplate.StartTextureColors[ 1 ].R = 0.10f;
    _ShieldMediumSparkTemplate.StartTextureColors[ 1 ].G = 0.85f;
    _ShieldMediumSparkTemplate.StartTextureColors[ 1 ].B = 0.10f;
    _ShieldMediumSparkTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _ShieldMediumSparkTemplate.FinishTextureColor.R = 0.0f;
    _ShieldMediumSparkTemplate.FinishTextureColor.G = 0.0f;
    _ShieldMediumSparkTemplate.FinishTextureColor.B = 0.0f;
    _ShieldMediumSparkTemplate.FinishTextureColor.A = 0.0f;

    _ShieldMediumSparkTemplate.TextureDeltaColor.R =  0.00f;
    _ShieldMediumSparkTemplate.TextureDeltaColor.G =  0.00f;
    _ShieldMediumSparkTemplate.TextureDeltaColor.B =  0.00f;
    _ShieldMediumSparkTemplate.TextureDeltaColor.A = -0.01f;

    _ShieldMediumSparkTemplate.StartTextureFrame  = 0;
    _ShieldMediumSparkTemplate.FinishTextureFrame = 0;
    _ShieldMediumSparkTemplate.FrameChangeDelay   = 0;
    _ShieldMediumSparkTemplate.NumFrames          = 1;
    _ShieldMediumSparkTemplate.AnimateFrames      = 0;
    _ShieldMediumSparkTemplate.AnimateColor       = 0;

    _ShieldMediumSparkTemplate.LifetimeMin   = 100;
    _ShieldMediumSparkTemplate.LifetimeMax   = 200;

    _ShieldMediumSparkTemplate.Gravity            = 0.05f;
    _ShieldMediumSparkTemplate.AirFriction        = 0.0f;
    _ShieldMediumSparkTemplate.GroundFriction     = 0.0f;

    _ShieldMediumSparkTemplate.CanCollideWithLandscape = 1;
    _ShieldMediumSparkTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _ShieldMediumSparkEmitter.Shape = EMITTERSHAPE_AABOX;
    _ShieldMediumSparkEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -750;
    _ShieldMediumSparkEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  750;
    _ShieldMediumSparkEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  750;
    _ShieldMediumSparkEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -750;
    _ShieldMediumSparkEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  750;
    _ShieldMediumSparkEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -750;

    _ShieldMediumSparkEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _ShieldMediumSparkEmitter.StartMoveVector.X = 0.0f;
    _ShieldMediumSparkEmitter.StartMoveVector.Y = 1.0f;
    _ShieldMediumSparkEmitter.StartMoveVector.Z = 0.0f;

    _ShieldMediumSparkEmitter.StartSpeedMin = 150;
    _ShieldMediumSparkEmitter.StartSpeedMax = 300;
    _ShieldMediumSparkEmitter.SpeedDeltaMin = -50;
    _ShieldMediumSparkEmitter.SpeedDeltaMax =  50;

    _ShieldMediumSparkEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _ShieldMediumSparkEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _ShieldMediumSparkEmitter.EmitAmountMin = 3;
    _ShieldMediumSparkEmitter.EmitAmountMax = 8;

    _ShieldMediumSparkEmitter.EmitDelayMin = 1;
    _ShieldMediumSparkEmitter.EmitDelayMax = 3;

    _ShieldMediumSparkEmitter.EmitLimit = 16;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteShieldMediumSpark()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteShieldMediumSpark( void )
{
    DeleteTexture( _ShieldMediumSparkTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartShieldMediumSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartShieldMediumSpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_ShieldMediumSparkTemplate, &_ShieldMediumSparkEmitter, FinishCallback );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
