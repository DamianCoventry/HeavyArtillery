//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#ifndef _MESH_H_
#define _MESH_H_


#include "Vector.h"
#include "TileSet.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct MinMax_s
{
    float Min;
    float Max;
}
MinMax_t;

#ifndef _COLOR_STRUCT_
#define _COLOR_STRUCT_

typedef struct Color_s
{
    float R, G, B, A;
}
Color_t;

#endif

typedef struct Mesh_s
{
    size_t Used;
    size_t ID;
    size_t TilesWide;
    size_t TilesDeep;

    float TileWidth;
    float TileDepth;

    short SmallestHeight;
    short LargestHeight;

    float MaxX, MaxZ;
    float XOffset, ZOffset;

    size_t PointCount;
    Vector_t *Points;
    Color_t *Colors;
    Vector_t *PointNormals;

    size_t PolygonCount;
    Polygon_t *Polygons;
    MinMax_t *PolyMinMaxs;

    TileSetType_t TileSetType;
}
Mesh_t;



#endif
