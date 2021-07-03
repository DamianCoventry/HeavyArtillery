//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <GL/GL.h>

#include "GameDecals.h"
#include "TextureManager.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

DecalTexture_t g_DecalBlastTiny;
DecalTexture_t g_DecalBlastSmall;
DecalTexture_t g_DecalBlastMedium;
DecalTexture_t g_DecalBlastLarge;
DecalTexture_t g_DecalBlastHuge;


//-----------------------------------------------------------------------------
// Name: CreateDecals()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateDecals( void )
{
    LogWrite( "CreateDecals() creating...\n" );

    g_DecalBlastTiny.TextureNumber    = CreateTexture( "TGA/DecalBlastSmall.tga", nullptr, nullptr, 1, 1 );
    g_DecalBlastTiny.Width            = 32.0f;
    g_DecalBlastTiny.Height           = 32.0f;
    g_DecalBlastTiny.HalfWidth        = g_DecalBlastSmall.Width / 2;
    g_DecalBlastTiny.HalfHeight       = g_DecalBlastSmall.Height / 2;

    g_DecalBlastSmall.TextureNumber   = CreateTexture( "TGA/DecalBlastSmall.tga", nullptr, nullptr, 1, 1 );
    g_DecalBlastSmall.Width           = 64.0f;
    g_DecalBlastSmall.Height          = 64.0f;
    g_DecalBlastSmall.HalfWidth       = g_DecalBlastSmall.Width / 2;
    g_DecalBlastSmall.HalfHeight      = g_DecalBlastSmall.Height / 2;

    g_DecalBlastMedium.TextureNumber  = CreateTexture( "TGA/DecalBlastMedium.tga", nullptr, nullptr, 1, 1 );
    g_DecalBlastMedium.Width          = 128.0f;
    g_DecalBlastMedium.Height         = 128.0f;
    g_DecalBlastMedium.HalfWidth      = g_DecalBlastMedium.Width / 2;
    g_DecalBlastMedium.HalfHeight     = g_DecalBlastMedium.Height / 2;

    g_DecalBlastLarge.TextureNumber   = CreateTexture( "TGA/DecalBlastLarge.tga", nullptr, nullptr, 1, 1 );
    g_DecalBlastLarge.Width           = 256.0f;
    g_DecalBlastLarge.Height          = 256.0f;
    g_DecalBlastLarge.HalfWidth       = g_DecalBlastLarge.Width / 2;
    g_DecalBlastLarge.HalfHeight      = g_DecalBlastLarge.Height / 2;

    g_DecalBlastHuge.TextureNumber    = CreateTexture( "TGA/DecalBlastHuge.tga", nullptr, nullptr, 1, 1 );
    g_DecalBlastHuge.Width            = 512.0f;
    g_DecalBlastHuge.Height           = 512.0f;
    g_DecalBlastHuge.HalfWidth        = g_DecalBlastHuge.Width / 2;
    g_DecalBlastHuge.HalfHeight       = g_DecalBlastHuge.Height / 2;

    LogWrite( "CreateDecals() created\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteDecals()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteDecals( void )
{
    LogWrite( "DeleteDecals() deleting...\n" );

    DeleteTexture( g_DecalBlastSmall.TextureNumber );
    DeleteTexture( g_DecalBlastMedium.TextureNumber );
    DeleteTexture( g_DecalBlastLarge.TextureNumber );
    DeleteTexture( g_DecalBlastHuge.TextureNumber );

    LogWrite( "DeleteDecals() deleted\n" );
}
