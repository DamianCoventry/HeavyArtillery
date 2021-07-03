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
#include "MeshManager.h"

#include "Object.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _GroundSmokeTemplate;
static ParticleEmitter_t _GroundSmokeEmitter;


//-----------------------------------------------------------------------------
// Name: CreateGroundSmoke()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateGroundSmoke( void )
{
    size_t Width, Height;

    // setup the template

    _GroundSmokeTemplate.TextureID = CreateTexture( "TGA/MissileSmoke.tga", &Width, &Height, 1, 0 );
    if( -1 == _GroundSmokeTemplate.TextureID )
        return 0;

    _GroundSmokeTemplate.TextureUSize = 128.0f / Width;
    _GroundSmokeTemplate.TextureVSize = 128.0f / Height;

    _GroundSmokeTemplate.StartFrameWidth = 8.0f;
    _GroundSmokeTemplate.StartFrameHeight = 8.0f;

    _GroundSmokeTemplate.Type = PARTICLETYPE_BILLBOARD;
    _GroundSmokeTemplate.BSphereSize = 64.0f;

    _GroundSmokeTemplate.FrameDeltaWidth  = 0.35f;
    _GroundSmokeTemplate.FrameDeltaHeight = 0.35f;

    _GroundSmokeTemplate.StartTextureColorCount = 1;

    _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.90f;
    _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.50f;
    _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.20f;
    _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;

    _GroundSmokeTemplate.FinishTextureColor.R = 0.10f;
    _GroundSmokeTemplate.FinishTextureColor.G = 0.10f;
    _GroundSmokeTemplate.FinishTextureColor.B = 0.10f;
    _GroundSmokeTemplate.FinishTextureColor.A = 0.00f;

    _GroundSmokeTemplate.TextureDeltaColor.R =  0.00f;
    _GroundSmokeTemplate.TextureDeltaColor.G =  0.00f;
    _GroundSmokeTemplate.TextureDeltaColor.B =  0.00f;
    _GroundSmokeTemplate.TextureDeltaColor.A = -0.01f;

    _GroundSmokeTemplate.StartTextureFrame  = 0;
    _GroundSmokeTemplate.FinishTextureFrame = 0;
    _GroundSmokeTemplate.FrameChangeDelay   = 0;
    _GroundSmokeTemplate.NumFrames          = 1;
    _GroundSmokeTemplate.AnimateFrames      = 0;
    _GroundSmokeTemplate.AnimateColor       = 0;

    _GroundSmokeTemplate.LifetimeMin   = -1;
    _GroundSmokeTemplate.LifetimeMax   = -1;

    _GroundSmokeTemplate.Gravity            = 0.0f;
    _GroundSmokeTemplate.AirFriction        = 0.0f;
    _GroundSmokeTemplate.GroundFriction     = 0.0f;

    _GroundSmokeTemplate.CanCollideWithLandscape = 0;
    _GroundSmokeTemplate.CanCollideWithPlayers   = 0;

    // setup the emitter

    _GroundSmokeEmitter.Shape = EMITTERSHAPE_AABOX;
    _GroundSmokeEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -750;
    _GroundSmokeEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  750;
    _GroundSmokeEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -750;
    _GroundSmokeEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  750;
    _GroundSmokeEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -750;
    _GroundSmokeEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  750;

    _GroundSmokeEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _GroundSmokeEmitter.StartMoveVector.X = 0.0f;
    _GroundSmokeEmitter.StartMoveVector.Y = 1.0f;
    _GroundSmokeEmitter.StartMoveVector.Z = 0.0f;

    _GroundSmokeEmitter.StartSpeedMin = 5;
    _GroundSmokeEmitter.StartSpeedMax = 20;
    _GroundSmokeEmitter.SpeedDeltaMin = -5;
    _GroundSmokeEmitter.SpeedDeltaMax =  5;

    _GroundSmokeEmitter.GLBlendSrc = GL_SRC_ALPHA;
    _GroundSmokeEmitter.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _GroundSmokeEmitter.EmitAmountMin = 4;
    _GroundSmokeEmitter.EmitAmountMax = 4;

    _GroundSmokeEmitter.EmitDelayMin = 0;
    _GroundSmokeEmitter.EmitDelayMax = 0;

    _GroundSmokeEmitter.EmitLimit = 4;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteGroundSmoke()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteGroundSmoke( void )
{
    DeleteTexture( _GroundSmokeTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartGroundSmoke()
// Desc: 
//-----------------------------------------------------------------------------

void StartGroundSmoke( Vector_t *EmitPoint )
{
    size_t ID = GetFreeParticleSystem();

    switch( GetMeshTileSetType( g_LandscapeMesh ) )
    {
        case TILESET_DIRT :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.488f;        // brownish
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.398f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.316f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
        case TILESET_GRASS :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.320f;        // greenish
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.539f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.304f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
        case TILESET_MUD :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.554f;        // brownish
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.382f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.140f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
        case TILESET_ROCK :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.714f;        // grey
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.714f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.714f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
        case TILESET_SAND :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 1.000f;        // yellowish
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.843f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.003f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
        case TILESET_SNOW :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.746f;        // white with slight blue tint
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.937f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.964f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
        case TILESET_STONE :
        default :
            _GroundSmokeTemplate.StartTextureColors[ 0 ].R = 0.601f;        // darkish grey
            _GroundSmokeTemplate.StartTextureColors[ 0 ].G = 0.562f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].B = 0.507f;
            _GroundSmokeTemplate.StartTextureColors[ 0 ].A = 1.00f;
            break;
    }

    InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_GroundSmokeTemplate, &_GroundSmokeEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardBillboardParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );
}
