//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 29 June 2002
//
//========================================================================================


extern size_t LoadGameConfig( void );
extern void SaveGameConfig( void );
extern void SetGameDefaults( void );

size_t GetUseShading( void );
void SetUseShading(size_t Value );

size_t GetLODSetting( void );
void SetLODSetting(size_t Value );

size_t GetParticleSetting( void );
void SetParticleSetting(size_t Value );

size_t GetLevelCount( void );
void SetLevelCount(size_t Value );

size_t GetCashAtStart( void );
void SetCashAtStart( size_t Value );

size_t GetTileSetType( void );
void SetTileSetType(size_t Value );
