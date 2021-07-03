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

#include "GameModels.h"
#include "TileSet.h"

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _DebrisGroundTemplateDirt;
static ParticleTemplate_t _DebrisGroundTemplateGrass;
static ParticleTemplate_t _DebrisGroundTemplateRock;
static ParticleTemplate_t _DebrisGroundTemplateSnow;

static ParticleEmitter_t _DebrisGroundEmitter;


//-----------------------------------------------------------------------------
// Name: CreateDebrisGround()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateDebrisGround( void )
{
    // setup the template

    _DebrisGroundTemplateDirt.Type = PARTICLETYPE_OBJECT;
    _DebrisGroundTemplateDirt.BSphereSize = 64.0f;

    _DebrisGroundTemplateDirt.LifetimeMin   = 350;
    _DebrisGroundTemplateDirt.LifetimeMax   = 450;

    _DebrisGroundTemplateDirt.Gravity            = 0.05f;
    _DebrisGroundTemplateDirt.AirFriction        = 0.0f;
    _DebrisGroundTemplateDirt.GroundFriction     = 0.05f;

    _DebrisGroundTemplateDirt.CanCollideWithLandscape = 1;
    _DebrisGroundTemplateDirt.CanCollideWithPlayers   = 0;

    _DebrisGroundTemplateDirt.ObjectCount = 2;
    _DebrisGroundTemplateDirt.Objects[ 0 ].ModelID     = g_ModelDebrisRockDirtP0;
    _DebrisGroundTemplateDirt.Objects[ 0 ].EmitAmount  = 2;
    _DebrisGroundTemplateDirt.Objects[ 1 ].ModelID     = g_ModelDebrisRockDirtP1;
    _DebrisGroundTemplateDirt.Objects[ 1 ].EmitAmount  = 2;

    memcpy( &_DebrisGroundTemplateGrass, &_DebrisGroundTemplateDirt, sizeof( ParticleTemplate_t ) );
    _DebrisGroundTemplateGrass.Objects[ 0 ].ModelID    = g_ModelDebrisRockGrassP0;
    _DebrisGroundTemplateGrass.Objects[ 1 ].ModelID    = g_ModelDebrisRockGrassP1;

    memcpy( &_DebrisGroundTemplateRock, &_DebrisGroundTemplateDirt, sizeof( ParticleTemplate_t ) );
    _DebrisGroundTemplateRock.Objects[ 0 ].ModelID     = g_ModelDebrisRockRockP0;
    _DebrisGroundTemplateRock.Objects[ 1 ].ModelID     = g_ModelDebrisRockRockP1;

    memcpy( &_DebrisGroundTemplateSnow, &_DebrisGroundTemplateDirt, sizeof( ParticleTemplate_t ) );
    _DebrisGroundTemplateSnow.Objects[ 0 ].ModelID     = g_ModelDebrisRockSnowP0;
    _DebrisGroundTemplateSnow.Objects[ 1 ].ModelID     = g_ModelDebrisRockSnowP1;

    // setup the emitter

    _DebrisGroundEmitter.Shape = EMITTERSHAPE_AABOX;
    _DebrisGroundEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _DebrisGroundEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _DebrisGroundEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _DebrisGroundEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _DebrisGroundEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _DebrisGroundEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _DebrisGroundEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _DebrisGroundEmitter.StartMoveVector.X = 0.0f;
    _DebrisGroundEmitter.StartMoveVector.Y = 1.0f;
    _DebrisGroundEmitter.StartMoveVector.Z = 0.0f;

    _DebrisGroundEmitter.StartSpeedMin = 250;
    _DebrisGroundEmitter.StartSpeedMax = 400;
    _DebrisGroundEmitter.SpeedDeltaMin = -100;
    _DebrisGroundEmitter.SpeedDeltaMax =  100;

    _DebrisGroundEmitter.EmitAmountMin = 4;
    _DebrisGroundEmitter.EmitAmountMax = 4;

    _DebrisGroundEmitter.EmitDelayMin = 0;
    _DebrisGroundEmitter.EmitDelayMax = 0;

    _DebrisGroundEmitter.EmitLimit = 4;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteDebrisGround()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteDebrisGround( void )
{
    DeleteTexture( _DebrisGroundTemplateDirt.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartDebrisGround()
// Desc: 
//-----------------------------------------------------------------------------

void StartDebrisGround( Vector_t *EmitPoint, Vector_t *Normal )
{
    size_t ID = GetFreeParticleSystem();
    ParticleTemplate_t *Template;

    switch( GetMeshTileSetType( g_LandscapeMesh ) )
    {
        case TILESET_MUD   :
        case TILESET_SAND  :
        case TILESET_DIRT  :
            Template = &_DebrisGroundTemplateDirt;
            break;

        case TILESET_GRASS :
            Template = &_DebrisGroundTemplateGrass;
            break;

        case TILESET_SNOW  :
            Template = &_DebrisGroundTemplateSnow;
            break;

        case TILESET_ROCK  :
        case TILESET_STONE :
        default :
            Template = &_DebrisGroundTemplateRock;
            break;
    }

    VectorCopy( (*Normal), _DebrisGroundEmitter.StartMoveVector );

    InitialiseParticleSystem( ID, PARTICLETYPE_OBJECT, -1, EmitPoint, Template, &_DebrisGroundEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardObjectParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
}
