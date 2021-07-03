//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 16 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"

#include "DialogPlayer.h"
#include "DialogGameOptions.h"

#include "Utility.h"
#include "PlayerCreate.h"

#include "State.h"
#include "StateBeginGame.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogNewGame;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t DialogProcNewGame(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnPlayerListSelect(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickAddPlayer(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickRemovePlayer(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickEditPlayer(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickStart(Dialog_t* Dialog, void* Param1, void* Param2);


//-----------------------------------------------------------------------------
// Name: DialogNewGameCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogNewGameCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxH = 300.0f;

    const float dialogW =
        DIALOG_PADDING + buttonW + BUTTON_PADDING + buttonW + BUTTON_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        listBoxH + DIALOG_PADDING +
        buttonH + DIALOG_PADDING;

    DialogNewGame = DialogCreate("New Game", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                 0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcNewGame, 0, 0);

    const float listBoxW = dialogW - buttonW - DIALOG_PADDINGx2 - DIALOG_PADDINGx2;

    DialogCreate("Players", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, listBoxW, listBoxH, DialogNewGame, DialogProcList, 0, 0);

    float buttonX = dialogW - buttonW - DIALOG_PADDING;
    float buttonY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("Add Player", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, buttonH, DialogNewGame, DialogProcButton, 0, 0); buttonY += buttonH + BUTTON_PADDING;

    DialogCreate("Remove Player", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, buttonH, DialogNewGame, DialogProcButton, 0, 0); buttonY += buttonH + BUTTON_PADDING;

    DialogCreate("Edit Player", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, buttonH, DialogNewGame, DialogProcButton, 0, 0); buttonY += buttonH + BUTTON_PADDING;

    buttonX = DIALOG_PADDING;
    buttonY = dialogH - buttonH - DIALOG_PADDING;

    DialogCreate("Game Options", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, buttonH, DialogNewGame, DialogProcButton, 0, 0); buttonX += buttonW + BUTTON_PADDING;

    DialogCreate("Start", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, buttonH, DialogNewGame, DialogProcButton, 0, 0); buttonX += buttonW + BUTTON_PADDING;

    DialogCreate("Cancel", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, buttonH, DialogNewGame, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcNewGame()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcNewGame(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
            OnPlayerListSelect(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 1 ])
            OnClickAddPlayer(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 2 ])
            OnClickRemovePlayer(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 3 ])
            OnClickEditPlayer(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 4 ])
            DialogShow(DialogGameOptions, 0);

        else if (Param2 == Dialog->Children[ 5 ])
            OnClickStart(Dialog, Param1, Param2);

        else if (Param2 == Dialog->Children[ 6 ])
            DialogHide(Dialog);

        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: EnableDisableButtons()
// Desc:
//-----------------------------------------------------------------------------

void EnableDisableButtons(Dialog_t* Dialog)
{
    size_t Ret;

    DialogSend(Dialog->Children[0], LB_GETCURSEL, 0, 0);
    Ret = DialogRetValue();

    DialogEnable(Dialog->Children[2], Ret != -1);
    DialogEnable(Dialog->Children[3], Ret != -1);

    DialogSend(Dialog->Children[0], LB_GETCOUNT, 0, 0);
    DialogEnable(Dialog->Children[5], DialogRetValue() >= 2);
}


//-----------------------------------------------------------------------------
// Name: OnActivate()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2)
{
    if (WA_ACTIVE == (size_t)Param1)
    {
        DialogSend(Dialog->Children[ 0 ], LB_RESETCONTENT, 0, 0);
        ResetAllPlayers();

        EnableDisableButtons(Dialog);
    }
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnPlayerListSelect()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnPlayerListSelect(Dialog_t* Dialog, void* Param1, void* Param2)
{
    EnableDisableButtons(Dialog);
    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickAddPlayer()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickAddPlayer(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogSend(Dialog->Children[ 0 ], LB_GETCOUNT, 0, 0);

    if (DialogRetValue() >= MAXPLAYERS)
        DialogMessageBox("Heavy Artillery", "Reached the maximum player count of " + std::to_string(MAXPLAYERS), MB_OK, nullptr);
    else
        DialogShow(DialogPlayer, (void*)-1);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickRemovePlayer()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickRemovePlayer(Dialog_t* Dialog, void* Param1, void* Param2)
{
    size_t Index;

    DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);

    Index = DialogRetValue();
    if (-1 != Index)
    {
        DialogSend(Dialog->Children[ 0 ], LB_DELETESTRING, (void*)Index, 0);
        RemovePlayer(Index);
    }

    EnableDisableButtons(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickEditPlayer()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickEditPlayer(Dialog_t* Dialog, void* Param1, void* Param2)
{
    DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);
    DialogShow(DialogPlayer, (void*)DialogRetValue());

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickStart()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickStart(Dialog_t* Dialog, void* Param1, void* Param2)
{
    if (GetNumberOfTeams() >= 2)
        SetGameState(&g_StateBeginGame);
    else
        DialogMessageBox("Heavy Artillery", "There must be at 2 teams before the game can start", MB_OK, nullptr);

    return DIALOG_MSGHANDLED;
}
