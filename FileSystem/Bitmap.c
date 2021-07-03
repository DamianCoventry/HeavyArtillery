//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 1 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Bitmap.h"

#define BITMAP_ID       0x4d42



//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t BitmapFlip( Bitmap_t *Bitmap );


//-----------------------------------------------------------------------------
// Name: BitmapLoad()
// Desc: 
//-----------------------------------------------------------------------------

Bitmap_t *BitmapLoad( const std::string& FileName )
{
    FILE *File;
    long Size;
    Bitmap_t *Bitmap;
    errno_t Error;

    Error = fopen_s( &File, FileName.c_str(), "rb" );
    if( Error != 0 )
        return nullptr;

    Bitmap = new Bitmap_t;
    memset( Bitmap, 0, sizeof( Bitmap_t ) );

    fread( &Bitmap->FileHeader, sizeof( BITMAPFILEHEADER ), 1, File );
    fread( &Bitmap->InfoHeader, sizeof( BITMAPINFOHEADER ), 1, File );

    // check it's a bitmap File, is not compressed and is 24 bit color
    if( BITMAP_ID   != Bitmap->FileHeader.bfType ||
        BI_RGB      != Bitmap->InfoHeader.biCompression ||
        24          != Bitmap->InfoHeader.biBitCount )
    {
        BitmapUnload( Bitmap );
        fclose( File );
        return nullptr;
    }

    // read the rgb data - check if the bitmap's upside down first
    if( Bitmap->InfoHeader.biHeight < 0 )
        Size = Bitmap->InfoHeader.biWidth * -Bitmap->InfoHeader.biHeight * ( Bitmap->InfoHeader.biBitCount >> 3 );
    else
        Size = Bitmap->InfoHeader.biWidth * Bitmap->InfoHeader.biHeight * ( Bitmap->InfoHeader.biBitCount >> 3 );

    fseek( File, -Size, SEEK_END );

    Bitmap->Pixels = new unsigned char[Size];
    if( nullptr == Bitmap->Pixels )
    {
        BitmapUnload( Bitmap );
        fclose( File );
        return nullptr;
    }

    fread( Bitmap->Pixels, sizeof( char ), Size, File );

    // flip the bitmap's rgb values if necessary
    if( Bitmap->InfoHeader.biHeight < 0 )
    {
        if( !BitmapFlip( Bitmap ) )
        {
            BitmapUnload( Bitmap );
            fclose( File );
            return 0;
        }
    }

    fclose( File );
    return Bitmap;
}


//-----------------------------------------------------------------------------
// Name: BitmapLoadGreyScale()
// Desc: 
//-----------------------------------------------------------------------------

Bitmap_t *BitmapLoadGreyScale( const std::string& FileName )
{
    Bitmap_t *Bitmap;
    unsigned char *Pixels;
    size_t BitmapIndex;
    size_t GrayIndex;

    Bitmap = BitmapLoad( FileName );
    if( nullptr == Bitmap )
        return nullptr;

    Pixels = new unsigned char[Bitmap->InfoHeader.biWidth * Bitmap->InfoHeader.biHeight];
    if( nullptr == Pixels )
    {
        BitmapUnload( Bitmap );
        return nullptr;
    }

    GrayIndex = 0;
    for( BitmapIndex = 0; BitmapIndex < (size_t)Bitmap->InfoHeader.biSize; BitmapIndex += 3 )
    {
        Pixels[ GrayIndex ] = Bitmap->Pixels[ BitmapIndex ];
        GrayIndex++;
    }

    delete [] Bitmap->Pixels;
    Bitmap->Pixels = Pixels;

    Bitmap->InfoHeader.biBitCount = 8;
    Bitmap->InfoHeader.biSize /= 3;

    return Bitmap;
}


//-----------------------------------------------------------------------------
// Name: BitmapUnload()
// Desc: 
//-----------------------------------------------------------------------------

void BitmapUnload( Bitmap_t *Bitmap )
{
    if( Bitmap )
    {
        if( Bitmap->Pixels )
            delete [] Bitmap->Pixels;

        delete Bitmap;
    }
}


//-----------------------------------------------------------------------------
// Name: BitmapLoad()
// Desc: 
//-----------------------------------------------------------------------------

static size_t BitmapFlip( Bitmap_t *Bitmap )
{
    unsigned char *TempPixels;
    long BytesWide;
    size_t Height;
    size_t RowIndex;

    // calc how many bytes per scanline
    BytesWide = Bitmap->InfoHeader.biWidth * ( Bitmap->InfoHeader.biBitCount >> 3 );

    // check if the bitmap's upside down
    if( Bitmap->InfoHeader.biHeight < 0 )
    {
        TempPixels = new unsigned char[-Bitmap->InfoHeader.biHeight * BytesWide];
        Height = -Bitmap->InfoHeader.biHeight;
    }
    else
    {
        TempPixels = new unsigned char[Bitmap->InfoHeader.biHeight * BytesWide];
        Height = Bitmap->InfoHeader.biHeight;
    }

    // copy each line
    for( RowIndex = 0; RowIndex < Height; RowIndex++ )
        memcpy( &TempPixels[ ( ( Height - 1 ) - RowIndex ) * BytesWide ]
              , &Bitmap->Pixels[ RowIndex * BytesWide ]
              , BytesWide );

    // store the final pointer
    delete [] Bitmap->Pixels;
    Bitmap->Pixels = TempPixels;

    return 1;
}


//-----------------------------------------------------------------------------
// Name: BitmapSetAlpha()
// Desc: converts a 24 bit bitmap to a 32 bit bitmap.  sets the alpha values to
// 0xFF except for all Pixels that are the same color as the top left Pixel.
//------------------------------------------------------------------------------

size_t BitmapSetAlpha( Bitmap_t *Bitmap )
{
    unsigned char *TempPixels;
    size_t X, Y, W, H;
    size_t Offset;
    size_t TransRed, TransGreen, TransBlue;

    W = Bitmap->InfoHeader.biWidth;
    H = Bitmap->InfoHeader.biHeight;

    TempPixels = new unsigned char[W * H * 4];

    TransBlue  = Bitmap->Pixels[ 0 ];      // note this is BGR not RGB
    TransGreen = Bitmap->Pixels[ 1 ];
    TransRed   = Bitmap->Pixels[ 2 ];

    for( Y = 0; Y < H; Y++ )
    {
        for( X = 0; X < W; X++ )
        {
            Offset = Y * W + X;

            TempPixels[ ( Offset * 4 ) + 0 ] = Bitmap->Pixels[ ( Offset * 3 ) + 0 ];
            TempPixels[ ( Offset * 4 ) + 1 ] = Bitmap->Pixels[ ( Offset * 3 ) + 1 ];
            TempPixels[ ( Offset * 4 ) + 2 ] = Bitmap->Pixels[ ( Offset * 3 ) + 2 ];

            if( TransBlue  == Bitmap->Pixels[ ( Offset * 3 ) + 0 ] &&
                TransGreen == Bitmap->Pixels[ ( Offset * 3 ) + 1 ] &&
                TransRed   == Bitmap->Pixels[ ( Offset * 3 ) + 2 ] )
                TempPixels[ ( Offset * 4 ) + 3 ] = (unsigned char)0;
            else
                TempPixels[ ( Offset * 4 ) + 3 ] = (unsigned char)0xFF;
        }
    }

    delete [] Bitmap->Pixels;
    Bitmap->Pixels = TempPixels;

    return 1;
}

size_t BitmapSave( const std::string& FileName, size_t Width, size_t Height, size_t BPP, unsigned char *Pixels )
{
    FILE *File;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFO Info;
    errno_t Error;

    if( FileName.empty() || nullptr == Pixels )
        return 0;

    Error = fopen_s( &File, FileName.c_str(), "wb" );
    if( Error != 0 )
        return 0;

    ZeroMemory( &FileHeader, sizeof( BITMAPFILEHEADER ) );
    FileHeader.bfType    = BITMAP_ID;
    FileHeader.bfSize    = static_cast<DWORD>(sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFO ) + ( Width * Height * ( BPP >> 3 ) ));
    FileHeader.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFO );

    ZeroMemory( &Info, sizeof( BITMAPINFO ) );
    Info.bmiHeader.biSize           = sizeof( BITMAPINFOHEADER );
    Info.bmiHeader.biWidth          = static_cast<LONG>(Width);
    Info.bmiHeader.biHeight         = static_cast<LONG>(Height);
    Info.bmiHeader.biPlanes         = 1;
    Info.bmiHeader.biBitCount       = static_cast<WORD>(BPP);
    Info.bmiHeader.biCompression    = BI_RGB;

    fwrite( &FileHeader, sizeof( BITMAPFILEHEADER ), 1, File );
    fwrite( &Info, sizeof( BITMAPINFO ), 1, File );
    fwrite( Pixels, sizeof( unsigned char ), Width * Height * ( BPP >> 3 ), File );

    fclose( File );

    return 1;
}
