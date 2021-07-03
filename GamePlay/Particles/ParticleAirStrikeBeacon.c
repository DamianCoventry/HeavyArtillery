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

static ParticleTemplate_t _AirStrikeBeaconTemplate;
static ParticleEmitter_t _AirStrikeBeaconEmitter;


//-----------------------------------------------------------------------------
// Name: CreateAirStrikeBeacon()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateAirStrikeBeacon( void )
{
    size_t Width, Height;

    // setup the template

    _AirStrikeBeaconTemplate.TextureID = CreateTexture( "TGA/AirStrikeBeacon.tga", &Width, &Height, 1, 0 );
    if( -1 == _AirStrikeBeaconTemplate.TextureID )
        return 0;

    _AirStrikeBeaconTemplate.TextureUSize = 128.0f / Width;
    _AirStrikeBeaconTemplate.TextureVSize = 128.0f / Height;

    _AirStrikeBeaconTemplate.StartFrameWidth = 32.0f;
    _AirStrikeBeaconTemplate.StartFrameHeight = 32.0f;

    _AirStrikeBeaconTemplate.Type = PARTICLETYPE_BILLBOARD;
    _AirStrikeBeaconTemplate.BSphereSize = 64.0f;

    _AirStrikeBeaconTemplate.FrameDeltaWidth  = 0.5f;
    _AirStrikeBeaconTemplate.FrameDeltaHeight = 0.5f;

    _AirStrikeBeaconTemplate.StartTextureColorCount = 1;

    _AirStrikeBeaconTemplate.StartTextureColors[ 0 ].R = 1.0f;
    _AirStrikeBeaconTemplate.StartTextureColors[ 0 ].G = 0.0f;
    _AirStrikeBeaconTemplate.StartTextureColors[ 0 ].B = 0.0f;
    _AirStrikeBeaconTemplate.StartTextureColors[ 0 ].A = 1.0f;

    _AirStrikeBeaconTemplate.FinishTextureColor.R = 0.5f;
    _AirStrikeBeaconTemplate.FinishTextureColor.G = 0.0f;
    _AirStrikeBeaconTemplate.FinishTextureColor.B = 0.0f;
    _AirStrikeBeaconTemplate.FinishTextureColor.A = 0.0f;

    _AirStrikeBeaconTemplate.TextureDeltaColor.R =  0.5f;
    _AirStrikeBeaconTemplate.TextureDeltaColor.G =  0.0f;
    _AirStrikeBeaconTemplate.TextureDeltaColor.B =  0.5f;
    _AirStrikeBeaconTemplate.TextureDeltaColor.A = -0.005f;

    _AirStrikeBeaconTemplate.StartTextureFrame  = 0;
    _AirStrikeBeaconTemplate.FinishTextureFrame = 0;
    _AirStrikeBeaconTemplate.FrameChangeDelay   = 0;
    _AirStrikeBeaconTemplate.NumFrames          = 1;
    _AirStrikeBeaconTemplate.AnimateFrames      = 0;
    _AirStrikeBeaconTemplate.AnimateColor       = 0;

    _AirStrikeBeaconTemplate.LifetimeMin   = -1;
    _AirStrikeBeaconTemplate.LifetimeMax   = -1;

    _AirStrikeBeaconTemplate.Gravity            = 0.0f;
    _AirStrikeBeaconTemplate.AirFriction        = 0.0f;
    _AirStrikeBeaconTemplate.GroundFriction     = 0.0f;

    _AirStrikeBeaconTemplate.CanCollideWithLandscape = 0;
    _AirStrikeBeaconTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _AirStrikeBeaconEmitter.Shape = EMITTERSHAPE_AASQUARE;
    _AirStrikeBeaconEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _AirStrikeBeaconEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _AirStrikeBeaconEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;
    _AirStrikeBeaconEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;

    _AirStrikeBeaconEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _AirStrikeBeaconEmitter.StartMoveVector.X = 0.0f;
    _AirStrikeBeaconEmitter.StartMoveVector.Y = 1.0f;
    _AirStrikeBeaconEmitter.StartMoveVector.Z = 0.0f;

    _AirStrikeBeaconEmitter.StartSpeedMin =  75;
    _AirStrikeBeaconEmitter.StartSpeedMax = 125;
    _AirStrikeBeaconEmitter.SpeedDeltaMin = -30;
    _AirStrikeBeaconEmitter.SpeedDeltaMax =  30;

    _AirStrikeBeaconEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _AirStrikeBeaconEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _AirStrikeBeaconEmitter.EmitAmountMin = 1;
    _AirStrikeBeaconEmitter.EmitAmountMax = 1;

    _AirStrikeBeaconEmitter.EmitDelayMin = 0;
    _AirStrikeBeaconEmitter.EmitDelayMax = 2;

    _AirStrikeBeaconEmitter.EmitLimit = 0;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteAirStrikeBeacon()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteAirStrikeBeacon( void )
{
    DeleteTexture( _AirStrikeBeaconTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartAirStrikeBeacon()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartAirStrikeBeacon( Vector_t *EmitPoint )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_AirStrikeBeaconTemplate, &_AirStrikeBeaconEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}
