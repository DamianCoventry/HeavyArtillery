//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 9 April 2002
//-----------------------------------------------------------------------------


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "DialogMyTank.h"

#include "DialogProcDefault.h"
#include "DialogProcButton.h"
#include "DialogProcList.h"
#include "DialogProcLWO.h"
#include "DialogProcLabel.h"

#include "WeaponTypes.h"
#include "LevelManager.h"
#include "PlayerManager.h"

#include "ObjectManager.h"
#include "ModelManager.h"

#include "GameModels.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Dialog_t* DialogWeapon;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static DialogObject_t _DialogObject;


//-----------------------------------------------------------------------------
// Name: DialogProcWeapon()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcWeapon(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2)
{
    Weapon_t* Weapon;

    switch (Msg)
    {
    case WM_CREATE :
        _DialogObject.Scale = 4.0f;
        _DialogObject.ObjectID = GetFreeObject();
        ResetObject(_DialogObject.ObjectID);

        SetObjectDrawFunction(_DialogObject.ObjectID, DrawShadedModel);
        break;

    case WM_DESTROY :
        ReturnObject(_DialogObject.ObjectID);
        return DIALOG_MSGHANDLED;

    case WM_ACTIVATE :
        if (WA_ACTIVE == (size_t)Param1)
        {
            AddBackpackItemsToListBox(Dialog->Children[ 0 ], GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEWEAPON);
            DialogSend(Dialog->Children[ 0 ], LB_SETCURSEL, (void*)GetPlayerCurrentWeapon(GetCurrentPlayer()), 0);
            //GamePushDialogState();
        }
        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 0 ])
        {
            DialogSend(Dialog->Children[ 0 ], LB_GETCURSEL, 0, 0);
            SetPlayerCurrentWeapon(GetCurrentPlayer(), DialogRetValue());

            Weapon = (Weapon_t*)BackpackGetItem(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEWEAPON, GetPlayerCurrentWeapon(GetCurrentPlayer()));

            SetObjectModels(_DialogObject.ObjectID, Weapon->ProjectileModelIDLOD0, -1);
            _DialogObject.Scale = Weapon->Scale;

            if (Weapon->ProjectileModelIDLOD0 == -1)
            {
                SetObjectModels(_DialogObject.ObjectID, g_ModelBuyFullVersion, -1);
                _DialogObject.Scale = 4.0f;
            }

            Dialog->Children[ 2 ]->Data = (void*)&_DialogObject;

            DialogSend(Dialog->Children[ 4 ], WM_SETTEXT, (void*)&Weapon->Line0, 0);
            DialogSend(Dialog->Children[ 5 ], WM_SETTEXT, (void*)&Weapon->Line1, 0);
            DialogSend(Dialog->Children[ 6 ], WM_SETTEXT, (void*)&Weapon->Line2, 0);
            DialogSend(Dialog->Children[ 7 ], WM_SETTEXT, (void*)&Weapon->Line3, 0);
        }
        else if (Param2 == Dialog->Children[ 1 ])
        {
            DialogUpdateMyTank(DialogMyTank);
            DialogHide(Dialog);
        }
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name: DialogWeaponCreate()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogWeaponCreate(void)
{
    const float buttonW = MESSAGEBOX_BUTTONWIDTH;
    const float buttonH = MESSAGEBOX_BUTTONHEIGHT;

    const float listBoxW = 250;
    const float listBoxH = 350;

    const float modelW = 200;
    const float modelH = 200;

    const float labelWideW = 3 * MESSAGEBOX_BUTTONWIDTH + MESSAGEBOX_BUTTONWIDTHHALF;
    const float labelH = MENUITEM_BUTTONHEIGHT;

    const float dialogW =
        DIALOG_PADDING + listBoxW + DIALOG_PADDING + labelWideW + DIALOG_PADDING;
    const float dialogH =
        DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 +
        listBoxH + DIALOG_PADDING +
        buttonH + DIALOG_PADDING;

    DialogWeapon = DialogCreate("Weapon", DIALOG_MODAL | DIALOG_MOVEABLE | DIALOG_TITLE | DIALOG_CENTER,
                                0.0f, 0.0f, dialogW, dialogH, nullptr, DialogProcWeapon, 0, 0);

    DialogCreate("Weapon ListBox", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CANGETFOCUS,
                 DIALOG_PADDING, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, listBoxW, listBoxH,
                 DialogWeapon, DialogProcList, 0, 0);

    DialogCreate("OK", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                 dialogW - buttonW - DIALOG_PADDING, dialogH - buttonH - DIALOG_PADDING, buttonW, buttonH,
                 DialogWeapon, DialogProcButton, 0, 0);

    DialogCreate("Model", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_BORDER,
                 listBoxW + DIALOG_PADDINGx2, DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2, modelW, modelH, DialogWeapon,
                 DialogProcLWO, 0, 0);

    float controlX = listBoxW + DIALOG_PADDINGx2;
    float controlY = DIALOG_TITLEBARHEIGHT + DIALOG_PADDINGx2 + modelH + DIALOG_PADDING;

    DialogCreate("Information:", DIALOG_CHILD | DIALOG_VISIBLE,
                 controlX, controlY, labelWideW, labelH,
                 DialogWeapon, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 controlX, controlY, labelWideW, labelH,
                 DialogWeapon, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 controlX, controlY, labelWideW, labelH,
                 DialogWeapon, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 controlX, controlY, labelWideW, labelH,
                 DialogWeapon, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 controlX, controlY, labelWideW, labelH,
                 DialogWeapon, DialogProcLabel, 0, 0); controlY += labelH;

    DialogCreate(" ", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_HIGHLIGHT,
                 controlX, controlY, labelWideW, labelH,
                 DialogWeapon, DialogProcLabel, 0, 0);

    return 1;
}
