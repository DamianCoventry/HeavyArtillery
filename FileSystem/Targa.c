//------------------------------------------------------------------------------
//
// Image Viewer
// Copyright (c) 2002
//
// Damian Coventry
// Tuesday, 21 May 2002
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Targa.h"


//------------------------------------------------------------------------------
// private enumerations
//------------------------------------------------------------------------------

typedef enum ImageType_e
{
    TGA_NOIMAGEDATA         = 0,
    TGA_INDEXEDCOLOR        = 1,
    TGA_TRUECOLOR           = 2,
    TGA_MONOCHROME          = 3,
    TGA_INDEXEDCOLOR_RLE    = 9,
    TGA_TRUECOLOR_RLE       = 10,
    TGA_MONOCHROME_RLE      = 11,
}
ImageType_t;


//------------------------------------------------------------------------------
// Name: TargaLoad()
// Desc: 
//------------------------------------------------------------------------------

Targa_t *TargaLoad( const std::string& FileName )
{
    Targa_t *Targa;
    char ImageID[ 256 ];
    FILE *File;
    errno_t Error;

    Targa = new Targa_t();
    if( nullptr == Targa )
        return nullptr;

    memset( Targa, 0, sizeof( Targa_t ) );

    Error = fopen_s( &File, FileName.c_str(), "rb" );
    if( Error != 0 )
    {
        delete Targa;
        return nullptr;
    }

    // get the image info
    fread( &Targa->Header, sizeof( TargaHeader_t ), 1, File );

    // currently only supporting true color non compressed images
    if( TGA_TRUECOLOR != Targa->Header.ImageType )
    {
        delete Targa;
        fclose( File );
        return nullptr;
    }

    // check for the optional image id field
    if( Targa->Header.IDLength > 0 )
    {
        fread( ImageID, sizeof( char ), Targa->Header.IDLength, File );
        ImageID[ Targa->Header.IDLength ] = 0;
    }

    // check for the optional palette
    if( Targa->Header.ColorMapType > 0 )
    {
        // currently we'll just seek past the palette
        fseek( File, Targa->Header.CMapLength * ( Targa->Header.CMapDepth >> 3 ), SEEK_CUR );
    }

    // read in the truecolor data
    Targa->Pixels = new unsigned char[Targa->Header.Width * Targa->Header.Height * ( Targa->Header.PixelDepth >> 3 )];
    if( nullptr == Targa->Pixels )
    {
        delete Targa;
        fclose( File );
        return nullptr;
    }

    fread( Targa->Pixels, sizeof( unsigned char ), Targa->Header.Width * Targa->Header.Height * ( Targa->Header.PixelDepth >> 3 ), File );

    fclose( File );
    return Targa;
}


//------------------------------------------------------------------------------
// Name: TargaUnload()
// Desc: 
//------------------------------------------------------------------------------

void TargaUnload( Targa_t *Targa )
{
    if( Targa )
    {
        if( Targa->Pixels )
            delete [] Targa->Pixels;

        delete Targa;
    }
}


//------------------------------------------------------------------------------
// Name: TargaSave()
// Desc: 
//------------------------------------------------------------------------------

size_t TargaSave( const std::string& FileName, size_t Width, size_t Height, size_t BPP, unsigned char *Pixels )
{
    TargaHeader_t Header;
    FILE *File;
    errno_t Error;

    memset( &Header, 0, sizeof( TargaHeader_t ) );
    Header.ImageType    = TGA_TRUECOLOR;
    Header.Width        = static_cast<unsigned short>(Width);
    Header.Height       = static_cast<unsigned short>(Height);
    Header.PixelDepth   = static_cast<unsigned char>(BPP);

    Error = fopen_s( &File, FileName.c_str(), "wb" );
    if( Error != 0 )
        return 0;

    fwrite( &Header, sizeof( TargaHeader_t ), 1, File );
    fwrite( Pixels, sizeof( unsigned char ), Width * Height * ( BPP >> 3 ), File );

    fclose( File );
    return 1;
}
