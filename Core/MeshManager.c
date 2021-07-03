//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>

#include "MeshTypes.h"
#include "MeshDraw.h"
#include "MeshCreate.h"
#include "MeshTex.h"

#include "LoadingScreen.h"
#include "GameTypes.h"

#include "MeshManager.h"
#include "TextureManager.h"

#define MAXMESHS                4

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXMESHS || !_MeshPool[ _id ].Used )


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern TileSet_t *g_TileSet;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _MeshIndex = 0;
static Mesh_t _MeshPool[ MAXMESHS ];

static float _RockFogColor[ 4 ]  = { 0.05000f, 0.05000f, 0.05000f, 1.0f };
static float _SandFogColor[ 4 ]  = { 0.66484f, 0.66484f, 0.26953f, 1.0f };
static float _SnowFogColor[ 4 ]  = { 0.87500f, 0.87500f, 0.93750f, 1.0f };


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static void MeshDraw( Mesh_t *Mesh, Plane2_t *ViewFrustum, float ViewPointX, float ViewPointZ );


//----------------------------------------------------------------------------------------
// Name: GetFreeMesh()
// Desc: 
//----------------------------------------------------------------------------------------

static size_t GetFreeMesh( void )
{
    size_t i;
    size_t ID;

    i = _MeshIndex;

    while( _MeshPool[ i ].Used )
    {
        i++;
        if( i >= MAXMESHS )
            i = 0;

        if( i == _MeshIndex )
            return -1;        // pool is full
    }

    _MeshPool[ i ].Used = 1;
    ID = i;

    _MeshIndex = i;
    if( ++_MeshIndex >= MAXMESHS )
        _MeshIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnMesh()
// Desc: 
//----------------------------------------------------------------------------------------

static void ReturnMesh( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _MeshPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: StartMeshAtmospheric()
// Desc: 
//----------------------------------------------------------------------------------------

static void StartMeshAtmospheric( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    switch( _MeshPool[ ID ].TileSetType )
    {
        case TILESET_DIRT :
            /*
            // create the rain particle system
            Point.X = ParticleEmitterRain.CellWidth / 2.0f;
            Point.Y = World->Landscape.LargestHeight * 2.0f;
            Point.Z = ParticleEmitterRain.CellDepth / 2.0f;
            System = ParticleMgrCreate( &ParticleTemplateRain, &ParticleEmitterRain, nullptr, &Point );
            if( System )
                ParticleMgrActivate( System );
            */
            break;

        case TILESET_GRASS :
            /*
            // create the rain particle system
            Point.X = ParticleEmitterRain.CellWidth / 2.0f;
            Point.Y = World->Landscape.LargestHeight * 2.0f;
            Point.Z = ParticleEmitterRain.CellDepth / 2.0f;
            System = ParticleMgrCreate( &ParticleTemplateRain, &ParticleEmitterRain, nullptr, &Point );
            if( System )
                ParticleMgrActivate( System );
            */
            break;

        case TILESET_MUD :
            /*
            // create the rain particle system
            Point.X = ParticleEmitterRain.CellWidth / 2.0f;
            Point.Y = World->Landscape.LargestHeight * 2.0f;
            Point.Z = ParticleEmitterRain.CellDepth / 2.0f;
            System = ParticleMgrCreate( &ParticleTemplateRain, &ParticleEmitterRain, nullptr, &Point );
            if( System )
                ParticleMgrActivate( System );
            */
            break;

        case TILESET_ROCK :
            glEnable( GL_FOG );
            glFogf( GL_FOG_MODE, GL_LINEAR );
            glFogf( GL_FOG_DENSITY, 0.5f );
            glFogf( GL_FOG_START, 0.0f );
            glFogf( GL_FOG_END, 10000.0f );

            glFogfv( GL_FOG_COLOR, _RockFogColor );
            break;

        case TILESET_SAND :
            glEnable( GL_FOG );
            glFogf( GL_FOG_MODE, GL_LINEAR );
            glFogf( GL_FOG_DENSITY, 1.0f );
            glFogf( GL_FOG_START, 0.0f );
            glFogf( GL_FOG_END, 7500.0f );

            glFogfv( GL_FOG_COLOR, _SandFogColor );
            break;

        case TILESET_SNOW :
            /*
            // create the snow particle system
            Point.X = ParticleEmitterSnow.CellWidth / 2.0f;
            Point.Y = World->Landscape.LargestHeight * 2.0f;
            Point.Z = ParticleEmitterSnow.CellDepth / 2.0f;
            ParticleEmitterSnow.VelocityChange = CHANGE_ALL;
            System = ParticleMgrCreate( &ParticleTemplateSnow, &ParticleEmitterSnow, nullptr, &Point );
            if( System )
                ParticleMgrActivate( System );
            */
            glEnable( GL_FOG );
            glFogf( GL_FOG_MODE, GL_LINEAR );
            glFogf( GL_FOG_DENSITY, 1.0f );
            glFogf( GL_FOG_START, 0.0f );
            glFogf( GL_FOG_END, 5000.0f );

            glFogfv( GL_FOG_COLOR, _SnowFogColor );
            break;

        case TILESET_STONE :
            /*
            // create the rain particle system
            Point.X = ParticleEmitterRain.CellWidth / 2.0f;
            Point.Y = World->Landscape.LargestHeight * 2.0f;
            Point.Z = ParticleEmitterRain.CellDepth / 2.0f;
            System = ParticleMgrCreate( &ParticleTemplateRain, &ParticleEmitterRain, nullptr, &Point );
            if( System )
                ParticleMgrActivate( System );
            */
            break;
    }
}


//----------------------------------------------------------------------------------------
// Name: StopMeshAtmospheric()
// Desc: 
//----------------------------------------------------------------------------------------

static void StopMeshAtmospheric( size_t ID )
{
    glDisable( GL_FOG );
}


//----------------------------------------------------------------------------------------
// Name: CreateMesh()
// Desc: 
//----------------------------------------------------------------------------------------

size_t CreateMesh( TileSetType_t TileSetType, size_t TilesWide, size_t TilesDeep, float TileWidth, float TileDepth )
{
    size_t ID;

    ID = GetFreeMesh();
    if( -1 == ID )
        return -1;

    _MeshPool[ ID ].Used = 1;
    _MeshPool[ ID ].ID = ID;

    MeshCreate( &_MeshPool[ ID ], TileSetType, TilesWide, TilesDeep, TileWidth, TileDepth );

    StartMeshAtmospheric( ID );

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: DeleteMesh()
// Desc: 
//----------------------------------------------------------------------------------------

void DeleteMesh( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    MeshDelete( &_MeshPool[ ID ] );

    ReturnMesh( ID );
}


//----------------------------------------------------------------------------------------
// Name: ChooseNewTileSet()
// Desc: 
//----------------------------------------------------------------------------------------

void ChooseNewTileSet( size_t ID, TileSetType_t TileSetType )
{
#if DEMO
    _MeshPool[ ID ].TileSetType = TILESET_STONE;
#else
    if( TILESET_RANDOM == TileSetType )
        MeshChooseRandomTileSet( &_MeshPool[ ID ] );
    else
        _MeshPool[ ID ].TileSetType = TileSetType;
#endif
}


//----------------------------------------------------------------------------------------
// Name: AssignNewTileSet()
// Desc: 
//----------------------------------------------------------------------------------------

size_t AssignNewTileSet( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    TileSetUnload( g_TileSet );
    StopMeshAtmospheric( ID );

    std::ostringstream oss;
    oss << "TGA/" << TileSetEnumToString(_MeshPool[ID].TileSetType);
    g_TileSet = TileSetLoad(oss.str());

    MeshAssignTextures( &_MeshPool[ ID ], g_TileSet, GetMeshLoadScreenTexture( ID ) );
    StartMeshAtmospheric( ID );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: GenerateNewMesh()
// Desc: 
//----------------------------------------------------------------------------------------

void GenerateNewMesh( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    MeshGenerateLandHeights( &_MeshPool[ ID ] );
    MeshCalcNormals( &_MeshPool[ ID ] );
    MeshCalcPointNormals( &_MeshPool[ ID ] );
    MeshCalcMinMaxValues( &_MeshPool[ ID ] );
    MeshCalcShading( &_MeshPool[ ID ] );
}


//----------------------------------------------------------------------------------------
// Name: MeshManagerThink()
// Desc: 
//----------------------------------------------------------------------------------------

void MeshManagerThink( void )
{
}


//----------------------------------------------------------------------------------------
// Name: MeshManagerDraw()
// Desc: 
//----------------------------------------------------------------------------------------

void MeshManagerDraw( Plane2_t *ViewFrustum, float ViewPointX, float ViewPointZ )
{
    size_t i;

    for( i = 0; i < MAXMESHS; i++ )
    {
        if( _MeshPool[ i ].Used )
            MeshDraw( &_MeshPool[ i ], ViewFrustum, ViewPointX, ViewPointZ );
    }
}


//----------------------------------------------------------------------------------------
// Name: LineMeshIntersect()
// Desc: 
//----------------------------------------------------------------------------------------

IntersectionResult LineMeshIntersect( size_t ID, Vector_t *LineStart, Vector_t *LineFinish, Polygon_t **Polygon, Vector_t *Intersect, float *Smallest )
{
    size_t Poly;
    float Mag;
    Vector_t Temp;
    Mesh_t *Mesh;

    if( INVALIDID( ID ) )
        return IntersectionResult::InvalidParameter;

    if( nullptr == Polygon || nullptr == Intersect || nullptr == Smallest )
        return IntersectionResult::InvalidParameter;

    Mesh = &_MeshPool[ ID ];

    (*Smallest) = 10000000.0f;
    IntersectionResult Result = IntersectionResult::ClearLineOfSight;

    for( Poly = 0; Poly < Mesh->PolygonCount; Poly++ )
    {
        if( MathLineTriangleIntersect( &Mesh->Points[ Mesh->Polygons[ Poly ].P1 ], &Mesh->Points[ Mesh->Polygons[ Poly ].P2 ]
                                     , &Mesh->Points[ Mesh->Polygons[ Poly ].P3 ], &Mesh->Polygons[ Poly ].Normal
                                     , LineStart, LineFinish ) )
        {
            MathLinePlaneIntersect( &Mesh->Points[ Mesh->Polygons[ Poly ].P1 ], &Mesh->Polygons[ Poly ].Normal
                                  , LineStart, LineFinish, &Temp );

            Mag = VectorMagnitude2( LineStart, &Temp );
            Mag = fabsf( Mag );

            if( Mag < (*Smallest) )
            {
                (*Smallest) = Mag;

                VectorCopy( Temp, (*Intersect) );
                (*Polygon) = &Mesh->Polygons[ Poly ];
            }

            Result = IntersectionResult::IntersectsLandscape;
        }
    }

    return Result;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshHeight()
// Desc: 
//----------------------------------------------------------------------------------------

float GetMeshHeight( size_t ID, float X, float Z, size_t *PolyIndex )
{
    size_t OffsetX, OffsetZ;
    size_t TileX, TileZ;
    size_t Index;
    size_t IntX, IntZ;
    Polygon_t *Polygon;
    float D;
    Mesh_t *Mesh;

    if( INVALIDID( ID ) )
        return 0;

    Mesh = &_MeshPool[ ID ];

    if( X < 0.0f || X >= Mesh->MaxX )
        return 0.0f;

    if( Z < 0.0f || Z >= Mesh->MaxZ )
        return 0.0f;

    IntX = (size_t)X;
    IntZ = (size_t)Z;

    TileX = IntX / (size_t)Mesh->TileWidth;
    TileZ = IntZ / (size_t)Mesh->TileDepth;

    OffsetX = IntX % (size_t)Mesh->TileWidth;
    OffsetZ = IntZ % (size_t)Mesh->TileDepth;

    Index = TileZ * ( 2 * Mesh->TilesWide ) + ( 2 * TileX );

    if( ( !(TileZ & 1) && (TileX & 1) ) || ( (TileZ & 1) && !(TileX & 1) ) )
    {
        if( OffsetZ > Mesh->TileWidth - OffsetX )
            Index++;
    }
    else
    {
        if( OffsetX > OffsetZ )
            Index++;
    }

    if( PolyIndex )
        (*PolyIndex) = Index;

    Polygon = &Mesh->Polygons[ Index ];

    if( Polygon->Normal.Y > -EPSILON && Polygon->Normal.Y < EPSILON )
        return Mesh->Points[ Polygon->P1 ].Y;

    D = -( ( Polygon->Normal.X * Mesh->Points[ Polygon->P1 ].X ) +
           ( Polygon->Normal.Y * Mesh->Points[ Polygon->P1 ].Y ) +
           ( Polygon->Normal.Z * Mesh->Points[ Polygon->P1 ].Z ) );

    // re-arranged plane equation
    return( ( ( Polygon->Normal.X * X ) + ( Polygon->Normal.Z * Z ) + D ) / ( -Polygon->Normal.Y ) );
}


//----------------------------------------------------------------------------------------
// Name: GetMeshRotations()
// Desc: 
//----------------------------------------------------------------------------------------

void GetMeshRotations( size_t ID, float X, float Z,
                       float *XRotation, float *ZRotation )
{
    size_t TileX, TileZ;
    size_t OffsetX, OffsetZ;
    size_t Index;
    size_t IntX, IntZ;
    Mesh_t *Mesh;

    if( INVALIDID( ID ) )
        return;

    Mesh = &_MeshPool[ ID ];

    while( X < 0.0f )
        X += Mesh->MaxX;

    while( X >= Mesh->MaxX )
        X -= Mesh->MaxX;

    while( Z < 0.0f )
        Z += Mesh->MaxZ;

    while( Z >= Mesh->MaxZ )
        Z -= Mesh->MaxZ;

    IntX = (size_t)X;
    IntZ = (size_t)Z;

    TileX = IntX / (size_t)Mesh->TileWidth;
    TileZ = IntZ / (size_t)Mesh->TileDepth;

    OffsetX = IntX % (size_t)Mesh->TileWidth;
    OffsetZ = IntZ % (size_t)Mesh->TileDepth;

    Index = TileZ * ( 2 * Mesh->TilesWide ) + ( 2 * TileX );

    if( ( !(TileZ & 1) && (TileX & 1) ) || ( (TileZ & 1) && !(TileX & 1) ) )
    {
        if( OffsetZ > Mesh->TileWidth - OffsetX )
            Index++;
    }
    else
    {
        if( OffsetX > OffsetZ )
            Index++;
    }

    if( XRotation )
        (*XRotation) = -Mesh->Polygons[ Index ].Normal.Z * RTOD;

    if( ZRotation )
        (*ZRotation) =  Mesh->Polygons[ Index ].Normal.X * RTOD;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshNormal()
// Desc: 
//----------------------------------------------------------------------------------------

void GetMeshNormal( size_t ID, float X, float Z, Vector_t *Normal )
{
    size_t TileX, TileZ;
    size_t OffsetX, OffsetZ;
    size_t Index;
    size_t IntX, IntZ;
    Mesh_t *Mesh;

    if( INVALIDID( ID ) )
        return;

    Mesh = &_MeshPool[ ID ];

    while( X < 0.0f )
        X += Mesh->MaxX;

    while( X >= Mesh->MaxX )
        X -= Mesh->MaxX;

    while( Z < 0.0f )
        Z += Mesh->MaxZ;

    while( Z >= Mesh->MaxZ )
        Z -= Mesh->MaxZ;

    IntX = (size_t)X;
    IntZ = (size_t)Z;

    TileX = IntX / (size_t)Mesh->TileWidth;
    TileZ = IntZ / (size_t)Mesh->TileDepth;

    OffsetX = IntX % (size_t)Mesh->TileWidth;
    OffsetZ = IntZ % (size_t)Mesh->TileDepth;

    Index = TileZ * ( 2 * Mesh->TilesWide ) + ( 2 * TileX );

    if( ( !(TileZ & 1) && (TileX & 1) ) || ( (TileZ & 1) && !(TileX & 1) ) )
    {
        if( OffsetZ > Mesh->TileWidth - OffsetX )
            Index++;
    }
    else
    {
        if( OffsetX > OffsetZ )
            Index++;
    }

    VectorCopy( Mesh->Polygons[ Index ].Normal, (*Normal) );
}


/*
    patch types:

    type 0      type 1      type 2      type 3
    -------     -------     -------     -------
    |\ | /|     |\   /|     |\   /|     |\    |
    | \|/ |     | \ / |     | \ / |     | \   |
    |--|--|     |--|--|     |  |  |     |  \  |
    | /|\ |     | /|\ |     | / \ |     |   \ |
    |/ | \|     |/ | \|     |/   \|     |    \|
    -------     -------     -------     -------
*/
//----------------------------------------------------------------------------------------
// Name: MeshDraw()
// Desc: 
//----------------------------------------------------------------------------------------

static void MeshDraw( Mesh_t *Mesh, Plane2_t *ViewFrustum, float ViewPointX, float ViewPointZ )
{
    long X, Z;
    long TileX, TileZ;
    long QuadTileX, QuadTileZ;
    long DistanceX, DistanceZ;
    long FirstTileX;
    long Index;
    float QuadCoordX, QuadCoordZ;
    float FirstCoordX;

    glEnable( GL_TEXTURE_2D );

    // the world is drawn in patches. each type of patch that can be draw is shown above.
    // the camera may not be over the world and so the TileX and TileZ that the camera is
    // in may be < 0 or > the size of the landscape.

    // get the quad tile coords of where the viewpoint (camera) is
    QuadTileX = (size_t)( ViewPointX / ( Mesh->TileWidth * 2.0f ) );
    QuadTileZ = (size_t)( ViewPointZ / ( Mesh->TileDepth * 2.0f ) );

    // get the coordinates of that tile
    FirstCoordX = QuadTileX * ( Mesh->TileWidth * 2.0f );
    QuadCoordZ  = QuadTileZ * ( Mesh->TileDepth * 2.0f );

    // the viewpoint is in the center of a large rectangle laying flat on the xz plane, so
    // the first patch to draw is half the view distance along the x and z axis. so
    // shift the tile coords we just got on the previous line along the x and z axis.
    FirstCoordX -= ( (float)GetMeshLODDistance() / 2.0f ) * ( Mesh->TileWidth * 2.0f );
    QuadCoordZ  -= ( (float)GetMeshLODDistance() / 2.0f ) * ( Mesh->TileDepth * 2.0f );

    FirstCoordX -= Mesh->XOffset;
    QuadCoordZ  -= Mesh->ZOffset;

    // convert the patch tile coords to actual map tile coords.  there's 4 map tiles to
    // patch tile, like this:
    //      -------
    //      |  |  |
    //      |--|--|
    //      |  |  |
    //      -------
    //
    FirstTileX  = ( QuadTileX * 2 ) - GetMeshLODDistance();
    TileZ       = ( QuadTileZ * 2 ) - GetMeshLODDistance();

    // DistanceX and DistanceZ are in patch tile coords, so just step across the map
    // and draw each patch.  the value of DistanceX and DistanceZ will tell us which
    // type of patch to draw.
    DistanceZ = GetMeshLODDistance() / -2;

    for( Z = 0; Z <= GetMeshLODDistance() && TileZ < long(Mesh->TilesDeep); Z++, TileZ += 2 )
    {
        DistanceX = GetMeshLODDistance() / -2;
        QuadCoordX = FirstCoordX;
        TileX = FirstTileX;

        for( X = 0; X <= GetMeshLODDistance() && TileX < long(Mesh->TilesWide); X++, TileX += 2 )
        {
            if( TileX >= 0 && TileX < long(Mesh->TilesWide) && TileZ >= 0 && TileZ < long(Mesh->TileDepth) )
            {
                Index = TileZ * long( 2 * Mesh->TilesWide ) + ( 2 * TileX );

                glBindTexture( GL_TEXTURE_2D, GetTextureNumber( Mesh->Polygons[ Index ].TextureNumber ) );

                glBegin( GL_TRIANGLES );

                if( abs( DistanceZ ) < GetMeshLODStep0() )
                {
                    if( abs( DistanceX ) < GetMeshLODStep0() )
                        MeshDrawPatch0( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );

                    else if( abs( DistanceX ) < GetMeshLODStep1() )
                        MeshDrawPatch1( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ, DistanceX, DistanceZ );

                    else if( abs( DistanceX ) < GetMeshLODStep2() )
                        MeshDrawPatch2( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );

                    else // GetMeshLODStep3()
                        MeshDrawPatch3( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );
                }
                else if( abs( DistanceZ ) < GetMeshLODStep1() )
                {
                    if( abs( DistanceX ) < GetMeshLODStep1() )
                        MeshDrawPatch1( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ, DistanceX, DistanceZ );

                    else if( abs( DistanceX ) < GetMeshLODStep2() )
                        MeshDrawPatch2( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );

                    else // GetMeshLODStep3()
                        MeshDrawPatch3( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );
                }
                else if( abs( DistanceZ ) < GetMeshLODStep2() )
                {
                    if( abs( DistanceX ) < GetMeshLODStep2() )
                        MeshDrawPatch2( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );

                    else // GetMeshLODStep3()
                        MeshDrawPatch3( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );
                }
                else // GetMeshLODStep3()
                    MeshDrawPatch3( Mesh, ViewFrustum, Index, QuadCoordX, QuadCoordZ );

                glEnd();
            }

            QuadCoordX += ( Mesh->TileWidth * 2.0f );
            DistanceX++;
        }

        QuadCoordZ += ( Mesh->TileDepth * 2.0f );
        DistanceZ++;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetMeshCenter()
// Desc: 
//----------------------------------------------------------------------------------------

void GetMeshCenter( size_t ID, Vector_t *Position )
{
    Mesh_t *Mesh;

    if( INVALIDID( ID ) )
        return;

    Mesh = &_MeshPool[ ID ];

    if( Position )
    {
        Position->X = Mesh->TilesWide * Mesh->TileWidth / 2.0f;
        Position->Z = Mesh->TilesDeep * Mesh->TileDepth / 2.0f;
        Position->Y = GetMeshHeight( ID, Position->X, Position->Z, nullptr ) + ( 10.0f * Mesh->TileWidth );
    }
}


//----------------------------------------------------------------------------------------
// Name: GetMeshCenter()
// Desc: 
//----------------------------------------------------------------------------------------

void GetMeshMaxDimensions( size_t ID, float *MaxX, float *MaxY, float *MaxZ )
{
    Mesh_t *Mesh;

    if( INVALIDID( ID ) )
        return;

    Mesh = &_MeshPool[ ID ];

    if( MaxX )
        (*MaxX) = Mesh->MaxX;

    if( MaxY )
        (*MaxY) = Mesh->LargestHeight + Mesh->TileWidth;

    if( MaxZ )
        (*MaxZ) = Mesh->MaxZ;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshTileSetType()
// Desc: 
//----------------------------------------------------------------------------------------

TileSetType_t GetMeshTileSetType( size_t ID )
{
    if( INVALIDID( ID ) )
        return TILESET_RANDOM;

    return _MeshPool[ ID ].TileSetType;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshSize()
// Desc: 
//----------------------------------------------------------------------------------------

void GetMeshSize( size_t ID, size_t *TilesWide, size_t *TilesDeep )
{
    if( INVALIDID( ID ) )
        return;

    if( TilesWide )
        (*TilesWide) = _MeshPool[ ID ].TilesWide;

    if( TilesDeep )
        (*TilesDeep) = _MeshPool[ ID ].TilesDeep;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshSize()
// Desc: 
//----------------------------------------------------------------------------------------

void GetMeshTileSize( size_t ID, float *TileWidth, float *TileDepth )
{
    if( INVALIDID( ID ) )
        return;

    if( TileWidth )
        (*TileWidth) = _MeshPool[ ID ].TileWidth;

    if( TileDepth )
        (*TileDepth) = _MeshPool[ ID ].TileDepth;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshSize()
// Desc: 
//----------------------------------------------------------------------------------------

void GetPolygonNormal( size_t ID, size_t PolyIndex, Vector_t *Vector )
{
    if( INVALIDID( ID ) )
        return;

    if( PolyIndex < 0 || PolyIndex >= _MeshPool[ ID ].PolygonCount )
        return;

    VectorCopy( _MeshPool[ ID ].Polygons[ PolyIndex ].Normal, (*Vector) );
}


//----------------------------------------------------------------------------------------
// Name: GetMeshLargestHeight()
// Desc: 
//----------------------------------------------------------------------------------------

float GetMeshLargestHeight( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0.0f;

    return _MeshPool[ ID ].LargestHeight;
}


//----------------------------------------------------------------------------------------
// Name: GetMeshStruct()
// Desc: 
//----------------------------------------------------------------------------------------

Mesh_t *GetMeshStruct( size_t ID )
{
    if( INVALIDID( ID ) )
        return nullptr;

    return &_MeshPool[ ID ];
}


//----------------------------------------------------------------------------------------
// Name: GetMeshLoadScreenTexture()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetMeshLoadScreenTexture( size_t ID )
{
    switch( GetMeshTileSetType( ID ) )
    {
        case TILESET_DIRT : return TextureDirt;
        case TILESET_GRASS : return TextureGrass;
        case TILESET_MUD : return TextureMud;
        case TILESET_ROCK : return TextureRock;
        case TILESET_SAND : return TextureSand;
        case TILESET_SNOW : return TextureSnow;
        case TILESET_STONE : return TextureStone;
    }

    return TextureGame;
}
