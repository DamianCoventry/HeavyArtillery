//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


extern size_t CreateShieldMediumSpark( void );
extern void DeleteShieldMediumSpark( void );

extern size_t StartShieldMediumSpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) );
