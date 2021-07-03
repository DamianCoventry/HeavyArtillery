//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


#include "Vector.h"
#include "ParticleTypes.h"


extern size_t CreateFireball( void );
extern void DeleteFireball( void );

extern size_t StartFireball( Vector_t *EmitPoint, size_t IsSmall );

extern size_t NapalmParticleThink( ParticleSystem_t *ParticleSystem );
