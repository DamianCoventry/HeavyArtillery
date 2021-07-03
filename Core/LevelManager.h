//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Sunday, 30 June 2002
//
//========================================================================================


extern void BeginNewGame( void );
extern void BeginNewLevel( void );
extern size_t EndLevel( void );

extern size_t IsEndOfLevel( void );

extern size_t GetCurrentLevel( void );
extern size_t GetLevelCount( void );

extern void GenerateNewPlayOrder( void );
extern size_t GetCurrentPlayer( void );
extern size_t GetCurrentPlayerIndex( void );
extern void SetNextPlayer( void );

extern void SetWinningTeam( size_t Team );
extern size_t GetWinningTeam( void );

extern size_t GetPlayOrderPlayer( size_t PlayOrderIndex );

extern void AwardEndOfLevelCash( void );

extern void UpdateAmbientSounds( void );
