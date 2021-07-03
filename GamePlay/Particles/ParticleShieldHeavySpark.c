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

static ParticleTemplate_t _ShieldHeavySparkTemplate;
static ParticleEmitter_t _ShieldHeavySparkEmitter;


//-----------------------------------------------------------------------------
// Name: CreateShieldHeavySpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateShieldHeavySpark( void )
{
    size_t Width, Height;

    // setup the template

    _ShieldHeavySparkTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _ShieldHeavySparkTemplate.TextureID )
        return 0;

    _ShieldHeavySparkTemplate.TextureUSize = 64.0f / Width;
    _ShieldHeavySparkTemplate.TextureVSize = 64.0f / Height;

    _ShieldHeavySparkTemplate.StartFrameWidth = 8.0f;
    _ShieldHeavySparkTemplate.StartFrameHeight = 8.0f;

    _ShieldHeavySparkTemplate.Type = PARTICLETYPE_BILLBOARD;
    _ShieldHeavySparkTemplate.BSphereSize = 64.0f;

    _ShieldHeavySparkTemplate.FrameDeltaWidth  = 0.0f;
    _ShieldHeavySparkTemplate.FrameDeltaHeight = 0.0f;

    _ShieldHeavySparkTemplate.StartTextureColorCount = 2;

    _ShieldHeavySparkTemplate.StartTextureColors[ 0 ].R = 0.00f;
    _ShieldHeavySparkTemplate.StartTextureColors[ 0 ].G = 0.00f;
    _ShieldHeavySparkTemplate.StartTextureColors[ 0 ].B = 1.00f;
    _ShieldHeavySparkTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _ShieldHeavySparkTemplate.StartTextureColors[ 1 ].R = 0.10f;
    _ShieldHeavySparkTemplate.StartTextureColors[ 1 ].G = 0.10f;
    _ShieldHeavySparkTemplate.StartTextureColors[ 1 ].B = 0.85f;
    _ShieldHeavySparkTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _ShieldHeavySparkTemplate.FinishTextureColor.R = 0.0f;
    _ShieldHeavySparkTemplate.FinishTextureColor.G = 0.0f;
    _ShieldHeavySparkTemplate.FinishTextureColor.B = 0.0f;
    _ShieldHeavySparkTemplate.FinishTextureColor.A = 0.0f;

    _ShieldHeavySparkTemplate.TextureDeltaColor.R =  0.00f;
    _ShieldHeavySparkTemplate.TextureDeltaColor.G =  0.00f;
    _ShieldHeavySparkTemplate.TextureDeltaColor.B =  0.00f;
    _ShieldHeavySparkTemplate.TextureDeltaColor.A = -0.01f;

    _ShieldHeavySparkTemplate.StartTextureFrame  = 0;
    _ShieldHeavySparkTemplate.FinishTextureFrame = 0;
    _ShieldHeavySparkTemplate.FrameChangeDelay   = 0;
    _ShieldHeavySparkTemplate.NumFrames          = 1;
    _ShieldHeavySparkTemplate.AnimateFrames      = 0;
    _ShieldHeavySparkTemplate.AnimateColor       = 0;

    _ShieldHeavySparkTemplate.LifetimeMin   = 100;
    _ShieldHeavySparkTemplate.LifetimeMax   = 200;

    _ShieldHeavySparkTemplate.Gravity            = 0.05f;
    _ShieldHeavySparkTemplate.AirFriction        = 0.0f;
    _ShieldHeavySparkTemplate.GroundFriction     = 0.0f;

    _ShieldHeavySparkTemplate.CanCollideWithLandscape = 1;
    _ShieldHeavySparkTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _ShieldHeavySparkEmitter.Shape = EMITTERSHAPE_AABOX;
    _ShieldHeavySparkEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -750;
    _ShieldHeavySparkEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  750;
    _ShieldHeavySparkEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  750;
    _ShieldHeavySparkEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -750;
    _ShieldHeavySparkEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  750;
    _ShieldHeavySparkEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -750;

    _ShieldHeavySparkEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _ShieldHeavySparkEmitter.StartMoveVector.X = 0.0f;
    _ShieldHeavySparkEmitter.StartMoveVector.Y = 1.0f;
    _ShieldHeavySparkEmitter.StartMoveVector.Z = 0.0f;

    _ShieldHeavySparkEmitter.StartSpeedMin = 150;
    _ShieldHeavySparkEmitter.StartSpeedMax = 300;
    _ShieldHeavySparkEmitter.SpeedDeltaMin = -50;
    _ShieldHeavySparkEmitter.SpeedDeltaMax =  50;

    _ShieldHeavySparkEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _ShieldHeavySparkEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _ShieldHeavySparkEmitter.EmitAmountMin = 3;
    _ShieldHeavySparkEmitter.EmitAmountMax = 8;

    _ShieldHeavySparkEmitter.EmitDelayMin = 1;
    _ShieldHeavySparkEmitter.EmitDelayMax = 3;

    _ShieldHeavySparkEmitter.EmitLimit = 16;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteShieldHeavySpark()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteShieldHeavySpark( void )
{
    DeleteTexture( _ShieldHeavySparkTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartShieldHeavySpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartShieldHeavySpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_ShieldHeavySparkTemplate, &_ShieldHeavySparkEmitter, FinishCallback );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
