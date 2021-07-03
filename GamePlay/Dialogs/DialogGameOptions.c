//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>

#include "DialogGameOptions.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLabel.h"
#include "DialogProcEdit.h"
#include "DialogProcCheck.h"
#include "DialogProcList.h"

#include "TileSet.h"
#include "Utility.h"

#include "GameTypes.h"
#include "GameConfig.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogGameOptions;


//-----------------------------------------------------------------------------
// private message handlers
//-----------------------------------------------------------------------------

static size_t DialogProcGameOptions(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickClose(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnSelectTileSet(Dialog_t* Dialog, void* Param1, void* Param2);


//-----------------------------------------------------------------------------
// Name: DialogGameOptionsCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogGameOptionsCreate(void)
{
    const float labelW = MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;

    const float editBoxW = MESSAGEBOX_BUTTONWIDTH * 2 + MESSAGEBOX_BUTTONWIDTHHALF;

    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = editBoxW;
    const float listBoxH = 300.0f;

    const float dialogW =
        labelW + DIALOG_PADDING +
        editBoxW + DIALOG_PADDINGx2;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING +
        buttonH + DIALOG_PADDING +
        listBoxH + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING;

    DialogGameOptions = DialogCreate("Game Options", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                     0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcGameOptions, 0, 0);

    float controlX = dialogW - editBoxW - DIALOG_PADDING;
    float controlY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Level Count:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, labelW, buttonH, DialogGameOptions, DialogProcLabel, 0, 0);

    DialogCreate("Levels", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 controlX, controlY, editBoxW, buttonH, DialogGameOptions, DialogProcEdit, 0, 0); controlY += buttonH + BUTTON_PADDING;

    DialogCreate("Starting cash:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, labelW, buttonH, DialogGameOptions, DialogProcLabel, 0, 0);

    DialogCreate("Cash", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 controlX, controlY, editBoxW, buttonH, DialogGameOptions, DialogProcEdit, 0, 0); controlY += buttonH + BUTTON_PADDING;

    DialogCreate("Landscape TileSet:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, controlY, labelW, buttonH, DialogGameOptions, DialogProcLabel, 0, 0);

    DialogCreate("TileSet", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 controlX, controlY, listBoxW, listBoxH, DialogGameOptions, DialogProcList, 0, 0);

    DialogCreate("OK", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogGameOptions, DialogProcButton, 0, 0);
#if DEMO
    controlY = dialogH - (2 * buttonH) - DIALOG_PADDING;
    DialogCreate("Buy the full version", DIALOG_CHILD | DIALOG_HIGHLIGHT,
                 BUTTON_PADDING, controlY, buttonW, buttonH, DialogGameOptions, DialogProcLabel, 0, 0); controlY += buttonH;

    DialogCreate("to get this tileset!", DIALOG_CHILD | DIALOG_HIGHLIGHT,
                 BUTTON_PADDING, controlY - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogGameOptions, DialogProcLabel, 0, 0);
#endif
    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcGameOptions()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcGameOptions(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 6 ])
            return OnClickClose(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 5 ])
            return OnSelectTileSet(Dialog, Param1, Param2);

        break;
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
        auto LevelCount = std::to_string(GetLevelCount());
        DialogSend(Dialog->Children[ 1 ], WM_SETTEXT, (void*)&LevelCount, 0);
        auto CashAtStart = std::to_string(GetCashAtStart());
        DialogSend(Dialog->Children[ 3 ], WM_SETTEXT, (void*)&CashAtStart, 0);

        DialogSend(Dialog->Children[ 5 ], LB_RESETCONTENT, 0, 0);
        std::string TileSet = TileSetEnumToString(TILESET_RANDOM);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_DIRT);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_GRASS);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_MUD);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_ROCK);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_SAND);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_SNOW);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        TileSet = TileSetEnumToString(TILESET_STONE);
        DialogSend(Dialog->Children[ 5 ], LB_ADDSTRING, (void*)&TileSet, 0);
        DialogSend(Dialog->Children[ 5 ], LB_SETCURSEL, (void*)GetTileSetType(), 0);
    }
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickClose()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickClose(Dialog_t* Dialog, void* Param1, void* Param2)
{
    SetLevelCount(std::stoi(((Edit_t*)Dialog->Children[1]->Data)->Text));
    SetCashAtStart(std::stoi(((Edit_t*)Dialog->Children[3]->Data)->Text));

    DialogSend(Dialog->Children[ 5 ], LB_GETCURSEL, 0, 0);
    SetTileSetType(DialogRetValue());

    DialogHide(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnSelectTileSet()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnSelectTileSet(Dialog_t* Dialog, void* Param1, void* Param2)
{
#if DEMO
    size_t TileSet;
    DialogSend(Dialog->Children[ 5 ], LB_GETCURSEL, 0, 0);

    TileSet = DialogRetValue();
    if (TileSet == TILESET_RANDOM || TileSet == TILESET_STONE)
    {
        Dialog->Children[ 7 ]->Flags &= ~DIALOG_VISIBLE;
        Dialog->Children[ 8 ]->Flags &= ~DIALOG_VISIBLE;

        DialogEnable(Dialog->Children[ 6 ], 1);
    }
    else
    {
        Dialog->Children[ 7 ]->Flags |= DIALOG_VISIBLE;
        Dialog->Children[ 8 ]->Flags |= DIALOG_VISIBLE;

        DialogEnable(Dialog->Children[ 6 ], 0);
    }
#endif

    return DIALOG_MSGHANDLED;
}
