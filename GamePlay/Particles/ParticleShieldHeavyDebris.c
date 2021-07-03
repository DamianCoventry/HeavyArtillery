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

static ParticleTemplate_t _ShieldHeavyDebrisTemplate;
static ParticleEmitter_t _ShieldHeavyDebrisEmitter;


//-----------------------------------------------------------------------------
// Name: CreateShieldHeavyDebris()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateShieldHeavyDebris( void )
{
    // setup the template

    _ShieldHeavyDebrisTemplate.Type = PARTICLETYPE_OBJECT;
    _ShieldHeavyDebrisTemplate.BSphereSize = 64.0f;

    _ShieldHeavyDebrisTemplate.LifetimeMin   = 400;
    _ShieldHeavyDebrisTemplate.LifetimeMax   = 500;

    _ShieldHeavyDebrisTemplate.Gravity            = 0.05f;
    _ShieldHeavyDebrisTemplate.AirFriction        = 0.0f;
    _ShieldHeavyDebrisTemplate.GroundFriction     = 0.0f;

    _ShieldHeavyDebrisTemplate.CanCollideWithLandscape = 1;
    _ShieldHeavyDebrisTemplate.CanCollideWithPlayers   = 0;

    _ShieldHeavyDebrisTemplate.ObjectCount = 3;
    _ShieldHeavyDebrisTemplate.Objects[ 0 ].ModelID     = g_ModelShieldHeavyP0;
    _ShieldHeavyDebrisTemplate.Objects[ 0 ].EmitAmount  = 6;
    _ShieldHeavyDebrisTemplate.Objects[ 1 ].ModelID     = g_ModelShieldHeavyP1;
    _ShieldHeavyDebrisTemplate.Objects[ 1 ].EmitAmount  = 3;
    _ShieldHeavyDebrisTemplate.Objects[ 2 ].ModelID     = g_ModelShieldHeavyP2;
    _ShieldHeavyDebrisTemplate.Objects[ 2 ].EmitAmount  = 3;

    // setup the emitter

    _ShieldHeavyDebrisEmitter.Shape = EMITTERSHAPE_AABOX;
    _ShieldHeavyDebrisEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _ShieldHeavyDebrisEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _ShieldHeavyDebrisEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _ShieldHeavyDebrisEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _ShieldHeavyDebrisEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _ShieldHeavyDebrisEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _ShieldHeavyDebrisEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _ShieldHeavyDebrisEmitter.StartMoveVector.X = 0.0f;
    _ShieldHeavyDebrisEmitter.StartMoveVector.Y = 1.0f;
    _ShieldHeavyDebrisEmitter.StartMoveVector.Z = 0.0f;

    _ShieldHeavyDebrisEmitter.StartSpeedMin = 250;
    _ShieldHeavyDebrisEmitter.StartSpeedMax = 400;
    _ShieldHeavyDebrisEmitter.SpeedDeltaMin = -100;
    _ShieldHeavyDebrisEmitter.SpeedDeltaMax =  100;

    _ShieldHeavyDebrisEmitter.EmitAmountMin = 12;
    _ShieldHeavyDebrisEmitter.EmitAmountMax = 12;

    _ShieldHeavyDebrisEmitter.EmitDelayMin = 0;
    _ShieldHeavyDebrisEmitter.EmitDelayMax = 0;

    _ShieldHeavyDebrisEmitter.EmitLimit = 12;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteShieldHeavyDebris()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteShieldHeavyDebris( void )
{
    DeleteTexture( _ShieldHeavyDebrisTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartShieldHeavyDebris()
// Desc: 
//-----------------------------------------------------------------------------

void StartShieldHeavyDebris( Vector_t *EmitPoint )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_OBJECT, -1, EmitPoint,
                              &_ShieldHeavyDebrisTemplate, &_ShieldHeavyDebrisEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardObjectParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
}
