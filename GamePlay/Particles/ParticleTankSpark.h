//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


extern size_t CreateTankSpark( void );
extern void DeleteTankSpark( void );

extern size_t StartTankSpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) );
