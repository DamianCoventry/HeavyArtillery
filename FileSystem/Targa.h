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
// defines
//------------------------------------------------------------------------------

#ifndef _TARGA_H_
#define _TARGA_H_

#include <string>


//------------------------------------------------------------------------------
// public structures
//------------------------------------------------------------------------------

#pragma pack( push, 1 )
typedef struct TargaHeader_s
{
    unsigned char IDLength;
    unsigned char ColorMapType;
    unsigned char ImageType;
    unsigned short CMapStart;
    unsigned short CMapLength;
    unsigned char CMapDepth;
    unsigned short XOffset;
    unsigned short YOffset;
    unsigned short Width;
    unsigned short Height;
    unsigned char PixelDepth;
    unsigned char ImageDescriptor;
}
TargaHeader_t;
#pragma pack( pop )


typedef struct Targa_s
{
    TargaHeader_t Header;
    unsigned char *Pixels;
}
Targa_t;


//------------------------------------------------------------------------------
// public functions
//------------------------------------------------------------------------------

Targa_t *TargaLoad( const std::string& FileName );
size_t TargaSave( const std::string& FileName, size_t Width, size_t Height, size_t BPP, unsigned char *Pixels );
void TargaUnload( Targa_t *Targa );


#endif
