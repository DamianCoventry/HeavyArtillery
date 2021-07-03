//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "ParticleTypes.h"
#include "Camera.h"

#define MAXDISPLAYLISTNODE      4096


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static DisplayListNode_t *_Head = nullptr;

static size_t _DisplayListCount = 0;
static DisplayListNode_t _DisplayListPool[ MAXDISPLAYLISTNODE ];


//----------------------------------------------------------------------------------------
// Name: ResetDisplayList()
// Desc: 
//----------------------------------------------------------------------------------------

void ResetDisplayList( void )
{
    _Head = nullptr;
    _DisplayListCount = 0;
}


//----------------------------------------------------------------------------------------
// Name: AddParticlesToDisplayList()
// Desc: 
//----------------------------------------------------------------------------------------

static void InsertDisplayListNode( DisplayListNode_t *Node )
{
    DisplayListNode_t *Current;
    DisplayListNode_t *Previous;

    // this is an insertion sort
    Current = _Head;
    Previous = nullptr;
    while( Current )
    {
        if( Current->Distance < Node->Distance )
            break;

        Previous = Current;
        Current = Current->Next;
    }

    // insert the new node before the "Current" node
    if( Previous == nullptr )
    {
        // create a new head of the list
        Node->Next = _Head;
        _Head = Node;
    }
    else
    {
        // linking the new node somewhere in the list
        Node->Next = Current;
        Previous->Next = Node;
    }
}


//----------------------------------------------------------------------------------------
// Name: AddParticlesToDisplayList()
// Desc: 
//----------------------------------------------------------------------------------------

void AddParticlesToDisplayList( ParticleSystem_t *System, float *ModelViewMatrix )
{
    size_t i;
    Vector_t *CameraPos;

    if( _DisplayListCount >= MAXDISPLAYLISTNODE )
        return;

    for( i = 0; i < MAXPARTICLESINSYSTEM; i++ )
    {
        if( PARTICLESTATE_ALIVE == System->Particles[ i ].State )
        {
            if( System->Template->AttachedToCamera )
            {
                CameraPos = GetCameraPosition();
                VectorAdd( (*CameraPos), System->Particles[ i ].Position, _DisplayListPool[ _DisplayListCount ].Position );
            }
            else
            {
                VectorCopy( System->Particles[ i ].Position, _DisplayListPool[ _DisplayListCount ].Position );
            }

            _DisplayListPool[ _DisplayListCount ].Next      = nullptr;
            _DisplayListPool[ _DisplayListCount ].System    = System;
            _DisplayListPool[ _DisplayListCount ].Particle  = &System->Particles[ i ];
            _DisplayListPool[ _DisplayListCount ].Distance  = fabsf( ( ModelViewMatrix[  2 ] * System->Particles[ i ].Position.X ) +
                                                                     ( ModelViewMatrix[  6 ] * System->Particles[ i ].Position.Y ) +
                                                                     ( ModelViewMatrix[ 10 ] * System->Particles[ i ].Position.Z ) +
                                                                     ( ModelViewMatrix[ 14 ] ) );

            InsertDisplayListNode( &_DisplayListPool[ _DisplayListCount ] );

            if( ++_DisplayListCount >= MAXDISPLAYLISTNODE )
                return;
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: DrawDisplayList()
// Desc: 
//----------------------------------------------------------------------------------------

void DrawDisplayList( Plane2_t *ViewFrustum, float *ModelViewMatrix )
{
    DisplayListNode_t *Current = _Head;

    while( Current )
    {
        if( PARTICLESTATE_DEAD != Current->Particle->State && Current->System->Draw &&
            VectorIsSphereInFrustum( ViewFrustum, &Current->Position, Current->Particle->BSphereSize ) )
            Current->System->Draw( Current->System, Current->Particle, &Current->Position, ModelViewMatrix );

        Current = Current->Next;
    }
}
