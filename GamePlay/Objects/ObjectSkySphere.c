//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "ObjectTypes.h"
#include "Camera.h"


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static float Rotation = 0.0f;


//-----------------------------------------------------------------------------
// Name: SkySphereThink()
// Desc: 
//-----------------------------------------------------------------------------

void SkySphereThink( Object_t *Object, size_t IsHiResModel )
{
    Vector_t *CameraPosition = GetCameraPosition();

    Object->Positions[ 0 ].X = CameraPosition->X;
    Object->Positions[ 0 ].Z = CameraPosition->Z;
    Object->Positions[ 0 ].Y = -20000.0f;

    Object->Rotations[ 0 ].Z = Rotation;
    Rotation += 0.032f;
}
