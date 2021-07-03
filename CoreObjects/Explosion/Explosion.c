//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 11 July 2002
//
//========================================================================================


#include "stdafx.h"
#include "ObjectTypes.h"
#include "ExplosionManager.h"
#include "ObjectManager.h"

#define SHOCKWAVE_FADESPEED     0.05f


//----------------------------------------------------------------------------------------
// Name: ExplosionThink()
// Desc: 
//----------------------------------------------------------------------------------------

void ExplosionThink( Object_t *Self, size_t IsHiResModel )
{
    Vector_t Scale;
    ExplosionType_t *ExplType = GetExplosionType( GetObjectData( Self->ID ) );

    GetObjectScale( Self->ID, 0, &Scale );
    Scale.X += 0.25f;
    Scale.Y += 0.25f;
    Scale.Z += 0.25f;
    SetObjectScale( Self->ID, 0, &Scale );

    IncrementExplosion( Self->Data );
}


//----------------------------------------------------------------------------------------
// Name: ExplosionNapalmThink()
// Desc: 
//----------------------------------------------------------------------------------------

void ExplosionNapalmThink( Object_t *Self, size_t IsHiResModel )
{
    Vector_t Scale;
    ExplosionType_t *ExplType = GetExplosionType( GetObjectData( Self->ID ) );

    GetObjectScale( Self->ID, 0, &Scale );
    Scale.X += 0.25f;
    Scale.Y += 0.25f;
    Scale.Z += 0.25f;
    SetObjectScale( Self->ID, 0, &Scale );

    IncrementNapalmExplosion( Self->Data );
}


//----------------------------------------------------------------------------------------
// Name: ShockwaveThink()
// Desc: 
//----------------------------------------------------------------------------------------

void ShockwaveThink( Object_t *Self, size_t IsHiResModel )
{
    Vector_t Scale;
    float BlendFactor;

    GetObjectScale( Self->ID, 0, &Scale );
    Scale.X += 3.0f;
    Scale.Z += 3.0f;
    SetObjectScale( Self->ID, 0, &Scale );

    BlendFactor = GetObjectBlendFactor( Self->ID );
    BlendFactor -= SHOCKWAVE_FADESPEED;

    if( BlendFactor < 0.0f )
        BlendFactor = 0.0f;

    SetObjectBlendFactor( Self->ID, BlendFactor );
}
