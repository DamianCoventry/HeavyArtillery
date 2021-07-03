//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN     // we'll include these here so that the users
#include <windows.h>            // of this header file don't have to
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>


extern size_t StartInputDevices( void );
extern void StopInputDevices( void );

extern void AcquireInputDevices( void );
extern void UnacquireInputDevices( void );

extern void PollInputDevices( void );
extern void StoreInputData( void );

extern size_t IsKeyPressed( size_t Key );
extern size_t IsKeyReleased( size_t Key );
extern size_t IsKeyHeldDown( size_t Key );

extern size_t IsMButtonPressed( size_t Button );
extern size_t IsMButtonReleased( size_t Button );
extern size_t IsMButtonHeldDown( size_t Button );

extern void GetMouseMovement(int *X, int *Y );
