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

static ParticleTemplate_t _TankSparkTemplate;
static ParticleEmitter_t _TankSparkEmitter;


//-----------------------------------------------------------------------------
// Name: CreateTankSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateTankSpark( void )
{
    size_t Width, Height;

    // setup the template

    _TankSparkTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _TankSparkTemplate.TextureID )
        return 0;

    _TankSparkTemplate.TextureUSize = 64.0f / Width;
    _TankSparkTemplate.TextureVSize = 64.0f / Height;

    _TankSparkTemplate.StartFrameWidth = 8.0f;
    _TankSparkTemplate.StartFrameHeight = 8.0f;

    _TankSparkTemplate.Type = PARTICLETYPE_BILLBOARD;
    _TankSparkTemplate.BSphereSize = 64.0f;

    _TankSparkTemplate.FrameDeltaWidth  = 0.0f;
    _TankSparkTemplate.FrameDeltaHeight = 0.0f;

    _TankSparkTemplate.StartTextureColorCount = 2;

    _TankSparkTemplate.StartTextureColors[ 0 ].R = 0.90f;
    _TankSparkTemplate.StartTextureColors[ 0 ].G = 0.90f;
    _TankSparkTemplate.StartTextureColors[ 0 ].B = 0.10f;
    _TankSparkTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _TankSparkTemplate.StartTextureColors[ 1 ].R = 0.80f;
    _TankSparkTemplate.StartTextureColors[ 1 ].G = 0.65f;
    _TankSparkTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _TankSparkTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _TankSparkTemplate.FinishTextureColor.R = 0.0f;
    _TankSparkTemplate.FinishTextureColor.G = 0.0f;
    _TankSparkTemplate.FinishTextureColor.B = 0.0f;
    _TankSparkTemplate.FinishTextureColor.A = 0.0f;

    _TankSparkTemplate.TextureDeltaColor.R =  0.00f;
    _TankSparkTemplate.TextureDeltaColor.G =  0.00f;
    _TankSparkTemplate.TextureDeltaColor.B =  0.00f;
    _TankSparkTemplate.TextureDeltaColor.A = -0.01f;

    _TankSparkTemplate.StartTextureFrame  = 0;
    _TankSparkTemplate.FinishTextureFrame = 0;
    _TankSparkTemplate.FrameChangeDelay   = 0;
    _TankSparkTemplate.NumFrames          = 1;
    _TankSparkTemplate.AnimateFrames      = 0;
    _TankSparkTemplate.AnimateColor       = 0;

    _TankSparkTemplate.LifetimeMin   = 100;
    _TankSparkTemplate.LifetimeMax   = 200;

    _TankSparkTemplate.Gravity            = 0.05f;
    _TankSparkTemplate.AirFriction        = 0.0f;
    _TankSparkTemplate.GroundFriction     = 0.0f;

    _TankSparkTemplate.CanCollideWithLandscape = 1;
    _TankSparkTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _TankSparkEmitter.Shape = EMITTERSHAPE_AABOX;
    _TankSparkEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -750;
    _TankSparkEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  750;
    _TankSparkEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  750;
    _TankSparkEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -750;
    _TankSparkEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  750;
    _TankSparkEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -750;

    _TankSparkEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _TankSparkEmitter.StartMoveVector.X = 0.0f;
    _TankSparkEmitter.StartMoveVector.Y = 1.0f;
    _TankSparkEmitter.StartMoveVector.Z = 0.0f;

    _TankSparkEmitter.StartSpeedMin = 150;
    _TankSparkEmitter.StartSpeedMax = 300;
    _TankSparkEmitter.SpeedDeltaMin = -50;
    _TankSparkEmitter.SpeedDeltaMax =  50;

    _TankSparkEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _TankSparkEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _TankSparkEmitter.EmitAmountMin = 3;
    _TankSparkEmitter.EmitAmountMax = 8;

    _TankSparkEmitter.EmitDelayMin = 1;
    _TankSparkEmitter.EmitDelayMax = 3;

    _TankSparkEmitter.EmitLimit = 16;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteTankSpark()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteTankSpark( void )
{
    DeleteTexture( _TankSparkTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartTankSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartTankSpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_TankSparkTemplate, &_TankSparkEmitter, FinishCallback );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
