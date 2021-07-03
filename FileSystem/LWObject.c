//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 1 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <stdio.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sstream>

#include "TextureManager.h"

#include "LWObject.h"
#include "Bitmap.h"
#include "Targa.h"
#include "Vector.h"
#include "Error.h"


//-----------------------------------------------------------------------------
// private macros
//-----------------------------------------------------------------------------

#define LWOFlipDWord( dw )          ((((dw)&0x000000ff)<<24)|(((dw)&0x0000ff00)<<8)|  \
                                    (((dw)&0x00ff0000)>>8)|(((dw)&0xff000000)>>24))
#define LWOFlipWord( dw )           ((((dw)&0x00ff)<<8)|(((dw)&0xff00)>>8))

#define LWID( a, b, c, d )          (((a)<<24)|((b)<<16)|((c)<<8)|(d))

#define LW_FORM                     LWID('F','O','R','M')
#define LW_LWO2                     LWID('L','W','O','2')
#define LW_LAYR                     LWID('L','A','Y','R')
#define LW_PNTS                     LWID('P','N','T','S')
#define LW_POLS                     LWID('P','O','L','S')
#define LW_FACE                     LWID('F','A','C','E')
#define LW_VMAP                     LWID('V','M','A','P')
#define LW_VMAD                     LWID('V','M','A','D')
#define LW_TXUV                     LWID('T','X','U','V')
#define LW_CLIP                     LWID('C','L','I','P')
#define LW_STIL                     LWID('S','T','I','L')


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct LWOHandler_s
{
    long ChunkID;
    size_t (*Handler)(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);
}
LWOHandler_t;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t LWODispatch(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);

static long LWODWord(FILE* File);
static short LWOWord(FILE* File);
static float LWOFloat(FILE* File);
static short LWOString(FILE* File, char* String);
static size_t LWOIndex(FILE* File);

static size_t LWOChunkLAYR(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);
static size_t LWOChunkPNTS(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);
static size_t LWOChunkPOLS(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);
static size_t LWOChunkVMAP(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);
static size_t LWOChunkVMAD(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);
static size_t LWOChunkCLIP(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize);

static size_t LWOLoadSkin(LWObject_t* LWO);
static void LWOCalcBoundingSphere(LWObject_t* LWO);
static void LWOBuildPointNormals(LWObject_t* LWO);


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t CurrentLayer;
static size_t TexCoordCount;

static LWOTexCoord_t* TexCoords;

static LWOHandler_t DispatchTable[] =
{
    { LW_LAYR, LWOChunkLAYR },
    { LW_PNTS, LWOChunkPNTS },
    { LW_POLS, LWOChunkPOLS },
    { LW_VMAP, LWOChunkVMAP },
    { LW_VMAD, LWOChunkVMAD },
    { LW_CLIP, LWOChunkCLIP },
    { 0, 0 }
};


//-----------------------------------------------------------------------------
// Name: LWOLoad()
// Desc:
//-----------------------------------------------------------------------------

size_t LWOLoad(const std::string& FileName, LWObject_t* LWO)
{
    FILE* File;
    long FileSize;
    long StartOffset;
    long ChunkID;
    long ChunkSize;
    size_t i;
    char String[ 5 ];
    errno_t Error;

    TexCoords = nullptr;

    Error = fopen_s(&File, FileName.c_str(), "rb");
    if (Error != 0)
        return SetError("LWOLoad() failed open/read the file %s", FileName);

    ChunkID = LWODWord(File);
    if (LW_FORM != ChunkID)
    {
        fclose(File);
        return SetError("LWOLoad() the file %s is an invalid Lightwave Object file", FileName);
    }

    FileSize = LWODWord(File);

    ChunkID = LWODWord(File);
    if (LW_LWO2 != ChunkID)
    {
        fclose(File);
        return SetError("LWOLoad() the file %s is an invalid Lightwave Object file", FileName);
    }

    StartOffset = ftell(File);

    while (ftell(File) - StartOffset < FileSize - 4)
    {
        ChunkID   = LWODWord(File);
        ChunkSize = LWODWord(File);

        memcpy(String, &ChunkID, 4); String[ 4 ] = 0;

        if (!LWODispatch(LWO, File, ChunkID, ChunkSize))
            fseek(File, ChunkSize, SEEK_CUR);
    }

    fclose(File);

    LWO->LayerCount = 0;
    for (i = 0; i < LWO_MAXLAYERS; i++)
        if (LWO->Layers[ i ].PointCount > 0)
            LWO->LayerCount++;

    if (TexCoords)
        delete [] TexCoords;

    if (-1 == LWOLoadSkin(LWO))
    {
        LWOUnload(LWO);
        return 0;
    }

    LWOCalcBoundingSphere(LWO);
    LWOBuildPointNormals(LWO);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOUnload()
// Desc:
//-----------------------------------------------------------------------------

void LWOUnload(LWObject_t* LWO)
{
    size_t i;

    if (LWO)
    {
        DeleteTexture(LWO->TextureID);

        for (i = 0; i < LWO_MAXLAYERS; i++)
        {
            if (LWO->Layers[ i ].PointCount > 0)
            {
                if (LWO->Layers[ i ].Points)
                    delete [] LWO->Layers[ i ].Points;

                if (LWO->Layers[ i ].Polygons)
                    delete [] LWO->Layers[ i ].Polygons;

                if (LWO->Layers[ i ].PointNormals)
                    delete [] LWO->Layers[ i ].PointNormals;
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: LWOLoadSkin()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOLoadSkin(LWObject_t* LWO)
{
    if (!LWO)
        return SetError("LWOLoadSkin() was passed a nullptr pointer");

    std::ostringstream oss;
    if (strstr(LWO->SkinFilename, ".bmp"))
        oss << "BMP/" << strrchr(LWO->SkinFilename, '/') + 1;
    else
        oss << "TGA/" << strrchr(LWO->SkinFilename, '/') + 1;

    LWO->TextureID = CreateTexture(oss.str(), nullptr, nullptr, 1, 1);
    return LWO->TextureID;
}


//-----------------------------------------------------------------------------
// Name: LWODWord()
// Desc:
//-----------------------------------------------------------------------------

static void LWOCalcBoundingSphere(LWObject_t* LWO)
{
    float Largest, Mag;
    size_t LayerIdx, PointIdx;
    LWOLayer_t* Layer;

    Largest = 0.0f;
    for (LayerIdx = 0; LayerIdx < LWO->LayerCount; LayerIdx++)
    {
        Layer = &LWO->Layers[ LayerIdx ];
        if (!Layer->PointCount)
            continue;

        for (PointIdx = 0; PointIdx < LWO->Layers[ LayerIdx ].PointCount; PointIdx++)
        {
            Mag = (float)fabs(VectorMagnitude((Vector_t*)&Layer->Points[ PointIdx ]));
            if (Mag > Largest)
                Largest = Mag;
        }
    }

    LWO->BSphereRadius = Largest;
}


//-----------------------------------------------------------------------------
// Name: LWODWord()
// Desc:
//-----------------------------------------------------------------------------

static long LWODWord(FILE* File)
{
    long DWord;
    fread(&DWord, sizeof(long), 1, File);
    return LWOFlipDWord(DWord);
}


//-----------------------------------------------------------------------------
// Name: LWOWord()
// Desc:
//-----------------------------------------------------------------------------

static short LWOWord(FILE* File)
{
    short Word;
    fread(&Word, sizeof(short), 1, File);
    return LWOFlipWord(Word);
}


//-----------------------------------------------------------------------------
// Name: LWOFloat()
// Desc:
//-----------------------------------------------------------------------------

static float LWOFloat(FILE* File)
{
    float Float;
    long* DWord;

    DWord = (long*)&Float;
    fread(DWord, sizeof(float), 1, File);

    (*DWord) = LWOFlipDWord((*DWord));
    return Float;
}


//-----------------------------------------------------------------------------
// Name: LWOString()
// Desc:
//-----------------------------------------------------------------------------

static short LWOString(FILE* File, char* String)
{
    char Char;
    short BytesRead;

    BytesRead = 0;
    do
    {
        Char = fgetc(File);
        String[ BytesRead++ ] = Char;
    }
    while (Char);

    if (BytesRead & 0x01)
    {
        Char = fgetc(File);
        BytesRead++;
    }

    return BytesRead;
}


//-----------------------------------------------------------------------------
// Name: LWOIndex()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOIndex(FILE* File)
{
    size_t Index;
    unsigned char Char;

    Index = 0;

    Char = fgetc(File);
    if (Char == 0xff)
    {
        Char = fgetc(File);
        Index |= Char << 16;
        Char = fgetc(File);
        Index |= Char << 8;
        Char = fgetc(File);
        Index |= Char;
    }
    else
    {
        Index |= Char << 8;
        Char = fgetc(File);
        Index |= Char;
    }

    return Index;
}


//-----------------------------------------------------------------------------
// Name: LWODispatch()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWODispatch(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    LWOHandler_t* Handler;

    for (Handler = DispatchTable; Handler->ChunkID; Handler++)
    {
        if (Handler->ChunkID == ChunkID)
        {
            Handler->Handler(LWO, File, ChunkID, ChunkSize);
            return 1;
        }
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: LWOChunkLAYR()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOChunkLAYR(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    short Word;
    float Float;
    char String[ 1024 ];

    CurrentLayer = LWOWord(File);
    Word = LWOWord(File);

    Float = LWOFloat(File);
    Float = LWOFloat(File);
    Float = LWOFloat(File);

    Word = LWOString(File, String);

    if (Word + 16 < ChunkSize)
        Word = LWOWord(File);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOChunkPNTS()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOChunkPNTS(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    LWOLayer_t* Layer;
    size_t PointIndex;

    Layer = &LWO->Layers[ CurrentLayer ];

    Layer->PointCount = ChunkSize / (sizeof(float) * 3);
    Layer->Points = new LWOPoint_t[Layer->PointCount];
    if (nullptr == Layer->Points)
        return 0;

    for (PointIndex = 0; PointIndex < Layer->PointCount; PointIndex++)
    {
        Layer->Points[ PointIndex ].X = LWOFloat(File);
        Layer->Points[ PointIndex ].Y = LWOFloat(File);
        Layer->Points[ PointIndex ].Z = LWOFloat(File);
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOChunkPOLS()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOChunkPOLS(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    LWOLayer_t* Layer;
    long StartOffset;
    short PointCount;
    short i;

    if (LW_FACE != LWODWord(File))
        return 0;

    Layer = &LWO->Layers[ CurrentLayer ];

    StartOffset = ftell(File);

    // Count the Polys first
    while (ftell(File) - StartOffset < ChunkSize - 4)
    {
        PointCount = LWOWord(File);
        PointCount &= 0x03ff;

        if (3 != PointCount)        // only supporting triangles
            return 0;

        for (i = 0; i < PointCount; i++)
            LWOIndex(File);

        Layer->PolygonCount++;
    }

    Layer->Polygons = new LWOPolygon_t[Layer->PolygonCount];
    if (nullptr == Layer->Polygons)
        return 0;

    // now go read them in
    fseek(File, StartOffset, SEEK_SET);
    i = 0;
    while (ftell(File) - StartOffset < ChunkSize - 4)
    {
        PointCount = LWOWord(File);
        PointCount &= 0x03ff;

        if (3 != PointCount)        // only supporting triangles
            return 0;

        Layer->Polygons[ i ].P1 = LWOIndex(File);
        Layer->Polygons[ i ].P2 = LWOIndex(File);
        Layer->Polygons[ i ].P3 = LWOIndex(File);

        i++;
    }

    // copy all texcoords into the polygons
    for (i = 0; i < short(Layer->PolygonCount); i++)
    {
        Layer->Polygons[ i ].T1.U = TexCoords[ Layer->Polygons[ i ].P1 ].U;
        Layer->Polygons[ i ].T1.V = TexCoords[ Layer->Polygons[ i ].P1 ].V;

        Layer->Polygons[ i ].T2.U = TexCoords[ Layer->Polygons[ i ].P2 ].U;
        Layer->Polygons[ i ].T2.V = TexCoords[ Layer->Polygons[ i ].P2 ].V;

        Layer->Polygons[ i ].T3.U = TexCoords[ Layer->Polygons[ i ].P3 ].U;
        Layer->Polygons[ i ].T3.V = TexCoords[ Layer->Polygons[ i ].P3 ].V;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOChunkVMAP()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOChunkVMAP(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    LWOLayer_t* Layer;
    size_t Word, i;
    long StartOffset;
    short BytesRead;
    char String[ 1024 ];

    if (LW_TXUV != LWODWord(File))
        return 0;

    Word = LWOWord(File);                           // dimension of mapped vertices, should be 2 for TXUV subchunks
    BytesRead = 6 + LWOString(File, String);        // read the name of this texture

    Layer = &LWO->Layers[ CurrentLayer ];
    StartOffset = ftell(File);

    // Count how many there are
    TexCoordCount = 0;
    while (ftell(File) - StartOffset < ChunkSize - BytesRead)
    {
        LWOIndex(File); LWOFloat(File); LWOFloat(File);
        TexCoordCount++;
    }

    if (TexCoords)
        delete [] TexCoords;

    TexCoords = new LWOTexCoord_t[TexCoordCount];
    fseek(File, StartOffset, SEEK_SET);

    for (i = 0; i < TexCoordCount; i++)
    {
        Word = LWOIndex(File);
        TexCoords[ Word ].U = LWOFloat(File);
        TexCoords[ Word ].V = LWOFloat(File);
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOChunkVMAD()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOChunkVMAD(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    LWOLayer_t* Layer;
    size_t Vertex, Polygon, i;
    char String[ 1024 ];
    long StartOffset;
    short BytesRead;

    if (LW_TXUV != LWODWord(File))
        return 0;

    Vertex = LWOWord(File);
    BytesRead = 6 + LWOString(File, String);

    Layer = &LWO->Layers[ CurrentLayer ];
    StartOffset = ftell(File);

    // read em all in
    i = 0;
    while (ftell(File) - StartOffset < ChunkSize - BytesRead)
    {
        Vertex  = LWOIndex(File);
        Polygon = LWOIndex(File);

        if (Vertex == Layer->Polygons[ Polygon ].P1)
        {
            Layer->Polygons[ Polygon ].T1.U = LWOFloat(File);
            Layer->Polygons[ Polygon ].T1.V = LWOFloat(File);
        }
        else if (Vertex == Layer->Polygons[ Polygon ].P2)
        {
            Layer->Polygons[ Polygon ].T2.U = LWOFloat(File);
            Layer->Polygons[ Polygon ].T2.V = LWOFloat(File);
        }
        else if (Vertex == Layer->Polygons[ Polygon ].P3)
        {
            Layer->Polygons[ Polygon ].T3.U = LWOFloat(File);
            Layer->Polygons[ Polygon ].T3.V = LWOFloat(File);
        }

        i++;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOChunkCLIP()
// Desc:
//-----------------------------------------------------------------------------

static size_t LWOChunkCLIP(LWObject_t* LWO, FILE* File, long ChunkID, long ChunkSize)
{
    LWODWord(File);                         // clip index

    if (LW_STIL != LWODWord(File))          // STIL
        return 0;

    LWOWord(File);                          // subchunk size
    LWOString(File, LWO->SkinFilename);     // pathname

    return 1;
}


//-----------------------------------------------------------------------------
// Name: LWOBuildPointNormals()
// Desc:
//-----------------------------------------------------------------------------

static void LWOBuildPointNormals(LWObject_t* LWO)
{
    size_t Layer;
    size_t PointIdx, PolyIdx;
    float Count;
    Vector_t Normal, PolyNormal;
    Vector_t V1, V2;
    LWOPolygon_t* Poly;
    LWOLayer_t* LWOLayer;

    for (Layer = 0; Layer < LWO->LayerCount; Layer++)
    {
        LWOLayer = &LWO->Layers[ Layer ];

        LWOLayer->PointNormals = new Vector_t[LWOLayer->PointCount];

        // brute force method
        for (PointIdx = 0; PointIdx < LWOLayer->PointCount; PointIdx++)
        {
            VectorSet(Normal, 0.0f);
            Count = 0.0f;

            for (PolyIdx = 0; PolyIdx < LWOLayer->PolygonCount; PolyIdx++)
            {
                Poly = &LWOLayer->Polygons[ PolyIdx ];

                if (Poly->P1 == PointIdx || Poly->P2 == PointIdx || Poly->P3 == PointIdx)
                {
                    VectorSubtract(LWOLayer->Points[ Poly->P2 ], LWOLayer->Points[ Poly->P1 ], V1);
                    VectorSubtract(LWOLayer->Points[ Poly->P3 ], LWOLayer->Points[ Poly->P1 ], V2);
                    VectorNormalisedCrossProd(&V1, &V2, &PolyNormal);

                    VectorAdd(Normal, PolyNormal, Normal);
                    Count += 1.0f;
                }
            }

            LWOLayer->PointNormals[ PointIdx ].X = Normal.X / Count;
            LWOLayer->PointNormals[ PointIdx ].Y = Normal.Y / Count;
            LWOLayer->PointNormals[ PointIdx ].Z = Normal.Z / Count;
            VectorNormalise(&LWOLayer->PointNormals[ PointIdx ]);
        }
    }
}
