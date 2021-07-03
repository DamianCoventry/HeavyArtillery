//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


#ifndef _PARTICLETYPES_H_
#define _PARTICLETYPES_H_

#define MAXPARTICLESINSYSTEM            1024
#define MAXSTARTTEXTURECOLORS           4

#define PARTICLETEMPLATE_MAXOBJECTS     16

#include "Vector.h"


//----------------------------------------------------------------------------------------
// enumerations
//----------------------------------------------------------------------------------------

typedef enum ParticleState_e
{
    PARTICLESTATE_DEAD = 0,
    PARTICLESTATE_DYING,
    PARTICLESTATE_ALIVE
}
ParticleState_t;

typedef enum ParticleType_e
{
    PARTICLETYPE_BILLBOARD,
    PARTICLETYPE_OBJECT
}
ParticleType_t;

typedef enum EmitterShape_e
{
    EMITTERSHAPE_POINT,
    EMITTERSHAPE_AASQUARE,
    EMITTERSHAPE_AABOX,
    EMITTERSHAPE_SPHERE
}
EmitterShape_t;

typedef enum EmitterSides_e
{
    EMITTERSIDE_LEFT = 0,
    EMITTERSIDE_RIGHT,
    EMITTERSIDE_TOP,
    EMITTERSIDE_BOTTOM,
    EMITTERSIDE_FRONT,
    EMITTERSIDE_BACK
}
EmitterSides_t;

typedef enum BillboardCorners_s
{
    CORNERS_TOPLEFT = 0,
    CORNERS_TOPRIGHT,
    CORNERS_BOTLEFT,
    CORNERS_BOTRIGHT
}
BillboardCorners_t;

typedef enum SpeedChange_e
{
    SPEEDCHANGE_X,
    SPEEDCHANGE_Y,
    SPEEDCHANGE_Z,
    SPEEDCHANGE_NONE,
    SPEEDCHANGE_ALL
}
SpeedChange_t;


//----------------------------------------------------------------------------------------
// structures
//----------------------------------------------------------------------------------------

#ifndef _COLOR_STRUCT_
#define _COLOR_STRUCT_

typedef struct Color_s
{
    float R, G, B, A;
}
Color_t;

#endif

typedef struct ParticleTemplateObject_s
{
    size_t ModelID;
    size_t EmitAmount;
}
ParticleTemplateObject_t;

typedef struct ParticleObject_s
{
    size_t NumEmitted;
}
ParticleObject_t;

typedef struct ParticleTemplate_s
{
    ParticleType_t Type;
    float BSphereSize;

    size_t AttachedToCamera;

    float FrameDeltaWidth, FrameDeltaHeight;
    float StartFrameWidth, StartFrameHeight;

    size_t TextureID;
    size_t StartTextureColorCount;
    Color_t StartTextureColors[ MAXSTARTTEXTURECOLORS ];
    Color_t FinishTextureColor;
    Color_t TextureDeltaColor;
    size_t StartTextureFrame, FinishTextureFrame;
    size_t NumFrames, FrameChangeDelay;
    size_t AnimateFrames, AnimateColor;
    float TextureUSize, TextureVSize;

    long LifetimeMin, LifetimeMax;           // # of frames to live for

    float Gravity;
    float AirFriction, GroundFriction;
    size_t CanCollideWithLandscape;
    size_t CanCollideWithPlayers;

    size_t ObjectCount;
    ParticleTemplateObject_t Objects[ PARTICLETEMPLATE_MAXOBJECTS ];
}
ParticleTemplate_t;

typedef struct ParticleEmitter_s
{
    EmitterShape_t Shape;
    size_t Dimensions[ 6 ];                    // in units per second * 100

    size_t EmitLimit;
    size_t EmitAmountMin, EmitAmountMax;
    size_t EmitDelayMin, EmitDelayMax;         // # of frames to wait

    SpeedChange_t SpeedChange;
    Vector_t StartMoveVector;
    size_t StartSpeedMin, StartSpeedMax;       // in units per second * 100
    size_t SpeedDeltaMin, SpeedDeltaMax;       // in units per second * 100

    GLenum GLBlendSrc, GLBlendDst;             // the parameters of glBlendFunc();
}
ParticleEmitter_t;

typedef struct Particle_s
{
    ParticleState_t State;
    ParticleType_t Type;

    float BSphereSize;
    float GroundHeight;
    size_t GroundPolyIndex;
    size_t HasCollided;

    Vector_t Position;
    Vector_t Move;

    size_t ObjectID;
    size_t ModelID;

    size_t CurrentFrame;
    size_t FrameCounter;
    size_t LifetimeCounter;
    size_t LifetimeMax;

    float TexX;             // for UV texture map animation

    float FrameWidth, FrameHeight;
    float FrameHalfWidth, FrameHalfHeight;
    Color_t TextureColor;
}
Particle_t;

struct Object_s;

typedef struct ParticleSystem_s
{
    size_t Used;
    size_t ID;

    size_t Data;

    size_t AttachedObjectID;

    ParticleState_t State;
    ParticleType_t Type;
    ParticleEmitter_t *Emitter;
    ParticleTemplate_t *Template;

    Vector_t EmitPoint;
    size_t EmitDelayCounter;
    size_t EmitDelayThreshold;

    size_t StopEmitting;
    size_t AmountEmitted;
    size_t ActiveParticleCount;
    Particle_t Particles[ MAXPARTICLESINSYSTEM ];

    ParticleObject_t Objects[ PARTICLETEMPLATE_MAXOBJECTS ];

    void (*FinishCallback)( size_t ID );

    size_t (*EmitterThink)( struct ParticleSystem_s *Self );
    size_t (*Think)( struct ParticleSystem_s *Self );
    void (*Move)( struct ParticleSystem_s *Self );
    void (*Collide)( struct ParticleSystem_s *Self );
    void (*Draw)( struct ParticleSystem_s *Self, Particle_t *Particle, Vector_t *Position, float *ModelViewMatrix );

    void (*OnCollideObject)( struct ParticleSystem_s *ParticleSystem, Particle_t *Particle, struct Object_s *Object );
}
ParticleSystem_t;

typedef struct DisplayListNode_s
{
    Particle_t *Particle;
    ParticleSystem_t *System;
    Vector_t Position;
    float Distance;
    struct DisplayListNode_s *Next;
}
DisplayListNode_t;


#endif
