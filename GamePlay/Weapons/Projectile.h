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


extern void CollideMeshExplode( Object_t *Object );

extern void CollideObjectBounce( Object_t *Projectile, Object_t *Object );
extern void CollideObjectExplode( Object_t *Projectile, Object_t *Object );

extern void RollerBouncerThink( Object_t *Projectile, size_t IsHisRes );

extern void FaceDirection( Object_t *Object, size_t IsHiResModel );
extern void UpdateShotShading( Object_t *Object, size_t IsHiResModel );

extern void RailgunThink( Object_t *Projectile, size_t IsHisRes );
