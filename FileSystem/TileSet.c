//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 25 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>

#include "TileSet.h"
#include "TextureManager.h"

//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static const char *Filenames[] =
{
    "TopLeft",                  // TILE_TOPLEFT
    "Top",                      // TILE_TOP
    "TopRight",                 // TILE_TOPRIGHT
    "Left",                     // TILE_LEFT
    "Center",                   // TILE_CENTER
    "Right",                    // TILE_RIGHT
    "BottomLeft",               // TILE_BOTTOMLEFT
    "Bottom",                   // TILE_BOTTOM
    "BottomRight",              // TILE_BOTTOMRIGHT
    "CornerTopLeft",            // TILE_CORNERTOPLEFT
    "CornerTopRight",           // TILE_CORNERTOPRIGHT
    "CornerBottomLeft",         // TILE_CORNERBOTLEFT
    "CornerBottomRight",        // TILE_CORNERBOTRIGHT
    "JoinedTLBR",               // TILE_JOINEDTLBR
    "JoinedBLTR",               // TILE_JOINEDBLTR
    nullptr
};


//-----------------------------------------------------------------------------
// Name: TileSetLoad()
// Desc: 
//-----------------------------------------------------------------------------

TileSet_t *TileSetLoad( const std::string& DirectoryName )
{
    TileSet_t *TileSet;
    size_t Tile, StyleDone, VariationDone;
    size_t TexNum, Found, FoundTileCount;
    TileVariation_t *TileVariation;

    TileSet = new TileSet_t();
    memset( TileSet, 0, sizeof( TileSet_t ) );

    StyleDone = 0;
    TileSet->StyleCount = 0;
    while( TileSet->StyleCount < TILE_MAXSTYLES && !StyleDone )
    {
        FoundTileCount = 0;
        for( Tile = 0; Tile < TILE_MAXTILES; Tile++ )
        {
            TileVariation = (TileVariation_t *)&TileSet->Styles[ TileSet->StyleCount ].Tiles[ Tile ];

            VariationDone = 0;
            Found = 0;
            TileVariation->TexCount = 0;

            while( TileVariation->TexCount < TILE_MAXVARIATIONS && !VariationDone )
            {
                std::ostringstream oss;
                oss << DirectoryName << "/Land" << TileSet->StyleCount << Filenames[Tile] << TileVariation->TexCount << ".tga";

                TexNum = CreateTexture(oss.str(), nullptr, nullptr, 1, 1 );
                if( -1 == TexNum )
                {
                    VariationDone = 1;
                }
                else
                {
                    TileVariation->Textures[ TileVariation->TexCount++ ] = TexNum;
                    Found = 1;
                }
            }

            if( Found )
                FoundTileCount++;
        }

        if( 0 == FoundTileCount )
            StyleDone = 1;
        else
            TileSet->StyleCount++;
    }

    for( Tile = 0; Tile < TILE_MAXWATERTEXS; Tile++ )
    {
        std::ostringstream oss;
        oss << DirectoryName << "/Water" << Tile << ".tga";
        TileSet->WaterTextures[ Tile ] = CreateTexture(oss.str(), nullptr, nullptr, 1, 1 );
    }

    return TileSet;
}


//-----------------------------------------------------------------------------
// Name: TileSetUnload()
// Desc: 
//-----------------------------------------------------------------------------

void TileSetUnload( TileSet_t *TileSet )
{
    size_t Style, Tile;
    TileVariation_t *TileVariation;

    DeleteTextures( TILE_MAXWATERTEXS, TileSet->WaterTextures );

    for( Style = 0; Style < TileSet->StyleCount; Style++ )
    {
        for( Tile = 0; Tile < TILE_MAXTILES; Tile++ )
        {
            TileVariation = (TileVariation_t *)&TileSet->Styles[ Style ].Tiles[ Tile ];
            DeleteTextures( TileVariation->TexCount, TileVariation->Textures );
        }
    }

    delete TileSet;
}


//-----------------------------------------------------------------------------
// Name: TileSetEnumToString()
// Desc: 
//-----------------------------------------------------------------------------

std::string TileSetEnumToString( TileSetType_t Enum )
{
    switch( Enum )
    {
        case TILESET_DIRT :
            return "Dirt";

        case TILESET_GRASS :
            return "Grass";

        case TILESET_MUD :
            return "Mud";

        case TILESET_ROCK :
            return "Rock";

        case TILESET_SAND :
            return "Sand";

        case TILESET_SNOW :
            return "Snow";

        case TILESET_STONE :
            return "Stone";
    }

    return "Random";
}
