//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "MeshTypes.h"


extern void MeshCreate( Mesh_t *Mesh, TileSetType_t TileSetType, size_t TilesWide, size_t TilesDeep, float TileWidth, float TileDepth );
extern void MeshDelete( Mesh_t *Mesh );

extern void MeshGenerateLandHeights( Mesh_t *Mesh );
extern void MeshFindHeightLimits( Mesh_t *Mesh );

extern void MeshCalcNormals( Mesh_t *Mesh );
extern void MeshCalcPointNormals( Mesh_t *Mesh );
extern void MeshCalcMinMaxValues( Mesh_t *Mesh );
extern void MeshCalcShading( Mesh_t *Mesh );

extern void MeshChooseRandomTileSet( Mesh_t *Mesh );
