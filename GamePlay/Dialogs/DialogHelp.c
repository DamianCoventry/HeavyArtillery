//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Thursday, 6 June 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"
#include "DialogProcCheck.h"
#include "DialogProcList.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogHelp;


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct TopicLine_s
{
    size_t DrawFlags;
    const std::string String;
}
TopicLine_t;

typedef struct TopicEntry_s
{
    const std::string TopicHeading;
    TopicLine_t Lines[ 10 ];
}
TopicEntry_t;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static const TopicEntry_t Instructions =
{
    "Instructions",
    {
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Instructions" },
        { DIALOG_VISIBLE, "Select a topic from the list to view help information." },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
    }
};

static const TopicEntry_t Overview =
{
    "Overview",
    {
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Overview" },
        { DIALOG_VISIBLE, "The idea is to destroy your enemy's tank.  There are many weapons that" },
        { DIALOG_VISIBLE, "you may choose to use to obliterate your opponents.  There's also a" },
        { DIALOG_VISIBLE, "handful of items that you can use to protect and assist your tank" },
        { DIALOG_VISIBLE, "in its encounters." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Gameplay" },
        { DIALOG_VISIBLE, "Each tank will be placed onto the landscape and a random play order will" },
        { DIALOG_VISIBLE, "be generated.  Each tank in turn will have a chance to either shoot at its" },
        { DIALOG_VISIBLE, "enemies or, if the player has purchased Fuel, move to another location." },
        { DIALOG_VISIBLE, "Before shooting or moving a player may use and activate Items." }
    }
};

static const TopicEntry_t Shop =
{
    "The Shop",
    {
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Overview" },
        { DIALOG_VISIBLE, "The Shop dialog is activated before a level is started.  It will cycle" },
        { DIALOG_VISIBLE, "through each player that has Cash and allow them to purchase weapons" },
        { DIALOG_VISIBLE, "and items.  You may not buy weapons and items at any other time." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "How it works" },
        { DIALOG_VISIBLE, "Select either a Weapon or an Item from the list box on the left and then" },
        { DIALOG_VISIBLE, "click buy.  Purchased goods will appear in the list box at the bottom.  You" },
        { DIALOG_VISIBLE, "may purchase the same thing more than once and you also may Sell any" },
        { DIALOG_VISIBLE, "Weapons or Items you have purchased.  Note that you always have an" },
        { DIALOG_VISIBLE, "unlimited supply of Small Missiles and you may not Sell them." }
    }
};

static const TopicEntry_t Weapons =
{
    "Weapons",
    {
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Overview" },
        { DIALOG_VISIBLE, "Weapons differ by what they do on impact, at apogee and how they" },
        { DIALOG_VISIBLE, "detonate.  Note that all projectile based weapons bounce of shields" },
        { DIALOG_VISIBLE, "but still do damage to those shields." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Impact, Apogee and Detonation" },
        { DIALOG_VISIBLE, "On impact weapons will either detonate, bounce or roll.  At apogee" },
        { DIALOG_VISIBLE, "a weapon may split into multiple projectiles.  All weapons will always" },
        { DIALOG_VISIBLE, "detonate but it is specific to each weapon." },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " }
    }
};

static const TopicEntry_t Items =
{
    "Items",
    {
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Guidance" },
        { DIALOG_VISIBLE, "Once activated, Guidance will bend a single projectile shot towards" },
        { DIALOG_VISIBLE, "the closest enemy tank, but only after the shot's reached apogee." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Shields" },
        { DIALOG_VISIBLE, "Once activated, Shields will bounce incoming projectiles away.  Shields" },
        { DIALOG_VISIBLE, "will be weakened each time and can only absorb a Railgun's shot." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Other" },
        { DIALOG_VISIBLE, "Purchase fuel to allow your tank to drive to different locations.  Purchase" },
        { DIALOG_VISIBLE, "then activate Parachutes to stop receiving falling damage.  Purchase then" },
        { DIALOG_VISIBLE, "Use batteries to recharge Shield and Tank power." }
    }
};

static const TopicEntry_t AutoDefense =
{
    "AutoDefense",
    {
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "What is it?" },
        { DIALOG_VISIBLE, "Automatic Defense is an Item you can purchase from The Shop." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "What does it do?" },
        { DIALOG_VISIBLE, "It allows each player that has purchased it to Activate and Use" },
        { DIALOG_VISIBLE, "other purchased items before the level starts." },
        { DIALOG_VISIBLE, "This gives a player the chance to repair their tank, activate Shields," },
        { DIALOG_VISIBLE, "Parachutes and Guidance Systems before the first shot is fired." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Tip" },
        { DIALOG_VISIBLE, "Only buy Automatic Defense if you have also purchased other items," },
        { DIALOG_VISIBLE, "otherwise you're just wasting your money." }
    }
};

static const TopicEntry_t Levels =
{
    "Levels",
    {   { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Overview" },
        { DIALOG_VISIBLE, "When setting up a game you may choose how many levels to play." },
        { DIALOG_VISIBLE, "The game will randomly choose a tile set and generate a level" },
        { DIALOG_VISIBLE, "for the battle to take place on.  For each level the game will" },
        { DIALOG_VISIBLE, "randomly generate a play order." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Computer Players" },
        { DIALOG_VISIBLE, "Computer Players, aka Bots, have a short term memory that is not" },
        { DIALOG_VISIBLE, "cleared between levels, only between games.  This allows Bots to" },
        { DIALOG_VISIBLE, "form a grudge and pick on a player if it wants to." },
        { DIALOG_VISIBLE, " " }
    }
};

static const TopicEntry_t HotKeys =
{
    "HotKeys",
    {   { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Some Useful Hotkeys" },
        { DIALOG_VISIBLE, "Alt+F4 Exit Game Immediately" },
        { DIALOG_VISIBLE, "Alt+Enter Toggle fullscreen/windowed mode" },
        { DIALOG_VISIBLE, "F3 Activate FindTank dialog (only works during your turn)" },
        { DIALOG_VISIBLE, "F8 ScreenShot (files will be stored in the ScreenShots directory)" },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, " " }
    }
};

static const TopicEntry_t Author =
{
    "Author",
    {   { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Designed and Written by Damian Coventry." },
        { DIALOG_VISIBLE | DIALOG_HIGHLIGHT, "Copyright (C) 2002, All rights reserved." },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, "Heavy Artillery has been written entirely in C.  It uses OpenGL, DirectInput" },
        { DIALOG_VISIBLE, "and DirectSound3D." },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, "You may email the author at omen2@paradise.net.nz" },
        { DIALOG_VISIBLE, " " },
        { DIALOG_VISIBLE, "Visit http://omen2.50megs.com/ for the latest info on this game" },
        { DIALOG_VISIBLE, "and other games by the author." }
    }
};


//-----------------------------------------------------------------------------
// private message handlers
//-----------------------------------------------------------------------------

static size_t DialogProcHelp(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2);

static void DialogHelpSetTopic(Dialog_t* Dialog, const TopicEntry_t* Entry);


//-----------------------------------------------------------------------------
// Name: DialogHelpCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogHelpCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 125.0f;
    const float listBoxH = 425.0f;

    const float labelW = 600.0f;

    const float dialogW =
        listBoxW + DIALOG_PADDING +
        labelW + DIALOG_PADDINGx2;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH +
        buttonH + DIALOG_PADDING +
        listBoxH + DIALOG_PADDINGx2;

    DialogHelp = DialogCreate("Heavy Artillery Help", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                              0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcHelp, 0, 0);

    DialogCreate("Available Topics", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, 64.0f, 20.0f, DialogHelp, DialogProcLabel, 0, 0);

    const float listBoxY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + buttonH;

    DialogCreate("Help Topics", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, listBoxY, listBoxW, listBoxH, DialogHelp, DialogProcList, 0, 0);

    const float labelX = DIALOG_PADDINGx2 + listBoxW;
    float labelY = listBoxY;

    DialogCreate("0", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("1", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("2", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("3", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("4", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("5", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("6", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("7", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("8", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0); labelY += buttonH;

    DialogCreate("9", DIALOG_CHILD | DIALOG_VISIBLE,
                 labelX, labelY, labelW, buttonH, DialogHelp, DialogProcLabel, 0, 0);

    DialogCreate("Close", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH, DialogHelp, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcHelp()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcHelp(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
        {
            DialogSend(Dialog->Children[ 1 ], LB_RESETCONTENT, 0, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&Overview.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&Shop.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&Weapons.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&Items.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&AutoDefense.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&Levels.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&HotKeys.TopicHeading, 0);
            DialogSend(Dialog->Children[ 1 ], LB_ADDSTRING, (void*)&Author.TopicHeading, 0);

            DialogHelpSetTopic(Dialog, &Instructions);
        }
        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 1 ])              // topic list box
        {
            DialogSend(Dialog->Children[ 1 ], LB_GETCURSEL, 0, 0);
            switch (DialogRetValue())
            {
            case 0 :
                DialogHelpSetTopic(Dialog, &Overview);
                break;
            case 1 :
                DialogHelpSetTopic(Dialog, &Shop);
                break;
            case 2 :
                DialogHelpSetTopic(Dialog, &Weapons);
                break;
            case 3 :
                DialogHelpSetTopic(Dialog, &Items);
                break;
            case 4 :
                DialogHelpSetTopic(Dialog, &AutoDefense);
                break;
            case 5 :
                DialogHelpSetTopic(Dialog, &Levels);
                break;
            case 6 :
                DialogHelpSetTopic(Dialog, &HotKeys);
                break;
            case 7 :
                DialogHelpSetTopic(Dialog, &Author);
                break;
            }
        }
        else if (Param2 == Dialog->Children[ 12 ])        // close
            DialogHide(Dialog);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogHelpSetTopic()
// Desc:
//-----------------------------------------------------------------------------

static void DialogHelpSetTopic(Dialog_t* Dialog, const TopicEntry_t* Entry)
{
    size_t i;

    for (i = 0; i < 10; i++)
    {
        Dialog->Children[2 + i]->Name = Entry->Lines[i].String;
        Dialog->Children[2 + i]->Flags = Entry->Lines[i].DrawFlags;
        DialogSend(Dialog->Children[2 + i], WM_SIZE, 0, 0);
    }
}
