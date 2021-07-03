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
#include "Utility.h"


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _RailgunInnerTemplate;
static ParticleEmitter_t _RailgunInnerEmitter;


//-----------------------------------------------------------------------------
// Name: CreateRailgunInner()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateRailgunInner( void )
{
    size_t Width, Height;

    // setup the template

    _RailgunInnerTemplate.TextureID = CreateTexture( "TGA/GeneralSpark.tga", &Width, &Height, 1, 0 );
    if( -1 == _RailgunInnerTemplate.TextureID )
        return 0;

    _RailgunInnerTemplate.TextureUSize = 64.0f / Width;
    _RailgunInnerTemplate.TextureVSize = 64.0f / Height;

    _RailgunInnerTemplate.StartFrameWidth = 12.0f;
    _RailgunInnerTemplate.StartFrameHeight = 12.0f;

    _RailgunInnerTemplate.Type = PARTICLETYPE_BILLBOARD;
    _RailgunInnerTemplate.BSphereSize = 16.0f;

    _RailgunInnerTemplate.FrameDeltaWidth  = 0.0f;
    _RailgunInnerTemplate.FrameDeltaHeight = 0.0f;

    _RailgunInnerTemplate.StartTextureColorCount = 3;

    _RailgunInnerTemplate.StartTextureColors[ 0 ].R = 0.90f;
    _RailgunInnerTemplate.StartTextureColors[ 0 ].G = 0.90f;
    _RailgunInnerTemplate.StartTextureColors[ 0 ].B = 0.00f;
    _RailgunInnerTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _RailgunInnerTemplate.StartTextureColors[ 1 ].R = 0.85f;
    _RailgunInnerTemplate.StartTextureColors[ 1 ].G = 0.80f;
    _RailgunInnerTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _RailgunInnerTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _RailgunInnerTemplate.StartTextureColors[ 2 ].R = 0.80f;
    _RailgunInnerTemplate.StartTextureColors[ 2 ].G = 0.60f;
    _RailgunInnerTemplate.StartTextureColors[ 2 ].B = 0.00f;
    _RailgunInnerTemplate.StartTextureColors[ 2 ].A = 1.00f;

    _RailgunInnerTemplate.FinishTextureColor.R = 0.0f;
    _RailgunInnerTemplate.FinishTextureColor.G = 0.0f;
    _RailgunInnerTemplate.FinishTextureColor.B = 0.0f;
    _RailgunInnerTemplate.FinishTextureColor.A = 0.0f;

    _RailgunInnerTemplate.TextureDeltaColor.R =  0.00f;
    _RailgunInnerTemplate.TextureDeltaColor.G =  0.00f;
    _RailgunInnerTemplate.TextureDeltaColor.B =  0.00f;
    _RailgunInnerTemplate.TextureDeltaColor.A = -0.005f;

    _RailgunInnerTemplate.StartTextureFrame  = 0;
    _RailgunInnerTemplate.FinishTextureFrame = 0;
    _RailgunInnerTemplate.FrameChangeDelay   = 0;
    _RailgunInnerTemplate.NumFrames          = 1;
    _RailgunInnerTemplate.AnimateFrames      = 0;
    _RailgunInnerTemplate.AnimateColor       = 0;

    _RailgunInnerTemplate.LifetimeMin   = -1;
    _RailgunInnerTemplate.LifetimeMax   = -1;

    _RailgunInnerTemplate.Gravity            = 0.0f;
    _RailgunInnerTemplate.AirFriction        = 0.0f;
    _RailgunInnerTemplate.GroundFriction     = 0.0f;

    _RailgunInnerTemplate.CanCollideWithLandscape = 0;
    _RailgunInnerTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _RailgunInnerEmitter.Shape = EMITTERSHAPE_AASQUARE;
    _RailgunInnerEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _RailgunInnerEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _RailgunInnerEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;
    _RailgunInnerEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;

    _RailgunInnerEmitter.SpeedChange = SPEEDCHANGE_NONE;

    _RailgunInnerEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _RailgunInnerEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _RailgunInnerEmitter.EmitAmountMin = 0;
    _RailgunInnerEmitter.EmitAmountMax = 0;

    _RailgunInnerEmitter.EmitDelayMin = 0;
    _RailgunInnerEmitter.EmitDelayMax = 0;

    _RailgunInnerEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteRailgunInner()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteRailgunInner( void )
{
    DeleteTexture( _RailgunInnerTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartRailgunInner()
// Desc: 
//-----------------------------------------------------------------------------

void StartRailgunInner( Vector_t *StartPoint, Vector_t *FinishPoint )
{
    size_t ID = GetFreeParticleSystem();
    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, nullptr, &_RailgunInnerTemplate, &_RailgunInnerEmitter, nullptr );

    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetDrawFunction( ID, StandardBillboardDraw );

    DrawParticleLine( ID, StartPoint, FinishPoint, 0.25f );
}
