//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 1 July 2002
//
//========================================================================================


#include "ObjectTypes.h"


extern void PlayerTankThink( Object_t *Object, size_t IsHiResModel );
extern void OnPlayerMove( Object_t *Object );

extern void OnPlayerCollideMesh( Object_t *Object );
extern void OnPlayerCollideObject( Object_t *PlayersTank, Object_t *OtherTank );
extern void OnPlayerFallingDamage( Object_t *Object );
