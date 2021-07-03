//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 5 April 2002
//-----------------------------------------------------------------------------


#ifndef _FontH_
#define _FontH_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL\GL.h>
#include <string>

#define FONT_MAXCHARS       96


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

typedef struct Font_s
{
    Font_s()
    {
        CellWidth = CellHeight = 0;
        TexCoordX = TexCoordY = 0.0f;
        Height = 0;
        TextureNumber = 0;
        memset(AdvanceWidths, 0, sizeof(INT) * FONT_MAXCHARS);
    }

    long CellWidth, CellHeight;
    float TexCoordX, TexCoordY;
    long Height;
    GLuint TextureNumber;
    INT AdvanceWidths[ FONT_MAXCHARS ];
}
Font_t;

struct Color_s;

//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

Font_t* FontCreate(const std::string& Face, long Height, long Style, HDC hDC, long* TextureNumber);
void FontDelete(Font_t* Font);

void FontWrite(Font_t* Font, float X, float Y, const Color_s& color, const std::string& String);
void FontWrite(Font_t* Font, float X, float Y, float R, float G, float B, const std::string& String);

size_t FontPixelWidth(Font_t* Font, const std::string& String, size_t Count);
size_t FontCharCount(Font_t* Font, const std::string& String, size_t PixelWidth);


#endif
