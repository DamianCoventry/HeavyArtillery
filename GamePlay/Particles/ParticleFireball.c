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
#include <math.h>

#include "ParticleTypes.h"
#include "ParticleFireball.h"

#include "ParticleManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"

#include "Object.h"

#define NAPALM_DAMAGE       0.1f


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static ParticleTemplate_t _FireballTemplate;
static ParticleEmitter_t _FireballEmitterSm;
static ParticleEmitter_t _FireballEmitterLg;


//-----------------------------------------------------------------------------
// Name: CreateFireball()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateFireball( void )
{
    size_t Width, Height;

    // setup the template

    _FireballTemplate.TextureID = CreateTexture( "TGA/Fireball.tga", &Width, &Height, 1, 0 );
    if( -1 == _FireballTemplate.TextureID )
        return 0;

    _FireballTemplate.TextureUSize = 32.0f / Width;
    _FireballTemplate.TextureVSize = 32.0f / Height;

    _FireballTemplate.StartFrameWidth = 32.0f;
    _FireballTemplate.StartFrameHeight = 32.0f;

    _FireballTemplate.Type = PARTICLETYPE_BILLBOARD;
    _FireballTemplate.BSphereSize = 32.0f;

    _FireballTemplate.FrameDeltaWidth  = 0.0f;
    _FireballTemplate.FrameDeltaHeight = 0.0f;

    _FireballTemplate.StartTextureColorCount = 4;

    _FireballTemplate.StartTextureColors[ 0 ].R = 0.90f;
    _FireballTemplate.StartTextureColors[ 0 ].G = 0.90f;
    _FireballTemplate.StartTextureColors[ 0 ].B = 0.00f;
    _FireballTemplate.StartTextureColors[ 0 ].A = 0.90f;

    _FireballTemplate.StartTextureColors[ 1 ].R = 0.85f;
    _FireballTemplate.StartTextureColors[ 1 ].G = 0.80f;
    _FireballTemplate.StartTextureColors[ 1 ].B = 0.00f;
    _FireballTemplate.StartTextureColors[ 1 ].A = 0.90f;

    _FireballTemplate.StartTextureColors[ 2 ].R = 0.80f;
    _FireballTemplate.StartTextureColors[ 2 ].G = 0.50f;
    _FireballTemplate.StartTextureColors[ 2 ].B = 0.00f;
    _FireballTemplate.StartTextureColors[ 2 ].A = 0.90f;

    _FireballTemplate.StartTextureColors[ 3 ].R = 0.75f;
    _FireballTemplate.StartTextureColors[ 3 ].G = 0.45f;
    _FireballTemplate.StartTextureColors[ 3 ].B = 0.00f;
    _FireballTemplate.StartTextureColors[ 3 ].A = 0.90f;

    _FireballTemplate.FinishTextureColor.R = 0.0f;
    _FireballTemplate.FinishTextureColor.G = 0.0f;
    _FireballTemplate.FinishTextureColor.B = 0.0f;
    _FireballTemplate.FinishTextureColor.A = 0.0f;

    _FireballTemplate.TextureDeltaColor.R = 0.0f;
    _FireballTemplate.TextureDeltaColor.G = 0.0f;
    _FireballTemplate.TextureDeltaColor.B = 0.0f;
    _FireballTemplate.TextureDeltaColor.A = 0.0f;

    _FireballTemplate.StartTextureFrame  = -1;
    _FireballTemplate.FinishTextureFrame = 0;
    _FireballTemplate.FrameChangeDelay   = 2;
    _FireballTemplate.NumFrames          = 16;
    _FireballTemplate.AnimateFrames      = 1;
    _FireballTemplate.AnimateColor       = 0;

    _FireballTemplate.LifetimeMin   = 200;
    _FireballTemplate.LifetimeMax   = 400;

    _FireballTemplate.Gravity           = 0.050f;
    _FireballTemplate.AirFriction       = 0.001f;
    _FireballTemplate.GroundFriction    = 0.005f;

    _FireballTemplate.CanCollideWithLandscape = 1;
    _FireballTemplate.CanCollideWithPlayers   = 1;

    // setup the emitter

    _FireballEmitterSm.Shape = EMITTERSHAPE_AABOX;
    _FireballEmitterSm.Dimensions[ EMITTERSIDE_LEFT   ] = -500;
    _FireballEmitterSm.Dimensions[ EMITTERSIDE_RIGHT  ] =  500;
    _FireballEmitterSm.Dimensions[ EMITTERSIDE_TOP    ] =  500;
    _FireballEmitterSm.Dimensions[ EMITTERSIDE_BOTTOM ] = -500;
    _FireballEmitterSm.Dimensions[ EMITTERSIDE_FRONT  ] =  500;
    _FireballEmitterSm.Dimensions[ EMITTERSIDE_BACK   ] = -500;

    _FireballEmitterSm.SpeedChange = SPEEDCHANGE_ALL;

    _FireballEmitterSm.StartMoveVector.X = 0.0f;
    _FireballEmitterSm.StartMoveVector.Y = 1.0f;
    _FireballEmitterSm.StartMoveVector.Z = 0.0f;

    _FireballEmitterSm.StartSpeedMin = 250;
    _FireballEmitterSm.StartSpeedMax = 400;
    _FireballEmitterSm.SpeedDeltaMin = -45;
    _FireballEmitterSm.SpeedDeltaMax =  45;

    _FireballEmitterSm.GLBlendSrc = GL_SRC_ALPHA;
    _FireballEmitterSm.GLBlendDst = GL_ONE_MINUS_SRC_ALPHA;

    _FireballEmitterSm.EmitAmountMin = 2;
    _FireballEmitterSm.EmitAmountMax = 5;

    _FireballEmitterSm.EmitDelayMin =  5;
    _FireballEmitterSm.EmitDelayMax = 10;

    _FireballEmitterSm.EmitLimit = 128;

    memcpy( &_FireballEmitterLg, &_FireballEmitterSm, sizeof( ParticleEmitter_t ) );
    _FireballEmitterLg.EmitLimit = 256;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteFireball()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteFireball( void )
{
    DeleteTexture( _FireballTemplate.TextureID );
}


//-----------------------------------------------------------------------------
// Name: StartFireball()
// Desc: 
//-----------------------------------------------------------------------------

size_t StartFireball( Vector_t *EmitPoint, size_t IsSmall )
{
    size_t ID = GetFreeParticleSystem();

    if( IsSmall )
        InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_FireballTemplate, &_FireballEmitterSm, nullptr );
    else
        InitialiseParticleSystem( ID, PARTICLETYPE_BILLBOARD, -1, EmitPoint, &_FireballTemplate, &_FireballEmitterLg, nullptr );

    SetEmitterThinkFunction( ID, StandardEmitterThink );
    SetThinkFunction( ID, NapalmParticleThink );
    SetMoveFunction( ID, StandardParticleMove );
    SetCollideFunction( ID, StandardParticleCollide );
    SetOnCollideObjectFunction( ID, CollideParticleWithObject );
    SetDrawFunction( ID, StandardBillboardDraw );

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: NapalmParticleThink()
// Desc: 
//----------------------------------------------------------------------------------------

size_t NapalmParticleThink( ParticleSystem_t *ParticleSystem )
{
    size_t i, PlyrIndex;
    float Distance;
    Particle_t *Particle;
    Vector_t Position;

    for( i = 0; i < MAXPARTICLESINSYSTEM; i++ )
    {
        Particle = &ParticleSystem->Particles[ i ];

        if( PARTICLESTATE_DEAD != Particle->State )
        {
            // issue damage to any player close to this particle
            Distance = Particle->BSphereSize * 2.0f;
            for( PlyrIndex = 0; PlyrIndex < GetPlayerCount(); PlyrIndex++ )
            {
                if( !IsPlayerDead( PlyrIndex ) )
                {
                    GetPlayerPosition( PlyrIndex, &Position );
                    if( fabsf( VectorMagnitude2( &Position, &Particle->Position ) ) <= Distance )
                        PlayerTakeDamage( PlyrIndex, NAPALM_DAMAGE );
                }
            }

            // animate the billboard size
            Particle->FrameWidth  += ParticleSystem->Template->FrameDeltaWidth;
            Particle->FrameHeight += ParticleSystem->Template->FrameDeltaHeight;

            Particle->FrameHalfWidth  = Particle->FrameWidth  / 2.0f;
            Particle->FrameHalfHeight = Particle->FrameHeight / 2.0f;

            // optionally animate the billboard image
            if( ParticleSystem->Template->AnimateFrames )
            {
                if( ++Particle->FrameCounter >= ParticleSystem->Template->FrameChangeDelay )
                {
                    Particle->FrameCounter = 0;
                    if( ++Particle->CurrentFrame >= ParticleSystem->Template->NumFrames )
                        Particle->CurrentFrame = 0; 
                }
            }
            Particle->TexX = (float)Particle->CurrentFrame * ParticleSystem->Template->TextureUSize;

            if( ParticleSystem->Template->AnimateColor )
            {
                // animate the billboard color
                Particle->TextureColor.R += ParticleSystem->Template->TextureDeltaColor.R;
                if( ParticleSystem->Template->TextureDeltaColor.R < 0.0f && Particle->TextureColor.R < ParticleSystem->Template->FinishTextureColor.R )
                    Particle->TextureColor.R = ParticleSystem->Template->FinishTextureColor.R;
                else if( ParticleSystem->Template->TextureDeltaColor.R > 0.0f && Particle->TextureColor.R > ParticleSystem->Template->FinishTextureColor.R )
                    Particle->TextureColor.R = ParticleSystem->Template->FinishTextureColor.R;

                Particle->TextureColor.G += ParticleSystem->Template->TextureDeltaColor.G;
                if( ParticleSystem->Template->TextureDeltaColor.G < 0.0f && Particle->TextureColor.G < ParticleSystem->Template->FinishTextureColor.G )
                    Particle->TextureColor.G = ParticleSystem->Template->FinishTextureColor.G;
                else if( ParticleSystem->Template->TextureDeltaColor.G > 0.0f && Particle->TextureColor.G > ParticleSystem->Template->FinishTextureColor.G )
                    Particle->TextureColor.G = ParticleSystem->Template->FinishTextureColor.G;

                Particle->TextureColor.B += ParticleSystem->Template->TextureDeltaColor.B;
                if( ParticleSystem->Template->TextureDeltaColor.B < 0.0f && Particle->TextureColor.B < ParticleSystem->Template->FinishTextureColor.B )
                    Particle->TextureColor.B = ParticleSystem->Template->FinishTextureColor.B;
                else if( ParticleSystem->Template->TextureDeltaColor.B > 0.0f && Particle->TextureColor.B > ParticleSystem->Template->FinishTextureColor.B )
                    Particle->TextureColor.B = ParticleSystem->Template->FinishTextureColor.B;
            }

            Particle->TextureColor.A += ParticleSystem->Template->TextureDeltaColor.A;
            if( ParticleSystem->Template->TextureDeltaColor.A < 0.0f && Particle->TextureColor.A < ParticleSystem->Template->FinishTextureColor.A )
                Particle->TextureColor.A = ParticleSystem->Template->FinishTextureColor.A;
            else if( ParticleSystem->Template->TextureDeltaColor.A > 0.0f && Particle->TextureColor.A > ParticleSystem->Template->FinishTextureColor.A )
                Particle->TextureColor.A = ParticleSystem->Template->FinishTextureColor.A;

            if( Particle->TextureColor.A <= 0.0f )
            {
                Particle->State = PARTICLESTATE_DEAD;

                ParticleSystem->ActiveParticleCount--;
                if( ParticleSystem->ActiveParticleCount == 0 )
                    ParticleSystem->State = PARTICLESTATE_DEAD;
            }

            if( Particle->LifetimeMax != -1 )
            {
                if( ++Particle->LifetimeCounter >= Particle->LifetimeMax )
                {
                    Particle->State = PARTICLESTATE_DEAD;

                    ParticleSystem->ActiveParticleCount--;
                    if( ParticleSystem->ActiveParticleCount == 0 )
                        ParticleSystem->State = PARTICLESTATE_DEAD;
                }
            }
        }
    }

    // return true if the entire particle system is still alive
    return( PARTICLESTATE_ALIVE == ParticleSystem->State );
}
