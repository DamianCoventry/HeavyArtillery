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
#include "TileSet.h"
#include "MeshTypes.h"


extern void MeshManagerThink( void );
extern void MeshManagerDraw( Plane2_t *ViewFrustum, float ViewPointX, float ViewPointZ );

extern size_t CreateMesh( TileSetType_t TileSetType, size_t TilesWide, size_t TilesDeep, float TileWidth, float TileDepth );
extern void DeleteMesh( size_t ID );

extern void ChooseNewTileSet( size_t ID, TileSetType_t TileSetType );
extern size_t AssignNewTileSet( size_t ID );
extern void GenerateNewMesh( size_t ID );

enum class IntersectionResult { IntersectsLandscape, ClearLineOfSight, InvalidParameter };
extern IntersectionResult LineMeshIntersect( size_t ID, Vector_t *LineStart, Vector_t *LineFinish,Polygon_t **Polygon, Vector_t *Intersect, float *Smallest );

extern float GetMeshHeight( size_t ID, float X, float Z, size_t *PolyIndex );

extern void GetMeshRotations( size_t ID, float X, float Z, float *XRotation, float *ZRotation );
extern void GetMeshNormal( size_t ID, float X, float Z, Vector_t *Normal );

extern void GetMeshCenter( size_t ID, Vector_t *Position );
extern void GetMeshMaxDimensions( size_t ID, float *MaxX, float *MaxY, float *MaxZ );

extern TileSetType_t GetMeshTileSetType( size_t ID );

extern void GetMeshSize( size_t ID, size_t *TilesWide, size_t *TilesDeep );
extern void GetMeshTileSize( size_t ID, float *TileWidth, float *TileDepth );

extern void GetPolygonNormal( size_t ID, size_t PolyIndex, Vector_t *Vector );

extern float GetMeshLargestHeight( size_t ID );

extern Mesh_t *GetMeshStruct( size_t ID );

extern size_t GetMeshLoadScreenTexture( size_t ID );
