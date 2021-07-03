//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include <gl/gl.h>

#include "TextureManager.h"
#include "Hardware.h"
#include "Log.h"
#include "DialogStartupProgress.h"
#include "Timer.h"

#define UPDATE_DELTA_MS       100


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern HDC g_hDC;
extern HardwareConfig_t g_HWConfig;

static LONGLONG _LastPaintTime;

size_t TextureGame     = 0;
size_t TextureDirt     = 0;
size_t TextureGrass    = 0;
size_t TextureMud      = 0;
size_t TextureRock     = 0;
size_t TextureSand     = 0;
size_t TextureSnow     = 0;
size_t TextureStone    = 0;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _Texture;


//----------------------------------------------------------------------------------------
// Name: CreateLoadingScreen()
// Desc:
//----------------------------------------------------------------------------------------

void CreateLoadingScreen(void)
{
    LogWrite("CreateLoadingScreen() loading textures...\n");

    TextureGame     = CreateTexture("BMP/LoadingGame.bmp", nullptr, nullptr, 1, 1);
    TextureDirt     = CreateTexture("BMP/LoadingDirt.bmp", nullptr, nullptr, 1, 1);
    TextureGrass    = CreateTexture("BMP/LoadingGrass.bmp", nullptr, nullptr, 1, 1);
    TextureMud      = CreateTexture("BMP/LoadingMud.bmp", nullptr, nullptr, 1, 1);
    TextureRock     = CreateTexture("BMP/LoadingRock.bmp", nullptr, nullptr, 1, 1);
    TextureSand     = CreateTexture("BMP/LoadingSand.bmp", nullptr, nullptr, 1, 1);
    TextureSnow     = CreateTexture("BMP/LoadingSnow.bmp", nullptr, nullptr, 1, 1);
    TextureStone    = CreateTexture("BMP/LoadingStone.bmp", nullptr, nullptr, 1, 1);

    LogWrite("CreateLoadingScreen() loaded\n");
}


//----------------------------------------------------------------------------------------
// Name: DeleteLoadingScreen()
// Desc:
//----------------------------------------------------------------------------------------

void DeleteLoadingScreen(void)
{
    LogWrite("DeleteLoadingScreen() unloading textures...\n");

    DeleteTexture(TextureStone);
    DeleteTexture(TextureSnow);
    DeleteTexture(TextureSand);
    DeleteTexture(TextureRock);
    DeleteTexture(TextureMud);
    DeleteTexture(TextureGrass);
    DeleteTexture(TextureGame);
    DeleteTexture(TextureDirt);

    LogWrite("DeleteLoadingScreen() loaded\n");
}


//----------------------------------------------------------------------------------------
// Name: DrawBackgroundImage()
// Desc:
//----------------------------------------------------------------------------------------

static void DrawBackgroundImage(GLuint Texture)
{
    glColor3f(1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, Texture);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, (float)g_HWConfig.GfxHeight);
    glTexCoord2f(1.0f, 0.0f); glVertex2f((float)g_HWConfig.GfxWidth, (float)g_HWConfig.GfxHeight);
    glTexCoord2f(1.0f, 1.0f); glVertex2f((float)g_HWConfig.GfxWidth, 0.0f);

    glEnd();
}


//----------------------------------------------------------------------------------------
// Name: BeginLoadingScreen()
// Desc:
//----------------------------------------------------------------------------------------

void BeginLoadingScreen(size_t Texture, const std::string& String, size_t Min, size_t Max)
{
    if (nullptr == DialogStartupProgress)
        return;

    glDisable(GL_DEPTH_TEST);

    _Texture = Texture;

    DrawBackgroundImage(GetTextureNumber(_Texture));
    DialogShow(DialogStartupProgress, 0);

    // update the progress bar
    DialogStartupProgress->Children[1]->Name = String;
    DialogSend(DialogStartupProgress->Children[0], PBM_SETRANGE, (void*)Min, (void*)Max);

    // draw it
    DialogSend(DialogStartupProgress, WM_PAINT, 0, 0);

    glFinish();
    SwapBuffers(g_hDC);

    glEnable(GL_DEPTH_TEST);

    _LastPaintTime = GetNowMs();
}


//----------------------------------------------------------------------------------------
// Name: EndLoadingScreen()
// Desc:
//----------------------------------------------------------------------------------------

void EndLoadingScreen(const std::string& String, size_t Value)
{
    if (nullptr == DialogStartupProgress)
        return;

    glDisable(GL_DEPTH_TEST);

    DrawBackgroundImage(GetTextureNumber(_Texture));

    // update the progress bar
    DialogStartupProgress->Children[1]->Name = String;
    DialogSend(DialogStartupProgress->Children[0], PBM_SETPOS, (void*)Value, 0);

    // draw it
    DialogSend(DialogStartupProgress, WM_PAINT, 0, 0);

    glFinish();
    SwapBuffers(g_hDC);

    glEnable(GL_DEPTH_TEST);
}


//----------------------------------------------------------------------------------------
// Name: UpdateLoadingScreen()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateLoadingScreen(size_t Value)
{
    if (nullptr == DialogStartupProgress)
        return;

    LONGLONG nowMs = GetNowMs();
    if (nowMs - _LastPaintTime < UPDATE_DELTA_MS)
        return;

    glDisable(GL_DEPTH_TEST);

    DrawBackgroundImage(GetTextureNumber(_Texture));

    // update the progress bar
    DialogSend(DialogStartupProgress->Children[ 0 ], PBM_SETPOS, (void*)Value, 0);

    // draw it
    DialogSend(DialogStartupProgress, WM_PAINT, 0, 0);

    glFinish();
    SwapBuffers(g_hDC);

    glEnable(GL_DEPTH_TEST);

    _LastPaintTime = nowMs;
}
