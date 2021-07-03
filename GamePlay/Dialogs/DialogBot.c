//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 16 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogBot.h"
#include "DialogPlayer.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLabel.h"
#include "DialogProcList.h"

#include "Utility.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogBot;


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct FiveStrings_s
{
    const std::string S0, S1, S2, S3, S4;
}
FiveStrings_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static const FiveStrings_t BotDescriptions[] =
{
    {
        "The Inept personality makes the Bot buy random",
        "weapons and items, and choose a random target.",
        "The Inept bot will fire a random shot and then",
        "refine its shot until it hits its target.",
        "Choose this personality if you're a newbie."
    },
    {
        "The Vindictive personality makes the Bot buy expensive",
        "or multi-projectile weapons.  This personality causes a",
        "Bot to carry out acts of revenge on whomever damages",
        "their tank.  A Vindicitve Bot's shot is the same as an",
        "Inept Bot unless it has a line of sight to its target."
    },
    {
        "The Competetive personality makes the Bot buy weapons",
        "that cause the most damage or are multi-projectile.",
        "This personality causes a Bot to attack the players",
        "that are winning or that have a lot of stuff.  This",
        "Bot will shoot towards you and then refine its shot."
    },
    {
        "The Greedy personality makes the Bot buy weapons that",
        "are the most expensive or are multi-projectile.  A Bot",
        "with this personality will try to multi-kill with every shot.",
        "This personality's shot is the same as the Competetive",
        "Bot's shot, unless it has a line of sight."
    },
    {
        "The Cruel personality makes the Bot buy weapons that",
        "are expensive or cause the most damage.  The Cruel",
        "personality attacks whomever has the least amount of",
        "power or who is loosing the game.  This personality",
        "has an accurate shot that keeps getting better."
    },
    {
        "Choosing this option will cause the game to randomly",
        "choose one of the above personalities when the game",
        "begins.  However, you will not be informed of which",
        "personality was chosen :)",
        " "
    },
};

//-----------------------------------------------------------------------------
// Name: DialogProcBot()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcBot(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    size_t i;

    switch (Msg)
    {
    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
        {
            DialogSend(Dialog->Children[ 0 ], LB_RESETCONTENT, 0, 0);

            std::string Temp = "Inept";
            DialogSend(Dialog->Children[ 0 ], LB_ADDSTRING, (void*)&Temp, 0);
            Temp = "Vindictive";
            DialogSend(Dialog->Children[ 0 ], LB_ADDSTRING, (void*)&Temp, 0);
            Temp = "Competetive";
            DialogSend(Dialog->Children[ 0 ], LB_ADDSTRING, (void*)&Temp, 0);
            Temp = "Greedy";
            DialogSend(Dialog->Children[ 0 ], LB_ADDSTRING, (void*)&Temp, 0);
            Temp = "Cruel";
            DialogSend(Dialog->Children[ 0 ], LB_ADDSTRING, (void*)&Temp, 0);
            Temp = "Random";
            DialogSend(Dialog->Children[ 0 ], LB_ADDSTRING, (void*)&Temp, 0);

            DialogSend(Dialog->Children[ 0 ], LB_SETCURSEL, (void*)((size_t)Param2 - 1), 0);
        }
        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
        {
            DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);
            i = DialogRetValue();
            DialogSend(Dialog->Children[ 1 ], WM_SETTEXT, (void*)&BotDescriptions[ i ].S0, 0);
            DialogSend(Dialog->Children[ 2 ], WM_SETTEXT, (void*)&BotDescriptions[ i ].S1, 0);
            DialogSend(Dialog->Children[ 3 ], WM_SETTEXT, (void*)&BotDescriptions[ i ].S2, 0);
            DialogSend(Dialog->Children[ 4 ], WM_SETTEXT, (void*)&BotDescriptions[ i ].S3, 0);
            DialogSend(Dialog->Children[ 5 ], WM_SETTEXT, (void*)&BotDescriptions[ i ].S4, 0);
        }
        else if (Param2 == Dialog->Children[ 6 ])
        {
            DialogHide(Dialog);

            DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);
            SetBotType(DialogRetValue() + 1);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogBotCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogBotCreate(void)
{
    const float labelW = 450;
    const float labelH = MESSAGEBOX_BUTTONHEIGHT / 2;

    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 175.0f;
    const float listBoxH = 225.0f;

    const float dialogW =
        DIALOG_PADDING +
        listBoxW + DIALOG_PADDING +
        labelW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        listBoxH + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDINGx2;

    DialogBot = DialogCreate("Bot Type", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                             0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcBot, 0, 0);

    DialogCreate("List", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, listBoxW, listBoxH, DialogBot, DialogProcList, 0, 0);

    float labelX = listBoxW + DIALOG_PADDINGx2;
    float labelY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, listBoxW, labelH, DialogBot, DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, listBoxW, labelH, DialogBot, DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, listBoxW, labelH, DialogBot, DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, listBoxW, labelH, DialogBot, DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, listBoxW, labelH, DialogBot, DialogProcLabel, 0, 0); labelY += labelH + DIALOG_PADDING;

    DialogCreate("OK", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogBot, DialogProcButton, 0, 0);

    return 1;
}
