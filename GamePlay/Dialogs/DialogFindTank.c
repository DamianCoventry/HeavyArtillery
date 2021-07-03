//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Monday, 10 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLabel.h"

#include "DialogGameToolbar.h"
#include "DialogMyTank.h"
#include "DialogPlayOrder.h"

#include "Hardware.h"
#include "Utility.h"

#include "LevelManager.h"
#include "PlayerManager.h"

#include "State.h"
#include "StateFindTank.h"

#include <stdio.h>
#include <sstream>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogFindTank;

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcFindTank()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcFindTank(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    size_t i;

    switch (Msg)
    {
    case WM_ACTIVATE :
    {
        if (WA_ACTIVE == (size_t)Param1)
        {
            Dialog->X = ((float)g_HWConfig.GfxWidth / 2.0f) - (Dialog->W / 2.0f);
            Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - 8.0f;

            DialogSend(Dialog->Children[1], LB_RESETCONTENT, 0, 0);

            // always add the current player first so that they're at the top of
            // the listbox
            std::string Temp = "Me";
            DialogSend(Dialog->Children[1], LB_ADDSTRING, (void*)&Temp, 0);
            DialogSend(Dialog->Children[1], LB_SETITEMDATA, 0, (void*)GetCurrentPlayer());

            for (i = 0; i < GetPlayerCount(); i++)
            {
                if (IsPlayerDead(GetCurrentPlayer()))
                    continue;

                std::ostringstream oss;

                // what is this tank's relationship with the current player?
                if (i == GetCurrentPlayer())
                {
                    // ignore, because we added the current player as 'Me' above
                }
                else if (GetPlayerTeam(i) == GetPlayerTeam(GetCurrentPlayer()))
                {
                    oss << GetPlayerName(i) << " (TeamMate)";
                    auto text = oss.str();
                    DialogSend(Dialog->Children[1], LB_ADDSTRING, (void*)&text, 0);
                    DialogSend(Dialog->Children[1], LB_GETCOUNT, 0, 0);
                    DialogSend(Dialog->Children[1], LB_SETITEMDATA, (void*)(DialogRetValue() - 1), (void*)i);
                }
                else
                {
                    oss << GetPlayerName(i) << " (" << GetPlayerTeam(i) << ")";
                    auto text = oss.str();
                    DialogSend(Dialog->Children[1], LB_ADDSTRING, (void*)&text, 0);
                    DialogSend(Dialog->Children[1], LB_GETCOUNT, 0, 0);
                    DialogSend(Dialog->Children[1], LB_SETITEMDATA, (void*)(DialogRetValue() - 1), (void*)i);
                }
            }

            // select the first string, ie. the "Me" string
            DialogSend(Dialog->Children[1], LB_SETCURSEL, 0, 0);

            DialogHide(DialogMyTank);
            DialogHide(DialogPlayOrder);
        }
        return DIALOG_MSGHANDLED;
    }
    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 2 ])
        {
            DialogSend(Dialog->Children[ 1 ], LB_GETCURSEL, 0, 0);
            DialogSend(Dialog->Children[ 1 ], LB_GETITEMDATA, (void*)DialogRetValue(), 0);

            FindPlayer(DialogRetValue());
        }
        else if (Param2 == Dialog->Children[ 3 ])
        {
            DialogHide(Dialog);
            DialogShow(DialogGameToolbar, 0);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogFindTankCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogFindTankCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 300;
    const float listBoxH = 125;

    const float dialogW =
        DIALOG_PADDING + listBoxW + DIALOG_PADDING + buttonW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING +
        listBoxH + DIALOG_PADDING;

    DialogFindTank = DialogCreate("Find Tank", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                  0.0f, 0.0f, dialogW, dialogH,
                                  nullptr, DialogProcFindTank, 0, 0);

    DialogCreate("Tanks:", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, buttonW, buttonH,
                 DialogFindTank, DialogProcLabel, 0, 0);

    const float listBoxY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + buttonH + DIALOG_PADDING;

    DialogCreate("List", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, listBoxY, listBoxW, listBoxH,
                 DialogFindTank, DialogProcList, 0, 0);

    DialogCreate("Find", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS | DIALOG_OK,
                 dialogW - buttonW - DIALOG_PADDING, listBoxY, buttonW, buttonH,
                 DialogFindTank, DialogProcButton, 0, 0);

    DialogCreate("Close", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS | DIALOG_CANCEL,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH,
                 DialogFindTank, DialogProcButton, 0, 0);

    return 1;
}
