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
#include <string.h>

#include "MeshTypes.h"
#include "LoadingScreen.h"

#define PATCH_LEFT          0
#define PATCH_RIGHT         1
#define PATCH_FRONT         2
#define PATCH_BACK          3


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct TwoIntegers_s
{
    int X, Z;
}
TwoIntegers_t;

typedef struct EdgeInfo_s
{
    size_t TextureStyles[ 4 ];
    size_t TileType[ 4 ];
}
EdgeInfo_t;

typedef struct PatchList_s
{
    TwoIntegers_t *Patches;
    EdgeInfo_t *Edges;
    size_t Count;
    size_t Max;
}
PatchList_t;

static TwoIntegers_t PatchNeighbours[] =
{
    { -1, -1 }, {  0, -1 }, {  1, -1 },
    { -1,  0 },             {  1,  0 },
    { -1,  1 }, {  0,  1 }, {  1,  1 }
};


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

// util functions
static void MeshSetPatchTexture( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, size_t Tile, size_t PatchX, size_t PatchZ );
static PatchList_t *MeshGetPatchesWithTexture( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle );
static void MeshChangeNeighbourPatches( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList, PatchList_t **ChangedPatches );
static size_t MeshIsTextureGreater( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, int PatchX, int PatchZ );

static PatchList_t *MeshGetEdgePatches( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle );
static void MeshGetEdgeInfo( Mesh_t *Mesh, TileSet_t *TileSet, PatchList_t *PatchList, size_t TextureStyle );
static void MeshGetPatchInfo( Mesh_t *Mesh, TileSet_t *TileSet, size_t CurrentStyle, size_t PatchX, size_t PatchZ, size_t *OutStyle, size_t *OutTile );

static void MeshChangeEdgePatchesOpp( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );
static void MeshChangeEdgePatchesSingle( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );
static void MeshChangeEdgePatchesCorner( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );
static void MeshChangeEdgePatchesOppFacing( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );
static void MeshChangeEdgePatches4WayJoin( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );
static void MeshChangeEdgePatchesRemovePatch( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );
static void MeshChangeEdgePatchesRemoveSingles( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList );

// patch list functions
static PatchList_t *MeshCreatePatchList( size_t Max );
static void MeshDeletePatchList( PatchList_t *PatchList );
static void MeshAddPatchToList( PatchList_t *PatchList, int PatchX, int PatchZ );
static size_t MeshIsPatchInList( PatchList_t *PatchList, int PatchX, int PatchZ );

// major steps in the algorithm
static void MeshAssignBasedOnHeight( Mesh_t *Mesh, TileSet_t *TileSet );


//-----------------------------------------------------------------------------
// Name: MeshAssignTextures()
// Desc: 
//-----------------------------------------------------------------------------

void MeshAssignTextures( Mesh_t *Mesh, TileSet_t *TileSet, size_t LoadScreenTexture )
{
    PatchList_t *PatchList;
    PatchList_t *ChangedPatches;
    size_t TextureStyle;
    size_t i;

    BeginLoadingScreen( LoadScreenTexture, "Texturing World...", 0, 3 );

    // step 1: assign initial textures
    MeshAssignBasedOnHeight( Mesh, TileSet );

    // step 2: make sure there's at least two patches between each texture
    // style on the landscape. ignore the very last texture style range because
    // it only has TILE_CENTER type tiles.
    for( TextureStyle = 0; TextureStyle < TileSet->StyleCount - 1; TextureStyle++ )
    {
        PatchList = MeshGetPatchesWithTexture( Mesh, TileSet, TextureStyle );

        // make a two patch thick border around this texture style on the landscape
        MeshChangeNeighbourPatches( Mesh, TileSet, TextureStyle, PatchList, &ChangedPatches );
        MeshChangeNeighbourPatches( Mesh, TileSet, TextureStyle, ChangedPatches, nullptr );

        MeshDeletePatchList( ChangedPatches );
        MeshDeletePatchList( PatchList );
    }

    UpdateLoadingScreen( 1 );

    // step 3: get a list of the patches on the edge of each texture style.  for each
    // patch in this list that is sharing 2 opposite edges with patches of the same 
    // style, change this patch to a use the texture style's center texture.
    for( TextureStyle = 0; TextureStyle < TileSet->StyleCount - 1; TextureStyle++ )
    {
        // patches that will be changed are those that share 4, 3 and 2 edges with other
        // patches, so here we loop three times to ensure all 3 types will be changed.
        for( i = 0; i < 3; i++ )
        {
            PatchList = MeshGetEdgePatches( Mesh, TileSet, TextureStyle );
            MeshGetEdgeInfo( Mesh, TileSet, PatchList, TextureStyle );
            MeshChangeEdgePatchesOpp( Mesh, TileSet, TextureStyle, PatchList );
            MeshDeletePatchList( PatchList );
        }
    }

    UpdateLoadingScreen( 2 );

    // step 4: now that the landscape has been cleaned up a lot, insert joiner textures on the
    // landscape so that there's a nice looking transition between texture styles. again,
    // ignore the last texture since it only has a center tile.

    for( TextureStyle = 0; TextureStyle < TileSet->StyleCount - 1; TextureStyle++ )
    {
        PatchList = MeshGetEdgePatches( Mesh, TileSet, TextureStyle );
        MeshGetEdgeInfo( Mesh, TileSet, PatchList, TextureStyle );

        // insert joiner texture for patches that share just one edge
        // with a patch of the same texture style.
        MeshChangeEdgePatchesSingle( Mesh, TileSet, TextureStyle, PatchList );

        // change the corner patches to use corner textures
        MeshChangeEdgePatchesCorner( Mesh, TileSet, TextureStyle, PatchList );

        // and finally, handle the special cases
        MeshGetEdgeInfo( Mesh, TileSet, PatchList, TextureStyle );
        MeshChangeEdgePatchesOppFacing( Mesh, TileSet, TextureStyle, PatchList );

        MeshGetEdgeInfo( Mesh, TileSet, PatchList, TextureStyle );
        MeshChangeEdgePatches4WayJoin( Mesh, TileSet, TextureStyle, PatchList );

        MeshGetEdgeInfo( Mesh, TileSet, PatchList, TextureStyle );
        MeshChangeEdgePatchesRemovePatch( Mesh, TileSet, TextureStyle, PatchList );

        MeshGetEdgeInfo( Mesh, TileSet, PatchList, TextureStyle );
        MeshChangeEdgePatchesRemoveSingles( Mesh, TileSet, TextureStyle, PatchList );

        // clean up...
        MeshDeletePatchList( PatchList );
    }

    EndLoadingScreen( "Texturing World...Done", 3 );
}


//-----------------------------------------------------------------------------
// Name: MeshAssignBasedOnHeight()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshAssignBasedOnHeight( Mesh_t *Mesh, TileSet_t *TileSet )
{
    size_t TileX, TileZ;
    size_t PolyIndex;
    size_t NumDiffTextures;
    size_t TexStyle;
    float AvgHeight;

    NumDiffTextures = size_t( (size_t)Mesh->LargestHeight - (size_t)Mesh->SmallestHeight ) / TileSet->StyleCount;

    for( TileZ = 0; TileZ < Mesh->TilesDeep; TileZ += 2 )
    {
        for( TileX = 0; TileX < Mesh->TilesWide; TileX += 2 )
        {
            PolyIndex = ( 2 * TileZ * Mesh->TilesWide ) + ( 2 * TileX );

            AvgHeight = ( Mesh->Points[ Mesh->Polygons[ PolyIndex ].P1 ].Y + Mesh->Points[ Mesh->Polygons[ PolyIndex ].P2 ].Y
                + Mesh->Points[ Mesh->Polygons[ PolyIndex ].P3 ].Y ) / 3.0f;

            TexStyle = (size_t)AvgHeight / NumDiffTextures;

            if( TexStyle < 0 )
                TexStyle = 0;
            else if( TexStyle > TileSet->StyleCount - 1 )
                TexStyle = TileSet->StyleCount - 1;

            // reverse the order so that texture 0 is at the top and texture 3 is at the bottom
            TexStyle = ( TileSet->StyleCount - 1 ) - TexStyle;

            MeshSetPatchTexture( Mesh, TileSet, TexStyle, (size_t)TILE_CENTER, TileX / 2, TileZ / 2 );
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshSetPatchTexture()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshSetPatchTexture( Mesh_t *Mesh, TileSet_t *TileSet, size_t Style, size_t Tile, size_t PatchX, size_t PatchZ )
{
    size_t PolyIndex;
    size_t X, Z, Tex;

    // choose a random texture if there's more than one texture here
    if( !TileSet->Styles[ Style ].Tiles[ Tile ].TexCount )
    {
        Tex = 0;
    }
    else
    {
        Tex = rand() % TileSet->Styles[ Style ].Tiles[ Tile ].TexCount;
        Tex = TileSet->Styles[ Style ].Tiles[ Tile ].Textures[ Tex ];
    }

    // convert the patch coords to tile coords
    X = PatchX * 2;
    Z = PatchZ * 2;
    if( X < 0 || X > Mesh->TilesWide || Z < 0 || Z > Mesh->TilesDeep )
        return;

    // get the starting poly index and simply assign the texture to each poly in the patch
    PolyIndex = ( 2 * Z * Mesh->TilesWide ) + ( 2 * X );

    Mesh->Polygons[ PolyIndex+0 ].TextureNumber = Tex;
    Mesh->Polygons[ PolyIndex+1 ].TextureNumber = Tex;
    Mesh->Polygons[ PolyIndex+2 ].TextureNumber = Tex;
    Mesh->Polygons[ PolyIndex+3 ].TextureNumber = Tex;

    Mesh->Polygons[ PolyIndex+(2*Mesh->TilesWide)+0 ].TextureNumber = Tex;
    Mesh->Polygons[ PolyIndex+(2*Mesh->TilesWide)+1 ].TextureNumber = Tex;
    Mesh->Polygons[ PolyIndex+(2*Mesh->TilesWide)+2 ].TextureNumber = Tex;
    Mesh->Polygons[ PolyIndex+(2*Mesh->TilesWide)+3 ].TextureNumber = Tex;
}


//-----------------------------------------------------------------------------
// Name: MeshGetPatchesWithTexture
// Desc: 
//-----------------------------------------------------------------------------

static PatchList_t *MeshGetPatchesWithTexture( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle )
{
    size_t TileX, TileZ;
    size_t PolyIndex;
    size_t i;
    PatchList_t *PatchList;

    PatchList = MeshCreatePatchList(size_t( Mesh->TilesWide / 2 ) * size_t( Mesh->TilesDeep / 2 ) );

    for( TileZ = 0; TileZ < Mesh->TilesDeep; TileZ += 2 )
    {
        for( TileX = 0; TileX < Mesh->TilesWide; TileX += 2 )
        {
            // get the first poly in this patch
            PolyIndex = ( 2 * TileZ * Mesh->TilesWide ) + ( 2 * TileX );

            // check if this poly's texture matches any of the variations of the center tile
            // of the given texture style

            for( i = 0; i < TileSet->Styles[ TextureStyle ].Tiles[ TILE_CENTER ].TexCount; i++ )
            {
                if( Mesh->Polygons[ PolyIndex ].TextureNumber == TileSet->Styles[ TextureStyle ].Tiles[ TILE_CENTER ].Textures[ i ] )
                {
                    // yup it matches.  add this patch to the list if it's not already in it.

                    if( !MeshIsPatchInList( PatchList, int(TileX / 2), int(TileZ / 2) ) )
                        MeshAddPatchToList( PatchList, int(TileX / 2), int(TileZ / 2) );

                    break;
                }
            }
        }
    }

    return PatchList;
}


//-----------------------------------------------------------------------------
// Name: MeshCreatePatchList
// Desc: 
//-----------------------------------------------------------------------------

static PatchList_t *MeshCreatePatchList(size_t Max )
{
    PatchList_t *PatchList;

    PatchList = new PatchList_t();

    PatchList->Count = 0;
    PatchList->Max = Max;

    PatchList->Patches = new TwoIntegers_t[PatchList->Max];
    memset( PatchList->Patches, 0, sizeof( TwoIntegers_t ) * PatchList->Max );

    PatchList->Edges = new EdgeInfo_t[PatchList->Max];
    memset( PatchList->Edges, 0, sizeof( EdgeInfo_t ) * PatchList->Max );

    return PatchList;
}


//-----------------------------------------------------------------------------
// Name: MeshDeletePatchList()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshDeletePatchList( PatchList_t *PatchList )
{
    if( PatchList )
    {
        if( PatchList->Patches )
            delete [] PatchList->Patches;

        if( PatchList->Edges )
            delete [] PatchList->Edges;

        delete PatchList;
    }
}


//-----------------------------------------------------------------------------
// Name: MeshAddPatchToList()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshAddPatchToList( PatchList_t *PatchList, int PatchX, int PatchZ )
{
    if( PatchList->Count >= PatchList->Max )
        return;

    PatchList->Patches[ PatchList->Count ].X = PatchX;
    PatchList->Patches[ PatchList->Count ].Z = PatchZ;

    PatchList->Count++;
}


//-----------------------------------------------------------------------------
// Name: MeshIsPatchInList
// Desc: 
//-----------------------------------------------------------------------------

static size_t MeshIsPatchInList( PatchList_t *PatchList, int PatchX, int PatchZ )
{
    size_t i;

    for( i = 0; i < PatchList->Count; i++ )
    {
        if( PatchX == PatchList->Patches[ i ].X && PatchZ == PatchList->Patches[ i ].Z )
            return 1;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MeshIsTextureGreater
// Desc: 
//-----------------------------------------------------------------------------

static size_t MeshIsTextureGreater( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, int PatchX, int PatchZ )
{
    size_t PolyIndex;
    int TileX, TileZ;
    size_t Style, Tile, Tex;

    TileX = PatchX * 2;
    TileZ = PatchZ * 2;

    if( TileX < 0 || TileX >= int(Mesh->TilesWide) ||
        TileZ < 0 || TileZ >= int(Mesh->TilesDeep) )
        return 0;

    // get the first polygon in the given patch
    PolyIndex = ( 2 * TileZ * Mesh->TilesWide ) + ( 2 * TileX );

    // find out which style this polygon's texture belongs too.
    for( Style = 0; Style < TileSet->StyleCount; Style++ )
    {
        for( Tile = 0; Tile < TILE_MAXTILES; Tile++ )
        {
            for( Tex = 0; Tex < TileSet->Styles[ Style ].Tiles[ Tile ].TexCount; Tex++ )
            {
                if( TileSet->Styles[ Style ].Tiles[ Tile ].Textures[ Tex ] == Mesh->Polygons[ PolyIndex ].TextureNumber )
                {
                    // we've found this polygon's texture in the given tileset.  return true if it's texture is greater
                    // than the given style.
                    return( Style > TextureStyle );
                }
            }
        }
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MeshChangeNeighbourPatches()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeNeighbourPatches( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList, PatchList_t **ChangedPatches )
{
    size_t PatchIndex;
    size_t OffsetIdx;
    int PatchX, PatchZ;

    // check the neighbour patch of each patch in the source list.  if the neighbour patch
    // uses a texture that is less than or equal to the current patch then leave it, otherwise
    // set it to one greater than the current patch's texture.  store all patches that have
    // their textures changed if the caller passed a pointer to a patchlist.

    if( ChangedPatches )
        (*ChangedPatches) = MeshCreatePatchList(size_t( Mesh->TilesWide / 2 ) * size_t( Mesh->TilesDeep / 2 ) );

    for( PatchIndex = 0; PatchIndex < SourceList->Count; PatchIndex++ )
    {
        for( OffsetIdx = 0; OffsetIdx < 8; OffsetIdx++ )
        {
            PatchX = SourceList->Patches[ PatchIndex ].X + PatchNeighbours[ OffsetIdx ].X;
            PatchZ = SourceList->Patches[ PatchIndex ].Z + PatchNeighbours[ OffsetIdx ].Z;

            if( MeshIsTextureGreater( Mesh, TileSet, TextureStyle, PatchX, PatchZ ) )
            {
                MeshSetPatchTexture( Mesh, TileSet, TextureStyle+1, TILE_CENTER, PatchX, PatchZ );
                if( ChangedPatches )
                    MeshAddPatchToList( (*ChangedPatches), PatchX, PatchZ );
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshGetEdgePatches
// Desc: 
//-----------------------------------------------------------------------------

static PatchList_t *MeshGetEdgePatches( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle )
{
    int TileX, TileZ;
    int PatchX, PatchZ;
    size_t PolyIndex;
    size_t OffsetIdx;
    size_t i;
    PatchList_t *PatchList;

    PatchList = MeshCreatePatchList(size_t( Mesh->TilesWide / 2 ) * size_t( Mesh->TilesDeep / 2 ) );

    for( TileZ = 0; TileZ < int(Mesh->TilesDeep); TileZ += 2 )
    {
        for( TileX = 0; TileX < int(Mesh->TilesWide); TileX += 2 )
        {
            // get the first poly in this patch
            PolyIndex = ( 2 * TileZ * Mesh->TilesWide ) + ( 2 * TileX );

            // check if this poly's texture matches any of the variations of the center tile
            // of the given texture style

            for( i = 0; i < TileSet->Styles[ TextureStyle ].Tiles[ TILE_CENTER ].TexCount; i++ )
            {
                if( Mesh->Polygons[ PolyIndex ].TextureNumber == TileSet->Styles[ TextureStyle ].Tiles[ TILE_CENTER ].Textures[ i ] )
                {
                    // yup it matches.  check all the neighbours and add any that pass the test to the list.

                    for( OffsetIdx = 0; OffsetIdx < 8; OffsetIdx++ )
                    {
                        PatchX = ( TileX / 2 ) + PatchNeighbours[ OffsetIdx ].X;
                        PatchZ = ( TileZ / 2 ) + PatchNeighbours[ OffsetIdx ].Z;

                        // does this neighbour have a greater texture?
                        if( MeshIsTextureGreater( Mesh, TileSet, TextureStyle, PatchX, PatchZ ) )
                        {
                            // yup, add it if it's not already in the list
                            if( !MeshIsPatchInList( PatchList, PatchX, PatchZ ) )
                                MeshAddPatchToList( PatchList, PatchX, PatchZ );
                        }
                    }

                }
            }
        }
    }

    return PatchList;
}


//-----------------------------------------------------------------------------
// Name: MeshGetEdgeInfo()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshGetEdgeInfo( Mesh_t *Mesh, TileSet_t *TileSet, PatchList_t *PatchList, size_t TextureStyle )
{
    size_t i;

    for( i = 0; i < PatchList->Count; i++ )
    {
        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, PatchList->Patches[ i ].X-1, PatchList->Patches[ i ].Z,
                          &PatchList->Edges[ i ].TextureStyles[ PATCH_LEFT ], &PatchList->Edges[ i ].TileType[ PATCH_LEFT ] );

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, PatchList->Patches[ i ].X+1, PatchList->Patches[ i ].Z,
                          &PatchList->Edges[ i ].TextureStyles[ PATCH_RIGHT ], &PatchList->Edges[ i ].TileType[ PATCH_RIGHT ] );

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, PatchList->Patches[ i ].X, PatchList->Patches[ i ].Z+1,
                          &PatchList->Edges[ i ].TextureStyles[ PATCH_FRONT ], &PatchList->Edges[ i ].TileType[ PATCH_FRONT ] );

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, PatchList->Patches[ i ].X, PatchList->Patches[ i ].Z-1,
                          &PatchList->Edges[ i ].TextureStyles[ PATCH_BACK ], &PatchList->Edges[ i ].TileType[ PATCH_BACK ] );
    }
}


//-----------------------------------------------------------------------------
// Name: MeshGetPatchInfo()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshGetPatchInfo( Mesh_t *Mesh, TileSet_t *TileSet, size_t CurrentStyle, size_t PatchX, size_t PatchZ, size_t *OutStyle, size_t *OutTile )
{
    size_t PolyIndex;
    size_t TileX, TileZ;
    size_t Style, Tile, Tex;

    TileX = PatchX * 2;
    TileZ = PatchZ * 2;

    if( TileX < 0 || TileX >= Mesh->TilesWide ||
        TileZ < 0 || TileZ >= Mesh->TilesDeep )
    {
        if( OutStyle )
            (*OutStyle) = -1;

        if( OutTile )
            (*OutTile) = -1;

        return;
    }

    // get the first polygon in the given patch
    PolyIndex = ( 2 * TileZ * Mesh->TilesWide ) + ( 2 * TileX );

    // find out which style this polygon's texture belongs too.
    for( Style = 0; Style < TileSet->StyleCount; Style++ )
    {
        for( Tile = 0; Tile < TILE_MAXTILES; Tile++ )
        {
            for( Tex = 0; Tex < TileSet->Styles[ Style ].Tiles[ Tile ].TexCount; Tex++ )
            {
                if( TileSet->Styles[ Style ].Tiles[ Tile ].Textures[ Tex ] == Mesh->Polygons[ PolyIndex ].TextureNumber )
                {
                    if( OutStyle )
                        (*OutStyle) = Style;

                    if( OutTile )
                        (*OutTile) = Tile;

                    return;
                }
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatchesOpp()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatchesOpp( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i;

    // go thru the given list and change each patch to use a center tile if the patch
    // shares 2 opposite edges with a other patches whose texture is a center tile.
    // all patches checked must be have the same texture style.

    for( i = 0; i < SourceList->Count; i++ )
    {
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT ]  == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT ]  == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
        }
        else if( SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] == TILE_CENTER &&
                 SourceList->Edges[ i ].TextureStyles[ PATCH_BACK ]  == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK ]  == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatchesSingle()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatchesSingle( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i, j;
    size_t Tile, Count;

    // search the given list for all patches that share exactly one edge with a
    // patch of the same style that is a center patch.

    for( i = 0; i < SourceList->Count; i++ )
    {
        Count = 0;
        for( j = 0; j < 4; j++ )
        {
            if( SourceList->Edges[ i ].TextureStyles[ j ] == TextureStyle &&
                SourceList->Edges[ i ].TileType[ j ] == TILE_CENTER )
            {
                Count++;
                Tile = j;
            }
        }

        if( 1 == Count )
        {
            switch( Tile )
            {
                case PATCH_LEFT :  Tile = TILE_LEFT; break;
                case PATCH_RIGHT : Tile = TILE_RIGHT; break;
                case PATCH_FRONT : Tile = TILE_TOP; break;
                case PATCH_BACK :  Tile = TILE_BOTTOM; break;
            }

            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, Tile, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatchesCorner()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatchesCorner( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i, j;
    size_t Style, Tile, Count;

    // search the given list for all patches that are either a convex or a concave
    // corner patch.

    for( i = 0; i < SourceList->Count; i++ )
    {
        // concave corners
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT  ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_BACK ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK ] == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_BACK  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK  ] == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }

        // convex corners.  these are the exception case becase they're on the diagonals.
        // first, make sure that the patches we change share no edges with any other center
        // patches of the same style.

        Count = 0;
        for( j = 0; j < 4; j++ )
        {
            if( SourceList->Edges[ i ].TextureStyles[ j ] == TextureStyle &&
                SourceList->Edges[ i ].TileType[ j ] == TILE_CENTER )
                Count++;
        }

        if( Count > 0 )
            continue;

        // second, find the patch on the diagonal from the current patch that has a center texture
        // and is of the same style.

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
        if( Style == TextureStyle && Tile == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_BOTTOMLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
        if( Style == TextureStyle && Tile == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_BOTTOMRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
        if( Style == TextureStyle && Tile == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_TOPLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }

        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
        if( Style == TextureStyle && Tile == TILE_CENTER )
        {
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_TOPRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
            continue;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatchesOppFacing()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatchesOppFacing( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i;
    size_t Style, Tile;
    size_t NeighbourStyle = 0, NeighbourTile = 0;

    // search the given list for all patches whose neighbour patch is a joiner and
    // is facing in the opposite direction to the patch.  change both the neighbour
    // and this patch to be corner patches.

    for( i = 0; i < SourceList->Count; i++ )
    {
        MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z, &Style, &Tile );
        if( Style == TextureStyle )
        {
            NeighbourStyle = NeighbourTile - 1;
            switch( Tile )
            {
                case TILE_LEFT :
                    // is the patch above or below this patch a TILE_RIGHT tile?
                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z-1, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_RIGHT )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z-1 );
                        break;
                    }

                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z+1, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_RIGHT )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z+1 );
                        break;
                    }
                    break;

                case TILE_RIGHT :
                    // is the patch above or below this patch a TILE_LEFT tile?
                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z-1, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_LEFT )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z-1 );
                        break;
                    }

                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z+1, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_LEFT )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z+1 );
                        break;
                    }
                    break;

                case TILE_TOP :
                    // is the patch to the left or right of this patch a TILE_BOTTOM tile?
                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_BOTTOM )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z );
                        break;
                    }

                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_BOTTOM )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z );
                        break;
                    }
                    break;

                case TILE_BOTTOM :
                    // is the patch to the left or right of this patch a TILE_TOP tile?
                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_TOP )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z );
                        break;
                    }

                    MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z, &NeighbourStyle, &NeighbourTile );
                    if( NeighbourStyle == TextureStyle && NeighbourTile == TILE_TOP )
                    {
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                        MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z );
                        break;
                    }
                    break;

                default :   // TILE_CENTER, TILE_TOPLEFT, TILE_TOPRIGHT etc
                    break;
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatches4WayJoin()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatches4WayJoin( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i;
    size_t Style, Tile;

    // search the given list for all patches whose diagonally opposite
    // patches are the same style and are both of type TILE_CENTER. all
    // of the horiz and vert neighbour patches must not be of type
    // TILE_CENTER also.

    for( i = 0; i < SourceList->Count; i++ )
    {
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT  ] != TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] != TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] != TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_BACK  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK  ] != TILE_CENTER )
        {
            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_JOINEDBLTR, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    continue;
                }
            }

            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_JOINEDTLBR, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    continue;
                }
            }

            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_JOINEDTLBR, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    continue;
                }
            }

            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_JOINEDBLTR, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    continue;
                }
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatchesRemovePatch()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatchesRemovePatch( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i;
    size_t Style, Tile;

    // search the list for patches that normally would be a corner patch but
    // have a TILE_CENTER in the center of the "L" part of the patch.

    for( i = 0; i < SourceList->Count; i++ )
    {
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT  ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] == TILE_CENTER )
        {
            // is there a TILE_CENTER on both the diagonals between the left and front?
            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    // remove this patch
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    // change the two affected patches to corner patches
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z );
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z-1 );
                    continue;
                }
            }
            continue;
        }

        if( SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] == TILE_CENTER )
        {
            // is there a TILE_CENTER on both the diagonals between the right and front?
            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    // remove this patch
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    // change the two affected patches to corner patches
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z );
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z-1 );
                    continue;
                }
            }
            continue;
        }

        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT  ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_BACK  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK  ] == TILE_CENTER )
        {
            // is there a TILE_CENTER on both the diagonals between the left and back?
            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    // remove this patch
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    // change the two affected patches to corner patches
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPLEFT, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z );
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTRIGHT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z+1 );
                    continue;
                }
            }
            continue;
        }

        if( SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] == TILE_CENTER &&
            SourceList->Edges[ i ].TextureStyles[ PATCH_BACK  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK  ] == TILE_CENTER )
        {
            // is there a TILE_CENTER on both the diagonals between the right and back?
            MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X+1, SourceList->Patches[ i ].Z-1, &Style, &Tile );
            if( Style == TextureStyle && Tile == TILE_CENTER )
            {
                MeshGetPatchInfo( Mesh, TileSet, TextureStyle, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z+1, &Style, &Tile );
                if( Style == TextureStyle && Tile == TILE_CENTER )
                {
                    // remove this patch
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
                    // change the two affected patches to corner patches
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERTOPRIGHT, SourceList->Patches[ i ].X-1, SourceList->Patches[ i ].Z );
                    MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CORNERBOTLEFT, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z+1 );
                    continue;
                }
            }
            continue;
        }
    }
}



//-----------------------------------------------------------------------------
// Name: MeshChangeEdgePatchesRemoveSingles()
// Desc: 
//-----------------------------------------------------------------------------

static void MeshChangeEdgePatchesRemoveSingles( Mesh_t *Mesh, TileSet_t *TileSet, size_t TextureStyle, PatchList_t *SourceList )
{
    size_t i, Count;

    // the previous call to MeshChangeEdgePatchesRemovePatch sometimes leaves
    // patches behind that have only one edge that's a joiner.  here we'll remove
    // these patches.

    for( i = 0; i < SourceList->Count; i++ )
    {
        Count = 0;
        if( SourceList->Edges[ i ].TextureStyles[ PATCH_LEFT  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_LEFT  ] != TILE_CENTER )
            Count++;

        if( SourceList->Edges[ i ].TextureStyles[ PATCH_RIGHT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_RIGHT ] != TILE_CENTER )
            Count++;

        if( SourceList->Edges[ i ].TextureStyles[ PATCH_FRONT ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_FRONT ] != TILE_CENTER )
            Count++;

        if( SourceList->Edges[ i ].TextureStyles[ PATCH_BACK  ] == TextureStyle && SourceList->Edges[ i ].TileType[ PATCH_BACK  ] != TILE_CENTER )
            Count++;

        if( Count == 1 )
            // remove this patch
            MeshSetPatchTexture( Mesh, TileSet, TextureStyle, TILE_CENTER, SourceList->Patches[ i ].X, SourceList->Patches[ i ].Z );
    }
}

