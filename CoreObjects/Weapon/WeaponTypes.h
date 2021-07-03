//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Sunday, 30 June 2002
//
//========================================================================================


#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "Vector.h"
#include "ObjectTypes.h"
#include "ExplosionTypes.h"

typedef struct Weapon_s
{
    std::string Name;
    size_t Price;
    size_t BundleSize;
    std::string Line0, Line1;
    std::string Line2, Line3;

    size_t ProjectileCount;
    size_t AeroplaneCount;
    size_t ShootSoundID;

    float Scale;
    float AngleIncrement;
    float BounceReduction;

    ExplosionType_t* ExplType;          // note: AirStrikeBeacon should actually be an explosion type.
    size_t DecalID;
    size_t (*StartSmokeTrail)(size_t ObjectID);
    size_t (*StartSparkTrail)(size_t ObjectID);
    size_t ParticleEmitterID;
    size_t ProjectileModelIDLOD0;
    size_t ProjectileModelIDLOD1;
    size_t PortraitModelID;

    // returns a ShotID
    size_t (*Shoot)(struct Weapon_s*, Vector_t*, Vector_t*, float, size_t);
    void (*CollideMesh)(struct Object_s*, size_t);

    void (*OnThink)(struct Object_s*, size_t);
    void (*OnCollideMesh)(struct Object_s*);
    void (*OnCollideObject)(struct Object_s*, struct Object_s*);
}
Weapon_t;


#endif
