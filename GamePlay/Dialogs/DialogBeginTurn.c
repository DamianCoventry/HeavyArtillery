//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 10 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "DialogProcDefault.h"
#include "DialogProcLabel.h"

#include "Utility.h"

#include "PlayerManager.h"
#include "LevelManager.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogBeginTurn;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t _TimerID;
static size_t _CanBeHidden;


//-----------------------------------------------------------------------------
// Name: DialogProcBeginTurn()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcBeginTurn(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
    {
        if (WA_ACTIVE == (size_t)Param1)
        {
            std::string Text = GetPlayerName(GetCurrentPlayer()) + ", it's your turn.";
            DialogSend(Dialog->Children[0], WM_SETTEXT, (void*)&Text, 0);
            DialogSend(Dialog->Children[0], WM_SIZE, 0, 0);

            _TimerID = DialogCreateTimer(Dialog, 120);
            _CanBeHidden = 0;

            DialogShowCursor(0);
        }
        return DIALOG_MSGHANDLED;
    }
    case WM_TIMER :
        DialogDeleteTimer(_TimerID);
        _CanBeHidden = 1;
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogBeginTurnCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogBeginTurnCreate(void)
{
    const float labelW = MESSAGEBOX_BUTTONWIDTH * 4;
    const float labelH = MESSAGEBOX_BUTTONHEIGHT;

    const float dialogW = labelW;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        labelH + DIALOG_PADDING +
        labelH + DIALOG_PADDING;

    DialogBeginTurn = DialogCreate("Heavy Artillery", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                   0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcBeginTurn, 0, 0);

    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 DIALOG_PADDING, labelY, labelW, labelH, DialogBeginTurn, DialogProcLabel, 0, 0); labelY += labelH;

    DialogCreate("Finding your Tank...", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 DIALOG_PADDING, labelY, labelW, labelH, DialogBeginTurn, DialogProcLabel, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: CanHideDialogBeginTurn()
// Desc:
//-----------------------------------------------------------------------------

size_t CanDialogBeHidden(void)
{
    return _CanBeHidden;
}