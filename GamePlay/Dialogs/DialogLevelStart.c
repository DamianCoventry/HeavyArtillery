//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 16 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogProcLabel.h"
#include "DialogProcDefault.h"

#include "Utility.h"
#include "State.h"
#include "StateBeginLevel.h"
#include "LevelManager.h"

#include <stdio.h>
#include <sstream>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogLevelStart;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t _TimerID;


//-----------------------------------------------------------------------------
// Name: DialogProcLevelStart()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcLevelStart(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
    {
        if (WA_ACTIVE == (size_t)Param1)
        {
            std::ostringstream oss;
            oss << "Beginning level " << GetCurrentLevel() + 1 << " of " << GetLevelCount();
            auto text = oss.str();
            DialogSend(Dialog->Children[0], WM_SETTEXT, (void*)&text, 0);

            DialogSend(Dialog->Children[0], WM_SIZE, 0, 0);

            _TimerID = DialogCreateTimer(Dialog, 120);
        }
        return DIALOG_MSGHANDLED;
    }
    case WM_TIMER :
        DialogDeleteTimer(_TimerID);
        DialogHide(Dialog);

        SetGameState(&g_StateBeginLevel);

        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogLevelStartCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogLevelStartCreate(void)
{
    const float labelW = MESSAGEBOX_BUTTONWIDTH * 4;
    const float labelH = MESSAGEBOX_BUTTONHEIGHT;

    const float dialogW = labelW;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        labelH + DIALOG_PADDING;

    DialogLevelStart = DialogCreate("Heavy Artillery", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                    0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcLevelStart, 0, 0);

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 0.0f, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, labelW, labelH, DialogLevelStart, DialogProcLabel, 0, 0);

    return 1;
}
