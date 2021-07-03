//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 22 June 2002
//-----------------------------------------------------------------------------


#ifndef _MESHDRAW_H_
#define _MESHDRAW_H_

#include "MeshTypes.h"


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

void SetMeshLOD(size_t Level );           // 0, 1 or 2
void SetMeshDrawStyle( long Style );     // 0=no shading, 1=shading

long GetMeshLODDistance( void );
long GetMeshLODStep0( void );
long GetMeshLODStep1( void );
long GetMeshLODStep2( void );
long GetMeshLODStep3( void );

void MeshDrawPatch0( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z );
void MeshDrawPatch1( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z, long DistanceX, long DistanceZ );
void MeshDrawPatch2( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z );
void MeshDrawPatch3( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z );
/*
void MeshDrawPatch0MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z );
void MeshDrawPatch1MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z, long DistanceX, long DistanceZ );
void MeshDrawPatch2MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z );
void MeshDrawPatch3MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z );
*/

#endif
