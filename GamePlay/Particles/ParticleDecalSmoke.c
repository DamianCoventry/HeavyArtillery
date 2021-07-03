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

static ParticleTemplate_t _DecalSmokeTemplate;
static ParticleEmitter_t _DecalSmokeEmitter;


//-----------------------------------------------------------------------------
// Name: CreateDecalSmoke()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateDecalSmoke( void )
{
    size_t Width, Height;

    // setup the template

    _DecalSmokeTemplate.TextureID = CreateTexture( "TGA/MissileSmoke.tga", &Width, &Height, 1, 0 );
    if( -1 == _DecalSmokeTemplate.TextureID )
        return 0;

    _DecalSmokeTemplate.TextureUSize = 128.0f / Width;
    _DecalSmokeTemplate.TextureVSize = 128.0f / Height;

    _DecalSmokeTemplate.StartFrameWidth = 8.0f;
    _DecalSmokeTemplate.StartFrameHeight = 8.0f;

    _DecalSmokeTemplate.Type = PARTICLETYPE_BILLBOARD;
    _DecalSmokeTemplate.BSphereSize = 64.0f;

    _DecalSmokeTemplate.FrameDeltaWidth  = 0.35f;
    _DecalSmokeTemplate.FrameDeltaHeight = 0.35f;

    _DecalSmokeTemplate.StartTextureColorCount = 1;

    _DecalSmokeTemplate.StartTextureColors[ 0 ].R = 0.10f;
    _DecalSmokeTemplate.StartTextureColors[ 0 ].G = 0.10f;
    _DecalSmokeTemplate.StartTextureColors[ 0 ].B = 0.10f;
    _DecalSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _DecalSmokeTemplate.FinishTextureColor.R = 0.10f;
    _DecalSmokeTemplate.FinishTextureColor.G = 0.10f;
    _DecalSmokeTemplate.FinishTextureColor.B = 0.10f;
    _DecalSmokeTemplate.FinishTextureColor.A = 0.00f;

    _DecalSmokeTemplate.TextureDeltaColor.R =  0.00f;
    _DecalSmokeTemplate.TextureDeltaColor.G =  0.00f;
    _DecalSmokeTemplate.TextureDeltaColor.B =  0.00f;
    _DecalSmokeTemplate.TextureDeltaColor.A = -0.007f;

    _DecalSmokeTemplate.StartTextureFrame  = 0;
    _DecalSmokeTemplate.FinishTextureFrame = 0;
    _DecalSmokeTemplate.FrameChangeDelay   = 0;
    _DecalSmokeTemplate.NumFrames          = 1;
    _DecalSmokeTemplate.AnimateFrames      = 0;
    _DecalSmokeTemplate.AnimateColor       = 0;

    _DecalSmokeTemplate.LifetimeMin   = -1;
    _DecalSmokeTemplate.LifetimeMax   = -1;

    _DecalSmokeTemplate.Gravity            = 0.0f;
    _DecalSmokeTemplate.AirFriction        = 0.0f;
    _DecalSmokeTemplate.GroundFriction     = 0.0f;

    _DecalSmokeTemplate.CanCollideWithLandscape = 0;
    _DecalSmokeTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _DecalSmokeEmitter.Shape = EMITTERSHAPE_AASQUARE;
    _DecalSmokeEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _DecalSmokeEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _DecalSmokeEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;
    _DecalSmokeEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;

    _DecalSmokeEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _DecalSmokeEmitter.StartMoveVector.X = 0.0f;
    _DecalSmokeEmitter.StartMoveVector.Y = 1.0f;
    _DecalSmokeEmitter.StartMoveVector.Z = 0.0f;

    _DecalSmokeEmitter.StartSpeedMin =  50;
    _DecalSmokeEmitter.StartSpeedMax = 100;
    _DecalSmokeEmitter.SpeedDeltaMin = -20;
    _DecalSmokeEmitter.SpeedDeltaMax =  20;

    _DecalSmokeEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _DecalSmokeEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _DecalSmokeEmitter.EmitAmountMin = 1;
    _DecalSmokeEmitter.EmitAmountMax = 1;

    _DecalSmokeEmitter.EmitDelayMin = 4;
    _DecalSmokeEmitter.EmitDelayMax = 8;

    _DecalSmokeEmitter.EmitLimit = 64;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteDecalSmoke()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteDecalSmoke( void )
{
    DeleteTexture( _DecalSmokeTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartDecalSmoke()
// Desc: 
//-----------------------------------------------------------------------------

void StartDecalSmoke( Vector_t *EmitPoint )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_DecalSmokeTemplate, &_DecalSmokeEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );
}
