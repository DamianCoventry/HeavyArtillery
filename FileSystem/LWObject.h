//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 1 April 2002
//-----------------------------------------------------------------------------

#ifndef _LWOBJECT_H_
#define _LWOBJECT_H_

#define LWO_MAXLAYERS       8

#include "Vector.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct LWOPoint_s
{
    float X, Y, Z;
}
LWOPoint_t;

typedef struct LWOTexCoord_s
{
    float U, V;
}
LWOTexCoord_t;

typedef struct LWOPolygon_s
{
    size_t P1, P2, P3;
    LWOTexCoord_t T1, T2, T3;
}
LWOPolygon_t;

typedef struct LWOLayer_s
{
    size_t PointCount;
    LWOPoint_t *Points;
    Vector_t *PointNormals;

    size_t PolygonCount;
    LWOPolygon_t *Polygons;
}
LWOLayer_t;

typedef struct LWObject_s
{
    size_t Used;
    size_t DrawWithAlpha;
    size_t TextureID;                  // used with TextureManager
    char SkinFilename[ 256 ];
    float BSphereRadius;
    float Scale;                    // used to draw into Dialog_t window
    size_t LayerCount;
    LWOLayer_t Layers[ LWO_MAXLAYERS ];
}
LWObject_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t LWOLoad( const std::string& FileName, LWObject_t *LWO );
void LWOUnload( LWObject_t *LWO );


#endif
