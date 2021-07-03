//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 16 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <string.h>

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLabel.h"
#include "DialogProcEdit.h"
#include "DialogProcList.h"
#include "DialogProcCheck.h"
#include "DialogProcButton.h"

#include "DialogBot.h"
#include "DialogNewGame.h"

#include "Utility.h"
#include "PlayerCreate.h"

#include <stdio.h>
#include <sstream>
#include <string>

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogPlayer;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static size_t _PlayerIndex = 0;
static size_t _BotType;

static Dialog_t* NameLabel;
static Dialog_t* NameEdit;
static Dialog_t* TeamLabel;
static Dialog_t* TeamList;
static Dialog_t* BotCheckbox;
static Dialog_t* SetupBotButton;
static Dialog_t* OkButton;
static Dialog_t* CancelButton;

//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static size_t DialogProcPlayer(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);

static size_t OnActivate(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickBot(Dialog_t* Dialog, void* Param1, void* Param2);
static size_t OnClickOK(Dialog_t* Dialog, void* Param1, void* Param2);

static void DialogUpdate(Dialog_t* Dialog);
static void AddPlayerToListBox(const std::string& Name, size_t Team, size_t BotType);
static void EditPlayerInListBox(void);


//-----------------------------------------------------------------------------
// Name: DialogPlayerCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogPlayerCreate(void)
{
    const float labelW = MESSAGEBOX_BUTTONWIDTH / 2;

    const float editBoxW = MESSAGEBOX_BUTTONWIDTH * 2;

    const float checkBoxW = MESSAGEBOX_BUTTONWIDTH * 2;

    const float buttonW = MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 200.0f;
    const float listBoxH = 300.0f;

    const float dialogW =
        labelW + DIALOG_PADDING +
        editBoxW + DIALOG_PADDING +
        buttonW + DIALOG_PADDINGx2;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        buttonH + DIALOG_PADDING +
        buttonH + DIALOG_PADDING +
        listBoxH + DIALOG_PADDINGx2;

    DialogPlayer = DialogCreate("Player", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcPlayer, 0, 0);

    float controlY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    NameLabel = DialogCreate("Name:", DIALOG_CHILD | DIALOG_VISIBLE,
                             DIALOG_PADDING, controlY, labelW, buttonH, DialogPlayer, DialogProcLabel, 0, 0);

    NameEdit = DialogCreate("Edit", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                            DIALOG_PADDINGx2 + labelW, controlY, editBoxW, buttonH, DialogPlayer, DialogProcEdit, 0, 0); controlY += buttonH + BUTTON_PADDING;

    BotCheckbox = DialogCreate("Player is a bot", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                               DIALOG_PADDINGx2 + labelW, controlY, checkBoxW, buttonH, DialogPlayer, DialogProcCheck, 0, 0);

    const float buttonX = dialogW - buttonW - DIALOG_PADDING;

    SetupBotButton = DialogCreate("Choose Bot Type...", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                                  buttonX, controlY, buttonW, buttonH, DialogPlayer, DialogProcButton, 0, 0); controlY += buttonH + BUTTON_PADDING;

    TeamLabel = DialogCreate("Team:", DIALOG_CHILD | DIALOG_VISIBLE,
                             DIALOG_PADDING, controlY, labelW, buttonH, DialogPlayer, DialogProcLabel, 0, 0);

    TeamList = DialogCreate("List", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                            DIALOG_PADDINGx2 + labelW, controlY, listBoxW, listBoxH, DialogPlayer, DialogProcList, 0, 0);

    controlY = dialogH - buttonH - BUTTON_PADDING - buttonH - BUTTON_PADDING;

    OkButton = DialogCreate("OK", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS | DIALOG_OK,
                            buttonX, controlY, buttonW, buttonH, DialogPlayer, DialogProcButton, 0, 0); controlY += buttonH + BUTTON_PADDING;

    CancelButton = DialogCreate("Cancel", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS | DIALOG_CANCEL,
                                buttonX, controlY, buttonW, buttonH, DialogPlayer, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogProcPlayer()
// Desc:
//-----------------------------------------------------------------------------

static size_t DialogProcPlayer(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_ACTIVATE :
        return OnActivate(Dialog, Param1, Param2);

    case WM_COMMAND :
        if (Param2 == NameEdit)
            DialogUpdate(Dialog);

        else if (Param2 == BotCheckbox)
            return OnClickBot(Dialog, Param1, Param2);

        else if (Param2 == SetupBotButton)
            DialogShow(DialogBot, (void*)_BotType);

        else if (Param2 == OkButton)
            return OnClickOK(Dialog, Param1, Param2);

        else if (Param2 == CancelButton)
            DialogHide(Dialog);

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
    size_t i;

    if (WA_ACTIVE == (size_t)Param1)
    {
        DialogSetFocus(Dialog, NameEdit);

        DialogSend(TeamList, LB_RESETCONTENT, 0, 0);
        for (i = 0; i < MAXPLAYERS; i++)
        {
            auto Index = std::to_string(i);
            DialogSend(TeamList, LB_ADDSTRING, (void*)&Index, 0);
        }

        _PlayerIndex = (size_t)Param2;

        if (_PlayerIndex != LB_ERR)
        {
            auto Name = GetPlayerCreateName(_PlayerIndex);
            DialogSend(NameEdit, WM_SETTEXT, (void*)&Name, 0);
            DialogSend(TeamList, LB_SETCURSEL, (void*)GetPlayerCreateTeam(_PlayerIndex), 0);

            _BotType = GetPlayerCreateBotType(_PlayerIndex);
            DialogCheckDialogItem(BotCheckbox, _BotType != BOTTYPE_NOTABOT);
        }
        else
        {
            DialogSend(NameEdit, WM_CLEAR, 0, 0);
            DialogSend(TeamList, LB_SETCURSEL, 0, 0);

            DialogCheckDialogItem(BotCheckbox, 0);
            _BotType = BOTTYPE_NOTABOT;
        }

        DialogUpdate(Dialog);
    }

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickBot()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickBot(Dialog_t* Dialog, void* Param1, void* Param2)
{
    if (((CheckBox_t*)BotCheckbox->Data)->Checked)
        _BotType = BOTTYPE_RANDOM;
    else
        _BotType = BOTTYPE_NOTABOT;

    DialogUpdate(Dialog);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: OnClickOK()
// Desc:
//-----------------------------------------------------------------------------

static size_t OnClickOK(Dialog_t* Dialog, void* Param1, void* Param2)
{
    std::string Trimmed{ ((Edit_t*)NameEdit->Data)->Text };
    if (Trimmed.length() > MAXNAMECHARACTERS)
        Trimmed = Trimmed.substr(0, MAXNAMECHARACTERS);

    DialogSend(TeamList, LB_GETCURSEL, 0, 0);

    if (_PlayerIndex == LB_ERR)
    {
        auto Team = DialogRetValue();
        AddPlayer(Trimmed, Team, (BotType_t)_BotType);
        AddPlayerToListBox(Trimmed, Team, _BotType);
    }
    else
    {
        EditPlayer(_PlayerIndex, Trimmed, DialogRetValue(), (BotType_t)_BotType);
        EditPlayerInListBox();
    }

    DialogHide(Dialog);
    EnableDisableButtons(DialogNewGame);

    return DIALOG_MSGHANDLED;
}


//-----------------------------------------------------------------------------
// Name: DialogUpdate()
// Desc:
//-----------------------------------------------------------------------------

static void DialogUpdate(Dialog_t* Dialog)
{
    DialogEnable(SetupBotButton, ((CheckBox_t*)BotCheckbox->Data)->Checked);
    DialogEnable(OkButton, ((Edit_t*)NameEdit->Data)->Text.empty() ? 0 : 1);
}


//-----------------------------------------------------------------------------
// Name: AddPlayerToListBox()
// Desc:
//-----------------------------------------------------------------------------

static void AddPlayerToListBox(const std::string& Name, size_t Team, size_t BotType)
{
    std::ostringstream oss;
    oss << Team << ", " << Name;

    if (BotType != BOTTYPE_NOTABOT)
        oss << " (Bot)";

    auto text = oss.str();
    DialogSend(DialogNewGame->Children[0], LB_ADDSTRING, (void*)&text, 0);
}


//-----------------------------------------------------------------------------
// Name: EditPlayerInListBox()
// Desc:
//-----------------------------------------------------------------------------

static void EditPlayerInListBox(void)
{
    DialogSend(TeamList, LB_GETCURSEL, 0, 0);
    size_t ListIndex = DialogRetValue();

    std::ostringstream oss;
    oss << GetPlayerCreateTeam(_PlayerIndex) << ", " << GetPlayerCreateName(_PlayerIndex);

    if (GetPlayerCreateBotType(_PlayerIndex) != BOTTYPE_NOTABOT)
        oss << " (Bot)";

    auto text = oss.str();
    DialogSend(DialogNewGame->Children[0], WM_SETTEXT, (void*)ListIndex, (void*)&text);
}


//-----------------------------------------------------------------------------
// Name: SetBotType()
// Desc:
//-----------------------------------------------------------------------------

void SetBotType(size_t BotType)
{
    _BotType = BotType;
}
