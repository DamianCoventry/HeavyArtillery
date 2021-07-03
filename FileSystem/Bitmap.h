//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 1 April 2002
//-----------------------------------------------------------------------------


#ifndef _BITMAP_H_
#define _BITMAP_H_


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct Bitmap_s
{
    BITMAPFILEHEADER FileHeader;    // note that 24 bit bitmaps are only supported
    BITMAPINFOHEADER InfoHeader;
    unsigned char *Pixels;
}
Bitmap_t;


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

Bitmap_t *BitmapLoad( const std::string& FileName );
Bitmap_t *BitmapLoadGreyScale( const std::string& FileName );

size_t BitmapSave( const std::string& FileName, size_t Width, size_t Height, size_t BPP, unsigned char *Pixels );

void BitmapUnload( Bitmap_t *Bitmap );

size_t BitmapSetAlpha( Bitmap_t *Bitmap );


#endif