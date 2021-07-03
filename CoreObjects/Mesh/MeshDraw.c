//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 22 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/GLExt.h>

#include "MeshDraw.h"


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct LODSetting_s
{
    long ViewDistance;
    long Step0, Step1;
    long Step2, Step3;
}
LODSetting_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static LODSetting_t _LODSettings[ 3 ] =
{
    { 100, 10, 14, 18, 22 },
    { 150, 15, 20, 25, 30 },
    { 200, 20, 27, 34, 41 }
};

static size_t _MeshLOD     = 2;
static long _DrawShaded  = 1;


//-----------------------------------------------------------------------------
// Name: SetMeshLOD()
// Desc: 
//-----------------------------------------------------------------------------

void SetMeshLOD(size_t Level )
{
    if( Level < 3 )
        _MeshLOD = Level;
}


//-----------------------------------------------------------------------------
// Name: SetMeshDrawStyle()
// Desc: 
//-----------------------------------------------------------------------------

void SetMeshDrawStyle( long Style )
{
    if( 0 == Style || 1 == Style )
        _DrawShaded = Style;
}


//-----------------------------------------------------------------------------
// Name: GetMeshLODDistance()
// Desc: 
//-----------------------------------------------------------------------------

long GetMeshLODDistance( void )
{
    return _LODSettings[ _MeshLOD ].ViewDistance;
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch0()
// Desc: 
//-----------------------------------------------------------------------------

long GetMeshLODStep0( void )
{
    return _LODSettings[ _MeshLOD ].Step0;
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch0()
// Desc: 
//-----------------------------------------------------------------------------

long GetMeshLODStep1( void )
{
    return _LODSettings[ _MeshLOD ].Step1;
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch0()
// Desc: 
//-----------------------------------------------------------------------------

long GetMeshLODStep2( void )
{
    return _LODSettings[ _MeshLOD ].Step2;
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch0()
// Desc: 
//-----------------------------------------------------------------------------

long GetMeshLODStep3( void )
{
    return _LODSettings[ _MeshLOD ].Step3;
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch0()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch0( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z )
{
    float W, W2;
    float D, D2;
    Polygon_t *P;
    MinMax_t *MM;

    W = Mesh->TileWidth;
    W2 = 2.0f * W;
    D = Mesh->TileDepth;
    D2 = 2.0f * D;

    // top left quad
    P = &Mesh->Polygons[ Index ];
    MM = &Mesh->PolyMinMaxs[ Index ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z, X+W, MM->Max, Z+D ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X,   Mesh->Points[ P->P2 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X,   Mesh->Points[ P->P2 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
        }
    }

    P = &Mesh->Polygons[ Index+1 ];
    MM = &Mesh->PolyMinMaxs[ Index+1 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z, X+W, MM->Max, Z+D ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P2 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P2 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z );
        }
    }

    // top right quad
    P = &Mesh->Polygons[ Index+2 ];
    MM = &Mesh->PolyMinMaxs[ Index+2 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z, X+W2, MM->Max, Z+D ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P2 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P2 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z );
        }
    }

    P = &Mesh->Polygons[ Index+3 ];
    MM = &Mesh->PolyMinMaxs[ Index+3 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z, X+W2, MM->Max, Z+D ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P->P2 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P->P2 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z );
        }
    }

    // bottom left quad
    Index += ( 2 * static_cast<long>(Mesh->TilesWide) );
    P = &Mesh->Polygons[ Index ];
    MM = &Mesh->PolyMinMaxs[ Index ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z+D, X+W, MM->Max, Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,   Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,   Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
        }
    }

    P = &Mesh->Polygons[ Index+1 ];
    MM = &Mesh->PolyMinMaxs[ Index+1 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z+D, X+W, MM->Max, Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W, Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W, Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
        }
    }

    // bottom right quad
    P = &Mesh->Polygons[ Index+2 ];
    MM = &Mesh->PolyMinMaxs[ Index+2 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z+D, X+W2, MM->Max, Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W,  Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z+D2 );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W,  Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z+D2 );
        }
    }

    P = &Mesh->Polygons[ Index+3 ];
    MM = &Mesh->PolyMinMaxs[ Index+3 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z+D, X+W2, MM->Max, Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P->P2 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P->P3 ] ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z+D );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z+D );
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch1()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch1( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z, long DistanceX, long DistanceZ )
{
    float W, W2;
    float D, D2;
    long Index2;
    Polygon_t *P0, *P1, *P2, *P3;
    Polygon_t *P4, *P5, *P6, *P7;

    W = Mesh->TileWidth;
    W2 = 2.0f * W;
    D = Mesh->TileDepth;
    D2 = 2.0f * D;

    P0  = &Mesh->Polygons[ Index ];
    P1  = &Mesh->Polygons[ Index + 1 ];
    P2  = &Mesh->Polygons[ Index + 2 ];
    P3  = &Mesh->Polygons[ Index + 3 ];

    Index2 = Index + ( 2 * static_cast<long>(Mesh->TilesWide) );
    P4  = &Mesh->Polygons[ Index2 ];
    P5  = &Mesh->Polygons[ Index2 + 1 ];
    P6  = &Mesh->Polygons[ Index2 + 2 ];
    P7  = &Mesh->Polygons[ Index2 + 3 ];

    // draw the left side as either 1 or 2 polys
    if( DistanceX < 0.0f && DistanceX - 1 == -_LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X, Min( Min( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P0->P1 ].Y ), Mesh->Points[ P4->P2 ].Y ), Z
                                , X+W, Max( Max( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P0->P1 ].Y ), Mesh->Points[ P4->P2 ].Y ), Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P0->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P0->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
                glColor4fv( (float *)&Mesh->Colors[ P4->P2 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            }
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index ].Min, Z, X+W, Mesh->PolyMinMaxs[ Index ].Max, Z+D ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P0->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
                glColor4fv( (float *)&Mesh->Colors[ P0->P2 ] ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X, Mesh->Points[ P0->P2 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P0->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X, Mesh->Points[ P0->P2 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            }
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index2 ].Min, Z+D, X+W, Mesh->PolyMinMaxs[ Index2 ].Max, Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P4->P1 ] ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X, Mesh->Points[ P4->P1 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P4->P2 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P4->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P4->P3 ].Y, Z+D );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.5f ); glVertex3f( X, Mesh->Points[ P4->P1 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P4->P3 ].Y, Z+D );
            }
        }
    }

    // draw the bottom as either 1 or two polys
    if( DistanceZ > 0.0f && DistanceZ + 1 == _LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X,    Min( Min( Mesh->Points[ P5->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P5->P3 ].Y ), Z+D
                                , X+W2, Max( Max( Mesh->Points[ P5->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P5->P3 ].Y ), Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P5->P1 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,    Mesh->Points[ P5->P1 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P6->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P5->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P5->P3 ].Y, Z+D  );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,    Mesh->Points[ P5->P1 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P5->P3 ].Y, Z+D  );
            }
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index2+1 ].Min, Z+D, X+W, Mesh->PolyMinMaxs[ Index2+1 ].Max, Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P5->P1 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,   Mesh->Points[ P5->P1 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P5->P2 ] ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W, Mesh->Points[ P5->P2 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P5->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P5->P3 ].Y, Z+D  );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X,   Mesh->Points[ P5->P1 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W, Mesh->Points[ P5->P2 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P5->P3 ].Y, Z+D  );
            }
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index2+2 ].Min, Z+D, X+W2, Mesh->PolyMinMaxs[ Index2+2 ].Max, Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P6->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P6->P1 ].Y, Z+D  );
                glColor4fv( (float *)&Mesh->Colors[ P6->P2 ] ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W,  Mesh->Points[ P6->P2 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P6->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P6->P1 ].Y, Z+D  );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 1.0f ); glVertex3f( X+W,  Mesh->Points[ P6->P2 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            }
        }
    }

    // draw the right side as 1 or 2 polys
    if( DistanceX > 0.0f && DistanceX + 1 == _LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X+W,  Min( Min( Mesh->Points[ P7->P1 ].Y, Mesh->Points[ P7->P2 ].Y ), Mesh->Points[ P3->P3 ].Y ), Z
                                , X+W2, Max( Max( Mesh->Points[ P7->P1 ].Y, Mesh->Points[ P7->P2 ].Y ), Mesh->Points[ P3->P3 ].Y ), Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P7->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P7->P1 ].Y, Z+D  );
                glColor4fv( (float *)&Mesh->Colors[ P7->P2 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P7->P2 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P3->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P3->P3 ].Y, Z    );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P7->P1 ].Y, Z+D  );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P7->P2 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P3->P3 ].Y, Z    );
            }
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index2+3 ].Min, Z+D, X+W2, Mesh->PolyMinMaxs[ Index2+3 ].Max, Z+D2 ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P7->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P7->P1 ].Y, Z+D  );
                glColor4fv( (float *)&Mesh->Colors[ P7->P2 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P7->P2 ].Y, Z+D2 );
                glColor4fv( (float *)&Mesh->Colors[ P7->P3 ] ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P7->P3 ].Y, Z+D  );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P7->P1 ].Y, Z+D  );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P7->P2 ].Y, Z+D2 );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P7->P3 ].Y, Z+D  );
            }
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index+3 ].Min, Z, X+W2, Mesh->PolyMinMaxs[ Index+3 ].Max, Z+D ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P3->P1 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P3->P1 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P3->P2 ] ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P3->P2 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P3->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P3->P3 ].Y, Z );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P3->P1 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.5f ); glVertex3f( X+W2, Mesh->Points[ P3->P2 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P3->P3 ].Y, Z );
            }
        }
    }

    // draw the top as 1 or 2 polys
    if( DistanceZ < 0.0f && DistanceZ - 1 == -_LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X,    Min( Min( Mesh->Points[ P1->P1 ].Y, Mesh->Points[ P1->P2 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                                , X+W2, Max( Max( Mesh->Points[ P1->P1 ].Y, Mesh->Points[ P1->P2 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P1->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,    Mesh->Points[ P1->P1 ].Y, Z   );
                glColor4fv( (float *)&Mesh->Colors[ P1->P2 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P1->P2 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P2->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z   );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,    Mesh->Points[ P1->P1 ].Y, Z   );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P1->P2 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z   );
            }
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index+1 ].Min, Z, X+W, Mesh->PolyMinMaxs[ Index+1 ].Max, Z+D ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P1->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,   Mesh->Points[ P1->P1 ].Y, Z   );
                glColor4fv( (float *)&Mesh->Colors[ P1->P2 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P1->P2 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P1->P3 ] ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W, Mesh->Points[ P1->P3 ].Y, Z   );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X,   Mesh->Points[ P1->P1 ].Y, Z   );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P1->P2 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W, Mesh->Points[ P1->P3 ].Y, Z   );
            }
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index+2 ].Min, Z, X+W2, Mesh->PolyMinMaxs[ Index+2 ].Max, Z+D ) )
        {
            if( _DrawShaded )
            {
                glColor4fv( (float *)&Mesh->Colors[ P2->P1 ] ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W,  Mesh->Points[ P2->P1 ].Y, Z   );
                glColor4fv( (float *)&Mesh->Colors[ P2->P2 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P2->P2 ].Y, Z+D );
                glColor4fv( (float *)&Mesh->Colors[ P2->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z   );
            }
            else
            {
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.0f ); glVertex3f( X+W,  Mesh->Points[ P2->P1 ].Y, Z   );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W,  Mesh->Points[ P2->P2 ].Y, Z+D );
                glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z   );
            }
        }
    }

}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch2()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch2( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z )
{
    float W, W2;
    float D, D2;
    Polygon_t *P0, *P2, *P4, *P6;

    W   = Mesh->TileWidth;
    W2  = 2.0f * W;
    D   = Mesh->TileDepth;
    D2  = 2.0f * D;

    P0  = &Mesh->Polygons[ Index ];
    P2  = &Mesh->Polygons[ Index+2 ];
    P4  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide ) ];
    P6  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide )+2 ];

    // left
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P0->P3 ].Y ), Z
                            , X+W, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P0->P3 ].Y ), Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P0->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P4->P2 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P0->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
        }
    }

    // right
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X+W, Min( Min( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P0->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P6->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P2->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
        }
    }

    // top
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P0->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P0->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P0->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P0->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P2->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
        }
    }

    // bottom
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z+D
                            , X+W2, Max( Max( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P0->P3 ] ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glColor4fv( (float *)&Mesh->Colors[ P4->P2 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P6->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.5f, 0.5f ); glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch3()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch3( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z )
{
    float W, W2;
    float D, D2;
    Polygon_t *P0, *P2, *P4, *P6;

    W   = Mesh->TileWidth;
    W2  = 2.0f * W;
    D   = Mesh->TileDepth;
    D2  = 2.0f * D;

    P0  = &Mesh->Polygons[ Index ];
    P2  = &Mesh->Polygons[ Index+2 ];
    P4  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide ) ];
    P6  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide )+2 ];

    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P0->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P4->P2 ] ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P6->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 1.0f ); glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        }
    }

    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D2 ) )
    {
        if( _DrawShaded )
        {
            glColor4fv( (float *)&Mesh->Colors[ P0->P1 ] ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor4fv( (float *)&Mesh->Colors[ P6->P3 ] ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            glColor4fv( (float *)&Mesh->Colors[ P2->P3 ] ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
        }
        else
        {
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 0.0f, 0.0f ); glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 1.0f ); glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            glColor3f( 1.0f, 1.0f, 1.0f ); glTexCoord2f( 1.0f, 0.0f ); glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
        }
    }
}

/*
//-----------------------------------------------------------------------------
// Name: MeshDrawPatch0MT()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch0MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z )
{
    float W, W2;
    float D, D2;
    Polygon_t *P;
    MinMax_t *MM;

    W = Mesh->TileWidth;
    W2 = 2.0f * W;
    D = Mesh->TileDepth;
    D2 = 2.0f * D;

    // top left quad
    P = &Mesh->Polygons[ Index ];
    MM = &Mesh->PolyMinMaxs[ Index ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z, X+W, MM->Max, Z+D ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X,   Mesh->Points[ P->P2 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
    }

    P = &Mesh->Polygons[ Index+1 ];
    MM = &Mesh->PolyMinMaxs[ Index+1 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z, X+W, MM->Max, Z+D ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P->P2 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z );
    }

    // top right quad
    P = &Mesh->Polygons[ Index+2 ];
    MM = &Mesh->PolyMinMaxs[ Index+2 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z, X+W2, MM->Max, Z+D ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W,  Mesh->Points[ P->P2 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z );
    }

    P = &Mesh->Polygons[ Index+3 ];
    MM = &Mesh->PolyMinMaxs[ Index+3 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z, X+W2, MM->Max, Z+D ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P->P2 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z );
    }

    // bottom left quad
    Index += ( 2 * Mesh->TilesWide );
    P = &Mesh->Polygons[ Index ];
    MM = &Mesh->PolyMinMaxs[ Index ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z+D, X+W, MM->Max, Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X,   Mesh->Points[ P->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
    }

    P = &Mesh->Polygons[ Index+1 ];
    MM = &Mesh->PolyMinMaxs[ Index+1 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X, MM->Min, Z+D, X+W, MM->Max, Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X,   Mesh->Points[ P->P1 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P->P3 ].Y, Z+D );
    }

    // bottom right quad
    P = &Mesh->Polygons[ Index+2 ];
    MM = &Mesh->PolyMinMaxs[ Index+2 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z+D, X+W2, MM->Max, Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W,  Mesh->Points[ P->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z+D2 );
    }

    P = &Mesh->Polygons[ Index+3 ];
    MM = &Mesh->PolyMinMaxs[ Index+3 ];
    if( VectorIsAABBInFrustum( ViewFrustum, X+W, MM->Min, Z+D, X+W2, MM->Max, Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W,  Mesh->Points[ P->P1 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P->P3 ].Y, Z+D );
    }
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch1MT()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch1MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z, long DistanceX, long DistanceZ )
{
    float W, W2;
    float D, D2;
    long Index2;
    Polygon_t *P0, *P1, *P2, *P3;
    Polygon_t *P4, *P5, *P6, *P7;

    W = Mesh->TileWidth;
    W2 = 2.0f * W;
    D = Mesh->TileDepth;
    D2 = 2.0f * D;

    P0  = &Mesh->Polygons[ Index ];
    P1  = &Mesh->Polygons[ Index + 1 ];
    P2  = &Mesh->Polygons[ Index + 2 ];
    P3  = &Mesh->Polygons[ Index + 3 ];

    Index2 = Index + ( 2 * Mesh->TilesWide );
    P4  = &Mesh->Polygons[ Index2 ];
    P5  = &Mesh->Polygons[ Index2 + 1 ];
    P6  = &Mesh->Polygons[ Index2 + 2 ];
    P7  = &Mesh->Polygons[ Index2 + 3 ];

    // draw the left side as either 1 or 2 polys
    if( DistanceX < 0.0f && DistanceX - 1 == -_LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X, Min( Min( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P0->P1 ].Y ), Mesh->Points[ P4->P2 ].Y ), Z
                                , X+W, Max( Max( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P0->P1 ].Y ), Mesh->Points[ P4->P2 ].Y ), Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index ].Min, Z, X+W, Mesh->PolyMinMaxs[ Index ].Max, Z+D ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X, Mesh->Points[ P0->P2 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index2 ].Min, Z+D, X+W, Mesh->PolyMinMaxs[ Index2 ].Max, Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P4->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P4->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P4->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P4->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X, Mesh->Points[ P4->P1 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P4->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P4->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P4->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P4->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P4->P3 ].Y, Z+D );
        }
    }

    // draw the bottom as either 1 or two polys
    if( DistanceZ > 0.0f && DistanceZ + 1 == _LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X,    Min( Min( Mesh->Points[ P5->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P5->P3 ].Y ), Z+D
                                , X+W2, Max( Max( Mesh->Points[ P5->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P5->P3 ].Y ), Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P5->P1%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P5->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P5->P1%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P5->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X,    Mesh->Points[ P5->P1 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P5->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P5->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P5->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P5->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P5->P3 ].Y, Z+D  );
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index2+1 ].Min, Z+D, X+W, Mesh->PolyMinMaxs[ Index2+1 ].Max, Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P5->P1%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P5->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P5->P1%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P5->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X,   Mesh->Points[ P5->P1 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P5->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P5->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P5->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P5->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P5->P2 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P5->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P5->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P5->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P5->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P5->P3 ].Y, Z+D  );
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index2+2 ].Min, Z+D, X+W2, Mesh->PolyMinMaxs[ Index2+2 ].Max, Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P6->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P6->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P6->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P6->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P6->P1 ].Y, Z+D  );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P6->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P6->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P6->P2 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        }
    }

    // draw the right side as 1 or 2 polys
    if( DistanceX > 0.0f && DistanceX + 1 == _LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X+W,  Min( Min( Mesh->Points[ P7->P1 ].Y, Mesh->Points[ P7->P2 ].Y ), Mesh->Points[ P3->P3 ].Y ), Z
                                , X+W2, Max( Max( Mesh->Points[ P7->P1 ].Y, Mesh->Points[ P7->P2 ].Y ), Mesh->Points[ P3->P3 ].Y ), Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P7->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P7->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P7->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P7->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P7->P1 ].Y, Z+D  );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P7->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P7->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P7->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P7->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P7->P2 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P3->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P3->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P3->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P3->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P3->P3 ].Y, Z    );
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index2+3 ].Min, Z+D, X+W2, Mesh->PolyMinMaxs[ Index2+3 ].Max, Z+D2 ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P7->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P7->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P7->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P7->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P7->P1 ].Y, Z+D  );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P7->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P7->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P7->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P7->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P7->P2 ].Y, Z+D2 );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P7->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P7->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P7->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P7->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P7->P3 ].Y, Z+D  );
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index+3 ].Min, Z, X+W2, Mesh->PolyMinMaxs[ Index+3 ].Max, Z+D ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P3->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P3->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P3->P1%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P3->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P3->P1 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P3->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P3->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P3->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P3->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P3->P2 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P3->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P3->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P3->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P3->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P3->P3 ].Y, Z );
        }
    }

    // draw the top as 1 or 2 polys
    if( DistanceZ < 0.0f && DistanceZ - 1 == -_LODSettings[ _MeshLOD ].Step1 )
    {
        if( VectorIsAABBInFrustum( ViewFrustum
                                , X,    Min( Min( Mesh->Points[ P1->P1 ].Y, Mesh->Points[ P1->P2 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                                , X+W2, Max( Max( Mesh->Points[ P1->P1 ].Y, Mesh->Points[ P1->P2 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P1->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P1->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P1->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P1->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X,    Mesh->Points[ P1->P1 ].Y, Z   );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P1->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P1->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P1->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P1->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P1->P2 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z   );
        }
    }
    else
    {
        if( VectorIsAABBInFrustum( ViewFrustum, X, Mesh->PolyMinMaxs[ Index+1 ].Min, Z, X+W, Mesh->PolyMinMaxs[ Index+1 ].Max, Z+D ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P1->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P1->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P1->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P1->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X,   Mesh->Points[ P1->P1 ].Y, Z   );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P1->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P1->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P1->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P1->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P1->P2 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P1->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P1->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P1->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P1->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W, Mesh->Points[ P1->P3 ].Y, Z   );
        }

        if( VectorIsAABBInFrustum( ViewFrustum, X+W, Mesh->PolyMinMaxs[ Index+2 ].Min, Z, X+W2, Mesh->PolyMinMaxs[ Index+2 ].Max, Z+D ) )
        {
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P2->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P2->P1%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P2->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P2->P1%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P2->P1 ].Y, Z   );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P2->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P2->P2%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P2->P2%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P2->P2%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W,  Mesh->Points[ P2->P2 ].Y, Z+D );
            glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
            glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
            glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z   );
        }
    }

}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch2MT()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch2MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z )
{
    float W, W2;
    float D, D2;
    Polygon_t *P0, *P2, *P4, *P6;

    W   = Mesh->TileWidth;
    W2  = 2.0f * W;
    D   = Mesh->TileDepth;
    D2  = 2.0f * D;

    P0  = &Mesh->Polygons[ Index ];
    P2  = &Mesh->Polygons[ Index+2 ];
    P4  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide ) ];
    P6  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide )+2 ];

    // left
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P0->P3 ].Y ), Z
                            , X+W, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P0->P3 ].Y ), Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
    }

    // right
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X+W, Min( Min( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
    }

    // top
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P0->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P0->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
    }

    // bottom
    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z+D
                            , X+W2, Max( Max( Mesh->Points[ P0->P3 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.5f+TexAnimInfo.SinOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets1[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.5f+TexAnimInfo.SinOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ], 0.5f+TexAnimInfo.CosOffsets2[ P0->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W, Mesh->Points[ P0->P3 ].Y, Z+D );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
    }
}


//-----------------------------------------------------------------------------
// Name: MeshDrawPatch3MT()
// Desc: 
//-----------------------------------------------------------------------------

void MeshDrawPatch3MT( Mesh_t *Mesh, Plane2_t *ViewFrustum, long Index, float X, float Z )
{
    float W, W2;
    float D, D2;
    Polygon_t *P0, *P2, *P4, *P6;

    W   = Mesh->TileWidth;
    W2  = 2.0f * W;
    D   = Mesh->TileDepth;
    D2  = 2.0f * D;

    P0  = &Mesh->Polygons[ Index ];
    P2  = &Mesh->Polygons[ Index+2 ];
    P4  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide ) ];
    P6  = &Mesh->Polygons[ Index+( 2 * Mesh->TilesWide )+2 ];

    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P4->P2 ].Y ), Mesh->Points[ P6->P3 ].Y ), Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P4->P2%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P4->P2 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
    }

    if( VectorIsAABBInFrustum( ViewFrustum
                            , X, Min( Min( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z
                            , X+W2, Max( Max( Mesh->Points[ P0->P1 ].Y, Mesh->Points[ P6->P3 ].Y ), Mesh->Points[ P2->P3 ].Y ), Z+D2 ) )
    {
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f+TexAnimInfo.SinOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f+TexAnimInfo.SinOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P0->P1%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X, Mesh->Points[ P0->P1 ].Y, Z );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets1[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ], 1.0f+TexAnimInfo.CosOffsets2[ P6->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P6->P3 ].Y, Z+D2 );
        glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f+TexAnimInfo.SinOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets1[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
        glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f+TexAnimInfo.SinOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ], 0.0f+TexAnimInfo.CosOffsets2[ P2->P3%MESH_TEXANIMNUMOFFSETS ] );
        glVertex3f( X+W2, Mesh->Points[ P2->P3 ].Y, Z );
    }
}
*/