//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 6 July 2002
//
//========================================================================================


#include "Vector.h"
#include "ObjectTypes.h"

#define FUNKYBOMB_EMITTHRESHOLD         10
#define FUNKYBOMB_MISSILECOUNT          5


extern void FunkyBombThink( Object_t *Projectile, size_t IsHisRes );
extern void FunkyBombMeshExplode( Object_t *Object );
extern void FunkyBombObjectExplode( Object_t *Projectile, Object_t *Object );
