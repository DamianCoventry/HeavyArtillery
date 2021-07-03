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

#include "DialogPlayOrder.h"
#include "DialogProcDefault.h"
#include "DialogProcLabel.h"

#include "Hardware.h"
#include "GameTypes.h"

#include "LevelManager.h"
#include "PlayerManager.h"

#include <sstream>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogPlayOrder;
size_t g_PlayOrderVisible = 1;

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// Name: DialogProcPlayOrder()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcPlayOrder(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_CREATE :
    case WM_DISPLAYCHANGE :
        Dialog->X = 8.0f;
        Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - 8.0f;
        return DIALOG_MSGHANDLED;

    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
            DialogUpdatePlayOrder(Dialog);

        return DIALOG_MSGHANDLED;

    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogPlayOrderCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPlayOrderCreate(void)
{
    const float labelThinW = MESSAGEBOX_BUTTONWIDTH;
    const float labelWideW = 2 * MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + labelWideW + MESSAGEBOX_BUTTONWIDTHHALF + labelThinW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        10 * (labelH + DIALOG_PADDING);

    DialogPlayOrder = DialogCreate("Play Order", DIALOG_MOVEABLE | DIALOG_TITLE,
                                   0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPlayOrder, 0, 0);

    float labelX = DIALOG_PADDING;
    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelWideW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0);

    labelX = DIALOG_PADDING + labelWideW + MESSAGEBOX_BUTTONWIDTHHALF;
    labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0); labelY += labelH;
    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE, labelX, labelY, labelThinW, labelH, DialogPlayOrder, DialogProcLabel, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogUpdatePlayOrder()
// Desc:
//-----------------------------------------------------------------------------

void DialogUpdatePlayOrder(Dialog_t* Dialog)
{
    size_t i;

    const float labelH = MENUITEM_BUTTONHEIGHT;

    Dialog->H = ((float)GetPlayerCount() * labelH) + DIALOG_PADDING + DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;
    Dialog->X = 8.0f;
    Dialog->Y = (float)g_HWConfig.GfxHeight - Dialog->H - 8.0f;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (i < GetPlayerCount())
        {
            if (i == GetCurrentPlayerIndex())
            {
                Dialog->Children[i]->Flags |= DIALOG_HIGHLIGHT;
                Dialog->Children[i + 10]->Flags |= DIALOG_HIGHLIGHT;
            }
            else
            {
                Dialog->Children[i]->Flags &= ~DIALOG_HIGHLIGHT;
                Dialog->Children[i + 10]->Flags &= ~DIALOG_HIGHLIGHT;
            }

            Dialog->Children[i]->Flags |= DIALOG_VISIBLE;
            Dialog->Children[i + 10]->Flags |= DIALOG_VISIBLE;

            std::ostringstream oss;
            oss << i << ". " << GetPlayerName(GetPlayOrderPlayer(i)) << " (" << GetPlayerTeam(GetPlayOrderPlayer(i)) << ")";
            Dialog->Children[i]->Name = oss.str();

            Dialog->Children[i + 10]->Name = IsPlayerDead(GetPlayOrderPlayer(i)) ? "Dead" : "Alive";
        }
        else
        {
            Dialog->Children[i]->Flags &= ~(DIALOG_VISIBLE | DIALOG_HIGHLIGHT);
            Dialog->Children[i + 10]->Flags &= ~(DIALOG_VISIBLE | DIALOG_HIGHLIGHT);
        }
    }
}
