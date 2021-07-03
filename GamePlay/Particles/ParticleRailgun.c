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

static ParticleTemplate_t _RailgunTemplate;
static ParticleEmitter_t _RailgunEmitter;


//-----------------------------------------------------------------------------
// Name: CreateRailgun()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateRailgun( void )
{
    size_t Width, Height;

    // setup the template

    _RailgunTemplate.TextureID = CreateTexture( "TGA/Railgun.tga", &Width, &Height, 1, 0 );
    if( -1 == _RailgunTemplate.TextureID )
        return 0;

    _RailgunTemplate.TextureUSize = 32.0f / Width;
    _RailgunTemplate.TextureVSize = 32.0f / Height;

    _RailgunTemplate.StartFrameWidth = 32.0f;
    _RailgunTemplate.StartFrameHeight = 32.0f;

    _RailgunTemplate.Type = PARTICLETYPE_BILLBOARD;
    _RailgunTemplate.BSphereSize = 64.0f;

    _RailgunTemplate.FrameDeltaWidth  = 0.0f;
    _RailgunTemplate.FrameDeltaHeight = 0.0f;

    _RailgunTemplate.StartTextureColorCount = 3;

    _RailgunTemplate.StartTextureColors[ 0 ].R = 0.00f;
    _RailgunTemplate.StartTextureColors[ 0 ].G = 0.35f;
    _RailgunTemplate.StartTextureColors[ 0 ].B = 0.55f;
    _RailgunTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _RailgunTemplate.StartTextureColors[ 1 ].R = 0.00f;
    _RailgunTemplate.StartTextureColors[ 1 ].G = 0.25f;
    _RailgunTemplate.StartTextureColors[ 1 ].B = 0.75f;
    _RailgunTemplate.StartTextureColors[ 1 ].A = 1.00f;

    _RailgunTemplate.StartTextureColors[ 2 ].R = 0.00f;
    _RailgunTemplate.StartTextureColors[ 2 ].G = 0.15f;
    _RailgunTemplate.StartTextureColors[ 2 ].B = 0.95f;
    _RailgunTemplate.StartTextureColors[ 2 ].A = 1.00f;

    _RailgunTemplate.FinishTextureColor.R = 0.0f;
    _RailgunTemplate.FinishTextureColor.G = 0.0f;
    _RailgunTemplate.FinishTextureColor.B = 0.0f;
    _RailgunTemplate.FinishTextureColor.A = 0.0f;

    _RailgunTemplate.TextureDeltaColor.R =  0.00f;
    _RailgunTemplate.TextureDeltaColor.G =  0.00f;
    _RailgunTemplate.TextureDeltaColor.B =  0.00f;
    _RailgunTemplate.TextureDeltaColor.A = -0.005f;

    _RailgunTemplate.StartTextureFrame  = -1;
    _RailgunTemplate.FinishTextureFrame = 0;
    _RailgunTemplate.FrameChangeDelay   = 5;
    _RailgunTemplate.NumFrames          = 16;
    _RailgunTemplate.AnimateFrames      = 1;
    _RailgunTemplate.AnimateColor       = 0;

    _RailgunTemplate.LifetimeMin   = -1;
    _RailgunTemplate.LifetimeMax   = -1;

    _RailgunTemplate.Gravity            = 0.0f;
    _RailgunTemplate.AirFriction        = 0.0f;
    _RailgunTemplate.GroundFriction     = 0.0f;

    _RailgunTemplate.CanCollideWithLandscape = 0;
    _RailgunTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _RailgunEmitter.Shape = EMITTERSHAPE_AASQUARE;
    _RailgunEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _RailgunEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _RailgunEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;
    _RailgunEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;

    _RailgunEmitter.SpeedChange = SPEEDCHANGE_NONE;

    _RailgunEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _RailgunEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _RailgunEmitter.EmitAmountMin = 0;
    _RailgunEmitter.EmitAmountMax = 0;

    _RailgunEmitter.EmitDelayMin = 0;
    _RailgunEmitter.EmitDelayMax = 0;

    _RailgunEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteRailgun()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteRailgun( void )
{
    DeleteTexture( _RailgunTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartRailgun()
// Desc: 
//-----------------------------------------------------------------------------

void StartRailgun( Vector_t *StartPoint, Vector_t *FinishPoint )
{
    size_t ID = GetFreeParticleSystem();
    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, nullptr, &_RailgunTemplate, &_RailgunEmitter, nullptr );

    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetDrawFunction( ID, StandardBillboardDraw );

    DrawParticleLine( ID, StartPoint, FinishPoint, 1.0f );
}
