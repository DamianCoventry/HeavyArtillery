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
#include <GL/GL.h>
//#include <GL/GLUT.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "DecalManager.h"
#include "MeshManager.h"
#include "TextureManager.h"

#define DECAL_MAXDECALS             128
#define DECAL_EXPIRETHRESHOLD       120
#define DECAL_EXPIRESPEED           0.001f

#define BBOX_FRONT                  0
#define BBOX_BACK                   1
#define BBOX_RIGHT                  2
#define BBOX_LEFT                   3
#define BBOX_TOP                    4
#define BBOX_BOTTOM                 5


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

typedef struct DecalBuildInfo_s
{
    Polygon_t *Polygon;
    size_t PolygonIndex;

    Vector_t ImpactPoint;

    Vector_t Forward;
    Vector_t Right;

    Plane2_t BBoxPlanes[ 6 ];
    Vector_t PointOnPlane[ 6 ];
}
DecalBuildInfo_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t DecalCount;
static Decal_t Decals[ DECAL_MAXDECALS ];


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t DecalMgrScanPolygons( Mesh_t *Mesh, DecalTexture_t *DecalTexture, float X, float Z );
static void DecalMgrBuildFragments( Mesh_t *Mesh, DecalBuildInfo_t *DecalBuildInfo, size_t PolygonIndex );
static void DecalMgrBuildBBox( Mesh_t *Mesh, DecalTexture_t *DecalTexture, float X, float Z, DecalBuildInfo_t *DecalBuildInfo );
static void DecalMgrBuildTexCoordsAA( DecalFragment_t *Fragment, Vector_t *Center, DecalTexture_t *DecalTexture );
static void DecalMgrBuildTexCoordsAP( DecalFragment_t *Fragment, DecalBuildInfo_t *DecalBuildInfo, DecalTexture_t *DecalTexture );
static void DecalMgrCleanTexCoords( Decal_t *Decal );

static void DecalMgrGetRightAndForward( size_t Up, DecalBuildInfo_t *DecalBuildInfo, Plane_t *Right, Plane_t *Forward );

static size_t DecalMgrSplitFragmentList( Decal_t *Decal, Vector_t *PlaneNormal, Vector_t *PointOnPlane, DecalFragment_t **InputList, DecalFragment_t **FrontList, DecalFragment_t **BackList );
static size_t DecalMgrSplitFragment( Vector_t *PlaneNormal, Vector_t *PointOnPlane, DecalFragment_t *Fragment, DecalFragment_t **FrontList, DecalFragment_t **BackList );
static void DecalMgrRemoveDecal( size_t Index );


//-----------------------------------------------------------------------------
// Name: DecalMgrCreate()
// Desc: 
//-----------------------------------------------------------------------------

size_t DecalMgrCreate( Mesh_t *Mesh, DecalTexture_t *DecalTexture, float X, float Z )
{
    if( DecalCount >= DECAL_MAXDECALS || nullptr == DecalTexture )
        return 0;

    memset( &Decals[ DecalCount ], 0, sizeof( Decal_t ) );

    Decals[ DecalCount ].ExpireAlpha    = 1.0f;
    Decals[ DecalCount ].Texture        = DecalTexture;
    Decals[ DecalCount ].Object         = (void *)Mesh;

    if( DecalMgrScanPolygons( Mesh, DecalTexture, X, Z ) )
        DecalCount++;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DecalMgrDelete()
// Desc: 
//-----------------------------------------------------------------------------

void DecalMgrDelete( Mesh_t *Mesh, size_t PolyIndex )
{
    size_t DecalIndex;
    size_t FragmentIndex;
    size_t Found;

    // find the decals that have one of their fragments in this polygon
    for( DecalIndex = 0; DecalIndex < DecalCount; DecalIndex++ )
    {
        if( Mesh == (Mesh_t *)Decals[ DecalIndex ].Object )
        {
            Found = 0;
            for( FragmentIndex = 0; !Found && FragmentIndex < Decals[ DecalIndex ].FragmentCount; FragmentIndex++ )
            {
                if( Decals[ DecalIndex ].Fragments[ FragmentIndex ].ObjectPolyIndex == PolyIndex )
                {
                    DecalMgrRemoveDecal( DecalIndex );
                    Found = 1;
                    DecalIndex--;       // re-test this index in the array
                }
            }
        }
    }

}


//-----------------------------------------------------------------------------
// Name: DecalMgrExpire()
// Desc: 
//-----------------------------------------------------------------------------

void DecalMgrExpire( void )
{
    size_t DecalIndex;

    // increment the time counter and check to see if it's time to
    // fade each decal off the landscape

    for( DecalIndex = 0; DecalIndex < DecalCount; DecalIndex++ )
    {
        if( Decals[ DecalIndex ].ExpireAlpha < 1.0f )
        {
            Decals[ DecalIndex ].ExpireAlpha -= DECAL_EXPIRESPEED;
            if( Decals[ DecalIndex ].ExpireAlpha <= 0.0f )
            {
                DecalMgrRemoveDecal( DecalIndex );
                DecalIndex--;       // re-test this index in the array
            }
        }
        else
        {
            if( ++Decals[ DecalIndex ].ExpireCounter > DECAL_EXPIRETHRESHOLD )
                Decals[ DecalIndex ].ExpireAlpha -= DECAL_EXPIRESPEED;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: DecalMgrDraw()
// Desc: 
//-----------------------------------------------------------------------------

void DecalMgrDraw( Plane2_t *ViewFrustum )
{
    size_t DecalIndex;
    size_t FragmentIndex;
    float ModelViewMatrix[ 16 ];
    float Distance;
    DecalFragment_t *Fragment;

    glGetFloatv( GL_MODELVIEW_MATRIX, ModelViewMatrix );

    // draw each decal to the screen, make sure to prevent z-fighting

    glDepthMask( GL_FALSE );
    glEnable( GL_BLEND );
    glEnable( GL_ALPHA_TEST );
    glEnable( GL_POLYGON_OFFSET_FILL );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    for( DecalIndex = 0; DecalIndex < DecalCount; DecalIndex++ )
    {
        // call WorldIsAABBInFrustum() here

        glBindTexture( GL_TEXTURE_2D, GetTextureNumber( Decals[ DecalIndex ].Texture->TextureNumber ) );
        // glColor4f( 1.0f, 1.0f, 1.0f, Decals[ DecalIndex ].ExpireAlpha );

        for( FragmentIndex = 0; FragmentIndex < Decals[ DecalIndex ].FragmentCount; FragmentIndex++ )
        {
            Fragment = &Decals[ DecalIndex ].Fragments[ FragmentIndex ];

            Distance = (float)fabs( ( ModelViewMatrix[  2 ] * Fragment->P1.X ) +
                                    ( ModelViewMatrix[  6 ] * Fragment->P1.Y ) +
                                    ( ModelViewMatrix[ 10 ] * Fragment->P1.Z ) +
                                    ( ModelViewMatrix[ 14 ] ) );

            Distance /= 32.0f;

            glPolygonOffset( -1.0f - Distance * 0.5f, 1.0f );

            glColor4f( 1.0f, 1.0f, 1.0f, Decals[ DecalIndex ].ExpireAlpha );
            glBegin( GL_TRIANGLES );
                glTexCoord2fv( (float *)&Fragment->T1 );
                glVertex3fv( (float *)&Fragment->P1 );

                glTexCoord2fv( (float *)&Fragment->T2 );
                glVertex3fv( (float *)&Fragment->P2 );

                glTexCoord2fv( (float *)&Fragment->T3 );
                glVertex3fv( (float *)&Fragment->P3 );
            glEnd();
/*
            glDisable( GL_TEXTURE_2D );
            glDisable( GL_DEPTH_TEST );
            glColor3f( 1.0f, 0.0f, 0.0f );
            glBegin( GL_LINE_LOOP );
                glVertex3fv( (float *)&Fragment->P1 ); glVertex3fv( (float *)&Fragment->P2 );glVertex3fv( (float *)&Fragment->P3 );
            glEnd();
            glEnable( GL_DEPTH_TEST );
            glEnable( GL_TEXTURE_2D );
*/
        }
    }

    glDisable( GL_POLYGON_OFFSET_FILL );
    glDisable( GL_ALPHA_TEST );
    glDisable( GL_BLEND );
    glDepthMask( GL_TRUE );
}


//-----------------------------------------------------------------------------
// Name: DecalMgrScanPolygons()
// Desc: 
//-----------------------------------------------------------------------------

static size_t DecalMgrScanPolygons( Mesh_t *Mesh, DecalTexture_t *DecalTexture, float X, float Z )
{
    size_t TileX, TileZ, FirstX;
    size_t XIdx, ZIdx;
    float XCoord, ZCoord;
    size_t CountX, CountZ;
    DecalBuildInfo_t DecalBuildInfo;
    MinMax_t *MinMax;
    size_t PolygonIndex;

    if( X < 0.0f || X >= Mesh->MaxX || Z < 0.0f || Z >= Mesh->MaxZ )
        return 0;

    CountX = (size_t)( DecalTexture->Width  / Mesh->TileWidth );
    CountX = ( ( CountX + 1 ) * 2 ) + 1;
    CountZ = (size_t)( DecalTexture->Height / Mesh->TileDepth );
    CountZ = ( ( CountZ + 1 ) * 2 ) + 1;

    FirstX = ( (size_t)X / (size_t)Mesh->TileWidth ) - ( CountX / 2 );
    TileZ  = ( (size_t)Z / (size_t)Mesh->TileDepth ) - ( CountZ / 2 );

    DecalMgrBuildBBox( Mesh, DecalTexture, X, Z, &DecalBuildInfo );

    ZCoord = TileZ * Mesh->TileDepth;

    // loop thru all of the possibly affected polygons
    for( ZIdx = 0; ZIdx < CountZ; ZIdx++ )
    {
        TileX = FirstX;
        XCoord = TileX * Mesh->TileWidth;

        if( TileZ >= 0 && TileZ < Mesh->TilesDeep )
        {
            for( XIdx = 0; XIdx < CountX; XIdx++ )
            {
                if( TileX >= 0 && TileZ < Mesh->TilesWide )
                {
                    PolygonIndex = ( TileZ * ( 2 * Mesh->TilesWide ) ) + ( 2 * TileX );
                    MinMax = &Mesh->PolyMinMaxs[ PolygonIndex ];

                    // filter out polygons that don't intersect the bounding box
                    if( VectorIsAABBInFrustum( DecalBuildInfo.BBoxPlanes, XCoord
                                             , MinMax->Min, ZCoord, XCoord+Mesh->TileWidth
                                             , MinMax->Max, ZCoord+Mesh->TileDepth ) )
                    {
                        DecalMgrBuildFragments( Mesh, &DecalBuildInfo, PolygonIndex );
                    }

                    MinMax = &Mesh->PolyMinMaxs[ ++PolygonIndex ];

                    // filter out polygons that don't interesect the bounding box
                    if( VectorIsAABBInFrustum( DecalBuildInfo.BBoxPlanes, XCoord
                                             , MinMax->Min, ZCoord, XCoord+Mesh->TileWidth
                                             , MinMax->Max, ZCoord+Mesh->TileDepth ) )
                    {
                        DecalMgrBuildFragments( Mesh, &DecalBuildInfo, PolygonIndex );
                    }
                }

                TileX++;
                if( TileX > Mesh->TilesWide )
                    break;

                XCoord += Mesh->TileWidth;
            }
        }

        TileZ++;
        if( TileZ > Mesh->TilesDeep )
            break;

        ZCoord += Mesh->TileDepth;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DecalMgrBuildFragments()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrBuildFragments( Mesh_t *Mesh, DecalBuildInfo_t *DecalBuildInfo, size_t PolygonIndex )
{
    DecalFragment_t *Current;
    DecalFragment_t *Previous;
    DecalFragment_t *InputList;
    Polygon_t *Polygon;
    DecalFragment_t *FrontList, *BackList;
    size_t i;

    Polygon = &Mesh->Polygons[ PolygonIndex ];

    FrontList = nullptr;
    BackList = nullptr;

    // create a fragment out of the entire polygon
    InputList = new DecalFragment_t();

    VectorCopy( Mesh->Points[ Polygon->P1 ], InputList->P1 );
    VectorCopy( Mesh->Points[ Polygon->P2 ], InputList->P2 );
    VectorCopy( Mesh->Points[ Polygon->P3 ], InputList->P3 );
    InputList->ObjectPolyIndex = PolygonIndex;

    // split this polygon against each plane
    for( i = 0; i < 6; i++ )
    {
        DecalMgrSplitFragmentList( &Decals[ DecalCount ],
                                   &DecalBuildInfo->BBoxPlanes[ i ].Normal, &DecalBuildInfo->PointOnPlane[ i ],
                                   &InputList, &FrontList, &BackList );

        // at this point the InputList is empty.

        // throw away the frontlist, we don't need it
        Current = FrontList;
        while( Current )
        {
            Previous = Current;
            Current = Current->Next;
            delete Previous;
        }

        InputList = BackList;

        if( nullptr == BackList )
            break;                  // the polygon must be behind all planes to be part of a decal

        FrontList = nullptr;
        BackList = nullptr;
    }

    // each fragment in the back list needs to go into this decal's array and also needs
    // texture coords calculated.
    Current = InputList;
    while( Current && Decals[ DecalCount ].FragmentCount < DECAL_MAXFRAGMENTS )
    {
        memcpy( &Decals[ DecalCount ].Fragments[ Decals[ DecalCount ].FragmentCount ], Current, sizeof( DecalFragment_t ) );

        //DecalMgrBuildTexCoordsAA( &Decals[ DecalCount ].Fragments[ Decals[ DecalCount ].FragmentCount ]
        //                        , &DecalBuildInfo->ImpactPoint
        //                        , Decals[ DecalCount ].Texture );

        DecalMgrBuildTexCoordsAP( &Decals[ DecalCount ].Fragments[ Decals[ DecalCount ].FragmentCount ]
                                , DecalBuildInfo, Decals[ DecalCount ].Texture );

        Previous = Current;
        Current = Current->Next;

        Decals[ DecalCount ].FragmentCount++;

        delete Previous;
    }

    // clean up the coords
    DecalMgrCleanTexCoords( &Decals[ DecalCount ] );
}


//-----------------------------------------------------------------------------
// Name: DecalMgrRemoveDecal()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrRemoveDecal( size_t Index )
{
    if( Index < 0 || Index >= DecalCount )
        return;

    if( DecalCount == 1 )
    {
        memset( &Decals[ 0 ], 0, sizeof( Decal_t ) );
    }
    else if( Index == DecalCount - 1 )
    {
        memset( &Decals[ DecalCount - 1 ], 0, sizeof( Decal_t ) );
    }
    else
    {
        // overwrite this decal with the last one in the list
        memcpy( &Decals[ Index ], &Decals[ DecalCount - 1 ], sizeof( Decal_t ) );
        memset( &Decals[ DecalCount - 1 ], 0, sizeof( Decal_t ) );
    }

    DecalCount--;
}


//-----------------------------------------------------------------------------
// Name: DecalMgrBuildBBox()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrBuildBBox( Mesh_t *Mesh, DecalTexture_t *DecalTexture, float X, float Z, DecalBuildInfo_t *DecalBuildInfo )
{
    Vector_t XOffset, YOffset, ZOffset;

    // get a pointer to the polygon to apply the decal too
    DecalBuildInfo->ImpactPoint.X = X;
    DecalBuildInfo->ImpactPoint.Z = Z;
    DecalBuildInfo->ImpactPoint.Y = GetMeshHeight( Mesh->ID, X, Z, &DecalBuildInfo->PolygonIndex );
    DecalBuildInfo->Polygon = &Mesh->Polygons[ DecalBuildInfo->PolygonIndex ];

    // get forward and right vectors that lie on the poly's plane 
    DecalBuildInfo->Forward.X = 0.0f;
    DecalBuildInfo->Forward.Y = 1.0f;
    DecalBuildInfo->Forward.Z = 0.0f;

    if( VectorFuzzyCompare( DecalBuildInfo->Forward, DecalBuildInfo->Polygon->Normal, 0.0001f ) )
    {
        DecalBuildInfo->Forward.X = 0.0f; DecalBuildInfo->Forward.Y = 0.0f; DecalBuildInfo->Forward.Z = -1.0f;
        DecalBuildInfo->Right.X   = 1.0f; DecalBuildInfo->Right.Y   = 0.0f; DecalBuildInfo->Right.Z   = 0.0f;
    }
    else
    {
        VectorCrossProd( DecalBuildInfo->Forward, DecalBuildInfo->Polygon->Normal, DecalBuildInfo->Right );
        VectorNormalise( &DecalBuildInfo->Right );
        VectorCrossProd( DecalBuildInfo->Polygon->Normal, DecalBuildInfo->Right, DecalBuildInfo->Forward );
        VectorNormalise( &DecalBuildInfo->Forward );
    }

    VectorScale( DecalBuildInfo->Right, DecalTexture->HalfWidth, XOffset );
    VectorScale( DecalBuildInfo->Polygon->Normal, DecalTexture->HalfWidth, YOffset );
    VectorScale( DecalBuildInfo->Forward, DecalTexture->HalfHeight, ZOffset );

    // build the bbox planes
    VectorCopy( DecalBuildInfo->Forward, DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal );                          // front
    VectorAdd( DecalBuildInfo->ImpactPoint, ZOffset, DecalBuildInfo->PointOnPlane[ BBOX_FRONT ] );
    DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Distance = -( ( DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal.X * DecalBuildInfo->PointOnPlane[ BBOX_FRONT ].X ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal.Y * DecalBuildInfo->PointOnPlane[ BBOX_FRONT ].Y ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal.Z * DecalBuildInfo->PointOnPlane[ BBOX_FRONT ].Z ) );

    VectorCopy( DecalBuildInfo->Forward, DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Normal );                          // back
    VectorInverse( DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Normal );
    VectorSubtract( DecalBuildInfo->ImpactPoint, ZOffset, DecalBuildInfo->PointOnPlane[ BBOX_BACK ] );
    DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Distance = -( ( DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Normal.X * DecalBuildInfo->PointOnPlane[ BBOX_BACK ].X ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Normal.Y * DecalBuildInfo->PointOnPlane[ BBOX_BACK ].Y ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Normal.Z * DecalBuildInfo->PointOnPlane[ BBOX_BACK ].Z ) );

    VectorCopy( DecalBuildInfo->Right, DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal );                            // right
    VectorAdd( DecalBuildInfo->ImpactPoint, XOffset, DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ] );
    DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Distance = -( ( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal.X * DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ].X ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal.Y * DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ].Y ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal.Z * DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ].Z ) );

    VectorCopy( DecalBuildInfo->Right, DecalBuildInfo->BBoxPlanes[ BBOX_LEFT ].Normal );                            // left
    VectorInverse( DecalBuildInfo->BBoxPlanes[ BBOX_LEFT ].Normal );
    VectorSubtract( DecalBuildInfo->ImpactPoint, XOffset, DecalBuildInfo->PointOnPlane[ BBOX_LEFT ] );
    DecalBuildInfo->BBoxPlanes[ BBOX_LEFT ].Distance = -( ( DecalBuildInfo->BBoxPlanes[ BBOX_LEFT ].Normal.X * DecalBuildInfo->PointOnPlane[ BBOX_LEFT ].X ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_LEFT ].Normal.Y * DecalBuildInfo->PointOnPlane[ BBOX_LEFT ].Y ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_LEFT ].Normal.Z * DecalBuildInfo->PointOnPlane[ BBOX_LEFT ].Z ) );

    VectorCopy( DecalBuildInfo->Polygon->Normal, DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Normal );                  // top
    VectorAdd( DecalBuildInfo->ImpactPoint, YOffset, DecalBuildInfo->PointOnPlane[ BBOX_TOP ] );
    DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Distance = -( ( DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Normal.X * DecalBuildInfo->PointOnPlane[ BBOX_TOP ].X ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Normal.Y * DecalBuildInfo->PointOnPlane[ BBOX_TOP ].Y ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Normal.Z * DecalBuildInfo->PointOnPlane[ BBOX_TOP ].Z ) );

    VectorCopy( DecalBuildInfo->Polygon->Normal, DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal );                  // bottom
    VectorInverse( DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal );
    VectorSubtract( DecalBuildInfo->ImpactPoint, YOffset, DecalBuildInfo->PointOnPlane[ BBOX_BOTTOM ] );
    DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Distance = -( ( DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal.X * DecalBuildInfo->PointOnPlane[ BBOX_BOTTOM ].X ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal.Y * DecalBuildInfo->PointOnPlane[ BBOX_BOTTOM ].Y ) +
                                                  ( DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal.Z * DecalBuildInfo->PointOnPlane[ BBOX_BOTTOM ].Z ) );
}


//-----------------------------------------------------------------------------
// Name: DecalMgrSplitFragmentList()
// Desc: 
//-----------------------------------------------------------------------------

static size_t DecalMgrSplitFragmentList( Decal_t *Decal, Vector_t *PlaneNormal, Vector_t *PointOnPlane,
                                      DecalFragment_t **InputList, DecalFragment_t **FrontList, DecalFragment_t **BackList )
{
    DecalFragment_t *Current, *Previous;

    Current = (*InputList);
    while( Current )
    {
        Previous = Current;
        Current = Current->Next;

        // only free the node if the fragment was actually split into smaller fragments
        if( DecalMgrSplitFragment( PlaneNormal, PointOnPlane, Previous, FrontList, BackList ) )
            delete Previous;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DecalMgrSplitFragment()
// Desc: 
//-----------------------------------------------------------------------------

static size_t DecalMgrSplitFragment( Vector_t *PlaneNormal, Vector_t *PointOnPlane, DecalFragment_t *Fragment, DecalFragment_t **FrontList, DecalFragment_t **BackList )
{
    size_t PointLocations[ 4 ];
    Vector_t FrontPoints[ 4 ];
    Vector_t BackPoints[ 4 ];
    size_t FrontCount, BackCount;
    Vector_t Intersect;
    float Percentage;
    Vector_t V1, V2;
    Plane_t Plane;
    DecalFragment_t *Fragment1, *Fragment2, *Fragment3;

    memcpy( &Plane.Point, PointOnPlane, sizeof( Vector_t ) );
    memcpy( &Plane.Normal, PlaneNormal, sizeof( Vector_t ) );

    PointLocations[ 0 ] = VectorClassifyPoint( &Fragment->P1, &Plane );
    PointLocations[ 1 ] = VectorClassifyPoint( &Fragment->P2, &Plane );
    PointLocations[ 2 ] = VectorClassifyPoint( &Fragment->P3, &Plane );

    // if all the points are infront of the plane then...
    if( INFRONT == PointLocations[ 0 ] && INFRONT == PointLocations[ 1 ] && INFRONT == PointLocations[ 2 ] )
    {
        VectorSubtract( Fragment->P2, Fragment->P1, V1 );
        VectorSubtract( Fragment->P3, Fragment->P1, V2 );
        VectorNormalisedCrossProd( &V1, &V2, &Fragment->Normal );

        // copy the fragment to the front list
        Fragment->Next = (*FrontList);
        (*FrontList) = Fragment;
        return 0;
    }
    // if all the points are behind the plane then...
    else if( BEHIND == PointLocations[ 0 ] && BEHIND == PointLocations[ 1 ] && BEHIND == PointLocations[ 2 ] )
    {
        VectorSubtract( Fragment->P2, Fragment->P1, V1 );
        VectorSubtract( Fragment->P3, Fragment->P1, V2 );
        VectorNormalisedCrossProd( &V1, &V2, &Fragment->Normal );

        // copy the fragment to the back list
        Fragment->TotallyBehind = 1;
        Fragment->Next = (*BackList);
        (*BackList) = Fragment;
        return 0;
    }

    // if we get to here then the points span the plane.  first, build a list of front and back points to create
    // the new fragments from
    FrontCount = BackCount = 0;
    switch( PointLocations[ 0 ] )
    {
        case ONPLANE :
        {
            VectorCopy( Fragment->P1, FrontPoints[ FrontCount ] );
            FrontCount++;
            VectorCopy( Fragment->P1, BackPoints[ BackCount ] );
            BackCount++;
            break;
        }
        case INFRONT :
        {
            VectorCopy( Fragment->P1, FrontPoints[ FrontCount ] );
            FrontCount++;
            break;
        }
        case BEHIND  :
        {
            VectorCopy( Fragment->P1, BackPoints[ BackCount ] );
            BackCount++;
            break;
        }
    }

    // does the next point cause the polygon to span the plane?
    if( PointLocations[ 1 ] != PointLocations[ 0 ] && PointLocations[ 1 ] != ONPLANE )
    {
        // create a new point
        VectorGetIntersect( &Fragment->P1, &Fragment->P2, &Plane.Point, &Plane.Normal, &Intersect, &Percentage );
        VectorCopy( Intersect, FrontPoints[ FrontCount ] );
        FrontCount++;
        VectorCopy( Intersect, BackPoints[ BackCount ] );
        BackCount++;
    }

    switch( PointLocations[ 1 ] )
    {
        case ONPLANE :
        {
            VectorCopy( Fragment->P2, FrontPoints[ FrontCount ] );
            FrontCount++;
            VectorCopy( Fragment->P2, BackPoints[ BackCount ] );
            BackCount++;
            break;
        }
        case INFRONT :
        {
            VectorCopy( Fragment->P2, FrontPoints[ FrontCount ] );
            FrontCount++;
            break;
        }
        case BEHIND  :
        {
            VectorCopy( Fragment->P2, BackPoints[ BackCount ] );
            BackCount++;
            break;
        }
    }

    // does the next point cause the polygon to span the plane?
    if( PointLocations[ 2 ] != PointLocations[ 1 ] && PointLocations[ 2 ] != ONPLANE )
    {
        // create a new point
        VectorGetIntersect( &Fragment->P2, &Fragment->P3, &Plane.Point, &Plane.Normal, &Intersect, &Percentage );
        VectorCopy( Intersect, FrontPoints[ FrontCount ] );
        FrontCount++;
        VectorCopy( Intersect, BackPoints[ BackCount ] );
        BackCount++;
    }

    switch( PointLocations[ 2 ] )
    {
        case ONPLANE :
        {
            VectorCopy( Fragment->P3, FrontPoints[ FrontCount ] );
            FrontCount++;
            VectorCopy( Fragment->P3, BackPoints[ BackCount ] );
            BackCount++;
            break;
        }
        case INFRONT :
        {
            VectorCopy( Fragment->P3, FrontPoints[ FrontCount ] );
            FrontCount++;
            break;
        }
        case BEHIND  :
        {
            VectorCopy( Fragment->P3, BackPoints[ BackCount ] );
            BackCount++;
            break;
        }
    }

    // does the next point cause the polygon to span the plane?
    if( PointLocations[ 0 ] != PointLocations[ 2 ] && PointLocations[ 0 ] != ONPLANE )
    {
        // create a new point
        VectorGetIntersect( &Fragment->P3, &Fragment->P1, &Plane.Point, &Plane.Normal, &Intersect, &Percentage );
        VectorCopy( Intersect, FrontPoints[ FrontCount ] );
        FrontCount++;
        VectorCopy( Intersect, BackPoints[ BackCount ] );
        BackCount++;
    }

    // build the new fragments from the front and back point lists
    Fragment1 = new DecalFragment_t();
    Fragment2 = new DecalFragment_t();

    Fragment1->ObjectPolyIndex = Fragment->ObjectPolyIndex;
    Fragment2->ObjectPolyIndex = Fragment->ObjectPolyIndex;

    // since we're only using triangles, either both lists will have 3 points, or one will have 3 points and
    // the other 4.

    VectorCopy( FrontPoints[ 0 ], Fragment1->P1 );
    VectorCopy( FrontPoints[ 1 ], Fragment1->P2 );
    VectorCopy( FrontPoints[ 2 ], Fragment1->P3 );

    // create this fragment's normal
    VectorSubtract( Fragment1->P2, Fragment1->P1, V1 );
    VectorSubtract( Fragment1->P3, Fragment1->P1, V2 );
    VectorNormalisedCrossProd( &V1, &V2, &Fragment1->Normal );

    // copy the fragment to the front list
    Fragment1->Next = (*FrontList);
    (*FrontList) = Fragment1;

    if( FrontCount == 4 )
    {
        Fragment3 = new DecalFragment_t();
        Fragment3->ObjectPolyIndex = Fragment->ObjectPolyIndex;

        VectorCopy( FrontPoints[ 0 ], Fragment3->P1 );
        VectorCopy( FrontPoints[ 2 ], Fragment3->P2 );
        VectorCopy( FrontPoints[ 3 ], Fragment3->P3 );

        // create this fragment's normal
        VectorCopy( Fragment1->Normal, Fragment3->Normal );

        // copy the fragment to the front list
        Fragment3->Next = (*FrontList);
        (*FrontList) = Fragment3;
    }

    VectorCopy( BackPoints[ 0 ], Fragment2->P1 );
    VectorCopy( BackPoints[ 1 ], Fragment2->P2 );
    VectorCopy( BackPoints[ 2 ], Fragment2->P3 );

    // create this fragment's normal
    VectorSubtract( Fragment2->P2, Fragment2->P1, V1 );
    VectorSubtract( Fragment2->P3, Fragment2->P1, V2 );
    VectorNormalisedCrossProd( &V1, &V2, &Fragment2->Normal );

    // copy the fragment to the Back list
    Fragment2->Next = (*BackList);
    (*BackList) = Fragment2;

    if( BackCount == 4 )
    {
        Fragment3 = new DecalFragment_t();
        Fragment3->ObjectPolyIndex = Fragment->ObjectPolyIndex;

        VectorCopy( BackPoints[ 0 ], Fragment3->P1 );
        VectorCopy( BackPoints[ 2 ], Fragment3->P2 );
        VectorCopy( BackPoints[ 3 ], Fragment3->P3 );

        // create this fragment's normal
        VectorCopy( Fragment2->Normal, Fragment3->Normal );

        // copy the fragment to the Back list
        Fragment3->Next = (*BackList);
        (*BackList) = Fragment3;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DecalMgrBuildTexCoordsAA()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrBuildTexCoordsAA( DecalFragment_t *Fragment, Vector_t *Center, DecalTexture_t *DecalTexture )
{
    // which is the major axis?
    if( fabsf( Fragment->Normal.X ) > fabsf( Fragment->Normal.Y ) )
    {
        if( fabsf( Fragment->Normal.X ) > fabsf( Fragment->Normal.Z ) )
        {
            // the x axis is the major axis
            Fragment->T1.U = ( ( Fragment->P1.Z - Center->Z ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T1.V = ( ( Fragment->P1.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T2.U = ( ( Fragment->P2.Z - Center->Z ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T2.V = ( ( Fragment->P2.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T3.U = ( ( Fragment->P3.Z - Center->Z ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T3.V = ( ( Fragment->P3.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;
        }
        else
        {
            // the z axis is the major axis
            Fragment->T1.U = ( ( Fragment->P1.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T1.V = ( ( Fragment->P1.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T2.U = ( ( Fragment->P2.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T2.V = ( ( Fragment->P2.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T3.U = ( ( Fragment->P3.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T3.V = ( ( Fragment->P3.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;
        }
    }
    else
    {
        if( fabsf( Fragment->Normal.Y ) > fabsf( Fragment->Normal.Z ) )
        {
            // the y axis is the major axis
            Fragment->T1.U = ( ( Fragment->P1.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T1.V = ( ( Fragment->P1.Z - Center->Z ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T2.U = ( ( Fragment->P2.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T2.V = ( ( Fragment->P2.Z - Center->Z ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T3.U = ( ( Fragment->P3.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T3.V = ( ( Fragment->P3.Z - Center->Z ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;
        }
        else
        {
            // the z axis is the major axis
            Fragment->T1.U = ( ( Fragment->P1.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T1.V = ( ( Fragment->P1.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T2.U = ( ( Fragment->P2.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T2.V = ( ( Fragment->P2.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;

            Fragment->T3.U = ( ( Fragment->P3.X - Center->X ) + DecalTexture->HalfHeight ) / DecalTexture->Height;
            Fragment->T3.V = ( ( Fragment->P3.Y - Center->Y ) + DecalTexture->HalfWidth  ) / DecalTexture->Width;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: DecalMgrBuildTexCoordsAP()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrBuildTexCoordsAP( DecalFragment_t *Fragment, DecalBuildInfo_t *DecalBuildInfo, DecalTexture_t *DecalTexture )
{
/*
    algorithm:
    1. find the plane that this fragment faces the most
    2. find the plane that this fragment faces the 2nd most
       repeat for each point

    3.     if this point is behind the plane from step 1. then
               use the plane from step 1. in the next step as the 'up' plane
           else
               use the plane from step 2. in the next step as the 'up' plane
           end if

    4.    project the point to the 'up' plane to get a temp point

    5.    calc the texture's origin in world space

    6.    project the temp point to the 'right' plane to get a new point
           compute the magnitude from this new point to the corner point to get the 'U' coord

    7.    project the temp point to the 'forward' plane to get a new point
           compute the magnitude from this new point to the corner point to get the 'V' coord

       end repeat
*/
    size_t i;
    size_t ClosestIndex1;
    size_t ClosestIndex2;
    float ClosestValue1;
    float ClosestValue2;
    float Dot, Distance;
    Vector_t *Points[ 3 ];
    DecalTexCoord_t *TexCoords[ 3 ];
    Plane_t Up, Right, Forward;
    Vector_t TempPoint, EdgePoint;
    Vector_t Origin;

    Points[ 0 ] = &Fragment->P1;
    Points[ 1 ] = &Fragment->P2;
    Points[ 2 ] = &Fragment->P3;
    TexCoords[ 0 ] = &Fragment->T1;
    TexCoords[ 1 ] = &Fragment->T2;
    TexCoords[ 2 ] = &Fragment->T3;

    // find the plane that this fragment faces the most
    ClosestValue1 = -1.0f;
    for( i = 0; i < 6; i++ )
    {
        Dot = VectorDotProd( Fragment->Normal, DecalBuildInfo->BBoxPlanes[ i ].Normal );
        if( Dot > ClosestValue1 )
        {
            ClosestValue1 = Dot;
            ClosestIndex1 = i;
        }
    }

    // find the plane that this fragment faces the 2nd most
    ClosestValue2 = -1.0f;
    for( i = 0; i < 6; i++ )
    {
        if( i != ClosestIndex1 )
        {
            Dot = VectorDotProd( Fragment->Normal, DecalBuildInfo->BBoxPlanes[ i ].Normal );
            if( Dot > ClosestValue2 )
            {
                ClosestValue2 = Dot;
                ClosestIndex2 = i;
            }
        }
    }

    // repeat for each point
    for( i = 0; i < 3; i++ )
    {
        //     if this point is behind the plane from step 1. then
        //         use the plane from step 1. in the next step as the 'up' plane

        VectorCopy( DecalBuildInfo->BBoxPlanes[ ClosestIndex1 ].Normal, Up.Normal );
        VectorCopy( DecalBuildInfo->PointOnPlane[ ClosestIndex1 ], Up.Point );

        if( BEHIND == VectorClassifyPoint( Points[ i ], &Up ) )
        {
            DecalMgrGetRightAndForward( ClosestIndex1, DecalBuildInfo, &Right, &Forward );
        }

        //     else
        //         use the plane from step 2. in the next step as the 'up' plane

        else
        {
            VectorCopy( DecalBuildInfo->BBoxPlanes[ ClosestIndex2 ].Normal, Up.Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ ClosestIndex2 ], Up.Point );
            DecalMgrGetRightAndForward( ClosestIndex2, DecalBuildInfo, &Right, &Forward );
        }

        //     end if

        // project the point to the 'up' plane to get a temp point
        Distance = MathPlaneEquation( &Up.Point, &Up.Normal, Points[ i ] );
        TempPoint.X = Points[ i ]->X + Up.Normal.X * -Distance;
        TempPoint.Y = Points[ i ]->Y + Up.Normal.Y * -Distance;
        TempPoint.Z = Points[ i ]->Z + Up.Normal.Z * -Distance;

        // calc the texture's origin in world space
        Origin.X = DecalBuildInfo->ImpactPoint.X + Right.Normal.X * DecalTexture->HalfWidth;
        Origin.Y = DecalBuildInfo->ImpactPoint.Y + Right.Normal.Y * DecalTexture->HalfWidth;
        Origin.Z = DecalBuildInfo->ImpactPoint.Z + Right.Normal.Z * DecalTexture->HalfWidth;
        Origin.X += Up.Normal.X * DecalTexture->HalfWidth;
        Origin.Y += Up.Normal.Y * DecalTexture->HalfWidth;
        Origin.Z += Up.Normal.Z * DecalTexture->HalfWidth;
        Origin.X += Forward.Normal.X * DecalTexture->HalfWidth;
        Origin.Y += Forward.Normal.Y * DecalTexture->HalfWidth;
        Origin.Z += Forward.Normal.Z * DecalTexture->HalfWidth;

        // project the temp point to the 'right' plane to get a new point
        // compute the magnitude from this new point to the corner point to get the 'V' coord

        Distance = MathPlaneEquation( &Right.Point, &Right.Normal, &TempPoint );
        EdgePoint.X = TempPoint.X + Right.Normal.X * -Distance;
        EdgePoint.Y = TempPoint.Y + Right.Normal.Y * -Distance;
        EdgePoint.Z = TempPoint.Z + Right.Normal.Z * -Distance;

        TexCoords[ i ]->V = fabsf( VectorMagnitude2( &EdgePoint, &Origin ) / DecalTexture->Width );

        // project the temp point to the 'forward' plane to get a new point
        // compute the magnitude from this new point to the corner point to get the 'U' coord

        Distance = MathPlaneEquation( &Forward.Point, &Forward.Normal, &TempPoint );
        EdgePoint.X = TempPoint.X + Forward.Normal.X * -Distance;
        EdgePoint.Y = TempPoint.Y + Forward.Normal.Y * -Distance;
        EdgePoint.Z = TempPoint.Z + Forward.Normal.Z * -Distance;

        TexCoords[ i ]->U = fabsf( VectorMagnitude2( &EdgePoint, &Origin ) / DecalTexture->Width );
    }
    // end repeat
}


//-----------------------------------------------------------------------------
// Name: DecalMgrGetRightAndForward()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrGetRightAndForward( size_t Up, DecalBuildInfo_t *DecalBuildInfo, Plane_t *Right, Plane_t *Forward )
{
    switch( Up )
    {
        case BBOX_FRONT :
            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal, Right->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ], Right->Point );

            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal, Forward->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_BOTTOM ], Forward->Point );
            break;

        case BBOX_BACK :
            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal, Right->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ], Right->Point );

            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Normal, Forward->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_TOP ], Forward->Point );
            break;

        case BBOX_RIGHT :
            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_BOTTOM ].Normal, Right->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_BOTTOM ], Right->Point );

            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal, Forward->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_FRONT ], Forward->Point );
            break;

        case BBOX_LEFT :
            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_TOP ].Normal, Right->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_TOP ], Right->Point );

            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal, Forward->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_FRONT ], Forward->Point );
            break;

        case BBOX_TOP :
            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal, Right->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ], Right->Point );

            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_FRONT ].Normal, Forward->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_FRONT ], Forward->Point );
            break;

        case BBOX_BOTTOM :
            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_RIGHT ].Normal, Right->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_RIGHT ], Right->Point );

            VectorCopy( DecalBuildInfo->BBoxPlanes[ BBOX_BACK ].Normal, Forward->Normal );
            VectorCopy( DecalBuildInfo->PointOnPlane[ BBOX_BACK ], Forward->Point );
            break;
    }
}


//-----------------------------------------------------------------------------
// Name: DecalMgrCleanTexCoords()
// Desc: 
//-----------------------------------------------------------------------------

static void DecalMgrCleanTexCoords( Decal_t *Decal )
{
    size_t i, j;

    for( i = 0; i < Decal->FragmentCount; i++ )
    {
        for( j = 0; j < Decal->FragmentCount; j++ )
        {
            if( i != j )
            {
                // compare point one
                if( VectorFuzzyCompare( Decal->Fragments[ i ].P1, Decal->Fragments[ j ].P1, 0.001f ) )
                {
                    Decal->Fragments[ i ].T1.U = Decal->Fragments[ j ].T1.U;
                    Decal->Fragments[ i ].T1.V = Decal->Fragments[ j ].T1.V;
                }
                else if( VectorFuzzyCompare( Decal->Fragments[ i ].P1, Decal->Fragments[ j ].P2, 0.001f ) )
                {
                    Decal->Fragments[ i ].T1.U = Decal->Fragments[ j ].T2.U;
                    Decal->Fragments[ i ].T1.V = Decal->Fragments[ j ].T2.V;
                }
                else if( VectorFuzzyCompare( Decal->Fragments[ i ].P1, Decal->Fragments[ j ].P3, 0.001f ) )
                {
                    Decal->Fragments[ i ].T1.U = Decal->Fragments[ j ].T3.U;
                    Decal->Fragments[ i ].T1.V = Decal->Fragments[ j ].T3.V;
                }

                // compare point two
                if( VectorFuzzyCompare( Decal->Fragments[ i ].P2, Decal->Fragments[ j ].P1, 0.001f ) )
                {
                    Decal->Fragments[ i ].T2.U = Decal->Fragments[ j ].T1.U;
                    Decal->Fragments[ i ].T2.V = Decal->Fragments[ j ].T1.V;
                }
                else if( VectorFuzzyCompare( Decal->Fragments[ i ].P2, Decal->Fragments[ j ].P2, 0.001f ) )
                {
                    Decal->Fragments[ i ].T2.U = Decal->Fragments[ j ].T2.U;
                    Decal->Fragments[ i ].T2.V = Decal->Fragments[ j ].T2.V;
                }
                else if( VectorFuzzyCompare( Decal->Fragments[ i ].P2, Decal->Fragments[ j ].P3, 0.001f ) )
                {
                    Decal->Fragments[ i ].T2.U = Decal->Fragments[ j ].T3.U;
                    Decal->Fragments[ i ].T2.V = Decal->Fragments[ j ].T3.V;
                }

                // compare point three
                if( VectorFuzzyCompare( Decal->Fragments[ i ].P3, Decal->Fragments[ j ].P1, 0.001f ) )
                {
                    Decal->Fragments[ i ].T3.U = Decal->Fragments[ j ].T1.U;
                    Decal->Fragments[ i ].T3.V = Decal->Fragments[ j ].T1.V;
                }
                else if( VectorFuzzyCompare( Decal->Fragments[ i ].P3, Decal->Fragments[ j ].P2, 0.001f ) )
                {
                    Decal->Fragments[ i ].T3.U = Decal->Fragments[ j ].T2.U;
                    Decal->Fragments[ i ].T3.V = Decal->Fragments[ j ].T2.V;
                }
                else if( VectorFuzzyCompare( Decal->Fragments[ i ].P3, Decal->Fragments[ j ].P3, 0.001f ) )
                {
                    Decal->Fragments[ i ].T3.U = Decal->Fragments[ j ].T3.U;
                    Decal->Fragments[ i ].T3.V = Decal->Fragments[ j ].T3.V;
                }

            }
        }
    }
}
