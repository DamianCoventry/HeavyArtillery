//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 5 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "Font.h"
#include "Log.h"
#include "DialogScheme.h" // For Color_s

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/GL.h>

#include <stdio.h>
#include <stdlib.h>

#define FONT_MAX            64
#define FONT_MINHEIGHT      64
#define FONT_MAXINAROW      16
#define FONT_CELLSIZE       32
#define FONT_FIRSTCHAR      32

#define FONT_TOTALWIDTH     512
#define FONT_TOTALHEIGHT    256


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static long FontCreateTexture(Font_t* Font, long Width, long Height, unsigned char* Pixels);
static long FontAlpha(long Width, long Height, unsigned char* Pixels, unsigned char** NewBuffer);


//-----------------------------------------------------------------------------
// Name: FontCreate()
// Desc:
//-----------------------------------------------------------------------------

Font_t* FontCreate(const std::string& Face, long Height, long Style, HDC hDC, long* TextureNumber)
{
    HFONT hFont, hOldFont;
    TEXTMETRIC TextMetrics;
    BITMAPINFO BitmapInfo;
    HBITMAP hBitmap, hOldBitmap;
    HDC hMemoryDC;
    RECT Rect;
    long i, X, Y;
    unsigned char* Pixels;
    Font_t* Font;

    LogWrite("FontCreate() creating [%s]\n", Face.c_str());

    Font = new Font_t();

    // create the Font
    hFont = CreateFont(-MulDiv(Height, GetDeviceCaps(hDC, LOGPIXELSY), 72)
                       , 0, 0, 0, Style, FALSE, FALSE, FALSE, ANSI_CHARSET
                       , OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                       , DEFAULT_PITCH | FF_DONTCARE, Face.c_str());
    if (nullptr == hFont)
    {
        delete Font;
        return nullptr;
    }

    hOldFont = (HFONT)SelectObject(hDC, hFont);
    GetTextMetrics(hDC, &TextMetrics);

    Font->Height        = TextMetrics.tmHeight;
    Font->CellWidth     = FONT_CELLSIZE;
    Font->CellHeight    = FONT_CELLSIZE;
    Font->TexCoordX     = (float)Font->CellWidth  / FONT_TOTALWIDTH;
    Font->TexCoordY     = (float)Font->CellHeight / FONT_TOTALHEIGHT;

    if (!GetCharWidth32(hDC, FONT_FIRSTCHAR, FONT_MAXCHARS + FONT_FIRSTCHAR - 1, Font->AdvanceWidths))
    {
        if (!GetCharWidth(hDC, FONT_FIRSTCHAR, FONT_MAXCHARS + FONT_FIRSTCHAR - 1, Font->AdvanceWidths))
        {
            LogWrite("FontCreate() the call to GetCharWidth() failed, GetLastError()=%ld\n", GetLastError());

            delete Font;
            return nullptr;
        }
    }

    SelectObject(hDC, hOldFont);

    // create the dib section
    ZeroMemory(&BitmapInfo, sizeof(BITMAPINFO));
    BitmapInfo.bmiHeader.biSize            = sizeof(BITMAPINFO);
    BitmapInfo.bmiHeader.biWidth           = FONT_TOTALWIDTH;
    BitmapInfo.bmiHeader.biHeight          = FONT_TOTALHEIGHT;
    BitmapInfo.bmiHeader.biPlanes          = 1;
    BitmapInfo.bmiHeader.biBitCount        = 24;
    BitmapInfo.bmiHeader.biCompression     = BI_RGB;

    hBitmap = CreateDIBSection(hDC, &BitmapInfo, DIB_RGB_COLORS, (void**)&Pixels, nullptr, 0);
    if (nullptr == hBitmap)
    {
        LogWrite("FontCreate() the call to CreateDIBSection() failed, GetLastError()=%ld\n", GetLastError());
        delete Font;
        return nullptr;
    }

    // draw each char to the bitmap
    hMemoryDC   = CreateCompatibleDC(hDC);
    hOldBitmap  = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    Rect.left   = Rect.top = 0;
    Rect.right  = FONT_TOTALWIDTH;
    Rect.bottom = FONT_TOTALHEIGHT;
    FillRect(hMemoryDC, &Rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    hOldFont = (HFONT)SelectObject(hMemoryDC, hFont);
    SetBkColor(hMemoryDC, RGB(0, 0, 0));
    SetTextColor(hMemoryDC, RGB(255, 255, 255));
    SetTextAlign(hMemoryDC, TA_LEFT);

    X = 0; Y = 0;
    char Buffer[2];
    Buffer[1] = 0;
    for (i = 0; i < FONT_MAXCHARS; i++)
    {
        Buffer[0] = FONT_FIRSTCHAR + i;
        TextOut(hMemoryDC, X, Y, Buffer, 1);

        X += Font->CellWidth;
        if (X + Font->CellWidth > FONT_TOTALWIDTH)
        {
            X = 0;
            Y += Font->CellHeight;
        }
    }

    SelectObject(hMemoryDC, hOldFont);

    // create an opengl texture out of the Pixels
    FontCreateTexture(Font, FONT_TOTALWIDTH, FONT_TOTALHEIGHT, Pixels);

    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteObject(hFont);

    if (TextureNumber)
        (*TextureNumber) = Font->TextureNumber;

    LogWrite("FontCreate() created\n");

    return Font;
}


//-----------------------------------------------------------------------------
// Name: FontDelete()
// Desc:
//-----------------------------------------------------------------------------

void FontDelete(Font_t* Font)
{
    glDeleteTextures(1, &Font->TextureNumber);
    delete Font;
}


//-----------------------------------------------------------------------------
// Name: FontWrite()
// Desc:
//-----------------------------------------------------------------------------

void FontWrite(Font_t* Font, float X, float Y, const Color_s& color, const std::string& String)
{
    FontWrite(Font, X, Y, color.R, color.G, color.B, String);
}


//-----------------------------------------------------------------------------
// Name: FontWrite()
// Desc:
//-----------------------------------------------------------------------------

void FontWrite(Font_t* Font, float X, float Y, float R, float G, float B, const std::string& String)
{
    size_t CharIndex;
    long Index, Row, Col;
    float X1, X2;
    float Y1, Y2;

    glEnable(GL_ALPHA_TEST);
    glDepthFunc(GL_ALWAYS);

    glBindTexture(GL_TEXTURE_2D, Font->TextureNumber);
    glBegin(GL_QUADS);
    glColor4f(R, G, B, 1.0f);

    for (CharIndex = 0; CharIndex < String.length(); CharIndex++)
    {
        Index = String[ CharIndex ] - FONT_FIRSTCHAR;
        Row = Index / (FONT_TOTALWIDTH / Font->CellWidth);
        Col = Index % (FONT_TOTALWIDTH / Font->CellWidth);

        X1 = (float)Col * Font->TexCoordX;
        Y1 = 1.0f - (float)(Row + 1) * Font->TexCoordY;

        X2 = X1 + Font->TexCoordX;
        Y2 = Y1 + Font->TexCoordY;

        glTexCoord2f(X1, Y2); glVertex2f(X, Y);
        glTexCoord2f(X1, Y1); glVertex2f(X, Y + Font->CellHeight);
        glTexCoord2f(X2, Y1); glVertex2f(X + Font->CellWidth, Y + Font->CellHeight);
        glTexCoord2f(X2, Y2); glVertex2f(X + Font->CellWidth, Y);

        X += Font->AdvanceWidths[ Index ];
    }

    glEnd();

    glDepthFunc(GL_LESS);
    glDisable(GL_ALPHA_TEST);
}


//-----------------------------------------------------------------------------
// Name: FontPixelWidth()
// Desc:
//-----------------------------------------------------------------------------

size_t FontPixelWidth(Font_t* Font, const std::string& String, size_t Count)
{
    if (String.empty())
        return 0;

    size_t Len;
    if (Count == -1)
        Len = 999999;
    else
        Len = Count;

    size_t Total = 0;

    for (size_t i = 0; i < Len && String[i]; i++)
        Total += Font->AdvanceWidths[String[i] - FONT_FIRSTCHAR];

    return Total;
}


//-----------------------------------------------------------------------------
// Name: FontCharCount()
// Desc:
//-----------------------------------------------------------------------------

size_t FontCharCount(Font_t* Font, const std::string& String, size_t PixelWidth)
{
    if (String.empty())
        return 0;

    size_t Total = 0;
    size_t CharCount = 0;

    for (size_t i = 0; i < String.length(); i++)
    {
        if (Total + Font->AdvanceWidths[String[i] - FONT_FIRSTCHAR] > PixelWidth)
            break;

        ++CharCount;
        Total += Font->AdvanceWidths[String[i] - FONT_FIRSTCHAR];
    }

    return CharCount;
}


//-----------------------------------------------------------------------------
// Name: FontCreateTexture()
// Desc:
//-----------------------------------------------------------------------------

static long FontCreateTexture(Font_t* Font, long Width, long Height, unsigned char* Pixels)
{
    unsigned char* Buffer;

    FontAlpha(Width, Height, Pixels, &Buffer);

    glGenTextures(1, &Font->TextureNumber);
    glBindTexture(GL_TEXTURE_2D, Font->TextureNumber);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Buffer);

    delete [] Buffer;
    return 1;
}


//-----------------------------------------------------------------------------
// Name: FontAlpha()
// Desc:
//-----------------------------------------------------------------------------

static long FontAlpha(long Width, long Height, unsigned char* Pixels, unsigned char** NewBuffer)
{
    long X, Y, Offset;
    long TransRed, TransGreen, TransBlue;

    (*NewBuffer) = new unsigned char[Width * Height * 4];
    if (nullptr == (*NewBuffer))
        return 0;

    TransRed   = Pixels[ 0 ];
    TransGreen = Pixels[ 1 ];
    TransBlue  = Pixels[ 2 ];

    for (Y = 0; Y < Height; Y++)
    {
        for (X = 0; X < Width; X++)
        {
            Offset = Y * Width + X;

            (*NewBuffer)[(Offset * 4) + 0 ] = Pixels[(Offset * 3) + 0 ];
            (*NewBuffer)[(Offset * 4) + 1 ] = Pixels[(Offset * 3) + 1 ];
            (*NewBuffer)[(Offset * 4) + 2 ] = Pixels[(Offset * 3) + 2 ];

            if (TransBlue  == Pixels[(Offset * 3) + 0 ] &&
                    TransGreen == Pixels[(Offset * 3) + 1 ] &&
                    TransRed   == Pixels[(Offset * 3) + 2 ])
                (*NewBuffer)[(Offset * 4) + 3 ] = (unsigned char)0;
            else
                (*NewBuffer)[(Offset * 4) + 3 ] = (unsigned char)0xFF;
        }
    }

    return 1;
}
