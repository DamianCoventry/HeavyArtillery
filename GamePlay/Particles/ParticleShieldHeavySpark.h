//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 17 July 2002
//
//========================================================================================


extern size_t CreateShieldHeavySpark( void );
extern void DeleteShieldHeavySpark( void );

extern size_t StartShieldHeavySpark( size_t ObjectID, void (*FinishCallback)( size_t ID ) );
