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

static ParticleTemplate_t _DebrisTankTemplateHellCat;
static ParticleTemplate_t _DebrisTankTemplateLocust;

static ParticleEmitter_t _DebrisTankEmitter;


//-----------------------------------------------------------------------------
// Name: CreateDebrisTank()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateDebrisTank( void )
{
    // setup the template

    _DebrisTankTemplateHellCat.Type = PARTICLETYPE_OBJECT;
    _DebrisTankTemplateHellCat.BSphereSize = 64.0f;

    _DebrisTankTemplateHellCat.LifetimeMin   = 350;
    _DebrisTankTemplateHellCat.LifetimeMax   = 450;

    _DebrisTankTemplateHellCat.Gravity          = 0.05f;
    _DebrisTankTemplateHellCat.AirFriction      = 0.0f;
    _DebrisTankTemplateHellCat.GroundFriction   = 0.05f;

    _DebrisTankTemplateHellCat.CanCollideWithLandscape = 1;
    _DebrisTankTemplateHellCat.CanCollideWithPlayers   = 0;

    _DebrisTankTemplateHellCat.ObjectCount = 11;
    _DebrisTankTemplateHellCat.Objects[ 0 ].ModelID     = g_ModelDebrisHellCatP0;
    _DebrisTankTemplateHellCat.Objects[ 0 ].EmitAmount  = 2;
    _DebrisTankTemplateHellCat.Objects[ 1 ].ModelID     = g_ModelDebrisHellCatP1;
    _DebrisTankTemplateHellCat.Objects[ 1 ].EmitAmount  = 2;
    _DebrisTankTemplateHellCat.Objects[ 2 ].ModelID     = g_ModelDebrisHellCatP2;
    _DebrisTankTemplateHellCat.Objects[ 2 ].EmitAmount  = 2;
    _DebrisTankTemplateHellCat.Objects[ 3 ].ModelID     = g_ModelDebrisHellCatP3;
    _DebrisTankTemplateHellCat.Objects[ 3 ].EmitAmount  = 2;
    _DebrisTankTemplateHellCat.Objects[ 4 ].ModelID     = g_ModelDebrisHellCatP4;
    _DebrisTankTemplateHellCat.Objects[ 4 ].EmitAmount  = 2;
    _DebrisTankTemplateHellCat.Objects[ 5 ].ModelID     = g_ModelDebrisHellCatP5;
    _DebrisTankTemplateHellCat.Objects[ 5 ].EmitAmount  = 2;
    _DebrisTankTemplateHellCat.Objects[ 6 ].ModelID     = g_ModelDebrisGeneral0;
    _DebrisTankTemplateHellCat.Objects[ 6 ].EmitAmount  = 3;
    _DebrisTankTemplateHellCat.Objects[ 7 ].ModelID     = g_ModelDebrisGeneral1;
    _DebrisTankTemplateHellCat.Objects[ 7 ].EmitAmount  = 3;
    _DebrisTankTemplateHellCat.Objects[ 8 ].ModelID     = g_ModelDebrisGeneral2;
    _DebrisTankTemplateHellCat.Objects[ 8 ].EmitAmount  = 3;
    _DebrisTankTemplateHellCat.Objects[ 9 ].ModelID     = g_ModelDebrisGeneral3;
    _DebrisTankTemplateHellCat.Objects[ 9 ].EmitAmount  = 3;
    _DebrisTankTemplateHellCat.Objects[ 10 ].ModelID    = g_ModelDebrisGeneral4;
    _DebrisTankTemplateHellCat.Objects[ 10 ].EmitAmount = 3;

    memcpy( &_DebrisTankTemplateLocust, &_DebrisTankTemplateHellCat, sizeof( ParticleTemplate_t ) );
    _DebrisTankTemplateLocust.Objects[ 0 ].ModelID    = g_ModelDebrisLocustP0;
    _DebrisTankTemplateLocust.Objects[ 1 ].ModelID    = g_ModelDebrisLocustP1;
    _DebrisTankTemplateLocust.Objects[ 2 ].ModelID    = g_ModelDebrisLocustP2;
    _DebrisTankTemplateLocust.Objects[ 3 ].ModelID    = g_ModelDebrisLocustP3;
    _DebrisTankTemplateLocust.Objects[ 4 ].ModelID    = g_ModelDebrisLocustP4;
    _DebrisTankTemplateLocust.Objects[ 5 ].ModelID    = g_ModelDebrisLocustP5;

    // setup the emitter

    _DebrisTankEmitter.Shape = EMITTERSHAPE_AABOX;
    _DebrisTankEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _DebrisTankEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _DebrisTankEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _DebrisTankEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _DebrisTankEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _DebrisTankEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _DebrisTankEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _DebrisTankEmitter.StartMoveVector.X = 0.0f;
    _DebrisTankEmitter.StartMoveVector.Y = 1.0f;
    _DebrisTankEmitter.StartMoveVector.Z = 0.0f;

    _DebrisTankEmitter.StartSpeedMin = 250;
    _DebrisTankEmitter.StartSpeedMax = 400;
    _DebrisTankEmitter.SpeedDeltaMin = -100;
    _DebrisTankEmitter.SpeedDeltaMax =  100;

    _DebrisTankEmitter.EmitAmountMin = 27;
    _DebrisTankEmitter.EmitAmountMax = 27;

    _DebrisTankEmitter.EmitDelayMin = 0;
    _DebrisTankEmitter.EmitDelayMax = 0;

    _DebrisTankEmitter.EmitLimit = 27;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteDebrisTank()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteDebrisTank( void )
{
    DeleteTexture( _DebrisTankTemplateHellCat.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartDebrisTank()
// Desc: 
//-----------------------------------------------------------------------------

void StartDebrisTank( Vector_t *EmitPoint, Vector_t *Normal, size_t ModelID )
{
    size_t ID = GetFreeParticleSystem();
    ParticleTemplate_t *Template;

    if( ModelID == g_ModelTank0LOD0 )
        Template = &_DebrisTankTemplateHellCat;
    else
        Template = &_DebrisTankTemplateLocust;

    VectorCopy( (*Normal), _DebrisTankEmitter.StartMoveVector );

    InitialiseParticleSystem( ID, PARTICLETYPE_OBJECT, -1, EmitPoint, Template, &_DebrisTankEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardObjectParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
}
