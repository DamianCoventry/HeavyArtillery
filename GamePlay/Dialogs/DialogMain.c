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

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"
#include "DialogProcCheck.h"

#include "DialogFlyBy.h"
#include "DialogHelp.h"
#include "DialogAppOptions.h"
#include "DialogNewGame.h"
#include "DialogStats.h"

#include "GameModels.h"
#include "GameTypes.h"

#include "GameTypes.h"

#include "ModelManager.h"
#include "ObjectManager.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogMain;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static DialogObject_t _DialogObject;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void CallbackQuit(size_t RetValue);


//-----------------------------------------------------------------------------
// Name: DialogProcMain()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcMain(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    Vector_t Position;

    switch (Msg)
    {
    case WM_CREATE :
        _DialogObject.Scale = 3.0f;
        _DialogObject.ObjectID = GetFreeObject();
        ResetObject(_DialogObject.ObjectID);
        SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModel);

        Position.X = Position.Z = 0.0f;
        Position.Y = TANK_BARRELLHEIGHT;
        SetObjectPosition(_DialogObject.ObjectID, 2, &Position);
        break;

    case WM_DESTROY :
        ReturnObject(_DialogObject.ObjectID);
        return DIALOG_MSGHANDLED;

    case WM_DISPLAYCHANGE :
        DialogCenter(Dialog);
        return DIALOG_MSGHANDLED;

    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
        {
            DialogShowCursor(1);

            if (0 == rand() % 2)
                SetObjectModels(_DialogObject.ObjectID, g_ModelTank0LOD0, -1);
            else
                SetObjectModels(_DialogObject.ObjectID, g_ModelTank1LOD0, -1);

            Dialog->Children[ 0 ]->Data = (void*)&_DialogObject;
        }
        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 1 ])
            DialogShow(DialogNewGame, 0);
        else if (Param2 == Dialog->Children[ 2 ])
            DialogShow(DialogAppOptions, 0);
        else if (Param2 == Dialog->Children[ 3 ])
            DialogShow(DialogHelp, 0);
        else if (Param2 == Dialog->Children[ 4 ])
        {
            DialogHide(Dialog);
            DialogShow(DialogFlyBy, 0);
        }
        else if (Param2 == Dialog->Children[ 5 ])
            DialogMessageBox("Heavy Artillery", "Do you really want to wuss out?", MB_YESNO, CallbackQuit);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogMainCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogMainCreate(void)
{
    const float modelW = 200.0f;
    const float modelH = 200.0f;

    const float buttonW = 150.0f;

    const float dialogW = 400.0f;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        (5 * (MESSAGEBOX_BUTTONHEIGHT + BUTTON_PADDING)) +
        50.0f;

    const float buttonX = dialogW - buttonW - DIALOG_PADDING;

    DialogMain = DialogCreate(GAME_NAME, DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                              0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcMain, 0, 0);

    DialogCreate("Model", DIALOG_CHILD | DIALOG_VISIBLE,
                 DIALOG_PADDING, 2.0f * DIALOG_TITLEBARHEIGHT, modelW, modelH, DialogMain, DialogProcLWO, 0, 0);

    float buttonY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2;

    DialogCreate("New Game", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, MESSAGEBOX_BUTTONHEIGHT, DialogMain, DialogProcButton, 0, 0); buttonY += MESSAGEBOX_BUTTONHEIGHT + BUTTON_PADDING;

    DialogCreate("Options", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, MESSAGEBOX_BUTTONHEIGHT, DialogMain, DialogProcButton, 0, 0); buttonY += MESSAGEBOX_BUTTONHEIGHT + BUTTON_PADDING;

    DialogCreate("Help", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, MESSAGEBOX_BUTTONHEIGHT, DialogMain, DialogProcButton, 0, 0); buttonY += MESSAGEBOX_BUTTONHEIGHT + BUTTON_PADDING;

    DialogCreate("Close", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, MESSAGEBOX_BUTTONHEIGHT, DialogMain, DialogProcButton, 0, 0); buttonY += MESSAGEBOX_BUTTONHEIGHT + BUTTON_PADDING;

    DialogCreate("Exit", DIALOG_CHILD | DIALOG_CENTER | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 buttonX, buttonY, buttonW, MESSAGEBOX_BUTTONHEIGHT, DialogMain, DialogProcButton, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: CallbackQuit()
// Desc:
//-----------------------------------------------------------------------------

static void CallbackQuit(size_t RetValue)
{
    if (IDYES == RetValue)
        PostQuitMessage(0);
}
