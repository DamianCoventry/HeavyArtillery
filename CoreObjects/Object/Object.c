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
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ObjectTypes.h"
#include "Object.h"

#include "GameTypes.h"
#include "GameSounds.h"

#include "ModelManager.h"
#include "MeshManager.h"
#include "SoundManager.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// Name: InitialiseObject()
// Desc: 
//----------------------------------------------------------------------------------------

void InitialiseObject( Object_t *Object )
{
    size_t i;

    Object->State           = OBJECTSTATE_DEAD;
    Object->ModelIDLoRes    = -1;
    Object->ModelIDHiRes    = -1;
    Object->RotateType      = 0;
    Object->Flags           = 0;

    Object->ThinkCounter1   = 0;
    Object->ThinkCounter2   = 0;
    Object->ThinkCounter3   = 0;
    Object->ThinkCounter4   = 0;

    Object->Draw            = nullptr;
    Object->Draw2           = nullptr;
    Object->CollideMesh     = nullptr;

    Object->OnThink         = nullptr;
    Object->OnCollideMesh   = nullptr;
    Object->OnCollideObject = nullptr;
    Object->OnFallingDamage = nullptr;

    memset( &Object->Physics, 0, sizeof( Physics_t ) );
    Object->Physics.Mass            = DEFAULT_MASS;
    Object->Physics.Gravity         = DEFAULT_GRAVITY;
    Object->Physics.BounceReduction = DEFAULT_BOUNCEREDUCTION;

    for( i = 0; i < OBJECT_MAXLAYERS; i++ )
    {
        VectorSet( Object->Positions[ i ], 0.0f );
        VectorSet( Object->Rotations[ i ], 0.0f );
    }

    FreePointColors( Object );
}


//----------------------------------------------------------------------------------------
// Name: MoveObject()
// Desc: 
//----------------------------------------------------------------------------------------

void MoveObject( Object_t *Object, float Friction )
{
    if( !( Object->Physics.Flags & PHYSICS_IGNOREGRAVITY ) )
        Object->Physics.Move.Y -= ( Object->Physics.Mass * Object->Physics.Gravity );

    if( !( Object->Physics.Flags & PHYSICS_IGNOREFRICTION ) )
    {
        VectorDecrease( Object->Physics.Move, ( Friction * Object->Physics.Mass ) );
    }

    VectorAdd( Object->Physics.Move, Object->Positions[ 0 ], Object->Positions[ 0 ] );
}


//----------------------------------------------------------------------------------------
// Name: CollideMeshSlide()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideMeshSlide( Object_t *Object, size_t MeshID )
{
    Vector_t Right;
    Vector_t Normal;
    
    if( Object->Positions[ 0 ].Y <= Object->Physics.GroundHeight )
    {
        Object->Positions[ 0 ].Y = Object->Physics.GroundHeight;
        GetPolygonNormal( MeshID, Object->Physics.PolyNormal, &Normal );

        VectorCrossProd( Object->Physics.Move, Normal, Right );
        VectorCrossProd( Normal, Right, Object->Physics.Move );

        if( Object->OnCollideMesh )
            Object->OnCollideMesh( Object );
    }
}


//----------------------------------------------------------------------------------------
// Name: CollideMeshBounce()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideMeshBounce( Object_t *Object, size_t MeshID )
{
    Vector_t Normal;
    size_t SoundID;

    if( Object->Positions[ 0 ].Y <= Object->Physics.GroundHeight )
    {
        Object->Positions[ 0 ].Y = Object->Physics.GroundHeight;
        GetPolygonNormal( MeshID, Object->Physics.PolyNormal, &Normal );

        VectorReflect( &Object->Physics.Move, &Normal, &Object->Physics.Move );
        VectorScale( Object->Physics.Move, Object->Physics.BounceReduction, Object->Physics.Move );

        SoundID = 0 == rand() % 2 ? SoundObjectImpact0 : SoundObjectImpact1;
        SetSoundPosition( SoundID, Object->Positions[ 0 ].X, Object->Positions[ 0 ].Y, Object->Positions[ 0 ].Z );
        PlaySound3D( SoundID, 0 );

        if( Object->OnCollideMesh )
            Object->OnCollideMesh( Object );
    }
}


//----------------------------------------------------------------------------------------
// Name: CollideMeshStick()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideMeshStick( Object_t *Object, size_t MeshID )
{
    if( Object->Positions[ 0 ].Y <= Object->Physics.GroundHeight )
    {
        Object->Positions[ 0 ].Y = Object->Physics.GroundHeight;

        if( Object->OnCollideMesh )
            Object->OnCollideMesh( Object );
    }
}


//----------------------------------------------------------------------------------------
// Name: CollideObjects()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideObjects( Object_t *Object1, Object_t *Object2 )
{
    float DistanceBetween;
    Vector_t V;

    VectorSubtract( Object1->Positions[ 0 ], Object2->Positions[ 0 ], V );
    DistanceBetween = fabsf( VectorMagnitude( &V ) );

    if( DistanceBetween <= GetModelBSphereRadius( Object1->ModelIDHiRes ) + GetModelBSphereRadius( Object2->ModelIDHiRes ) )
    {
        if( Object1->OnCollideObject )
            Object1->OnCollideObject( Object1, Object2 );

        if( Object2->OnCollideObject )
            Object2->OnCollideObject( Object2, Object1 );
    }
}


//----------------------------------------------------------------------------------------
// Name: AllocatePointColors()
// Desc: 
//----------------------------------------------------------------------------------------

void AllocatePointColors( Object_t *Object )
{
    size_t i;

    for( i = 0; i < OBJECT_MAXLAYERS; i++ )
    {
        if( GetModelPointCount( Object->ModelIDLoRes, i ) > 0 )
            Object->PointColorsLoRes[ i ] = new Color_t[GetModelPointCount( Object->ModelIDLoRes, i )];
        else
            Object->PointColorsLoRes[ i ] = nullptr;

        if( GetModelPointCount( Object->ModelIDHiRes, i ) > 0 )
            Object->PointColorsHiRes[ i ] = new Color_t[GetModelPointCount( Object->ModelIDHiRes, i )];
        else
            Object->PointColorsHiRes[ i ] = nullptr;

    }
}


//----------------------------------------------------------------------------------------
// Name: FreePointColors()
// Desc: 
//----------------------------------------------------------------------------------------

void FreePointColors( Object_t *Object )
{
    size_t i;

    for( i = 0; i < OBJECT_MAXLAYERS; i++ )
    {
        if( Object->PointColorsLoRes[ i ] )
            delete [] Object->PointColorsLoRes[ i ];

        if( Object->PointColorsHiRes[ i ] )
            delete [] Object->PointColorsHiRes[ i ];

        Object->PointColorsLoRes[ i ] = nullptr;
        Object->PointColorsHiRes[ i ] = nullptr;
    }
}


//----------------------------------------------------------------------------------------
// Name: FreePointColors()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IsObjectOnGround( Object_t *Object )
{
    Object->Physics.GroundHeight = GetMeshHeight( g_LandscapeMesh, Object->Positions[ 0 ].X,
                                                  Object->Positions[ 0 ].Z, &Object->Physics.PolyNormal );

    return( Object->Positions[ 0 ].Y <= Object->Physics.GroundHeight + DAMAGEHEIGHT );
}
