//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 29 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogFlyBy.h"
#include "DialogProcDefault.h"
#include "DialogProcLabel.h"
#include "DialogProcProgress.h"
#include "Hardware.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogFlyBy;
extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// private message handlers
//-----------------------------------------------------------------------------

static size_t DialogProcFlyBy(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);


//-----------------------------------------------------------------------------
// Name: DialogFlyByCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogFlyByCreate(void)
{
    const float labelH = 30.0f;

    const float dialogW = 250.0f;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        labelH + DIALOG_PADDING;

    DialogFlyBy = DialogCreate("FlyBy Mode", DIALOG_MODAL | DIALOG_TITLE,
                               0.0f, 0.0f, dialogW, dialogH,
                               nullptr, DialogProcFlyBy, 0, 0);

    DialogCreate("Press Esc to activate menu", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, dialogW - DIALOG_PADDINGx2, labelH,
                 DialogFlyBy, DialogProcLabel, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcFlyBy()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcFlyBy(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: OnActivate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2)
{
    Dialog->X = (float)g_HWConfig.GfxWidth  - Dialog->W - DIALOG_PADDING;
    Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - DIALOG_PADDING;

    DialogShowCursor(0);

    return DIALOG_MSGHANDLED;
}
