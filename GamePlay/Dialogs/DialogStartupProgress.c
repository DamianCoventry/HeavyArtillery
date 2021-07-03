//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 9 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogStartupProgress.h"
#include "DialogProcDefault.h"
#include "DialogProcLabel.h"
#include "DialogProcProgress.h"
#include "Hardware.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogStartupProgress;
extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcStartupProgress()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcStartupProgress(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        Dialog->X = (float)g_HWConfig.GfxWidth  - Dialog->W - 8.0f;
        Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - 8.0f;
        return DIALOG_MSGHANDLED;

    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogStartupProgressCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogStartupProgressCreate(void)
{
    const float progressH = 25.0f;
    const float labelH = 30.0f;

    const float dialogW = 450.0f;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        progressH + 8.0f +
        labelH + 8.0f;

    DialogStartupProgress = DialogCreate("Loading...", DIALOG_MODAL | DIALOG_TITLE,
                                         (float)g_HWConfig.GfxWidth - dialogW - 8.0f, (float)g_HWConfig.GfxHeight - dialogH - 8.0f,
                                         dialogW, dialogH, nullptr, DialogProcStartupProgress, 0, 0);

    DialogCreate("Progress", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, dialogW - DIALOG_PADDINGx2, progressH, DialogStartupProgress, DialogProcProgress, 0, 0);

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + progressH + DIALOG_PADDING, dialogW - DIALOG_PADDINGx2, labelH, DialogStartupProgress, DialogProcLabel, 0, 0);

    return 1;
}
