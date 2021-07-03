//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include "StateTypes.h"
#include "State.h"
#include "StateRunTurn.h"
#include "StateEndTurn.h"
#include "StateEndLevel.h"

#include "Camera.h"
#include "Input.h"
#include "Utility.h"
#include "Game.h"
#include "View.h"
#include "Shoot.h"

#include "GameSounds.h"
#include "GameItems.h"

#include "LevelManager.h"
#include "PlayerManager.h"
#include "SoundManager.h"

#include "Backpack.h"
#include "KeyList.h"
#include "DialogKeys.h"
#include "DialogTankMove.h"

#include <stdio.h>

//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _FuelCounter;
static size_t _EngineStarted;


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc:
//----------------------------------------------------------------------------------------

static void Constructor(void)
{
    Vector_t Position;

    DialogHideAll(0);

    DialogShow(DialogTankMove, 0);
    DialogShow(DialogKeys, &g_TankMoveKeys);

    std::string Label{ "Tank Move Activated (" };
    Label += GetPlayerName(GetCurrentPlayer()) + ")";
    DialogSend(DialogTankMove->Children[ 0 ], WM_SETTEXT, (void*)&Label, nullptr);

    StopPlayerReceivingFallingDamage(GetCurrentPlayer());

    _FuelCounter = 0;
    _EngineStarted = 0;

    GetPlayerPosition(GetCurrentPlayer(), &Position);
    SetSoundPosition(SoundTankEngineStart0, Position.X, Position.X, Position.Z);
    PlaySound3D(SoundTankEngineStart0, 0);
}


//----------------------------------------------------------------------------------------
// Name: Destructor()
// Desc:
//----------------------------------------------------------------------------------------

static void Destructor(void)
{
}


//----------------------------------------------------------------------------------------
// Name: UpdateFuel()
// Desc:
//----------------------------------------------------------------------------------------

static size_t UpdateFuel(void)
{
    size_t FuelLeft;

    FuelLeft = BackpackGetAmountLeft(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEITEM, (void*)&ItemFuel);

    std::string Label{"Fuel Remaing: "};
    Label += std::to_string(FuelLeft);
    DialogSend(DialogTankMove->Children[ 1 ], WM_SETTEXT, (void*)&Label, 0);

    if (IsPlayerMoving(GetCurrentPlayer()))
        _FuelCounter++;

    if (++_FuelCounter > NUMFRAMESPERLITRE)
    {
        _FuelCounter = 0;

        if (BACKPACK_NOMORELEFT == BackpackUse(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEITEM, (void*)&ItemFuel))
            return 0;
    }

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: DriveTank()
// Desc:
//----------------------------------------------------------------------------------------

static void DriveTank(void)
{
    float HeadingDelta = 0.0f;
    float MoveScale;
    int XDelta = 0;

    if (!IsPlayerOnGround(GetCurrentPlayer()))
        return;

    if (CAMERATYPE_MOUSELOOK != GetCameraType())
    {
        // then allow the user to use the mouse to modify the player's tank
        GetMouseMovement(&XDelta, nullptr);

        if (XDelta != 0)
        {
            if (IsKeyHeldDown(DIK_LSHIFT) || IsKeyHeldDown(DIK_RSHIFT))
                HeadingDelta += ((float)XDelta * 0.25f);
            else if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
                HeadingDelta += ((float)XDelta * 0.10f);
            else
                HeadingDelta += (float)XDelta;
        }
    }

    if (IsKeyHeldDown(DIK_LEFT))
    {
        if (IsKeyHeldDown(DIK_LSHIFT) || IsKeyHeldDown(DIK_RSHIFT))
            HeadingDelta += -1.0f;
        else if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
            HeadingDelta += -0.5f;
        else
            HeadingDelta += -5.0f;
    }

    if (IsKeyHeldDown(DIK_RIGHT))
    {
        if (IsKeyHeldDown(DIK_LSHIFT) || IsKeyHeldDown(DIK_RSHIFT))
            HeadingDelta += 1.0f;
        else if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
            HeadingDelta += 0.5f;
        else
            HeadingDelta += 5.0f;
    }

    ModifyTankRotation(GetCurrentPlayer(), HeadingDelta);

    if (IsKeyHeldDown(DIK_UP))
    {
        if (IsKeyHeldDown(DIK_LSHIFT) || IsKeyHeldDown(DIK_RSHIFT))
            MoveScale = 0.5f;
        else if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
            MoveScale = 0.1f;
        else
            MoveScale = 1.0f;

        MovePlayerForward(GetCurrentPlayer(), MoveScale);
    }
    else if (IsKeyHeldDown(DIK_DOWN))
    {
        if (IsKeyHeldDown(DIK_LSHIFT) || IsKeyHeldDown(DIK_RSHIFT))
            MoveScale = 0.5f;
        else if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
            MoveScale = 0.1f;
        else
            MoveScale = 1.0f;

        MovePlayerBackward(GetCurrentPlayer(), MoveScale);
    }
}


//----------------------------------------------------------------------------------------
// Name: Poll()
// Desc:
//----------------------------------------------------------------------------------------

static void Poll(void)
{
    Vector_t Position;

    if (IsKeyPressed(DIK_F1))
        SetFirstPersonView(GetCurrentPlayer(), -1);

    if (IsKeyPressed(DIK_F2))
        SetThirdPersonView(GetCurrentPlayer(), -1);

    if (IsKeyPressed(DIK_F3))
        SetChaseView(GetCurrentPlayer(), -1);

    if (IsKeyPressed(DIK_F4))
        SetBirdsEyeView(GetCurrentPlayer(), -1);

    if (IsKeyPressed(DIK_F5))
    {
        SetCameraOverCurrentPlayer();
        SetMouseLookView(GetCurrentPlayer(), -1);
    }

    GetPlayerPosition(GetCurrentPlayer(), &Position);
    if (!_EngineStarted)
    {
        SetSoundPosition(SoundTankEngineStart0, Position.X, Position.X, Position.Z);

        if (!IsSoundPlaying(SoundTankEngineStart0))
        {
            _EngineStarted = 1;

            SetSoundPosition(SoundTankEngine0, Position.X, Position.X, Position.Z);
            PlaySound3D(SoundTankEngine0, 1);
        }
    }
    else
        SetSoundPosition(SoundTankEngine0, Position.X, Position.X, Position.Z);

    DriveTank();
    if (!UpdateFuel())
    {
        SetGameState(&g_StateEndTurn);
        AllowPlayerToReceiveFallingDamage(GetCurrentPlayer());

        ShowPlayer(GetCurrentPlayer());
        DisableCrossHair();

        StopSound(SoundTankEngine0);
        return;
    }

    if (UpdateView)
        UpdateView();

    if (IsKeyPressed(DIK_RETURN) || IsKeyPressed(DIK_ESCAPE))
    {
        SetGameState(&g_StateEndTurn);
        AllowPlayerToReceiveFallingDamage(GetCurrentPlayer());

        ShowPlayer(GetCurrentPlayer());
        DisableCrossHair();

        StopSound(SoundTankEngine0);
        return;
    }

    if (IsEndOfLevel())
    {
        SetGameState(&g_StateEndLevel);
        AllowPlayerToReceiveFallingDamage(GetCurrentPlayer());

        ShowPlayer(GetCurrentPlayer());
        DisableCrossHair();

        StopSound(SoundTankEngine0);
        return;
    }

    if (IsKeyPressed(DIK_F8))
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateMoveTank =
{
    Constructor, Destructor, Poll
};
