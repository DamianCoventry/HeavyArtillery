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

#include "GameModels.h"


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _ShieldLightDebrisTemplate;
static ParticleEmitter_t _ShieldLightDebrisEmitter;


//-----------------------------------------------------------------------------
// Name: CreateShieldLightDebris()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateShieldLightDebris( void )
{
    // setup the template

    _ShieldLightDebrisTemplate.Type = PARTICLETYPE_OBJECT;
    _ShieldLightDebrisTemplate.BSphereSize = 64.0f;

    _ShieldLightDebrisTemplate.LifetimeMin   = 400;
    _ShieldLightDebrisTemplate.LifetimeMax   = 500;

    _ShieldLightDebrisTemplate.Gravity            = 0.05f;
    _ShieldLightDebrisTemplate.AirFriction        = 0.0f;
    _ShieldLightDebrisTemplate.GroundFriction     = 0.0f;

    _ShieldLightDebrisTemplate.CanCollideWithLandscape = 1;
    _ShieldLightDebrisTemplate.CanCollideWithPlayers   = 0;

    _ShieldLightDebrisTemplate.ObjectCount = 3;
    _ShieldLightDebrisTemplate.Objects[ 0 ].ModelID     = g_ModelShieldLightP0;
    _ShieldLightDebrisTemplate.Objects[ 0 ].EmitAmount  = 6;
    _ShieldLightDebrisTemplate.Objects[ 1 ].ModelID     = g_ModelShieldLightP1;
    _ShieldLightDebrisTemplate.Objects[ 1 ].EmitAmount  = 3;
    _ShieldLightDebrisTemplate.Objects[ 2 ].ModelID     = g_ModelShieldLightP2;
    _ShieldLightDebrisTemplate.Objects[ 2 ].EmitAmount  = 3;

    // setup the emitter

    _ShieldLightDebrisEmitter.Shape = EMITTERSHAPE_AABOX;
    _ShieldLightDebrisEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _ShieldLightDebrisEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _ShieldLightDebrisEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _ShieldLightDebrisEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _ShieldLightDebrisEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _ShieldLightDebrisEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _ShieldLightDebrisEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _ShieldLightDebrisEmitter.StartMoveVector.X = 0.0f;
    _ShieldLightDebrisEmitter.StartMoveVector.Y = 1.0f;
    _ShieldLightDebrisEmitter.StartMoveVector.Z = 0.0f;

    _ShieldLightDebrisEmitter.StartSpeedMin = 250;
    _ShieldLightDebrisEmitter.StartSpeedMax = 400;
    _ShieldLightDebrisEmitter.SpeedDeltaMin = -100;
    _ShieldLightDebrisEmitter.SpeedDeltaMax =  100;

    _ShieldLightDebrisEmitter.EmitAmountMin = 12;
    _ShieldLightDebrisEmitter.EmitAmountMax = 12;

    _ShieldLightDebrisEmitter.EmitDelayMin = 0;
    _ShieldLightDebrisEmitter.EmitDelayMax = 0;

    _ShieldLightDebrisEmitter.EmitLimit = 12;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteShieldLightDebris()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteShieldLightDebris( void )
{
    DeleteTexture( _ShieldLightDebrisTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartShieldLightDebris()
// Desc: 
//-----------------------------------------------------------------------------

void StartShieldLightDebris( Vector_t *EmitPoint )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_OBJECT, -1, EmitPoint,
                              &_ShieldLightDebrisTemplate, &_ShieldLightDebrisEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardObjectParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
}
