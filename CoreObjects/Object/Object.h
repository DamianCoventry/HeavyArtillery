//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#include "ObjectTypes.h"
#include "ParticleTypes.h"

extern void InitialiseObject( Object_t *Object );

extern void MoveObject( Object_t *Object, float Friction );

extern void CollideMeshSlide( Object_t *Object, size_t MeshID );
extern void CollideMeshBounce( Object_t *Object, size_t MeshID );
extern void CollideMeshStick( Object_t *Object, size_t MeshID );

extern void CollideObjects( Object_t *Object1, Object_t *Object2 );

extern void AllocatePointColors( Object_t *Object );
extern void FreePointColors( Object_t *Object );

extern size_t IsObjectOnGround( Object_t *Object );
