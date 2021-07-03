//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 17 May 2002
//-----------------------------------------------------------------------------


#ifndef _DECALSTRUCTS_H_
#define _DECALSTRUCTS_H_


#define DECAL_MAXFRAGMENTS      64

#include "Vector.h"


typedef struct DecalTexture_s
{
    float Width, Height;
    float HalfWidth, HalfHeight;
    size_t TextureNumber;
}
DecalTexture_t;


typedef struct DecalTexCoord_s
{
    float U, V;
}
DecalTexCoord_t;


typedef struct DecalFragment_s
{
    DecalFragment_s()
    {
        ObjectPolyIndex = 0;
        VectorSet(Normal, 0.0f);
        TotallyBehind = 0;
        Next = nullptr;
    }

    Vector_t P1, P2, P3;
    DecalTexCoord_t T1, T2, T3;
    size_t ObjectPolyIndex;
    Vector_t Normal;
    size_t TotallyBehind;
    struct DecalFragment_s *Next;
}
DecalFragment_t;


typedef struct Decal_s
{
    size_t ExpireCounter;
    float ExpireAlpha;

    size_t FragmentCount;
    DecalFragment_t Fragments[ DECAL_MAXFRAGMENTS ];
    Vector_t Min, Max;

    DecalTexture_t *Texture;
    void *Object;                                       // pointer to object/mesh that this decal is applied to
}
Decal_t;


#endif
