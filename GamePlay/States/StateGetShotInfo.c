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
#include <stdio.h>

#include "StateTypes.h"
#include "State.h"
#include "StateWaitForShot.h"
#include "StateRunTurn.h"
#include "StateFindTank.h"

#include "KeyList.h"
#include "DialogKeys.h"
#include "DialogShotInfo.h"
#include "DialogGameToolbar.h"

#include "Input.h"
#include "Camera.h"
#include "Game.h"
#include "View.h"
#include "Backpack.h"
#include "Shoot.h"
#include "Sound.h"

#include "GameTypes.h"
#include "GameWeapons.h"
#include "GameSounds.h"

#include "WeaponTypes.h"
#include "PlayerHistory.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "ShotManager.h"
#include "SoundManager.h"

#include <iomanip>
#include <sstream>


//----------------------------------------------------------------------------------------
// Name: Constructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Constructor( void )
{
    DialogHideAll( nullptr );
    DialogShow( DialogKeys, &g_GetShotInfoKeys );
    DialogShow( DialogShotInfo, 0 );
}


//----------------------------------------------------------------------------------------
// Name: Destructor()
// Desc: 
//----------------------------------------------------------------------------------------

static void Destructor( void )
{
}


//----------------------------------------------------------------------------------------
// Name: UpdateBarrell()
// Desc: 
//----------------------------------------------------------------------------------------

static void UpdateBarrell( void )
{
    float HeadingDelta = 0.0f;
    float PitchDelta   = 0.0f;
    int XDelta = 0;
    int YDelta = 0;

    if( CAMERATYPE_MOUSELOOK != GetCameraType() )
    {
        // then allow the user to use the mouse to modify the player's tank barrell
        GetMouseMovement( &XDelta, &YDelta );

        if( XDelta != 0 )
        {
            if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
                HeadingDelta += ( (float)XDelta * 0.25f );
            else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
                HeadingDelta += ( (float)XDelta * 0.10f );
            else
                HeadingDelta += (float)XDelta;
        }

        if( YDelta != 0 )
        {
            if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
                PitchDelta -= ( (float)YDelta * 0.25f );
            else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
                PitchDelta -= ( (float)YDelta * 0.10f );
            else
                PitchDelta -= (float)YDelta;
        }
    }

    if( IsKeyHeldDown( DIK_LEFT ) )
    {
        if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
            HeadingDelta += -1.0f;
        else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
            HeadingDelta += -0.5f;
        else
            HeadingDelta += -5.0f;
    }

    if( IsKeyHeldDown( DIK_RIGHT ) )
    {
        if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
            HeadingDelta += 1.0f;
        else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
            HeadingDelta += 0.5f;
        else
            HeadingDelta += 5.0f;
    }

    if( IsKeyHeldDown( DIK_UP ) )
    {
        if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
            PitchDelta += 1.0f;
        else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
            PitchDelta += 0.5f;
        else
            PitchDelta += 5.0f;
    }

    if( IsKeyHeldDown( DIK_DOWN ) )
    {
        if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
            PitchDelta += -1.0f;
        else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
            PitchDelta += -0.5f;
        else
            PitchDelta += -5.0f;
    }

    ModifyTankBarrell( GetCurrentPlayer(), HeadingDelta, PitchDelta );
}


//----------------------------------------------------------------------------------------
// Name: UpdateBarrell()
// Desc: 
//----------------------------------------------------------------------------------------

static void UpdateShotPower( void )
{
    float PowerDelta = 0.0f;

    if( IsKeyHeldDown( DIK_PGDN ) )
    {
        if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
            PowerDelta = -1.0f;
        else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
            PowerDelta = -0.5f;
        else
            PowerDelta = -5.0f;
    }

    if( IsKeyHeldDown( DIK_PGUP ) )
    {
        if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
            PowerDelta = 1.0f;
        else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
            PowerDelta = 0.5f;
        else
            PowerDelta = 5.0f;
    }

    ModifyTankShotPower( GetCurrentPlayer(), PowerDelta );
}


//----------------------------------------------------------------------------------------
// Name: CalculateShotParameters()
// Desc: 
//----------------------------------------------------------------------------------------

static void CalculateShotParameters( Vector_t *ShootVector, Vector_t *StartPoint )
{
    Vector_t ObjRot0, ObjRot1, ObjRot2;
    Vector_t Normal;
    Vector_t Position;
    float BSphere;
    size_t TankID, ModelID;

    ShootVector->X = 0.0f;
    ShootVector->Y = 0.0f;
    ShootVector->Z = 1.0f;

    TankID = GetPlayerTank( GetCurrentPlayer() );
    GetObjectRotation( TankID, 0, &ObjRot0 );
    GetObjectRotation( TankID, 1, &ObjRot1 );
    GetObjectRotation( TankID, 2, &ObjRot2 );
    GetObjectPosition( TankID, 0, &Position );

    // get the world rotation
    VectorRotateX( -ObjRot2.X * DTOR, ShootVector );
    VectorRotateY( -(ObjRot0.Y + ObjRot1.Y) * DTOR, ShootVector );
    VectorRotateZ( -ObjRot0.Z * DTOR, ShootVector );
    VectorRotateX( -ObjRot0.X * DTOR, ShootVector );

    // move the point out past the bounding sphere, adding a bit
    // more so that the shot projectile doesn't collide on firing

    GetObjectModels( TankID, &ModelID, nullptr );
    BSphere = GetModelBSphereRadius( ModelID ) + 50.0f;

    StartPoint->X = Position.X + ( BSphere * ShootVector->X );
    StartPoint->Y = Position.Y + ( BSphere * ShootVector->Y );
    StartPoint->Z = Position.Z + ( BSphere * ShootVector->Z );

    // raise the point up to the barrell's height
    GetPlayerGroundNormal( GetCurrentPlayer(), &Normal );
    StartPoint->X += Normal.X * TANK_BARRELLHEIGHT;
    StartPoint->Y += Normal.Y * TANK_BARRELLHEIGHT;
    StartPoint->Z += Normal.Z * TANK_BARRELLHEIGHT;
}


//----------------------------------------------------------------------------------------
// Name: FireWeapon()
// Desc: 
//----------------------------------------------------------------------------------------

void FireWeapon( void )
{
    Weapon_t *Weapon;
    Vector_t StartPoint;
    Vector_t ShootVector;
    size_t ShotID, WeaponID;
    float ShotPower;
    PlayerHistory_t *History;

    // gather the info needed to fire the current player's current weapon and call
    // the weapon's Shoot function pointer

    WeaponID = GetPlayerCurrentWeapon( GetCurrentPlayer() );

    Weapon = (Weapon_t*)BackpackGetItem( GetPlayerBackpack( GetCurrentPlayer() ), BPITEM_TYPEWEAPON, WeaponID );
    if( Weapon && Weapon->Shoot )
    {
        GetPlayerShotDetails( GetCurrentPlayer(), nullptr, nullptr, &ShotPower );
        CalculateShotParameters( &ShootVector, &StartPoint );

        ShotID = Weapon->Shoot( Weapon, &StartPoint, &ShootVector, ShotPower, 1 );

        ChooseShotCameraState(ShotID, Weapon);
        //if( &WeaponRailgun != Weapon )
        //    SetShotID( ShotID );

        SetSoundPosition( Weapon->ShootSoundID, StartPoint.X, StartPoint.Y, StartPoint.Z );
        PlaySound3D( Weapon->ShootSoundID, 0 );

        if( BACKPACK_NOMORELEFT == BackpackUse( GetPlayerBackpack( GetCurrentPlayer() ), BPITEM_TYPEWEAPON, (void *)Weapon ) )
            SetPlayerCurrentWeapon( GetCurrentPlayer(), 0 );

        // store the fact that the player has fired a shot in the player's history
        History = GetPlayerHistory( GetCurrentPlayer() );
        History->NumShotsFired++;
    }
}


//----------------------------------------------------------------------------------------
// Name: Poll()
// Desc: 
//----------------------------------------------------------------------------------------

static void Poll( void )
{
    float Heading, Pitch, Power;

    if( IsKeyPressed( DIK_F1 ) )
        SetFirstPersonView( -1, GetCurrentPlayer() );

    if( IsKeyPressed( DIK_F2 ) )
        SetThirdPersonView( -1, GetCurrentPlayer() );

    if( IsKeyPressed( DIK_F3 ) )
        SetChaseView( -1, GetCurrentPlayer() );

    if( IsKeyPressed( DIK_F4 ) )
        SetBirdsEyeView( -1, GetCurrentPlayer() );

    if( IsKeyPressed( DIK_F5 ) )
        SetMouseLookView( -1, GetCurrentPlayer() );

    UpdateBarrell();
    UpdateShotPower();

    if( UpdateView )
        UpdateView();

    GetPlayerShotDetails( GetCurrentPlayer(), &Heading, &Pitch, &Power );

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << Heading;
    DialogShotInfo->Children[3]->Name = oss.str();

    oss.str("");
    oss << std::fixed << std::setprecision(1) << Pitch;
    DialogShotInfo->Children[4]->Name = oss.str();

    oss.str("");
    oss << std::fixed << std::setprecision(1) << Power;
    DialogShotInfo->Children[5]->Name = oss.str();

    if( IsKeyPressed( DIK_ESCAPE ) )
    {
        FindPlayer( GetCurrentPlayer() );
        SetGameState( &g_StateRunTurn );

        ShowPlayer( GetCurrentPlayer() );
        DisableCrossHair();
        return;
    }

    if( IsKeyPressed( DIK_RETURN ) || IsMButtonPressed( 0 ) )
    {
        SetCameraOverCurrentPlayer();

        FireWeapon();

        //SetCameraState( CAMERASTATE_IDLE, CAMERATYPE_MOUSELOOK );
        SetGameState( &g_StateWaitForShot );

        ShowPlayer( GetCurrentPlayer() );
        DisableCrossHair();
        return;
    }

    if( IsKeyPressed( DIK_F8 ) )
        ScreenShot();
}


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

State_t g_StateGetShotInfo =
{
    Constructor, Destructor, Poll
};
