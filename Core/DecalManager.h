//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "Vector.h"
#include "DecalStructs.h"
#include "MeshTypes.h"

extern size_t DecalMgrCreate( Mesh_t *Mesh, DecalTexture_t *DecalTexture, float X, float Z );
extern void DecalMgrDelete( Mesh_t *Mesh, size_t PolyIndex );

extern void DecalMgrExpire( void );
extern void DecalMgrDraw( Plane2_t *ViewFrustum );
