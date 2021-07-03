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

static ParticleTemplate_t _GeneralSparkTemplate;
static ParticleEmitter_t _GeneralSparkEmitter;


//-----------------------------------------------------------------------------
// Name: CreateGeneralSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateGeneralSpark( void )
{
    size_t Width, Height;

    // setup the template

    _GeneralSparkTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _GeneralSparkTemplate.TextureID )
        return 0;

    _GeneralSparkTemplate.TextureUSize = 64.0f / Width;
    _GeneralSparkTemplate.TextureVSize = 64.0f / Height;

    _GeneralSparkTemplate.StartFrameWidth = 8.0f;
    _GeneralSparkTemplate.StartFrameHeight = 8.0f;

    _GeneralSparkTemplate.Type = PARTICLETYPE_BILLBOARD;
    _GeneralSparkTemplate.BSphereSize = 64.0f;

    _GeneralSparkTemplate.FrameDeltaWidth  = 0.0f;
    _GeneralSparkTemplate.FrameDeltaHeight = 0.0f;

    _GeneralSparkTemplate.StartTextureColorCount = 2;

    _GeneralSparkTemplate.StartTextureColors[ 0 ].R = 1.00f;
    _GeneralSparkTemplate.StartTextureColors[ 0 ].G = 1.00f;
    _GeneralSparkTemplate.StartTextureColors[ 0 ].B = 0.10f;
    _GeneralSparkTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _GeneralSparkTemplate.StartTextureColors[ 1 ].R = 1.00f;
    _GeneralSparkTemplate.StartTextureColors[ 1 ].G = 0.50f;
    _GeneralSparkTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _GeneralSparkTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _GeneralSparkTemplate.FinishTextureColor.R = 0.0f;
    _GeneralSparkTemplate.FinishTextureColor.G = 0.0f;
    _GeneralSparkTemplate.FinishTextureColor.B = 0.0f;
    _GeneralSparkTemplate.FinishTextureColor.A = 0.0f;

    _GeneralSparkTemplate.TextureDeltaColor.R =  0.00f;
    _GeneralSparkTemplate.TextureDeltaColor.G =  0.00f;
    _GeneralSparkTemplate.TextureDeltaColor.B =  0.00f;
    _GeneralSparkTemplate.TextureDeltaColor.A = -0.05f;

    _GeneralSparkTemplate.StartTextureFrame  = 0;
    _GeneralSparkTemplate.FinishTextureFrame = 0;
    _GeneralSparkTemplate.FrameChangeDelay   = 0;
    _GeneralSparkTemplate.NumFrames          = 1;
    _GeneralSparkTemplate.AnimateFrames      = 0;
    _GeneralSparkTemplate.AnimateColor       = 0;

    _GeneralSparkTemplate.LifetimeMin   = -1;
    _GeneralSparkTemplate.LifetimeMax   = -1;

    _GeneralSparkTemplate.Gravity            = 0.05f;
    _GeneralSparkTemplate.AirFriction        = 0.0f;
    _GeneralSparkTemplate.GroundFriction     = 0.0f;

    _GeneralSparkTemplate.CanCollideWithLandscape = 0;
    _GeneralSparkTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _GeneralSparkEmitter.Shape = EMITTERSHAPE_AABOX;
    _GeneralSparkEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _GeneralSparkEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _GeneralSparkEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _GeneralSparkEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _GeneralSparkEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _GeneralSparkEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _GeneralSparkEmitter.SpeedChange = SPEEDCHANGE_NONE;

    _GeneralSparkEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _GeneralSparkEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _GeneralSparkEmitter.EmitAmountMin = 1;
    _GeneralSparkEmitter.EmitAmountMax = 3;

    _GeneralSparkEmitter.EmitDelayMin = 0;
    _GeneralSparkEmitter.EmitDelayMax = 2;

    _GeneralSparkEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteGeneralSpark()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteGeneralSpark( void )
{
    DeleteTexture( _GeneralSparkTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartGeneralSpark()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartGeneralSpark( size_t ObjectID )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, ObjectID, nullptr, &_GeneralSparkTemplate, &_GeneralSparkEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
