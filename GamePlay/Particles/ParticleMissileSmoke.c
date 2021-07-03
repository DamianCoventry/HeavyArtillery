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

static ParticleTemplate_t _MissileSmokeTemplate;
static ParticleEmitter_t _MissileSmokeEmitter;


//-----------------------------------------------------------------------------
// Name: CreateMissileSmoke()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateMissileSmoke( void )
{
    size_t Width, Height;

    // setup the template

    _MissileSmokeTemplate.TextureID = CreateTexture( "TGA/MissileSmoke.tga", &Width, &Height, 1, 0 );
    if( -1 == _MissileSmokeTemplate.TextureID )
        return 0;

    _MissileSmokeTemplate.TextureUSize = 128.0f / Width;
    _MissileSmokeTemplate.TextureVSize = 128.0f / Height;

    _MissileSmokeTemplate.StartFrameWidth = 8.0f;
    _MissileSmokeTemplate.StartFrameHeight = 8.0f;

    _MissileSmokeTemplate.Type = PARTICLETYPE_BILLBOARD;
    _MissileSmokeTemplate.BSphereSize = 64.0f;

    _MissileSmokeTemplate.FrameDeltaWidth  = 0.35f;
    _MissileSmokeTemplate.FrameDeltaHeight = 0.35f;

    _MissileSmokeTemplate.StartTextureColorCount = 4;

    _MissileSmokeTemplate.StartTextureColors[ 0 ].R = 1.00f;
    _MissileSmokeTemplate.StartTextureColors[ 0 ].G = 1.00f;
    _MissileSmokeTemplate.StartTextureColors[ 0 ].B = 0.10f;
    _MissileSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _MissileSmokeTemplate.StartTextureColors[ 1 ].R = 1.00f;
    _MissileSmokeTemplate.StartTextureColors[ 1 ].G = 0.50f;
    _MissileSmokeTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _MissileSmokeTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _MissileSmokeTemplate.StartTextureColors[ 2 ].R = 0.90f;
    _MissileSmokeTemplate.StartTextureColors[ 2 ].G = 0.85f;
    _MissileSmokeTemplate.StartTextureColors[ 2 ].B = 0.10f;
    _MissileSmokeTemplate.StartTextureColors[ 2 ].A = 1.00f;

    _MissileSmokeTemplate.StartTextureColors[ 3 ].R = 0.90f;
    _MissileSmokeTemplate.StartTextureColors[ 3 ].G = 0.40f;
    _MissileSmokeTemplate.StartTextureColors[ 3 ].B = 0.10f;
    _MissileSmokeTemplate.StartTextureColors[ 3 ].A = 1.00f;

    _MissileSmokeTemplate.FinishTextureColor.R = 0.1f;
    _MissileSmokeTemplate.FinishTextureColor.G = 0.1f;
    _MissileSmokeTemplate.FinishTextureColor.B = 0.1f;
    _MissileSmokeTemplate.FinishTextureColor.A = 0.0f;

    _MissileSmokeTemplate.TextureDeltaColor.R = -0.01f;
    _MissileSmokeTemplate.TextureDeltaColor.G = -0.01f;
    _MissileSmokeTemplate.TextureDeltaColor.B =  0.00f;
    _MissileSmokeTemplate.TextureDeltaColor.A = -0.06f;

    _MissileSmokeTemplate.StartTextureFrame  = 0;
    _MissileSmokeTemplate.FinishTextureFrame = 0;
    _MissileSmokeTemplate.FrameChangeDelay   = 0;
    _MissileSmokeTemplate.NumFrames          = 1;
    _MissileSmokeTemplate.AnimateFrames      = 0;
    _MissileSmokeTemplate.AnimateColor       = 1;

    _MissileSmokeTemplate.LifetimeMin   = -1;
    _MissileSmokeTemplate.LifetimeMax   = -1;

    _MissileSmokeTemplate.Gravity            = 0.0f;
    _MissileSmokeTemplate.AirFriction        = 0.0f;
    _MissileSmokeTemplate.GroundFriction     = 0.0f;

    _MissileSmokeTemplate.CanCollideWithLandscape = 0;
    _MissileSmokeTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _MissileSmokeEmitter.Shape = EMITTERSHAPE_AABOX;
    _MissileSmokeEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _MissileSmokeEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _MissileSmokeEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _MissileSmokeEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _MissileSmokeEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _MissileSmokeEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _MissileSmokeEmitter.SpeedChange = SPEEDCHANGE_NONE;

    _MissileSmokeEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _MissileSmokeEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _MissileSmokeEmitter.EmitAmountMin = 1;
    _MissileSmokeEmitter.EmitAmountMax = 1;

    _MissileSmokeEmitter.EmitDelayMin = 0;
    _MissileSmokeEmitter.EmitDelayMax = 0;

    _MissileSmokeEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteMissileSmoke()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteMissileSmoke( void )
{
    DeleteTexture( _MissileSmokeTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartMissileSmoke()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartMissileSmoke( size_t ObjectID )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_MissileSmokeTemplate, &_MissileSmokeEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
