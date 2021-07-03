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
#include "ExplosionTypes.h"


extern size_t CreateExplSparks( void );
extern void DeleteExplSparks( void );

extern size_t StartExplSparks( Vector_t *EmitPoint, Vector_t *Normal, ExplosionType_t *ExplType, void (*FinishCallback)( size_t ) );
