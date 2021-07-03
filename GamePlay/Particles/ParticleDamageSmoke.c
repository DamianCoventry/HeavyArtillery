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

static ParticleTemplate_t _DamageSmokeTemplate;
static ParticleEmitter_t _DamageSmokeEmitter;


//-----------------------------------------------------------------------------
// Name: CreateDamageSmoke()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateDamageSmoke( void )
{
    size_t Width, Height;

    // setup the template

    _DamageSmokeTemplate.TextureID = CreateTexture( "TGA/MissileSmoke.tga", &Width, &Height, 1, 0 );
    if( -1 == _DamageSmokeTemplate.TextureID )
        return 0;

    _DamageSmokeTemplate.TextureUSize = 128.0f / Width;
    _DamageSmokeTemplate.TextureVSize = 128.0f / Height;

    _DamageSmokeTemplate.StartFrameWidth = 16.0f;
    _DamageSmokeTemplate.StartFrameHeight = 16.0f;

    _DamageSmokeTemplate.Type = PARTICLETYPE_BILLBOARD;
    _DamageSmokeTemplate.BSphereSize = 64.0f;

    _DamageSmokeTemplate.FrameDeltaWidth  = 0.1f;
    _DamageSmokeTemplate.FrameDeltaHeight = 0.1f;

    _DamageSmokeTemplate.StartTextureColorCount = 1;

    _DamageSmokeTemplate.StartTextureColors[ 0 ].R = 0.10f;
    _DamageSmokeTemplate.StartTextureColors[ 0 ].G = 0.10f;
    _DamageSmokeTemplate.StartTextureColors[ 0 ].B = 0.10f;
    _DamageSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _DamageSmokeTemplate.FinishTextureColor.R = 0.10f;
    _DamageSmokeTemplate.FinishTextureColor.G = 0.10f;
    _DamageSmokeTemplate.FinishTextureColor.B = 0.10f;
    _DamageSmokeTemplate.FinishTextureColor.A = 0.00f;

    _DamageSmokeTemplate.TextureDeltaColor.R =  0.00f;
    _DamageSmokeTemplate.TextureDeltaColor.G =  0.00f;
    _DamageSmokeTemplate.TextureDeltaColor.B =  0.00f;
    _DamageSmokeTemplate.TextureDeltaColor.A = -0.01f;

    _DamageSmokeTemplate.StartTextureFrame  = 0;
    _DamageSmokeTemplate.FinishTextureFrame = 0;
    _DamageSmokeTemplate.FrameChangeDelay   = 0;
    _DamageSmokeTemplate.NumFrames          = 1;
    _DamageSmokeTemplate.AnimateFrames      = 0;
    _DamageSmokeTemplate.AnimateColor       = 0;

    _DamageSmokeTemplate.LifetimeMin   = -1;
    _DamageSmokeTemplate.LifetimeMax   = -1;

    _DamageSmokeTemplate.Gravity            = 0.0f;
    _DamageSmokeTemplate.AirFriction        = 0.0f;
    _DamageSmokeTemplate.GroundFriction     = 0.0f;

    _DamageSmokeTemplate.CanCollideWithLandscape = 0;
    _DamageSmokeTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _DamageSmokeEmitter.Shape = EMITTERSHAPE_AASQUARE;
    _DamageSmokeEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _DamageSmokeEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _DamageSmokeEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;
    _DamageSmokeEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;

    _DamageSmokeEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _DamageSmokeEmitter.StartMoveVector.X = 0.0f;
    _DamageSmokeEmitter.StartMoveVector.Y = 1.0f;
    _DamageSmokeEmitter.StartMoveVector.Z = 0.0f;

    _DamageSmokeEmitter.StartSpeedMin =  50;
    _DamageSmokeEmitter.StartSpeedMax = 100;
    _DamageSmokeEmitter.SpeedDeltaMin = -20;
    _DamageSmokeEmitter.SpeedDeltaMax =  20;

    _DamageSmokeEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _DamageSmokeEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _DamageSmokeEmitter.EmitAmountMin = 1;
    _DamageSmokeEmitter.EmitAmountMax = 1;

    _DamageSmokeEmitter.EmitDelayMin = 3;
    _DamageSmokeEmitter.EmitDelayMax = 8;

    _DamageSmokeEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteDamageSmoke()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteDamageSmoke( void )
{
    DeleteTexture( _DamageSmokeTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartDamageSmoke()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartDamageSmoke( size_t ObjectID )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_DamageSmokeTemplate, &_DamageSmokeEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
