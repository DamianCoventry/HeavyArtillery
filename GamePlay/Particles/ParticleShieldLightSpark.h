//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


extern size_t CreateShieldLightSpark( void );
extern void DeleteShieldLightSpark( void );

extern size_t StartShieldLightSpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) );
