//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "MeshCreate.h"
#include "MeshTypes.h"
#include "MeshTex.h"

#include "Noise.h"
#include "LightSource.h"
#include "Utility.h"
#include "LoadingScreen.h"
#include "MeshManager.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

TileSet_t* g_TileSet;

extern size_t g_LandscapeMesh;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t _LoadingCount = 0;
static size_t _LoadingMax = 0;

static size_t _LoadscreenTexture = 0;
static size_t _NewMeshID = -1;

static TileSetType_t PreviousType = TILESET_RANDOM;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void MeshSetupPolygons(Mesh_t* Mesh);
static void MeshLookUpColor(Mesh_t* Mesh, Color_t* ColorTable, size_t ColorTableSize, float Height, Color_t* Color);

static void MeshProgress(unsigned short Cmd, unsigned short Count);



//-----------------------------------------------------------------------------
// Name: MeshCreateLand()
// Desc:
//-----------------------------------------------------------------------------

void MeshCreate(Mesh_t* Mesh, TileSetType_t TileSetType, size_t TilesWide, size_t TilesDeep, float TileWidth, float TileDepth)
{
    size_t ID = Mesh->ID;

    memset(Mesh, 0, sizeof(Mesh_t));
    Mesh->ID = ID;
    Mesh->Used = 1;

    Mesh->PointCount = (TilesWide + 1) * (TilesDeep + 1);
    Mesh->Points = new Vector_t[Mesh->PointCount];
    memset(Mesh->Points, 0, sizeof(Vector_t) * Mesh->PointCount);

    Mesh->Colors = new Color_t[Mesh->PointCount];
    memset(Mesh->Colors, 0, sizeof(Color_t) * Mesh->PointCount);

    Mesh->PointNormals = new Vector_t[Mesh->PointCount];
    memset(Mesh->PointNormals, 0, sizeof(Vector_t) * Mesh->PointCount);

    Mesh->PolygonCount = (2 * TilesWide) * TilesDeep;
    Mesh->Polygons = new Polygon_t[Mesh->PolygonCount];
    memset(Mesh->Polygons, 0, sizeof(Polygon_t) * Mesh->PolygonCount);

    Mesh->PolyMinMaxs = new MinMax_t[Mesh->PolygonCount];
    memset(Mesh->PolyMinMaxs, 0, sizeof(MinMax_t) * Mesh->PolygonCount);

    Mesh->TilesWide = TilesWide;
    Mesh->TilesDeep = TilesDeep;
    Mesh->TileWidth = TileWidth;
    Mesh->TileDepth = TileDepth;

    Mesh->MaxX = Mesh->TileWidth * Mesh->TilesWide;
    Mesh->MaxZ = Mesh->TileDepth * Mesh->TilesDeep;

    if (TileSetType == TILESET_RANDOM)      // we'll choose the tileset before we gen the heights.  so that we
        MeshChooseRandomTileSet(Mesh);      // can display the appropriate background pic for the load screen.
    else
        Mesh->TileSetType = TileSetType;

    MeshGenerateLandHeights(Mesh);
    MeshFindHeightLimits(Mesh);

    MeshSetupPolygons(Mesh);
    MeshCalcNormals(Mesh);
    MeshCalcPointNormals(Mesh);
    MeshCalcMinMaxValues(Mesh);
    MeshCalcShading(Mesh);

    std::ostringstream oss;
    oss << "TGA/" << TileSetEnumToString(Mesh->TileSetType);
    g_TileSet = TileSetLoad(oss.str());

    MeshAssignTextures(Mesh, g_TileSet, GetMeshLoadScreenTexture(Mesh->ID));
}


//-----------------------------------------------------------------------------
// Name: MeshDelete()
// Desc:
//-----------------------------------------------------------------------------

void MeshDelete(Mesh_t* Mesh)
{
    if (Mesh)
    {
        if (g_TileSet)
        {
            TileSetUnload(g_TileSet);
            g_TileSet = nullptr;
        }

        if (Mesh->Colors)
            delete []  Mesh->Colors;

        if (Mesh->Points)
            delete []  Mesh->Points;

        if (Mesh->PointNormals)
            delete []  Mesh->PointNormals;

        if (Mesh->PolyMinMaxs)
            delete [] Mesh->PolyMinMaxs;

        if (Mesh->Polygons)
            delete [] Mesh->Polygons;

        memset(Mesh, 0, sizeof(Mesh_t));
    }
}


//-----------------------------------------------------------------------------
// Name: MeshGenerateLandHeights()
// Desc:
//-----------------------------------------------------------------------------

void MeshGenerateLandHeights(Mesh_t* Mesh)
{
    short* Buffer;
    size_t X, Z, Index;

    _NewMeshID = Mesh->ID;

    Buffer = new short[Mesh->TilesWide * Mesh->TilesDeep];
    NoiseCanyon(Buffer, MeshProgress);

    Index = 0;
    for (Z = 0; Z < Mesh->TilesDeep + 1; Z++)
    {
        for (X = 0; X < Mesh->TilesWide + 1; X++)
        {
            Mesh->Points[ Index ].X = X * Mesh->TileWidth;
            Mesh->Points[ Index ].Z = Z * Mesh->TileDepth;

            if (X == Mesh->TilesWide)
            {
                if (Z == Mesh->TilesDeep)
                    Mesh->Points[ Index ].Y = (float)Buffer[ 0 ];
                else
                    Mesh->Points[ Index ].Y = (float)Buffer[ Z * Mesh->TilesWide ];
            }
            else
            {
                if (Z == Mesh->TilesDeep)
                    Mesh->Points[ Index ].Y = (float)Buffer[ X ];
                else
                    Mesh->Points[ Index ].Y = (float)Buffer[ Z * Mesh->TilesWide + X ];
            }

            Index++;
        }
    }

    delete [] Buffer;
}


//-----------------------------------------------------------------------------
// Name: MeshSetupPolygons()
// Desc:
//-----------------------------------------------------------------------------

static void MeshSetupPolygons(Mesh_t* Mesh)
{
    size_t X, Z, Index;
    Polygon_t* Poly;

    Index = 0;
    for (Z = 0; Z < Mesh->TilesDeep; Z++)
    {
        for (X = 0; X < Mesh->TilesWide; X++)
        {
            // every second triangle goes in the opposite direction
            if ((!(Z & 1) && (X & 1)) || ((Z & 1) && !(X & 1)))
            {
                Poly = &Mesh->Polygons[ Index ];
                Poly->P1 = (Z * (Mesh->TilesWide + 1)) + X ;
                Poly->P2 = Poly->P1 + Mesh->TilesWide + 1;
                Poly->P3 = Poly->P1 + 1;

                Mesh->PolyMinMaxs[ Index ].Min = Min(Min(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);
                Mesh->PolyMinMaxs[ Index ].Max = Max(Max(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);

                Index++;

                Poly = &Mesh->Polygons[ Index ];
                Poly->P1 = Mesh->Polygons[ Index - 1 ].P1 + Mesh->TilesWide + 1;
                Poly->P2 = Mesh->Polygons[ Index - 1 ].P1 + Mesh->TilesWide + 2;
                Poly->P3 = Mesh->Polygons[ Index - 1 ].P1 + 1;

                Mesh->PolyMinMaxs[ Index ].Min = Min(Min(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);
                Mesh->PolyMinMaxs[ Index ].Max = Max(Max(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);

                Index++;
            }
            else
            {
                Poly = &Mesh->Polygons[ Index ];
                Poly->P1 = (Z * (Mesh->TilesWide + 1)) + X ;
                Poly->P2 = Poly->P1 + Mesh->TilesWide + 1;
                Poly->P3 = Poly->P1 + Mesh->TilesWide + 2;

                Mesh->PolyMinMaxs[ Index ].Min = Min(Min(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);
                Mesh->PolyMinMaxs[ Index ].Max = Max(Max(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);

                Index++;

                Poly = &Mesh->Polygons[ Index ];
                Poly->P1 = Mesh->Polygons[ Index - 1 ].P1;
                Poly->P2 = Mesh->Polygons[ Index - 1 ].P1 + Mesh->TilesWide + 2;
                Poly->P3 = Mesh->Polygons[ Index - 1 ].P1 + 1;

                Mesh->PolyMinMaxs[ Index ].Min = Min(Min(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);
                Mesh->PolyMinMaxs[ Index ].Max = Max(Max(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);

                Index++;
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshFindHeightLimits()
// Desc:
//-----------------------------------------------------------------------------

void MeshFindHeightLimits(Mesh_t* Mesh)
{
    size_t Z;

    Mesh->SmallestHeight =  32767;
    Mesh->LargestHeight  = -32767;

    for (Z = 0; Z < Mesh->PointCount; Z++)
    {
        if (Mesh->Points[ Z ].Y < Mesh->SmallestHeight)
            Mesh->SmallestHeight = (short)Mesh->Points[ Z ].Y;

        if (Mesh->Points[ Z ].Y > Mesh->LargestHeight)
            Mesh->LargestHeight = (short)Mesh->Points[ Z ].Y;
    }

    for (Z = 0; Z < Mesh->PointCount; Z++)
        Mesh->Points[ Z ].Y -= Mesh->SmallestHeight;

    Mesh->LargestHeight  -= Mesh->SmallestHeight;
    Mesh->SmallestHeight -= Mesh->SmallestHeight;
}


//-----------------------------------------------------------------------------
// Name: MeshCalcNormals()
// Desc:
//-----------------------------------------------------------------------------

void MeshCalcNormals(Mesh_t* Mesh)
{
    size_t i;
    Vector_t V1, V2;

    for (i = 0; i < Mesh->PolygonCount; i++)
    {
        VectorSubtract(Mesh->Points[ Mesh->Polygons[ i ].P2 ], Mesh->Points[ Mesh->Polygons[ i ].P1 ], V1);
        VectorSubtract(Mesh->Points[ Mesh->Polygons[ i ].P3 ], Mesh->Points[ Mesh->Polygons[ i ].P1 ], V2);
        VectorNormalisedCrossProd(&V1, &V2, &Mesh->Polygons[ i ].Normal);
    }
}



//-----------------------------------------------------------------------------
// Name: MeshLookUpColor()
// Desc:
//-----------------------------------------------------------------------------

static void MeshLookUpColor(Mesh_t* Mesh, Color_t* ColorTable, size_t ColorTableSize, float Height, Color_t* Color)
{
    size_t Index;
    size_t Divisor;
    float Percent;

    Divisor = (Mesh->LargestHeight - Mesh->SmallestHeight) / (ColorTableSize - 1);
    if (0 == Divisor)
    {
        Color->R = 0.0f;
        Color->G = 0.5f;
        Color->B = 0.0f;
    }
    else
    {
        Index = (size_t)Height / Divisor;

        Percent = (Height - (Index * Divisor)) / (float)Divisor;

        Color->R = CosineInterp(ColorTable[ Index ].R, ColorTable[ Index + 1 ].R, Percent) / 256.0f;
        Color->G = CosineInterp(ColorTable[ Index ].G, ColorTable[ Index + 1 ].G, Percent) / 256.0f;
        Color->B = CosineInterp(ColorTable[ Index ].B, ColorTable[ Index + 1 ].B, Percent) / 256.0f;
        Color->A = ColorTable[ Index ].A;
    }
}


//-----------------------------------------------------------------------------
// Name: MeshCalcMinMaxValues()
// Desc:
//-----------------------------------------------------------------------------

void MeshCalcMinMaxValues(Mesh_t* Mesh)
{
    size_t i;
    Polygon_t* Poly;

    for (i = 0; i < Mesh->PolygonCount; i++)
    {
        Poly = &Mesh->Polygons[ i ];

        Mesh->PolyMinMaxs[ i ].Min = Min(Min(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);
        Mesh->PolyMinMaxs[ i ].Max = Max(Max(Mesh->Points[ Poly->P1 ].Y, Mesh->Points[ Poly->P2 ].Y), Mesh->Points[ Poly->P3 ].Y);
    }
}


//-----------------------------------------------------------------------------
// Name: MeshCalcShading()
// Desc:
//-----------------------------------------------------------------------------

void MeshCalcShading(Mesh_t* Mesh)
{
    size_t i;
    float Dot, Ambient;
    Vector_t V;

    GetLightSourceDirection(&V.X, &V.Y, &V.Z);
    GetLightSourceAmbient(&Ambient);

    // per vertex shading
    for (i = 0; i < Mesh->PointCount; i++)
    {
        Dot = -VectorDotProd(Mesh->PointNormals[ i ], V);
        if (Dot < 0.0f)
            Dot = 0.0f;

        Mesh->Colors[ i ].R = Dot + Ambient;
        if (Mesh->Colors[ i ].R > 1.0f)
            Mesh->Colors[ i ].R = 1.0f;

        Mesh->Colors[ i ].G = Mesh->Colors[ i ].R;
        Mesh->Colors[ i ].B = Mesh->Colors[ i ].R;
        Mesh->Colors[ i ].A = 1.0f;
    }
}


//-----------------------------------------------------------------------------
// Name: MeshCalcPointNormals()
// Desc:
//-----------------------------------------------------------------------------

void MeshCalcPointNormals(Mesh_t* Mesh)
{
    size_t PointIdx, PolyIdx;
    float Count;
    Vector_t Normal;
    Polygon_t* Poly;

    // brute force method
    for (PointIdx = 0; PointIdx < Mesh->PointCount; PointIdx++)
    {
        VectorSet(Normal, 0.0f);
        Count = 0.0f;

        for (PolyIdx = 0; PolyIdx < Mesh->PolygonCount; PolyIdx++)
        {
            Poly = &Mesh->Polygons[ PolyIdx ];

            if (Poly->P1 == PointIdx || Poly->P2 == PointIdx || Poly->P3 == PointIdx)
            {
                VectorAdd(Normal, Poly->Normal, Normal);
                Count += 1.0f;
            }
        }

        Mesh->PointNormals[ PointIdx ].X = Normal.X / Count;
        Mesh->PointNormals[ PointIdx ].Y = Normal.Y / Count;
        Mesh->PointNormals[ PointIdx ].Z = Normal.Z / Count;
        VectorNormalise(&Mesh->PointNormals[ PointIdx ]);
    }
}


//-----------------------------------------------------------------------------
// Name: MeshProgress()
// Desc:
//-----------------------------------------------------------------------------

static void MeshProgress(unsigned short Cmd, unsigned short Count)
{
    switch (Cmd)
    {
    case 0 :
        _LoadingCount = 0;
        _LoadingMax = static_cast<size_t>(Count);

        _LoadscreenTexture = GetMeshLoadScreenTexture(_NewMeshID);

        BeginLoadingScreen(_LoadscreenTexture, "Generating World...", 0, _LoadingMax);
        break;

    case 1 :
        UpdateLoadingScreen(++_LoadingCount);
        break;

    case 2 :
        EndLoadingScreen("Generating World...Done", _LoadingMax);
        break;
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChooseRandomTileSet()
// Desc:
//-----------------------------------------------------------------------------

void MeshChooseRandomTileSet(Mesh_t* Mesh)
{
    if (nullptr == Mesh)
        return;

    do
    {
        Mesh->TileSetType = (TileSetType_t)((rand() % TILESET_STONE) + 1);
    }
    while (PreviousType == Mesh->TileSetType);

    PreviousType = Mesh->TileSetType;
}
