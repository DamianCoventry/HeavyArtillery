//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 25 April 2002
//-----------------------------------------------------------------------------


#ifndef _TILESET_H_
#define _TILESET_H_

#include <string>

#define TILE_MAXSTYLES      8
#define TILE_MAXVARIATIONS  4
#define TILE_MAXWATERTEXS   2


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef enum TileSetType_e
{
    TILESET_RANDOM  = 0,
    TILESET_DIRT    = 1,
    TILESET_GRASS   = 2,
    TILESET_MUD     = 3,
    TILESET_ROCK    = 4,
    TILESET_SAND    = 5,
    TILESET_SNOW    = 6,
    TILESET_STONE   = 7,
}
TileSetType_t;

typedef enum TilePiece_e
{
    TILE_TOPLEFT = 0,
    TILE_TOP,
    TILE_TOPRIGHT,
    TILE_LEFT,
    TILE_CENTER,
    TILE_RIGHT,
    TILE_BOTTOMLEFT,
    TILE_BOTTOM,
    TILE_BOTTOMRIGHT,
    TILE_CORNERTOPLEFT,
    TILE_CORNERTOPRIGHT,
    TILE_CORNERBOTLEFT,
    TILE_CORNERBOTRIGHT,
    TILE_JOINEDTLBR,
    TILE_JOINEDBLTR,
    TILE_MAXTILES
}
TilePiece_t;

typedef struct TileVariation_s
{
    size_t TexCount;
    size_t Textures[ TILE_MAXVARIATIONS ];
}
TileVariation_t;

typedef struct TileStyle_s
{
    TileVariation_t Tiles[ TILE_MAXTILES ];
}
TileStyle_t;

typedef struct TileSet_s
{
    size_t StyleCount;
    size_t WaterTextures[ TILE_MAXWATERTEXS ];
    TileStyle_t Styles[ TILE_MAXSTYLES ];
}
TileSet_t;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

TileSet_t *TileSetLoad( const std::string& DirectoryName );
void TileSetUnload( TileSet_t *TileSet );

std::string TileSetEnumToString( TileSetType_t Enum );


#endif
