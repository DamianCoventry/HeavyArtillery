//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXCHARACTERS           100
#define MAXPARTICLESYSTEMS      128
#define RAILGUN_STEPSIZE        50.0f
#define SNOWSTORM_STEPCOUNT     64

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXPARTICLESYSTEMS || !_ParticlePool[ _id ].Used )

#include "Vector.h"
#include "ParticleTypes.h"
#include "ExplosionTypes.h"
#include "GameSounds.h"

#include "MeshManager.h"
#include "ObjectManager.h"
#include "PlayerManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "SoundManager.h"

#include "Object.h"
#include "DisplayList.h"
#include "Utility.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _ParticleIndex = 0;
static ParticleSystem_t _ParticlePool[ MAXPARTICLESYSTEMS ];


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static void CreateDebrisPiece(size_t SystemID, Particle_t* Particle, size_t ModelID);


//----------------------------------------------------------------------------------------
// Name: GetFreeParticleSystem()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetFreeParticleSystem(void)
{
    size_t i;
    size_t ID;

    i = _ParticleIndex;

    while (_ParticlePool[ i ].Used)
    {
        i++;
        if (i >= MAXPARTICLESYSTEMS)
            i = 0;

        if (i == _ParticleIndex)
            return -1;        // pool is full
    }

    _ParticlePool[ i ].Used = 1;
    ID = i;

    _ParticleIndex = i;
    if (++_ParticleIndex >= MAXPARTICLESYSTEMS)
        _ParticleIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnParticleSystem()
// Desc:
//----------------------------------------------------------------------------------------

void ReturnParticleSystem(size_t ID)
{
    if (INVALIDID(ID))
        return;

    memset(&_ParticlePool[ ID ], 0, sizeof(ParticleSystem_t));
}


//----------------------------------------------------------------------------------------
// Name: ReturnAllParticleSystems()
// Desc:
//----------------------------------------------------------------------------------------

void ReturnAllParticleSystems(void)
{
    size_t i;

    for (i = 0; i < MAXPARTICLESYSTEMS; i++)
        memset(&_ParticlePool[ i ], 0, sizeof(ParticleSystem_t));
}


//----------------------------------------------------------------------------------------
// Name: ParticleManagerThink()
// Desc:
//----------------------------------------------------------------------------------------

void ParticleManagerThink(void)
{
    size_t i;

    for (i = 0; i < MAXPARTICLESYSTEMS; i++)
    {
        if (_ParticlePool[ i ].Used && _ParticlePool[ i ].State != PARTICLESTATE_DEAD && _ParticlePool[ i ].Think)
        {
            if (_ParticlePool[ i ].EmitterThink)
                _ParticlePool[ i ].EmitterThink(&_ParticlePool[ i ]);

            if (!_ParticlePool[ i ].Think(&_ParticlePool[ i ]))
            {
                if (_ParticlePool[ i ].FinishCallback)
                    _ParticlePool[ i ].FinishCallback(_ParticlePool[ i ].ID);

                ReturnParticleSystem(i);
            }
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: ParticleManagerMove()
// Desc:
//----------------------------------------------------------------------------------------

void ParticleManagerMove(void)
{
    size_t i;

    for (i = 0; i < MAXPARTICLESYSTEMS; i++)
    {
        if (_ParticlePool[ i ].Used && _ParticlePool[ i ].State != PARTICLESTATE_DEAD && _ParticlePool[ i ].Move)
            _ParticlePool[ i ].Move(&_ParticlePool[ i ]);
    }
}


//----------------------------------------------------------------------------------------
// Name: ParticleManagerCollide()
// Desc:
//----------------------------------------------------------------------------------------

void ParticleManagerCollide(void)
{
    size_t i;

    for (i = 0; i < MAXPARTICLESYSTEMS; i++)
    {
        if (_ParticlePool[ i ].Used && _ParticlePool[ i ].State != PARTICLESTATE_DEAD && _ParticlePool[ i ].Collide)
            _ParticlePool[ i ].Collide(&_ParticlePool[ i ]);
    }
}


//----------------------------------------------------------------------------------------
// Name: ParticleManagerDraw()
// Desc:
//----------------------------------------------------------------------------------------

void ParticleManagerDraw(Plane2_t* ViewFrustum)
{
    size_t SystemIndex;
    float ModelViewMatrix[ 16 ];
    ParticleSystem_t* System;

    glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix);

    ResetDisplayList();

    for (SystemIndex = 0; SystemIndex < MAXPARTICLESYSTEMS; SystemIndex++)
    {
        System = &_ParticlePool[ SystemIndex ];

        if (System->Used && System->State != PARTICLESTATE_DEAD)
        {
            if (PARTICLETYPE_BILLBOARD == System->Type)
                AddParticlesToDisplayList(System, ModelViewMatrix);
            // else objects are drawn by the object manager
        }
    }

    glDepthMask(GL_FALSE);
    DrawDisplayList(ViewFrustum, ModelViewMatrix);
    glDepthMask(GL_TRUE);
}


//----------------------------------------------------------------------------------------
// Name: StopEmittingParticles()
// Desc:
//----------------------------------------------------------------------------------------

void StopEmittingParticles(size_t ID)
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].StopEmitting = 1;
}


//----------------------------------------------------------------------------------------
// Name: InitialiseParticleSystem()
// Desc:
//----------------------------------------------------------------------------------------

void InitialiseParticleSystem(size_t ID, ParticleType_t Type, size_t ObjectID,
                              Vector_t* EmitPoint, ParticleTemplate_t* Template, ParticleEmitter_t* Emitter,
                              void (*FinishCallback)(size_t ID))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].ID      = ID;
    _ParticlePool[ ID ].Type    = Type;
    _ParticlePool[ ID ].State   = PARTICLESTATE_ALIVE;

    _ParticlePool[ ID ].AttachedObjectID    = ObjectID;
    _ParticlePool[ ID ].EmitDelayCounter    = 0;
    _ParticlePool[ ID ].StopEmitting        = 0;
    _ParticlePool[ ID ].AmountEmitted       = 0;

    memset(_ParticlePool[ ID ].Objects, 0, sizeof(ParticleObject_t) * PARTICLETEMPLATE_MAXOBJECTS);

    if (Emitter)
    {
        if (Emitter->EmitDelayMax != Emitter->EmitDelayMin)
            _ParticlePool[ ID ].EmitDelayThreshold = (rand() % (Emitter->EmitDelayMax - Emitter->EmitDelayMin)) + Emitter->EmitDelayMin;
        else
            _ParticlePool[ ID ].EmitDelayThreshold = Emitter->EmitDelayMin;

        if (EmitPoint)
            VectorCopy((*EmitPoint), _ParticlePool[ ID ].EmitPoint);
    }

    _ParticlePool[ ID ].Template    = Template;
    _ParticlePool[ ID ].Emitter     = Emitter;

    _ParticlePool[ ID ].ActiveParticleCount = 0;
    _ParticlePool[ ID ].FinishCallback      = FinishCallback;

    memset(_ParticlePool[ ID ].Particles, 0, sizeof(Particle_t));

    _ParticlePool[ ID ].Think   = nullptr;
    _ParticlePool[ ID ].Move    = nullptr;
    _ParticlePool[ ID ].Collide = nullptr;
    _ParticlePool[ ID ].Draw    = nullptr;
}


//----------------------------------------------------------------------------------------
// Name: SetEmitterThinkFunction()
// Desc:
//----------------------------------------------------------------------------------------

void SetEmitterThinkFunction(size_t ID, size_t (*EmitterThink)(struct ParticleSystem_s* Self))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].EmitterThink = EmitterThink;
}


//----------------------------------------------------------------------------------------
// Name: SetThinkFunction()
// Desc:
//----------------------------------------------------------------------------------------

void SetThinkFunction(size_t ID, size_t (*Think)(struct ParticleSystem_s* Self))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].Think = Think;
}


//----------------------------------------------------------------------------------------
// Name: SetMoveFunction()
// Desc:
//----------------------------------------------------------------------------------------

void SetMoveFunction(size_t ID, void (*Move)(struct ParticleSystem_s* Self))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].Move = Move;
}


//----------------------------------------------------------------------------------------
// Name: SetCollideFunction()
// Desc:
//----------------------------------------------------------------------------------------

void SetCollideFunction(size_t ID, void (*Collide)(struct ParticleSystem_s* Self))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].Collide = Collide;
}


//----------------------------------------------------------------------------------------
// Name: SetDrawFunction()
// Desc:
//----------------------------------------------------------------------------------------

void SetDrawFunction(size_t ID, void (*Draw)(struct ParticleSystem_s* Self, Particle_t* Particle, Vector_t* Position, float* ModelViewMatrix))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].Draw = Draw;
}


//----------------------------------------------------------------------------------------
// Name: SetOnCollideObjectFunction()
// Desc:
//----------------------------------------------------------------------------------------

void SetOnCollideObjectFunction(size_t ID, void (*OnCollideObject)(ParticleSystem_t*, Particle_t*, struct Object_s*))
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].OnCollideObject = OnCollideObject;
}


//----------------------------------------------------------------------------------------
// Name: StandardEmitterThink()
// Desc:
//----------------------------------------------------------------------------------------

size_t StandardEmitterThink(ParticleSystem_t* ParticleSystem)
{
    size_t i, j, Index;
    size_t Size, Speed, EmitAmount;
    Particle_t* Particle;
    Vector_t UnitVec;

    if (ParticleSystem->StopEmitting || !ParticleSystem->Used)
        return 0;

    if (++ParticleSystem->EmitDelayCounter > ParticleSystem->EmitDelayThreshold)
    {
        // reset the counter and pick a new threshold
        ParticleSystem->EmitDelayCounter   = 0;

        if (ParticleSystem->Emitter->EmitDelayMax != ParticleSystem->Emitter->EmitDelayMin)
            ParticleSystem->EmitDelayThreshold = (rand() % (ParticleSystem->Emitter->EmitDelayMax - ParticleSystem->Emitter->EmitDelayMin)) + ParticleSystem->Emitter->EmitDelayMin;
        else
            ParticleSystem->EmitDelayThreshold = ParticleSystem->Emitter->EmitDelayMin;

        if (ParticleSystem->Emitter->EmitAmountMax != ParticleSystem->Emitter->EmitAmountMin)
            EmitAmount = (rand() % (ParticleSystem->Emitter->EmitAmountMax - ParticleSystem->Emitter->EmitAmountMin)) + ParticleSystem->Emitter->EmitAmountMin;
        else
            EmitAmount = ParticleSystem->Emitter->EmitAmountMin;

        for (i = 0; i < EmitAmount; i++)
        {
            // find a free array index
            for (Index = 0; Index < MAXPARTICLESINSYSTEM; Index++)
            {
                if (ParticleSystem->Particles[ Index ].State == PARTICLESTATE_DEAD)
                    break;
            }

            if (Index >= MAXPARTICLESINSYSTEM)
                return 0;   // the pool is full

            // setup the new particle
            Particle = &ParticleSystem->Particles[ Index ];
            Particle->State = PARTICLESTATE_ALIVE;
            Particle->BSphereSize = ParticleSystem->Template->BSphereSize;

            Particle->LifetimeCounter = 0;
            if (ParticleSystem->Template->LifetimeMin != -1)
            {
                if (ParticleSystem->Template->LifetimeMin != ParticleSystem->Template->LifetimeMax)
                    Particle->LifetimeMax = RandRange(ParticleSystem->Template->LifetimeMin, ParticleSystem->Template->LifetimeMax);
                else
                    Particle->LifetimeMax = ParticleSystem->Template->LifetimeMin;
            }
            else
                Particle->LifetimeMax = -1;

            // choose an initial position
            if (ParticleSystem->AttachedObjectID != -1)
                GetObjectPosition(ParticleSystem->AttachedObjectID, 0, &Particle->Position);
            else
                VectorCopy(ParticleSystem->EmitPoint, Particle->Position);

            switch (ParticleSystem->Emitter->Shape)
            {
            case EMITTERSHAPE_POINT :
                break;

            case EMITTERSHAPE_AASQUARE :
                Particle->Position.X += (float)RandRange(
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_LEFT ],
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_RIGHT ]) / 100.0f;

                Particle->Position.Z += (float)RandRange(
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_BACK ],
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_FRONT ]) / 100.0f;
                break;

            case EMITTERSHAPE_AABOX :
                Particle->Position.X += (float)RandRange(
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_LEFT ],
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_RIGHT ]) / 100.0f;

                Particle->Position.Y += (float)RandRange(
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_BOTTOM ],
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_TOP ]) / 100.0f;

                Particle->Position.Z += (float)RandRange(
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_BACK ],
                                            (long)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_FRONT ]) / 100.0f;
                break;

            case EMITTERSHAPE_SPHERE :
                Size = (size_t)((float)(rand() % ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_LEFT ]) / 100.0f);
                VectorGetMoveXY((float)((size_t)rand() % 90), (float)((size_t)rand() % 360), &UnitVec);

                VectorScale(UnitVec, (float)Size, UnitVec);
                VectorAdd(Particle->Position, UnitVec, Particle->Position);

                break;
            }

            // setup the initial speed
            VectorCopy(ParticleSystem->Emitter->StartMoveVector, Particle->Move);
            switch (ParticleSystem->Emitter->SpeedChange)
            {
            case SPEEDCHANGE_X :
                if (ParticleSystem->Emitter->StartSpeedMax != ParticleSystem->Emitter->StartSpeedMin)
                    Size = (rand() % (ParticleSystem->Emitter->StartSpeedMax - ParticleSystem->Emitter->StartSpeedMin)) + ParticleSystem->Emitter->StartSpeedMin;
                else
                    Size = ParticleSystem->Emitter->StartSpeedMin;

                if (ParticleSystem->Emitter->SpeedDeltaMax != ParticleSystem->Emitter->SpeedDeltaMin)
                    Speed = (rand() % (ParticleSystem->Emitter->SpeedDeltaMax - ParticleSystem->Emitter->SpeedDeltaMin)) + ParticleSystem->Emitter->SpeedDeltaMin;
                else
                    Speed = ParticleSystem->Emitter->SpeedDeltaMin;

                Particle->Move.X += (float)Speed / 100.0f;
                VectorScale(Particle->Move, (float)Size / 100.0f, Particle->Move);
                break;

            case SPEEDCHANGE_Y :
                if (ParticleSystem->Emitter->StartSpeedMax != ParticleSystem->Emitter->StartSpeedMin)
                    Size = (rand() % (ParticleSystem->Emitter->StartSpeedMax - ParticleSystem->Emitter->StartSpeedMin)) + ParticleSystem->Emitter->StartSpeedMin;
                else
                    Size = ParticleSystem->Emitter->StartSpeedMin;

                if (ParticleSystem->Emitter->SpeedDeltaMax != ParticleSystem->Emitter->SpeedDeltaMin)
                    Speed = (rand() % (ParticleSystem->Emitter->SpeedDeltaMax - ParticleSystem->Emitter->SpeedDeltaMin)) + ParticleSystem->Emitter->SpeedDeltaMin;
                else
                    Speed = ParticleSystem->Emitter->SpeedDeltaMin;

                Particle->Move.Y += (float)Speed / 100.0f;
                VectorScale(Particle->Move, (float)Size / 100.0f, Particle->Move);
                break;

            case SPEEDCHANGE_Z :
                if (ParticleSystem->Emitter->StartSpeedMax != ParticleSystem->Emitter->StartSpeedMin)
                    Size = (rand() % (ParticleSystem->Emitter->StartSpeedMax - ParticleSystem->Emitter->StartSpeedMin)) + ParticleSystem->Emitter->StartSpeedMin;
                else
                    Size = ParticleSystem->Emitter->StartSpeedMin;

                if (ParticleSystem->Emitter->SpeedDeltaMax != ParticleSystem->Emitter->SpeedDeltaMin)
                    Speed = (rand() % (ParticleSystem->Emitter->SpeedDeltaMax - ParticleSystem->Emitter->SpeedDeltaMin)) + ParticleSystem->Emitter->SpeedDeltaMin;
                else
                    Speed = ParticleSystem->Emitter->SpeedDeltaMin;

                Particle->Move.Z += (float)Speed / 100.0f;
                VectorScale(Particle->Move, (float)Size / 100.0f, Particle->Move);
                break;

            case SPEEDCHANGE_NONE :
                break;

            case SPEEDCHANGE_ALL :
                if (ParticleSystem->Emitter->StartSpeedMax != ParticleSystem->Emitter->StartSpeedMin)
                    Size = (rand() % (ParticleSystem->Emitter->StartSpeedMax - ParticleSystem->Emitter->StartSpeedMin)) + ParticleSystem->Emitter->StartSpeedMin;
                else
                    Size = ParticleSystem->Emitter->StartSpeedMin;

                if (ParticleSystem->Emitter->SpeedDeltaMax != ParticleSystem->Emitter->SpeedDeltaMin)
                    Speed = (rand() % (ParticleSystem->Emitter->SpeedDeltaMax - ParticleSystem->Emitter->SpeedDeltaMin)) + ParticleSystem->Emitter->SpeedDeltaMin;
                else
                    Speed = ParticleSystem->Emitter->SpeedDeltaMin;

                Particle->Move.X += (float)Speed / 100.0f;

                if (ParticleSystem->Emitter->SpeedDeltaMax != ParticleSystem->Emitter->SpeedDeltaMin)
                    Speed = (rand() % (ParticleSystem->Emitter->SpeedDeltaMax - ParticleSystem->Emitter->SpeedDeltaMin)) + ParticleSystem->Emitter->SpeedDeltaMin;
                else
                    Speed = ParticleSystem->Emitter->SpeedDeltaMin;

                Particle->Move.Y += (float)Speed / 100.0f;

                if (ParticleSystem->Emitter->SpeedDeltaMax != ParticleSystem->Emitter->SpeedDeltaMin)
                    Speed = (rand() % (ParticleSystem->Emitter->SpeedDeltaMax - ParticleSystem->Emitter->SpeedDeltaMin)) + ParticleSystem->Emitter->SpeedDeltaMin;
                else
                    Speed = ParticleSystem->Emitter->SpeedDeltaMin;

                Particle->Move.Z += (float)Speed / 100.0f;

                VectorScale(Particle->Move, (float)Size / 100.0f, Particle->Move);
                break;
            }

            // setup the visual aspects of the particle
            switch (ParticleSystem->Template->Type)
            {
            case PARTICLETYPE_BILLBOARD :
                Index = rand() % ParticleSystem->Template->StartTextureColorCount;
                memcpy(&Particle->TextureColor, &ParticleSystem->Template->StartTextureColors[ Index ], sizeof(Color_t));

                Particle->FrameWidth  = ParticleSystem->Template->StartFrameWidth;
                Particle->FrameHeight = ParticleSystem->Template->StartFrameHeight;

                if (ParticleSystem->Template->StartTextureFrame == -1)
                    Particle->CurrentFrame = rand() % ParticleSystem->Template->NumFrames;
                else
                    Particle->CurrentFrame = ParticleSystem->Template->StartTextureFrame;

                break;

            case PARTICLETYPE_OBJECT :
                for (j = 0; j < ParticleSystem->Template->ObjectCount; j++)
                {
                    if (ParticleSystem->Objects[ j ].NumEmitted < ParticleSystem->Template->Objects[ j ].EmitAmount)
                    {
                        // then we can emit an object of this type
                        ParticleSystem->Objects[ j ].NumEmitted++;
                        CreateDebrisPiece(ParticleSystem->ID, Particle, ParticleSystem->Template->Objects[ j ].ModelID);
                        break;
                    }
                }
                break;
            }

            ParticleSystem->ActiveParticleCount++;

            if (ParticleSystem->Emitter->EmitLimit > 0)
            {
                if (++ParticleSystem->AmountEmitted >= ParticleSystem->Emitter->EmitLimit)
                    ParticleSystem->StopEmitting = 1;
            }
        }
    }

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StandardBillboardParticleThink()
// Desc:
//----------------------------------------------------------------------------------------

size_t StandardBillboardParticleThink(ParticleSystem_t* ParticleSystem)
{
    size_t i;
    Particle_t* Particle;

    for (i = 0; i < MAXPARTICLESINSYSTEM; i++)
    {
        Particle = &ParticleSystem->Particles[ i ];

        if (PARTICLESTATE_DEAD != Particle->State)
        {
            // animate the billboard size
            Particle->FrameWidth  += ParticleSystem->Template->FrameDeltaWidth;
            Particle->FrameHeight += ParticleSystem->Template->FrameDeltaHeight;

            Particle->FrameHalfWidth  = Particle->FrameWidth  / 2.0f;
            Particle->FrameHalfHeight = Particle->FrameHeight / 2.0f;

            // optionally animate the billboard image
            if (ParticleSystem->Template->AnimateFrames)
            {
                if (++Particle->FrameCounter >= ParticleSystem->Template->FrameChangeDelay)
                {
                    Particle->FrameCounter = 0;
                    if (++Particle->CurrentFrame >= ParticleSystem->Template->NumFrames)
                        Particle->CurrentFrame = 0;
                }
            }
            Particle->TexX = (float)Particle->CurrentFrame * ParticleSystem->Template->TextureUSize;

            if (ParticleSystem->Template->AnimateColor)
            {
                // animate the billboard color
                Particle->TextureColor.R += ParticleSystem->Template->TextureDeltaColor.R;
                if (ParticleSystem->Template->TextureDeltaColor.R < 0.0f && Particle->TextureColor.R < ParticleSystem->Template->FinishTextureColor.R)
                    Particle->TextureColor.R = ParticleSystem->Template->FinishTextureColor.R;
                else if (ParticleSystem->Template->TextureDeltaColor.R > 0.0f && Particle->TextureColor.R > ParticleSystem->Template->FinishTextureColor.R)
                    Particle->TextureColor.R = ParticleSystem->Template->FinishTextureColor.R;

                Particle->TextureColor.G += ParticleSystem->Template->TextureDeltaColor.G;
                if (ParticleSystem->Template->TextureDeltaColor.G < 0.0f && Particle->TextureColor.G < ParticleSystem->Template->FinishTextureColor.G)
                    Particle->TextureColor.G = ParticleSystem->Template->FinishTextureColor.G;
                else if (ParticleSystem->Template->TextureDeltaColor.G > 0.0f && Particle->TextureColor.G > ParticleSystem->Template->FinishTextureColor.G)
                    Particle->TextureColor.G = ParticleSystem->Template->FinishTextureColor.G;

                Particle->TextureColor.B += ParticleSystem->Template->TextureDeltaColor.B;
                if (ParticleSystem->Template->TextureDeltaColor.B < 0.0f && Particle->TextureColor.B < ParticleSystem->Template->FinishTextureColor.B)
                    Particle->TextureColor.B = ParticleSystem->Template->FinishTextureColor.B;
                else if (ParticleSystem->Template->TextureDeltaColor.B > 0.0f && Particle->TextureColor.B > ParticleSystem->Template->FinishTextureColor.B)
                    Particle->TextureColor.B = ParticleSystem->Template->FinishTextureColor.B;
            }

            Particle->TextureColor.A += ParticleSystem->Template->TextureDeltaColor.A;
            if (ParticleSystem->Template->TextureDeltaColor.A < 0.0f && Particle->TextureColor.A < ParticleSystem->Template->FinishTextureColor.A)
                Particle->TextureColor.A = ParticleSystem->Template->FinishTextureColor.A;
            else if (ParticleSystem->Template->TextureDeltaColor.A > 0.0f && Particle->TextureColor.A > ParticleSystem->Template->FinishTextureColor.A)
                Particle->TextureColor.A = ParticleSystem->Template->FinishTextureColor.A;

            if (Particle->TextureColor.A <= 0.0f)
            {
                Particle->State = PARTICLESTATE_DEAD;

                ParticleSystem->ActiveParticleCount--;
                if (ParticleSystem->ActiveParticleCount == 0)
                    ParticleSystem->State = PARTICLESTATE_DEAD;
            }

            if (Particle->LifetimeMax != -1)
            {
                if (++Particle->LifetimeCounter >= Particle->LifetimeMax)
                {
                    Particle->State = PARTICLESTATE_DEAD;

                    ParticleSystem->ActiveParticleCount--;
                    if (ParticleSystem->ActiveParticleCount == 0)
                        ParticleSystem->State = PARTICLESTATE_DEAD;
                }
            }
        }
    }

    // return true if the entire particle system is still alive
    return (PARTICLESTATE_ALIVE == ParticleSystem->State);
}


//----------------------------------------------------------------------------------------
// Name: StandardObjectParticleThink()
// Desc:
//----------------------------------------------------------------------------------------

size_t StandardObjectParticleThink(ParticleSystem_t* ParticleSystem)
{
    size_t i;
    Vector_t Rotation;
    Vector_t Delta;
    Particle_t* Particle;

    for (i = 0; i < MAXPARTICLESINSYSTEM; i++)
    {
        Particle = &ParticleSystem->Particles[ i ];

        if (PARTICLESTATE_DEAD != Particle->State)
        {
            CalcObjectPointColors2(Particle->ObjectID);

            SetObjectPosition(Particle->ObjectID, 0, &Particle->Position);

            if (!IsObjectOnGround2(Particle->ObjectID))
            {
                // rotate the object
                GetObjectRotation(Particle->ObjectID, 0, &Rotation);
                GetObjectRotationDeltas(Particle->ObjectID, &Delta);
                VectorAdd(Rotation, Delta, Rotation);
                SetObjectRotation(Particle->ObjectID, 0, &Rotation);
            }

            if (Particle->LifetimeMax != -1)
            {
                if (++Particle->LifetimeCounter >= Particle->LifetimeMax)
                {
                    ReturnObject(Particle->ObjectID);

                    Particle->State = PARTICLESTATE_DEAD;

                    ParticleSystem->ActiveParticleCount--;
                    if (ParticleSystem->ActiveParticleCount == 0)
                        ParticleSystem->State = PARTICLESTATE_DEAD;
                }
            }
        }
    }

    // return true if the entire particle system is still alive
    return (PARTICLESTATE_ALIVE == ParticleSystem->State);
}


//----------------------------------------------------------------------------------------
// Name: StandardParticleMove()
// Desc:
//----------------------------------------------------------------------------------------

void StandardParticleMove(ParticleSystem_t* ParticleSystem)
{
    Particle_t* Particle;
    float Friction;
    size_t i;

    for (i = 0; i < MAXPARTICLESINSYSTEM; i++)
    {
        Particle = &ParticleSystem->Particles[ i ];

        if (PARTICLESTATE_DEAD != Particle->State)
        {
            if (ParticleSystem->Template->CanCollideWithLandscape)
            {
                Particle->GroundHeight = GetMeshHeight(g_LandscapeMesh, Particle->Position.X, Particle->Position.Z, &Particle->GroundPolyIndex);

                Friction = (Particle->Position.Y > Particle->GroundHeight ? ParticleSystem->Template->AirFriction : ParticleSystem->Template->GroundFriction);
            }
            else
                Friction = ParticleSystem->Template->AirFriction;

            Particle->Move.Y -= ParticleSystem->Template->Gravity;

            VectorDecrease(Particle->Move, Friction);

            VectorAdd(Particle->Position, Particle->Move, Particle->Position);
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: StandardParticleCollide()
// Desc:
//----------------------------------------------------------------------------------------

void StandardParticleCollide(ParticleSystem_t* ParticleSystem)
{
    Particle_t* Particle;
    Vector_t Normal;
    size_t i;

    for (i = 0; i < MAXPARTICLESINSYSTEM; i++)
    {
        Particle = &ParticleSystem->Particles[ i ];

        if (PARTICLESTATE_DEAD != Particle->State)
        {
            if (ParticleSystem->Template->CanCollideWithLandscape)
            {
                if (Particle->Position.Y <= Particle->GroundHeight)
                {
                    Particle->Position.Y = Particle->GroundHeight;
                    GetPolygonNormal(g_LandscapeMesh, Particle->GroundPolyIndex, &Normal);

                    VectorReflect(&Particle->Move, &Normal, &Particle->Move);
                    VectorScale(Particle->Move, 0.75f, Particle->Move);
                }
            }

            if (ParticleSystem->Template->CanCollideWithPlayers)
                CollideParticleWithObjects(ParticleSystem, Particle);
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: StandardParticleCollide()
// Desc:
//----------------------------------------------------------------------------------------

void CollideParticleWithObject(ParticleSystem_t* System, Particle_t* Particle, Object_t* Object)
{
    Vector_t V;
    Vector_t Zero;
    float Mag, Damage;

    VectorSet(Zero, 0.0f);

    if (VectorFuzzyCompare(Particle->Move, Zero, EPSILON))
    {
        if (VectorFuzzyCompare(Object->Physics.Move, Zero, EPSILON))
        {
            // then both objects are stationary - do nothing
        }
        else
        {
            // Particle is stationary but Object is moving

            // create a vector from the Object to the Particle
            VectorSubtract(Object->Positions[ 0 ], Particle->Position, V);
            VectorNormalise(&V);
            Mag = fabsf(VectorMagnitude(&Object->Physics.Move));

            if (!(Object->Physics.Flags & PHYSICS_HASCOLLIDED))
            {
                // bounce Object off Particle
                VectorReflect(&Object->Physics.Move, &V, &Object->Physics.Move);
            }

            if (!Particle->HasCollided)
            {
                // push Particle away
                VectorInverse(V);
                VectorScale(V, Mag, V);
                VectorAdd(Particle->Move, V, Particle->Move);
            }

            Damage = Mag + fabsf(VectorMagnitude(&Particle->Move));
        }
    }
    else if (VectorFuzzyCompare(Object->Physics.Move, Zero, EPSILON))
    {
        // Object is stationary but Particle is moving

        // create a vector from the Object to the Particle
        VectorSubtract(Particle->Position, Object->Positions[ 0 ], V);
        VectorNormalise(&V);
        Mag = fabsf(VectorMagnitude(&Particle->Move));

        if (!Particle->HasCollided)
        {
            // bounce Particle off Particle
            VectorReflect(&Particle->Move, &V, &Particle->Move);
        }

        if (!(Object->Physics.Flags & PHYSICS_HASCOLLIDED))
        {
            // push Object away
            VectorInverse(V);
            VectorScale(V, Mag, V);
            VectorAdd(Object->Physics.Move, V, Object->Physics.Move);
        }

        Damage = Mag + fabsf(VectorMagnitude(&Object->Physics.Move));
    }
    else
    {
        // both objects are moving

        // create a vector from the Object to the Particle
        VectorSubtract(Particle->Position, Object->Positions[ 0 ], V);
        VectorNormalise(&V);

        if (!Particle->HasCollided)
        {
            // bounce Particle off object
            VectorReflect(&Particle->Move, &V, &Particle->Move);
        }

        if (!(Object->Physics.Flags & PHYSICS_HASCOLLIDED))
        {
            // bounce Object off Particle
            VectorInverse(V);
            VectorReflect(&Object->Physics.Move, &V, &Object->Physics.Move);
        }

        Damage = fabsf(VectorMagnitude(&Particle->Move)) + fabsf(VectorMagnitude(&Object->Physics.Move));
    }

    // take one step so the two objects are no longer colliding
    if (!Particle->HasCollided)
        VectorAdd(Particle->Move, Particle->Position, Particle->Position);

    if (!(Object->Physics.Flags & PHYSICS_HASCOLLIDED))
    {
        VectorAdd(Object->Physics.Move, Object->Positions[ 0 ], Object->Positions[ 0 ]);
        PlayerTakeCollisionDamage(Object->ID, Damage * 0.25f);
    }

    Particle->HasCollided = 1;
    Object->Physics.Flags |= PHYSICS_HASCOLLIDED;
}


//----------------------------------------------------------------------------------------
// Name: StandardBillboardDraw()
// Desc:
//----------------------------------------------------------------------------------------

void StandardBillboardDraw(ParticleSystem_t* ParticleSystem, Particle_t* Particle, Vector_t* Position, float* ModelViewMatrix)
{
    Vector_t Points[ 4 ];
    Vector_t OffsetVert;
    Vector_t OffsetHoriz;
    Vector_t Right;
    Vector_t Up;

    if (nullptr == Particle || nullptr == ModelViewMatrix)
        return;

    // extract right and up vectors from the model-view matrix
    Right.X = ModelViewMatrix[ 0 ];
    Right.Y = ModelViewMatrix[ 4 ];
    Right.Z = ModelViewMatrix[ 8 ];

    Up.X = ModelViewMatrix[ 1 ];
    Up.Y = ModelViewMatrix[ 5 ];
    Up.Z = ModelViewMatrix[ 9 ];

    // scale to the desired size of our billboard image
    VectorScale(Right, Particle->FrameHalfWidth, OffsetHoriz);
    VectorScale(Up, Particle->FrameHalfHeight, OffsetVert);

    // setup the four corners of our billboard
    VectorSubtract((*Position), OffsetHoriz, Points[ CORNERS_BOTLEFT ]);
    VectorSubtract(Points[ CORNERS_BOTLEFT ], OffsetVert, Points[ CORNERS_BOTLEFT ]);

    VectorAdd((*Position), OffsetHoriz, Points[ CORNERS_BOTRIGHT ]);
    VectorSubtract(Points[ CORNERS_BOTRIGHT ], OffsetVert, Points[ CORNERS_BOTRIGHT ]);

    VectorAdd((*Position), OffsetHoriz, Points[ CORNERS_TOPRIGHT ]);
    VectorAdd(Points[ CORNERS_TOPRIGHT ], OffsetVert, Points[ CORNERS_TOPRIGHT ]);

    VectorSubtract((*Position), OffsetHoriz, Points[ CORNERS_TOPLEFT ]);
    VectorAdd(Points[ CORNERS_TOPLEFT ], OffsetVert, Points[ CORNERS_TOPLEFT ]);

    // draw it
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(ParticleSystem->Emitter->GLBlendSrc, ParticleSystem->Emitter->GLBlendDst);
    glBindTexture(GL_TEXTURE_2D, GetTextureNumber(ParticleSystem->Template->TextureID));

    glBegin(GL_QUADS);
    glColor4f(Particle->TextureColor.R, Particle->TextureColor.G, Particle->TextureColor.B, Particle->TextureColor.A);

    glTexCoord2f(Particle->TexX, 0.0f);
    glVertex3fv((float*)&Points[ CORNERS_BOTLEFT ]);

    glTexCoord2f(Particle->TexX + ParticleSystem->Template->TextureUSize, 0.0f);
    glVertex3fv((float*)&Points[ CORNERS_BOTRIGHT ]);

    glTexCoord2f(Particle->TexX + ParticleSystem->Template->TextureUSize, 1.0f);
    glVertex3fv((float*)&Points[ CORNERS_TOPRIGHT ]);

    glTexCoord2f(Particle->TexX, 1.0f);
    glVertex3fv((float*)&Points[ CORNERS_TOPLEFT ]);

    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}


//----------------------------------------------------------------------------------------
// Name: GetActiveParticleCount()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetActiveParticleCount(size_t ID)
{
    if (INVALIDID(ID))
        return 0;

    return _ParticlePool[ ID ].ActiveParticleCount;
}


//----------------------------------------------------------------------------------------
// Name: DrawParticleLine()
// Desc:
//----------------------------------------------------------------------------------------

void DrawParticleLine(size_t ID, Vector_t* StartPoint, Vector_t* FinishPoint, float Multiplier)
{
    size_t i, Index, NumParticles;
    Vector_t Direction;
    Vector_t Position;
    Particle_t* Particle;

    if (INVALIDID(ID))
        return;

    VectorSubtract((*FinishPoint), (*StartPoint), Direction);
    VectorNormalise(&Direction);
    VectorScale(Direction, (RAILGUN_STEPSIZE * Multiplier), Direction);

    NumParticles = (size_t)(fabsf(VectorMagnitude2(StartPoint, FinishPoint)) / (RAILGUN_STEPSIZE * Multiplier));

    VectorCopy((*StartPoint), Position);

    // draw a line of particles from the start point to the finish point

    for (i = 0; i < MAXPARTICLESINSYSTEM && i < NumParticles; i++)
    {
        Particle = &_ParticlePool[ ID ].Particles[ i ];

        // setup state variables
        Particle->State             = PARTICLESTATE_ALIVE;
        Particle->BSphereSize       = _ParticlePool[ ID ].Template->BSphereSize;
        Particle->LifetimeCounter   = 0;
        Particle->LifetimeMax       = -1;

        // setup the visual aspects of the particle
        Index = rand() % _ParticlePool[ ID ].Template->StartTextureColorCount;
        memcpy(&Particle->TextureColor, &_ParticlePool[ ID ].Template->StartTextureColors[ Index ], sizeof(Color_t));

        Particle->FrameWidth  = _ParticlePool[ ID ].Template->StartFrameWidth;
        Particle->FrameHeight = _ParticlePool[ ID ].Template->StartFrameHeight;

        if (_ParticlePool[ ID ].Template->StartTextureFrame == -1)
            Particle->CurrentFrame = rand() % _ParticlePool[ ID ].Template->NumFrames;
        else
            Particle->CurrentFrame = _ParticlePool[ ID ].Template->StartTextureFrame;

        VectorSet(Particle->Move, 0.0f);
        VectorCopy(Position, Particle->Position);

        // increment the position for the next particle to come
        VectorAdd(Position, Direction, Position);

        _ParticlePool[ ID ].ActiveParticleCount++;
    }
}


//-----------------------------------------------------------------------------
// Name: CreateSnowStorm()
// Desc:
//-----------------------------------------------------------------------------

void CreateSnowStorm(size_t ID, size_t* Dimensions, ParticleTemplate_t* Template)
{
    size_t WidthRange, DepthRange, HeightRange;
    size_t i, j, Index;
    size_t Size, Speed;
    size_t VerticalStepSize, VerticalStep;
    size_t ParticleIndex, NumParticlesInStep;
    Particle_t* Particle;

    if (INVALIDID(ID))
        return;

    // generate a point for each snowflake within the emitter box

    WidthRange  = Dimensions[ EMITTERSIDE_RIGHT ] - Dimensions[ EMITTERSIDE_LEFT   ];
    DepthRange  = Dimensions[ EMITTERSIDE_FRONT ] - Dimensions[ EMITTERSIDE_BACK   ];
    HeightRange = Dimensions[ EMITTERSIDE_TOP   ] - Dimensions[ EMITTERSIDE_BOTTOM ];

    NumParticlesInStep  = MAXPARTICLESINSYSTEM / SNOWSTORM_STEPCOUNT;
    VerticalStepSize    = HeightRange / SNOWSTORM_STEPCOUNT;

    ParticleIndex = 0;
    VerticalStep  = Dimensions[ EMITTERSIDE_TOP ];

    for (i = 0; i < SNOWSTORM_STEPCOUNT; i++)
    {
        for (j = 0; j < NumParticlesInStep; j++)
        {
            Particle = &_ParticlePool[ ID ].Particles[ ParticleIndex ];

            // setup state variables
            Particle->State         = PARTICLESTATE_ALIVE;
            Particle->BSphereSize   = Template->BSphereSize;

            Particle->LifetimeCounter   = 0;
            Particle->LifetimeMax       = -1;
            Particle->CurrentFrame      = 0;

            Index = rand() % _ParticlePool[ ID ].Template->StartTextureColorCount;
            memcpy(&Particle->TextureColor, &_ParticlePool[ ID ].Template->StartTextureColors[ Index ], sizeof(Color_t));

            Particle->FrameWidth  = _ParticlePool[ ID ].Template->StartFrameWidth;
            Particle->FrameHeight = _ParticlePool[ ID ].Template->StartFrameHeight;

            // choose initial position
            Particle->Position.X = (float)(((size_t)rand() % WidthRange) + (size_t)Dimensions[ EMITTERSIDE_LEFT ]);
            Particle->Position.Y = (float)VerticalStep;
            Particle->Position.Z = (float)(((size_t)rand() % DepthRange) + (size_t)Dimensions[ EMITTERSIDE_BACK ]);

            // setup speed
            VectorSet(Particle->Move, 0.0f);

            if (_ParticlePool[ ID ].Emitter->StartSpeedMax != _ParticlePool[ ID ].Emitter->StartSpeedMin)
                Size = (rand() % (_ParticlePool[ ID ].Emitter->StartSpeedMax - _ParticlePool[ ID ].Emitter->StartSpeedMin)) + _ParticlePool[ ID ].Emitter->StartSpeedMin;
            else
                Size = _ParticlePool[ ID ].Emitter->StartSpeedMin;

            if (_ParticlePool[ ID ].Emitter->SpeedDeltaMax != _ParticlePool[ ID ].Emitter->SpeedDeltaMin)
                Speed = (rand() % (_ParticlePool[ ID ].Emitter->SpeedDeltaMax - _ParticlePool[ ID ].Emitter->SpeedDeltaMin)) + _ParticlePool[ ID ].Emitter->SpeedDeltaMin;
            else
                Speed = _ParticlePool[ ID ].Emitter->SpeedDeltaMin;

            Particle->Move.X += (float)Speed / 100.0f;

            if (_ParticlePool[ ID ].Emitter->SpeedDeltaMax != _ParticlePool[ ID ].Emitter->SpeedDeltaMin)
                Speed = (rand() % (_ParticlePool[ ID ].Emitter->SpeedDeltaMax - _ParticlePool[ ID ].Emitter->SpeedDeltaMin)) + _ParticlePool[ ID ].Emitter->SpeedDeltaMin;
            else
                Speed = _ParticlePool[ ID ].Emitter->SpeedDeltaMin;

            Particle->Move.Y += (float)Speed / 100.0f;

            if (_ParticlePool[ ID ].Emitter->SpeedDeltaMax != _ParticlePool[ ID ].Emitter->SpeedDeltaMin)
                Speed = (rand() % (_ParticlePool[ ID ].Emitter->SpeedDeltaMax - _ParticlePool[ ID ].Emitter->SpeedDeltaMin)) + _ParticlePool[ ID ].Emitter->SpeedDeltaMin;
            else
                Speed = _ParticlePool[ ID ].Emitter->SpeedDeltaMin;

            Particle->Move.Z += (float)Speed / 100.0f;

            VectorScale(Particle->Move, (float)Size / 100.0f, Particle->Move);

            // increment counters
            _ParticlePool[ ID ].ActiveParticleCount++;
            ParticleIndex++;
        }

        VerticalStep -= VerticalStepSize;
    }
}


//----------------------------------------------------------------------------------------
// Name: SnowflakeParticleCollide()
// Desc:
//----------------------------------------------------------------------------------------

void SnowflakeParticleCollide(ParticleSystem_t* ParticleSystem)
{
    size_t i;

    for (i = 0; i < MAXPARTICLESINSYSTEM; i++)
    {
        if (ParticleSystem->Particles[ i ].Position.Y < (float)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_BOTTOM ])
        {
            ParticleSystem->Particles[ i ].Position.Y = (float)ParticleSystem->Emitter->Dimensions[ EMITTERSIDE_TOP ];
            VectorSet(ParticleSystem->Particles[ i ].Move, 0.0f);
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: SetupCentralExplosion()
// Desc:
//----------------------------------------------------------------------------------------

void SetupCentralExplosion(size_t ID, Vector_t* EmitPoint, ExplosionType_t* ExplType, ParticleTemplate_t* Template)
{
    Particle_t* Particle;
    Vector_t Normal;

    // create one particle to use to put the explosion texture on

    Particle = &_ParticlePool[ ID ].Particles[ 0 ];

    // setup state variables
    Particle->State         = PARTICLESTATE_ALIVE;
    Particle->BSphereSize   = Template->BSphereSize;

    Particle->LifetimeCounter   = 0;
    Particle->LifetimeMax       = -1;
    Particle->CurrentFrame      = 0;

    memcpy(&Particle->TextureColor, &_ParticlePool[ ID ].Template->StartTextureColors[ 0 ], sizeof(Color_t));

    Particle->FrameWidth  = ExplType->FrameWidth;
    Particle->FrameHeight = ExplType->FrameHeight;

    // set initial position
    VectorCopy((*EmitPoint), Particle->Position);
    GetMeshNormal(g_LandscapeMesh, EmitPoint->X, EmitPoint->Z, &Normal);
    /*Particle->Position.X += Normal.X * ( ExplType->FrameHeight / 2.0f );
    Particle->Position.Y += Normal.Y * ( ExplType->FrameHeight / 2.0f );
    Particle->Position.Z += Normal.Z * ( ExplType->FrameHeight / 2.0f );*/

    // set speed
    VectorSet(Particle->Move, 0.0f);

    _ParticlePool[ ID ].ActiveParticleCount = 1;
}


//----------------------------------------------------------------------------------------
// Name: SetParticleData()
// Desc:
//----------------------------------------------------------------------------------------

void SetParticleData(size_t ID, size_t Data)
{
    if (INVALIDID(ID))
        return;

    _ParticlePool[ ID ].Data = Data;
}


//----------------------------------------------------------------------------------------
// Name: GetParticleData()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetParticleData(size_t ID)
{
    if (INVALIDID(ID))
        return 0;

    return _ParticlePool[ ID ].Data;
}


//----------------------------------------------------------------------------------------
// Name: GetParticleData()
// Desc:
//----------------------------------------------------------------------------------------

static void CreateDebrisPiece(size_t SystemID, Particle_t* Particle, size_t ModelID)
{
    Vector_t Delta;

    Particle->ModelID = ModelID;

    Particle->ObjectID = GetFreeObject();
    ResetObject(Particle->ObjectID);

    SetObjectModels(Particle->ObjectID, ModelID, -1);

    SetObjectDrawFunction(Particle->ObjectID, DrawShadedModelTrans);
    SetObjectRotateType(Particle->ObjectID, MODELROTATE_XZY);

    ActivateObject(Particle->ObjectID);
    ShowObject(Particle->ObjectID);

    // choose random rotation deltas
    switch (rand() % 3)
    {
    case 0 :
        Delta.X = (float)RandRange(-50, 50) / 10.0f;
        Delta.Y = (float)RandRange(-50, 50) / 10.0f;
        Delta.Z = 0.0f;
        break;
    case 1 :
        Delta.X = (float)RandRange(-50, 50) / 10.0f;
        Delta.Z = (float)RandRange(-50, 50) / 10.0f;
        Delta.Y = 0.0f;
        break;
    default :
        Delta.Y = (float)RandRange(-50, 50) / 10.0f;
        Delta.Z = (float)RandRange(-50, 50) / 10.0f;
        Delta.X = 0.0f;
        break;
    }
    SetObjectRotationDeltas(Particle->ObjectID, &Delta);
}
