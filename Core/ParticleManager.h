//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "Vector.h"
#include "ParticleTypes.h"
#include "ObjectTypes.h"
#include "ExplosionTypes.h"


extern size_t GetFreeParticleSystem(void);
extern void ReturnParticleSystem(size_t ID);
extern void ReturnAllParticleSystems(void);

extern void ParticleManagerThink(void);
extern void ParticleManagerMove(void);
extern void ParticleManagerCollide(void);
extern void ParticleManagerDraw(Plane2_t* ViewFrustum);

extern void StopEmittingParticles(size_t ID);

extern void InitialiseParticleSystem(size_t ID, ParticleType_t Type, size_t ObjectID,
                                     Vector_t* EmitPoint, ParticleTemplate_t* Template, ParticleEmitter_t* Emitter,
                                     void (*FinishCallback)(size_t ID));

extern void SetEmitterThinkFunction(size_t ID, size_t (*Think)(ParticleSystem_t* Self));
extern void SetThinkFunction(size_t ID, size_t (*Think)(ParticleSystem_t* Self));
extern void SetMoveFunction(size_t ID, void (*Move)(ParticleSystem_t* Self));
extern void SetCollideFunction(size_t ID, void (*Collide)(ParticleSystem_t* Self));
extern void SetDrawFunction(size_t ID, void (*Draw)(ParticleSystem_t* Self, Particle_t* Particle, Vector_t* Position, float* ModelViewMatrix));
extern void SetOnCollideObjectFunction(size_t ID, void (*OnCollideObject)(ParticleSystem_t*, Particle_t* Particle, struct Object_s* Object));
extern size_t StandardEmitterThink(ParticleSystem_t* ParticleSystem);
extern size_t StandardBillboardParticleThink(ParticleSystem_t* ParticleSystem);
extern size_t StandardObjectParticleThink(ParticleSystem_t* ParticleSystem);

extern void StandardParticleMove(ParticleSystem_t* ParticleSystem);
extern void StandardParticleCollide(ParticleSystem_t* ParticleSystem);
extern void CollideParticleWithObject(ParticleSystem_t* System, Particle_t* Particle, Object_t* Object);
extern void StandardBillboardDraw(ParticleSystem_t* ParticleSystem, Particle_t* Particle, Vector_t* Position, float* ModelViewMatrix);
extern void StandardObjectDraw(ParticleSystem_t* ParticleSystem, Particle_t* Particle, Vector_t* Position, float* ModelViewMatrix);

extern size_t GetActiveParticleCount(size_t ID);

extern void DrawParticleLine(size_t ID, Vector_t* StartPoint, Vector_t* FinishPoint, float Multiplier);

extern void CreateSnowStorm(size_t ID, size_t* Dimensions, ParticleTemplate_t* Template);
extern void SnowflakeParticleCollide(ParticleSystem_t* ParticleSystem);
extern void SetupCentralExplosion(size_t ID, Vector_t* EmitPoint, ExplosionType_t* ExplType, ParticleTemplate_t* Template);

extern void SetParticleData(size_t ID, size_t Data);
extern size_t GetParticleData(size_t ID);
