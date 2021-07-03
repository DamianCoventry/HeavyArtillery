//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 26 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <string>

#include "Bitmap.h"
#include "Targa.h"
#include "LoadingScreen.h"
#include "Error.h"


#define MAXTEXTURES             256

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXTEXTURES || !_TexturePool[ _id ].Used )


//----------------------------------------------------------------------------------------
// private structures
//----------------------------------------------------------------------------------------

typedef struct Texture_s
{
    size_t Used;
    size_t Width, Height, BPP;
    std::string FileName;
    GLuint TextureNumber;
}
Texture_t;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _TextureIndex = 0;
static Texture_t _TexturePool[ MAXTEXTURES ];


//----------------------------------------------------------------------------------------
// Name: GetFreeTexture()
// Desc:
//----------------------------------------------------------------------------------------

static size_t GetFreeTexture(void)
{
    size_t i;
    size_t ID;

    i = _TextureIndex;

    while (_TexturePool[ i ].Used)
    {
        i++;
        if (i >= MAXTEXTURES)
            i = 0;

        if (i == _TextureIndex)
            return -1;        // pool is full
    }

    _TexturePool[ i ].Used = 1;
    ID = i;

    _TextureIndex = i;
    if (++_TextureIndex >= MAXTEXTURES)
        _TextureIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnTexture()
// Desc:
//----------------------------------------------------------------------------------------

static void ReturnTexture(size_t ID)
{
    if (INVALIDID(ID))
        return;

    _TexturePool[ID].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: CreateTexture()
// Desc:
//----------------------------------------------------------------------------------------

size_t CreateTexture(const std::string& FileName, size_t* OutWidth, size_t* OutHeight, size_t Filter, size_t RepeatTex)
{
    Bitmap_t* Bitmap;
    Targa_t* Targa;
    unsigned char* Pixels;
    size_t ID;

    ID = GetFreeTexture();
    if (-1 == ID)
        return -1;

    _TexturePool[ ID ].TextureNumber = 0;
    Pixels = nullptr;
    Bitmap = nullptr;
    Targa = nullptr;

    if (FileName.find(".bmp") != std::string::npos)
    {
        Bitmap = BitmapLoad(FileName);
        if (Bitmap)
        {
            BitmapSetAlpha(Bitmap);
            _TexturePool[ ID ].Width  = Bitmap->InfoHeader.biWidth;
            _TexturePool[ ID ].Height = Bitmap->InfoHeader.biHeight;
            _TexturePool[ ID ].BPP    = 32;            // always 32 becoz we called BitmapSetAlpha()
            Pixels = Bitmap->Pixels;
        }
        else
        {
            ReturnTexture(ID);
            //SetError( "CreateTexture() failed to open/read the file [%s]", FileName );
            return -1;
        }
    }
    else
    {
        Targa = TargaLoad(FileName);
        if (Targa)
        {
            _TexturePool[ ID ].Width  = Targa->Header.Width;
            _TexturePool[ ID ].Height = Targa->Header.Height;
            _TexturePool[ ID ].BPP    = Targa->Header.PixelDepth;
            Pixels = Targa->Pixels;
        }
        else
        {
            ReturnTexture(ID);
            //SetError( "CreateTexture() failed to open/read the file [%s]", FileName );
            return -1;
        }
    }

    if (Pixels)
    {
        glGenTextures(1, &_TexturePool[ ID ].TextureNumber);
        glBindTexture(GL_TEXTURE_2D, _TexturePool[ ID ].TextureNumber);

        /*if( RepeatTex )*/
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        /*else
        {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
        }*/

        if (Filter)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4,
                          static_cast<GLint>(_TexturePool[ ID ].Width),
                          static_cast<GLint>(_TexturePool[ ID ].Height),
                          _TexturePool[ ID ].BPP == 32 ? GL_BGRA_EXT : GL_BGR_EXT,
                          GL_UNSIGNED_BYTE, Pixels);

        if (OutWidth)
            (*OutWidth) = _TexturePool[ ID ].Width;

        if (OutHeight)
            (*OutHeight) = _TexturePool[ ID ].Height;

        if (Bitmap)
            BitmapUnload(Bitmap);

        if (Targa)
            TargaUnload(Targa);

        _TexturePool[ID].FileName = FileName;
    }

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: DeleteTexture()
// Desc:
//----------------------------------------------------------------------------------------

void DeleteTexture(size_t ID)
{
    if (INVALIDID(ID))
        return;

    glDeleteTextures(1, &_TexturePool[ ID ].TextureNumber);

    ReturnTexture(ID);
}


//----------------------------------------------------------------------------------------
// Name: DeleteTextures()
// Desc:
//----------------------------------------------------------------------------------------

void DeleteTextures(size_t Count, size_t* IDs)
{
    GLuint i;

    if (nullptr == IDs)
        return;

    for (i = 0; i < Count; i++)
        DeleteTexture(IDs[ i ]);
}


//----------------------------------------------------------------------------------------
// Name: ReloadTexture()
// Desc:
//----------------------------------------------------------------------------------------

size_t ReloadTexture(size_t ID)
{
    Bitmap_t* Bitmap = nullptr;
    Targa_t* Targa = nullptr;
    unsigned char* Pixels = nullptr;

    if (INVALIDID(ID))
        return -1;

    if (_TexturePool[ID].FileName.find(".bmp") != std::string::npos)
    {
        Bitmap = BitmapLoad(_TexturePool[ ID ].FileName);
        if (Bitmap)
        {
            BitmapSetAlpha(Bitmap);
            Pixels = Bitmap->Pixels;
        }
        else
        {
            DeleteTexture(ID);
            return -1;
        }
    }
    else
    {
        Targa = TargaLoad(_TexturePool[ ID ].FileName);
        if (Targa)
            Pixels = Targa->Pixels;
        else
        {
            DeleteTexture(ID);
            return -1;
        }
    }

    if (Pixels)
    {
        glBindTexture(GL_TEXTURE_2D, _TexturePool[ ID ].TextureNumber);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        gluBuild2DMipmaps(GL_TEXTURE_2D, 4,
                          static_cast<GLint>(_TexturePool[ ID ].Width),
                          static_cast<GLint>(_TexturePool[ ID ].Height),
                          _TexturePool[ ID ].BPP == 32 ? GL_BGRA_EXT : GL_BGR_EXT,
                          GL_UNSIGNED_BYTE, Pixels);

        if (Bitmap)
            BitmapUnload(Bitmap);

        if (Targa)
            TargaUnload(Targa);
    }

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReloadAllTextures()
// Desc:
//----------------------------------------------------------------------------------------

void ReloadAllTextures(void)
{
    size_t i, Last;

    BeginLoadingScreen(TextureGame, "Reloading Textures...", 0, MAXTEXTURES / 10);
    Last = 0;

    for (i = 0; i < MAXTEXTURES; i++)
    {
        if (_TexturePool[ i ].Used)
            ReloadTexture(i);

        if (i / (MAXTEXTURES / 10) > Last)
        {
            Last = i / (MAXTEXTURES / 10);
            UpdateLoadingScreen(Last);
        }
    }

    EndLoadingScreen("Reloading Textures...Done", MAXTEXTURES / 10);
}


//----------------------------------------------------------------------------------------
// Name: GetTextureNumber()
// Desc:
//----------------------------------------------------------------------------------------

GLuint GetTextureNumber(size_t ID)
{
    if (INVALIDID(ID))
        return 0;

    return _TexturePool[ ID ].TextureNumber;
}
