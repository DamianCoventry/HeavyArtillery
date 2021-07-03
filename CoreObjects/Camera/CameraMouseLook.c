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
#include "CameraTypes.h"
#include "Camera.h"

#include "Input.h"
#include "Vector.h"
#include "MeshManager.h"
#include "SoundManager.h"

#define CAMERA_SPEED        20.0f


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern size_t g_LandscapeMesh;

size_t g_ClipCamera = 1;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void CameraForward( Camera_t *Camera );
static void CameraBackward( Camera_t *Camera );
static void CameraLeft( Camera_t *Camera );
static void CameraRight( Camera_t *Camera );
static void CameraUp( Camera_t *Camera );
static void CameraDown( Camera_t *Camera );


//-----------------------------------------------------------------------------
// Name: CameraMouseLook()
// Desc: 
//-----------------------------------------------------------------------------

void CameraMouseLook( Camera_t *Camera )
{
    int MouseX, MouseY;

    GetMouseMovement( &MouseX, &MouseY );

    // handle the mouse being moved in X direction
    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Camera->Rotation.Y -= (float)MouseX * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Camera->Rotation.Y -= (float)MouseX * 0.1f;
    else
        Camera->Rotation.Y -= (float)MouseX * 0.5f;

    // cap the rotation
    if( Camera->Rotation.Y < 0.0f )
        Camera->Rotation.Y += 360;
    else if( Camera->Rotation.Y > 360 )
        Camera->Rotation.Y -= 360;

    // handle the mouse being moved in Y direction
    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Camera->Rotation.X -= (float)MouseY * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Camera->Rotation.X -= (float)MouseY * 0.1f;
    else
        Camera->Rotation.X -= (float)MouseY * 0.5f;

    // cap the rotation
    if( Camera->Rotation.X > 90.0f )
        Camera->Rotation.X = 90.0f;
    else if( Camera->Rotation.X < -90.0f )
        Camera->Rotation.X = -90.0f;

    Camera->Rotation.Z = 0.0f;

    // check for movement keys
    if( IsKeyHeldDown( DIK_E ) )
        CameraForward( Camera );

    if( IsKeyHeldDown( DIK_D ) )
        CameraBackward( Camera );

    if( IsKeyHeldDown( DIK_S ) )
        CameraLeft( Camera );

    if( IsKeyHeldDown( DIK_F ) )
        CameraRight( Camera );

    if( IsKeyHeldDown( DIK_Z ) )
        CameraDown( Camera );

    if( IsKeyHeldDown( DIK_SPACE ) )
        CameraUp( Camera );

    SetListenerOnCamera();
}


//-----------------------------------------------------------------------------
// Name: CameraForward
// Desc: 
//-----------------------------------------------------------------------------

static void CameraForward( Camera_t *Camera )
{
    Vector_t Move;
    float Speed;

    VectorGetMoveYX( Camera->Rotation.Y * DTOR, Camera->Rotation.X * DTOR, &Move );

    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Speed = CAMERA_SPEED * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Speed = CAMERA_SPEED * 0.10f;
    else
        Speed = CAMERA_SPEED;

    Camera->Position.X -= Speed * Move.X;
    Camera->Position.Y -= Speed * Move.Y;
    Camera->Position.Z -= Speed * Move.Z;

    if( g_ClipCamera )
        ClipCamera( &Camera->Position );
}


//-----------------------------------------------------------------------------
// Name: CameraBackward
// Desc: 
//-----------------------------------------------------------------------------

static void CameraBackward( Camera_t *Camera )
{
    Vector_t Move;
    float Speed;

    VectorGetMoveYX( Camera->Rotation.Y * DTOR, Camera->Rotation.X * DTOR, &Move );

    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Speed = CAMERA_SPEED * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Speed = CAMERA_SPEED * 0.10f;
    else
        Speed = CAMERA_SPEED;

    Camera->Position.X += Speed * Move.X;
    Camera->Position.Y += Speed * Move.Y;
    Camera->Position.Z += Speed * Move.Z;

    if( g_ClipCamera )
        ClipCamera( &Camera->Position );
}


//-----------------------------------------------------------------------------
// Name: CameraLeft
// Desc: 
//-----------------------------------------------------------------------------

static void CameraLeft( Camera_t *Camera )
{
    Vector_t Move;
    float Speed;

    Move.X = Move.Y = 0.0f;
    Move.Z = 1.0f;
    VectorRotateY( (Camera->Rotation.Y+90.0f) * DTOR, &Move );

    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Speed = CAMERA_SPEED * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Speed = CAMERA_SPEED * 0.10f;
    else
        Speed = CAMERA_SPEED;

    Camera->Position.X -= Speed * Move.X;
    Camera->Position.Z -= Speed * Move.Z;

    if( g_ClipCamera )
        ClipCamera( &Camera->Position );
}


//-----------------------------------------------------------------------------
// Name: CameraRight
// Desc: 
//-----------------------------------------------------------------------------

static void CameraRight( Camera_t *Camera )
{
    Vector_t Move;
    float Speed;

    Move.X = Move.Y = 0.0f;
    Move.Z = 1.0f;
    VectorRotateY( (Camera->Rotation.Y-90.0f) * DTOR, &Move );

    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Speed = CAMERA_SPEED * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Speed = CAMERA_SPEED * 0.10f;
    else
        Speed = CAMERA_SPEED;

    Camera->Position.X -= Speed * Move.X;
    Camera->Position.Z -= Speed * Move.Z;

    if( g_ClipCamera )
        ClipCamera( &Camera->Position );
}


//-----------------------------------------------------------------------------
// Name: CameraUp
// Desc: 
//-----------------------------------------------------------------------------

static void CameraUp( Camera_t *Camera )
{
    float Speed;

    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Speed = CAMERA_SPEED * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Speed = CAMERA_SPEED * 0.10f;
    else
        Speed = CAMERA_SPEED;

    Camera->Position.Y += Speed;

    if( g_ClipCamera )
        ClipCamera( &Camera->Position );
}


//-----------------------------------------------------------------------------
// Name: CameraDown
// Desc: 
//-----------------------------------------------------------------------------

static void CameraDown( Camera_t *Camera )
{
    float Speed;

    if( IsKeyHeldDown( DIK_LSHIFT ) || IsKeyHeldDown( DIK_RSHIFT ) )
        Speed = CAMERA_SPEED * 0.25f;
    else if( IsKeyHeldDown( DIK_LCONTROL ) || IsKeyHeldDown( DIK_RCONTROL ) )
        Speed = CAMERA_SPEED * 0.10f;
    else
        Speed = CAMERA_SPEED;

    Camera->Position.Y -= Speed;

    if( g_ClipCamera )
        ClipCamera( &Camera->Position );
}
