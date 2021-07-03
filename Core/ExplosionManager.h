//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 6 July 2002
//
//========================================================================================


#include "ExplosionTypes.h"


extern void ReturnExplosion( size_t ID );

extern size_t GetExplosionCount( void );

extern size_t StartExplosion( Vector_t *Position, size_t Initiator, ExplosionType_t *ExplType );

extern size_t StartNapalmExplosion( Vector_t *Position, size_t Initiator, ExplosionType_t *ExplType, size_t ParticleID );

extern size_t IncrementExplosion( size_t ID );
extern size_t IncrementNapalmExplosion( size_t ID );

extern ExplosionType_t *GetExplosionType( size_t ID );
