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

static ParticleTemplate_t _CentralExplosionTemplate;
static ParticleEmitter_t _CentralExplosionEmitter;


//-----------------------------------------------------------------------------
// Name: CreateCentralExplosion()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateCentralExplosion( void )
{
    size_t Width, Height;

    // setup the template

    _CentralExplosionTemplate.TextureID = CreateTexture( "TGA/CentralExplosion.tga", &Width, &Height, 1, 0 );
    if( -1 == _CentralExplosionTemplate.TextureID )
        return 0;

    _CentralExplosionTemplate.TextureUSize = 128.0f / Width;
    _CentralExplosionTemplate.TextureVSize = 128.0f / Height;

    _CentralExplosionTemplate.StartFrameWidth = 32.0f;
    _CentralExplosionTemplate.StartFrameHeight = 32.0f;

    _CentralExplosionTemplate.Type = PARTICLETYPE_BILLBOARD;
    _CentralExplosionTemplate.BSphereSize = 64.0f;

    _CentralExplosionTemplate.FrameDeltaWidth  = -0.05f;
    _CentralExplosionTemplate.FrameDeltaHeight = -0.05f;

    _CentralExplosionTemplate.StartTextureColorCount = 1;

    _CentralExplosionTemplate.StartTextureColors[ 0 ].R = 1.0f;
    _CentralExplosionTemplate.StartTextureColors[ 0 ].G = 1.0f;
    _CentralExplosionTemplate.StartTextureColors[ 0 ].B = 1.0f;
    _CentralExplosionTemplate.StartTextureColors[ 0 ].A = 1.0f;

    _CentralExplosionTemplate.FinishTextureColor.R = 0.0f;
    _CentralExplosionTemplate.FinishTextureColor.G = 0.0f;
    _CentralExplosionTemplate.FinishTextureColor.B = 0.0f;
    _CentralExplosionTemplate.FinishTextureColor.A = 0.0f;

    _CentralExplosionTemplate.TextureDeltaColor.R = 0.0f;
    _CentralExplosionTemplate.TextureDeltaColor.G = 0.0f;
    _CentralExplosionTemplate.TextureDeltaColor.B = 0.0f;
    _CentralExplosionTemplate.TextureDeltaColor.A = -0.01f;

    _CentralExplosionTemplate.StartTextureFrame  = 0;
    _CentralExplosionTemplate.FinishTextureFrame = 0;
    _CentralExplosionTemplate.FrameChangeDelay   = 0;
    _CentralExplosionTemplate.NumFrames          = 1;
    _CentralExplosionTemplate.AnimateFrames      = 0;
    _CentralExplosionTemplate.AnimateColor       = 0;

    _CentralExplosionTemplate.LifetimeMin   = -1;
    _CentralExplosionTemplate.LifetimeMax   = -1;

    _CentralExplosionTemplate.Gravity            = 0.0f;
    _CentralExplosionTemplate.AirFriction        = 0.0f;
    _CentralExplosionTemplate.GroundFriction     = 0.0f;

    _CentralExplosionTemplate.CanCollideWithLandscape = 0;
    _CentralExplosionTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _CentralExplosionEmitter.Shape = EMITTERSHAPE_AASQUARE;
    _CentralExplosionEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _CentralExplosionEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _CentralExplosionEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;
    _CentralExplosionEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;

    _CentralExplosionEmitter.SpeedChange = SPEEDCHANGE_NONE;

    _CentralExplosionEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _CentralExplosionEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _CentralExplosionEmitter.EmitAmountMin = 0;
    _CentralExplosionEmitter.EmitAmountMax = 0;

    _CentralExplosionEmitter.EmitDelayMin = 0;
    _CentralExplosionEmitter.EmitDelayMax = 0;

    _CentralExplosionEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteCentralExplosion()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteCentralExplosion( void )
{
    DeleteTexture( _CentralExplosionTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartCentralExplosion()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartCentralExplosion( Vector_t *EmitPoint, ExplosionType_t *ExplType )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_CentralExplosionTemplate, &_CentralExplosionEmitter, nullptr );

    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetDrawFunction( ID, StandardBillboardDraw );

    SetupCentralExplosion( ID, EmitPoint, ExplType, &_CentralExplosionTemplate );

    return ID;
}
