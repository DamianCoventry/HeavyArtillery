//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "Vector.h"


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct LightSource_s
{
    float Ambient;
    Vector_t Position;
    Vector_t Direction;
}
LightSource_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static LightSource_t _Sun;


//----------------------------------------------------------------------------------------
// Name: SetupLightSource()
// Desc: 
//----------------------------------------------------------------------------------------

void SetupLightSource( void )
{
    _Sun.Ambient = 0.25f;
    _Sun.Position.X = 8192.0f;
    _Sun.Position.Y = 8192.0f;
    _Sun.Position.Z = 8192.0f;
    _Sun.Direction.X = -0.7071067f;
    _Sun.Direction.Y = -0.7071067f;
    _Sun.Direction.Z = -0.7071067f;
}


//----------------------------------------------------------------------------------------
// Name: GetLightSourceAmbient()
// Desc: 
//----------------------------------------------------------------------------------------

void GetLightSourceAmbient( float *Ambient )
{
    if( Ambient )
        (*Ambient) = _Sun.Ambient;
}


//----------------------------------------------------------------------------------------
// Name: GetLightSourceDirection()
// Desc: 
//----------------------------------------------------------------------------------------

void GetLightSourceDirection( float *X, float *Y, float *Z )
{
    if( X )
        (*X) = _Sun.Direction.X;
    if( Y )
        (*Y) = _Sun.Direction.Y;
    if( Z )
        (*Z) = _Sun.Direction.Z;
}


//----------------------------------------------------------------------------------------
// Name: GetLightSourcePosition()
// Desc: 
//----------------------------------------------------------------------------------------

void GetLightSourcePosition( float *X, float *Y, float *Z )
{
    if( X )
        (*X) = _Sun.Position.X;
    if( Y )
        (*Y) = _Sun.Position.Y;
    if( Z )
        (*Z) = _Sun.Position.Z;
}
