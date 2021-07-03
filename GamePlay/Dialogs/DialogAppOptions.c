//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 7 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"
#include "DialogProcCheck.h"
#include "DialogProcList.h"

#include "Hardware.h"
#include "Graphics.h"

#include "MeshDraw.h"
#include "ObjectManager.h"
#include "GameConfig.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogAppOptions;
extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// private message handlers
//-----------------------------------------------------------------------------

static size_t DialogProcAppOptions(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnGraphicsModeSelect(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickApply(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickFullscreen(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickClose(Dialog_t* Dialog, void* Param1, void* Param2);

static void FillListBoxWithDisplayModes(Dialog_t* ListBox);


//-----------------------------------------------------------------------------
// Name: DialogAppOptionsCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogAppOptionsCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float checkBoxW = MESSAGEBOX_BUTTONWIDTH * 2;

    const float listBoxModesW = 250;
    const float listBoxModesH = 200;

    const float listBoxSettingsW = 175;
    const float listBoxSettingsH = 100;

    const float dialogW =
        DIALOG_PADDING + listBoxModesW + DIALOG_PADDING + checkBoxW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDINGx2 +
        listBoxModesH + DIALOG_PADDINGx2 +
        listBoxSettingsH + DIALOG_PADDINGx2 +
        listBoxSettingsH + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING;

    DialogAppOptions = DialogCreate("Options", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                    0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcAppOptions, 0, 0);

    float controlY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Display Modes:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, buttonW, buttonH, DialogAppOptions, DialogProcLabel, 0, 0); controlY += buttonH;

    const float listBoxY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + buttonH;

    DialogCreate("Display Modes", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, listBoxY, listBoxModesW, listBoxModesH, DialogAppOptions, DialogProcList, 0, 0);

    DialogCreate("Apply", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 DIALOG_PADDING, listBoxY + listBoxModesH + DIALOG_PADDING, buttonW, buttonH, DialogAppOptions, DialogProcButton, 0, 0);

    DialogCreate("Fullscreen Graphics", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING + listBoxModesW + DIALOG_PADDING, controlY, checkBoxW, buttonH, DialogAppOptions, DialogProcCheck, 0, 0); controlY += buttonH;

    DialogCreate("Enable object shading.", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING + listBoxModesW + DIALOG_PADDING, controlY, checkBoxW, buttonH, DialogAppOptions, DialogProcCheck, 0, 0);

    controlY = listBoxY + listBoxModesH + DIALOG_PADDING + buttonH + DIALOG_PADDING;

    DialogCreate("LOD Distance", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, buttonW, buttonH, DialogAppOptions, DialogProcLabel, 0, 0);

    DialogCreate("Object LOD", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDINGx2 + buttonW, controlY, listBoxSettingsW, listBoxSettingsH, DialogAppOptions, DialogProcList, 0, 0); controlY += listBoxSettingsH + DIALOG_PADDING;

    DialogCreate("Particles", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, buttonW, buttonH, DialogAppOptions, DialogProcLabel, 0, 0);

    DialogCreate("Particles", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDINGx2 + buttonW, controlY, listBoxSettingsW, listBoxSettingsH, DialogAppOptions, DialogProcList, 0, 0);

    DialogCreate("Close", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogAppOptions, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcAppOptions()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcAppOptions(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 1 ])
            OnGraphicsModeSelect(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 2 ])
            OnClickApply(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 3 ])
            OnClickFullscreen(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 9 ])
            OnClickClose(Dialog, Param1, Param2);

        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: OnActivate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2)
{
    if (WA_ACTIVE == (size_t)Param1)
    {
        FillListBoxWithDisplayModes(Dialog->Children[ 1 ]);

        DialogEnable(Dialog->Children[ 2 ], 0);
        DialogEnable(Dialog->Children[ 3 ], 0);

        DialogSend(Dialog->Children[ 6 ], LB_RESETCONTENT, 0, 0);
        std::string Temp = "Small";
        DialogSend(Dialog->Children[ 6 ], LB_ADDSTRING, (void*)&Temp, 0);
        Temp = "Medium";
        DialogSend(Dialog->Children[ 6 ], LB_ADDSTRING, (void*)&Temp, 0);
        Temp = "Large";
        DialogSend(Dialog->Children[ 6 ], LB_ADDSTRING, (void*)&Temp, 0);
        DialogSend(Dialog->Children[ 6 ], LB_SETCURSEL, (void*)GetLODSetting(), 0);

        DialogSend(Dialog->Children[ 8 ], LB_RESETCONTENT, 0, 0);
        Temp = "Few";
        DialogSend(Dialog->Children[ 8 ], LB_ADDSTRING, (void*)&Temp, 0);
        Temp = "Average";
        DialogSend(Dialog->Children[ 8 ], LB_ADDSTRING, (void*)&Temp, 0);
        Temp = "Heaps";
        DialogSend(Dialog->Children[ 8 ], LB_ADDSTRING, (void*)&Temp, 0);
        DialogSend(Dialog->Children[ 8 ], LB_SETCURSEL, (void*)GetParticleSetting(), 0);

        ((CheckBox_t*)Dialog->Children[ 3 ]->Data)->Checked = g_HWConfig.GfxFullScreen;
        ((CheckBox_t*)Dialog->Children[ 4 ]->Data)->Checked = GetUseShading();
    }
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnGraphicsModeSelect()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnGraphicsModeSelect(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogSend(Dialog->Children[ 1 ], LB_GETCURSEL, 0, 0);
    if (-1 != DialogRetValue())
    {
        DialogEnable(Dialog->Children[ 2 ], 1);
        DialogEnable(Dialog->Children[ 3 ], 1);
    }
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickApply()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickApply(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DEVMODE Mode;
    size_t Index;

    memset(&Mode, 0, sizeof(DEVMODE));
    Mode.dmSize = sizeof(DEVMODE);

    DialogSend(Dialog->Children[ 1 ], LB_GETCURSEL, 0, 0);
    Index = DialogRetValue();

    DialogSend(Dialog->Children[ 1 ], LB_GETITEMDATA, (void*)Index, 0);
    EnumDisplaySettings(nullptr, static_cast<DWORD>(DialogRetValue()), &Mode);

    if (((CheckBox_t*)Dialog->Children[ 3 ]->Data)->Checked != g_HWConfig.GfxFullScreen)
    {
        SetWindowSize(Mode.dmPelsWidth, Mode.dmPelsHeight, Mode.dmBitsPerPel, Mode.dmDisplayFrequency);
        ToggleFullScreen();
    }
    else
        SetWindowSize(Mode.dmPelsWidth, Mode.dmPelsHeight, Mode.dmBitsPerPel, Mode.dmDisplayFrequency);

    DialogSetViewport(0.0f, 0.0f, (float)Mode.dmPelsWidth, (float)Mode.dmPelsHeight);
    DialogSend(DIALOG_BROADCAST, WM_DISPLAYCHANGE, 0, 0);
    DialogCenter(Dialog);

    DialogEnable(Dialog->Children[ 2 ], 0);
    DialogEnable(Dialog->Children[ 3 ], 0);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickFullscreen()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickFullscreen(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogSend(Dialog->Children[ 1 ], LB_GETCURSEL, 0, 0);
    if (-1 != DialogRetValue())
        DialogEnable(Dialog->Children[ 2 ], 1);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickClose()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickClose(Dialog_t* Dialog, void* Param1, void* Param2)
{
    SetUseShading(((CheckBox_t*)Dialog->Children[ 4 ]->Data)->Checked);

    DialogSend(Dialog->Children[ 6 ], LB_GETCURSEL, 0, 0);
    SetLODSetting(DialogRetValue());
    SetObjectLOD(GetLODSetting());
    SetMeshLOD(GetLODSetting());

    DialogSend(Dialog->Children[ 8 ], LB_GETCURSEL, 0, 0);
    SetParticleSetting(DialogRetValue());

    DialogHide(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: FillListBoxWithDisplayModes()
// Desc:
//-----------------------------------------------------------------------------

static void FillListBoxWithDisplayModes(Dialog_t* ListBox)
{
    DEVMODE Mode;
    size_t i;

    DialogSend(ListBox, LB_RESETCONTENT, 0, 0);

    memset(&Mode, 0, sizeof(DEVMODE));
    Mode.dmSize = sizeof(DEVMODE);

    for (i = 0; EnumDisplaySettings(nullptr, static_cast<DWORD>(i), &Mode); i++)
    {
        if ((Mode.dmBitsPerPel == 24 || Mode.dmBitsPerPel == 32) && Mode.dmPelsWidth >= 1024 && Mode.dmDisplayFrequency >= 50 && Mode.dmDisplayFrequency <= 75)
        {
            std::ostringstream oss;
            oss << Mode.dmPelsWidth << "x" << Mode.dmPelsHeight << ", " << Mode.dmDisplayFrequency << "Hz";
            auto text = oss.str();
            DialogSend(ListBox, LB_ADDSTRING, (void*)&text, 0);

            // store the index of this mode as the string's data
            DialogSend(ListBox, LB_GETCOUNT, 0, 0);
            DialogSend(ListBox, LB_SETITEMDATA, (void*)(DialogRetValue() - 1), (void*)i);
        }
    }

    DialogSend(ListBox, WM_VSCROLL, (void*)SB_TOP, 0);
}
