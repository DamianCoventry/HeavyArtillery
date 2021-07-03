//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <string.h>

#include "Error.h"

#include "Log.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern HWND g_hMainWindow;
extern HINSTANCE g_hInstance;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static IDirectInput8 *_DInput;
static IDirectInputDevice8 *_MouseDevice;
static IDirectInputDevice8 *_KeyboardDevice;

static char _Keys[ 256 ];
static char _OldKeys[ 256 ];

static DIMOUSESTATE _Mouse;
static DIMOUSESTATE _OldMouse;


//----------------------------------------------------------------------------------------
// Name: StartInputDevices()
// Desc: 
//----------------------------------------------------------------------------------------

size_t StartInputDevices( void )
{
    LogWrite( "StartInputDevices() starting...\n" );

    if( FAILED( DirectInput8Create( g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&_DInput, nullptr ) ) )
        return SetError( "StartInputDevices() failed to create a directinput 8 object" );

    // create the keyboard object
    if( FAILED( _DInput->CreateDevice( GUID_SysKeyboard, &_KeyboardDevice, nullptr ) ) )
        return SetError( "StartInputDevices() The call to CreateDevice() failed for the keyboard" );

    if( FAILED( _KeyboardDevice->SetDataFormat( &c_dfDIKeyboard ) ) )
        return SetError( "StartInputDevices() The call to SetDataFormat() failed for the keyboard" );

    if( FAILED( _KeyboardDevice->SetCooperativeLevel( g_hMainWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) )
        return SetError( "StartInputDevices() The call to SetCooperativeLevel() failed for the keyboard" );

    // create the mouse object
    if( FAILED( _DInput->CreateDevice( GUID_SysMouse, &_MouseDevice, nullptr ) ) )
        return SetError( "StartInputDevices() The call to CreateDevice() failed for the mouse" );

    if( FAILED( _MouseDevice->SetDataFormat( &c_dfDIMouse ) ) )
        return SetError( "StartInputDevices() The call to SetDataFormat() failed for the mouse" );

    if( FAILED( _MouseDevice->SetCooperativeLevel( g_hMainWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) )
        return SetError( "StartInputDevices() The call to SetCooperativeLevel() failed for the mouse" );

    LogWrite( "StartInputDevices() succeeded\n" );

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: StopInputDevices()
// Desc: 
//----------------------------------------------------------------------------------------

void StopInputDevices( void )
{
    LogWrite( "StopInputDevices() stopping...\n" );

    if( _DInput )
    {
        if( _KeyboardDevice )
        {
            _KeyboardDevice->Release();
            _KeyboardDevice = nullptr;
        }

        if( _MouseDevice )
        {
            _MouseDevice->Release();
            _MouseDevice = nullptr;
        }

        _DInput->Release();
        _DInput = nullptr;
    }

    LogWrite( "StopInputDevices() stopped\n" );
}


//-----------------------------------------------------------------------------
// Name: AcquireInputDevices()
// Desc: 
//-----------------------------------------------------------------------------

void AcquireInputDevices( void )
{
    LogWrite( "AcquireInputDevices() acquiring...\n" );

    if( _KeyboardDevice )
    {
        LogWrite( "AcquireInputDevices() acquiring keyboard...\n" );
        if( FAILED( _KeyboardDevice->Acquire() ) )
            LogWrite( "AcquireInputDevices() failed to acquire keyboard\n" );
        else
            LogWrite( "AcquireInputDevices() successfully acquired keyboard\n" );
    }
    else
        LogWrite( "AcquireInputDevices() keyboard isn't started yet\n" );

    if( _MouseDevice )
    {
        LogWrite( "AcquireInputDevices() acquiring mouse...\n" );
        if( FAILED( _MouseDevice->Acquire() ) )
            LogWrite( "AcquireInputDevices() failed to acquire mouse\n" );
        else
            LogWrite( "AcquireInputDevices() successfully acquired mouse\n" );
    }
    else
        LogWrite( "AcquireInputDevices() mouse isn't started yet\n" );

    LogWrite( "AcquireInputDevices() succeeded\n" );
}


//-----------------------------------------------------------------------------
// Name: UnacquireInputDevices()
// Desc: 
//-----------------------------------------------------------------------------

void UnacquireInputDevices( void )
{
    LogWrite( "UnacquireInputDevices() unacquiring...\n" );

    if( _KeyboardDevice )
        _KeyboardDevice->Unacquire();

    if( _MouseDevice )
        _MouseDevice->Unacquire();

    LogWrite( "UnacquireInputDevices() succeeded\n" );
}


//-----------------------------------------------------------------------------
// Name: PollInputDevices()
// Desc: 
//-----------------------------------------------------------------------------

void PollInputDevices( void )
{
    size_t Acquire = 0;

    if( DIERR_NOTACQUIRED == _KeyboardDevice->GetDeviceState( sizeof( char ) * 256, (void *)_Keys ) )
        Acquire = 1;

    if( DIERR_NOTACQUIRED == _MouseDevice->GetDeviceState( sizeof( DIMOUSESTATE ), (void *)&_Mouse ) )
        Acquire = 1;

    if( Acquire )
        AcquireInputDevices();
}


//-----------------------------------------------------------------------------
// Name: StoreInputData()
// Desc: 
//-----------------------------------------------------------------------------

void StoreInputData( void )
{
    memcpy( _OldKeys, _Keys, sizeof( char ) * 256 );
    memcpy( &_OldMouse, &_Mouse, sizeof( DIMOUSESTATE ) );
}


//-----------------------------------------------------------------------------
// Name: IsKeyPressed()
// Desc: 
//-----------------------------------------------------------------------------

size_t IsKeyPressed( size_t Key )
{
    return ( ( _Keys[ Key ] & 0x80 ) && !( _OldKeys[ Key ] & 0x80 ) );
}


//-----------------------------------------------------------------------------
// Name: IsKeyReleased()
// Desc: 
//-----------------------------------------------------------------------------

size_t IsKeyReleased( size_t Key )
{
    return ( !( _Keys[ Key ] & 0x80 ) && ( _OldKeys[ Key ] & 0x80 ) );
}


//-----------------------------------------------------------------------------
// Name: IsKeyHeldDown()
// Desc: 
//-----------------------------------------------------------------------------

size_t IsKeyHeldDown( size_t Key )
{
    return ( _Keys[ Key ] & 0x80 );
}


//-----------------------------------------------------------------------------
// Name: IsMButtonPressed()
// Desc: 
//-----------------------------------------------------------------------------

size_t IsMButtonPressed( size_t Button )
{
    return ( ( _Mouse.rgbButtons[ Button ] & 0x80 ) && !( _OldMouse.rgbButtons[ Button ] & 0x80 ) );
}


//-----------------------------------------------------------------------------
// Name: IsMButtonReleased()
// Desc: 
//-----------------------------------------------------------------------------

size_t IsMButtonReleased( size_t Button )
{
    return ( !( _Mouse.rgbButtons[ Button ] & 0x80 ) && ( _OldMouse.rgbButtons[ Button ] & 0x80 ) );
}


//-----------------------------------------------------------------------------
// Name: IsMButtonHeldDown()
// Desc: 
//-----------------------------------------------------------------------------

size_t IsMButtonHeldDown( size_t Button )
{
    return ( _Mouse.rgbButtons[ Button ] & 0x80 );
}


//-----------------------------------------------------------------------------
// Name: GetMouseMovement()
// Desc: 
//-----------------------------------------------------------------------------

void GetMouseMovement( int *X, int *Y )
{
    if( X )
        (*X) = _Mouse.lX;

    if( Y )
        (*Y) = _Mouse.lY;
}

