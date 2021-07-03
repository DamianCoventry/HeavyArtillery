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

static ParticleTemplate_t _ShieldMediumDebrisTemplate;
static ParticleEmitter_t _ShieldMediumDebrisEmitter;


//-----------------------------------------------------------------------------
// Name: CreateShieldMediumDebris()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateShieldMediumDebris( void )
{
    // setup the template

    _ShieldMediumDebrisTemplate.Type = PARTICLETYPE_OBJECT;
    _ShieldMediumDebrisTemplate.BSphereSize = 64.0f;

    _ShieldMediumDebrisTemplate.LifetimeMin   = 400;
    _ShieldMediumDebrisTemplate.LifetimeMax   = 500;

    _ShieldMediumDebrisTemplate.Gravity            = 0.05f;
    _ShieldMediumDebrisTemplate.AirFriction        = 0.0f;
    _ShieldMediumDebrisTemplate.GroundFriction     = 0.0f;

    _ShieldMediumDebrisTemplate.CanCollideWithLandscape = 1;
    _ShieldMediumDebrisTemplate.CanCollideWithPlayers   = 0;

    _ShieldMediumDebrisTemplate.ObjectCount = 3;
    _ShieldMediumDebrisTemplate.Objects[ 0 ].ModelID     = g_ModelShieldMediumP0;
    _ShieldMediumDebrisTemplate.Objects[ 0 ].EmitAmount  = 6;
    _ShieldMediumDebrisTemplate.Objects[ 1 ].ModelID     = g_ModelShieldMediumP1;
    _ShieldMediumDebrisTemplate.Objects[ 1 ].EmitAmount  = 3;
    _ShieldMediumDebrisTemplate.Objects[ 2 ].ModelID     = g_ModelShieldMediumP2;
    _ShieldMediumDebrisTemplate.Objects[ 2 ].EmitAmount  = 3;

    // setup the emitter

    _ShieldMediumDebrisEmitter.Shape = EMITTERSHAPE_AABOX;
    _ShieldMediumDebrisEmitter.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _ShieldMediumDebrisEmitter.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _ShieldMediumDebrisEmitter.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _ShieldMediumDebrisEmitter.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _ShieldMediumDebrisEmitter.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _ShieldMediumDebrisEmitter.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _ShieldMediumDebrisEmitter.SpeedChange = SPEEDCHANGE_ALL;

    _ShieldMediumDebrisEmitter.StartMoveVector.X = 0.0f;
    _ShieldMediumDebrisEmitter.StartMoveVector.Y = 1.0f;
    _ShieldMediumDebrisEmitter.StartMoveVector.Z = 0.0f;

    _ShieldMediumDebrisEmitter.StartSpeedMin = 250;
    _ShieldMediumDebrisEmitter.StartSpeedMax = 400;
    _ShieldMediumDebrisEmitter.SpeedDeltaMin = -100;
    _ShieldMediumDebrisEmitter.SpeedDeltaMax =  100;

    _ShieldMediumDebrisEmitter.EmitAmountMin = 12;
    _ShieldMediumDebrisEmitter.EmitAmountMax = 12;

    _ShieldMediumDebrisEmitter.EmitDelayMin = 0;
    _ShieldMediumDebrisEmitter.EmitDelayMax = 0;

    _ShieldMediumDebrisEmitter.EmitLimit = 12;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteShieldMediumDebris()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteShieldMediumDebris( void )
{
    DeleteTexture( _ShieldMediumDebrisTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartShieldMediumDebris()
// Desc: 
//-----------------------------------------------------------------------------

void StartShieldMediumDebris( Vector_t *EmitPoint )
{
    size_t ID = GetFreeParticleSystem();

    InitialiseParticleSystem( ID, PARTICLETYPE_OBJECT, -1, EmitPoint,
                              &_ShieldMediumDebrisTemplate, &_ShieldMediumDebrisEmitter, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, StandardObjectParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
}
