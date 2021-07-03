//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 29 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include "Log.h"

#define GAMECONFIGFILE              "Game.bin"


//----------------------------------------------------------------------------------------
// private structures
//----------------------------------------------------------------------------------------

typedef struct GameConfig_s
{
    size_t UseShading;
    size_t LODSetting;
    size_t ParticleSetting;
    size_t LevelCount;
    size_t CashAtStart;
    size_t TileSetType;
}
GameConfig_t;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static GameConfig_t _GameConfig;


//----------------------------------------------------------------------------------------
// Name: LoadGameConfig()
// Desc: 
//----------------------------------------------------------------------------------------

size_t LoadGameConfig( void )
{
    FILE *File;
    errno_t Error;

    LogWrite( "LoadGameConfig() loading [%s]...\n", GAMECONFIGFILE );

    Error = fopen_s( &File, GAMECONFIGFILE, "rb" );
    if( Error != 0 )
    {
        LogWrite( "LoadGameConfig() failed to open/find [%s], will create it instead\n", GAMECONFIGFILE );
        return 0;
    }

    if( 1 != fread( &_GameConfig, sizeof( GameConfig_t ), 1, File ) )
    {
        fclose( File );
        return 0;
    }

    fclose( File );

    LogWrite( "LoadGameConfig() succeeded\n" );
    return 1;
}


//----------------------------------------------------------------------------------------
// Name: SaveGameConfig()
// Desc: 
//----------------------------------------------------------------------------------------

void SaveGameConfig( void )
{
    FILE *File;
    errno_t Error;

    LogWrite( "SaveGameConfig() saving...\n" );

    Error = fopen_s( &File, GAMECONFIGFILE, "wb" );
    if( Error == 0 )
    {
        fwrite( &_GameConfig, sizeof( GameConfig_t ), 1, File );
        fclose( File );
    }

    LogWrite( "SaveGameConfig() succeeded\n" );
}


//----------------------------------------------------------------------------------------
// Name: SetGameDefaults()
// Desc: 
//----------------------------------------------------------------------------------------

void SetGameDefaults( void )
{
    LogWrite( "SetGameDefaults() setting defaults...\n" );

    _GameConfig.UseShading      = 1;
    _GameConfig.LODSetting      = 2;
    _GameConfig.ParticleSetting = 2;

    _GameConfig.LevelCount  = 5;
    _GameConfig.CashAtStart = 100000;
    _GameConfig.TileSetType = 0;

    LogWrite( "SetGameDefaults() succeeded\n" );
}


//----------------------------------------------------------------------------------------
// Name: GetUseShading()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetUseShading( void )
{
    return _GameConfig.UseShading;
}


//----------------------------------------------------------------------------------------
// Name: SetUseShading()
// Desc: 
//----------------------------------------------------------------------------------------

void SetUseShading( size_t Value )
{
    _GameConfig.UseShading = Value;
}


//----------------------------------------------------------------------------------------
// Name: GetLODSetting()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetLODSetting( void )
{
    return _GameConfig.LODSetting;
}


//----------------------------------------------------------------------------------------
// Name: SetLODSetting()
// Desc: 
//----------------------------------------------------------------------------------------

void SetLODSetting(size_t Value )
{
    switch( Value )
    {
        case 0 : case 1 : case 2 :
            _GameConfig.LODSetting = Value;
            break;

        default :
            break;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetParticleSetting()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetParticleSetting( void )
{
    return _GameConfig.ParticleSetting;
}


//----------------------------------------------------------------------------------------
// Name: SetParticleSetting()
// Desc: 
//----------------------------------------------------------------------------------------

void SetParticleSetting(size_t Value )
{
    switch( Value )
    {
        case 0 : case 1 : case 2 :
            _GameConfig.ParticleSetting = Value;
            break;

        default :
            break;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetLevelCount()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetLevelCount( void )
{
    return _GameConfig.LevelCount;
}


//----------------------------------------------------------------------------------------
// Name: SetLevelCount()
// Desc: 
//----------------------------------------------------------------------------------------

void SetLevelCount( size_t Value )
{
    if( Value > 0 )
        _GameConfig.LevelCount = Value;
}


//----------------------------------------------------------------------------------------
// Name: GetCashAtStart()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetCashAtStart( void )
{
    return _GameConfig.CashAtStart;
}


//----------------------------------------------------------------------------------------
// Name: SetCashAtStart()
// Desc: 
//----------------------------------------------------------------------------------------

void SetCashAtStart( size_t Value )
{
    if( Value > 0 )
        _GameConfig.CashAtStart = Value;
}


//----------------------------------------------------------------------------------------
// Name: GetTileSetType()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetTileSetType( void )
{
    return _GameConfig.TileSetType;
}


//----------------------------------------------------------------------------------------
// Name: SetTileSetType()
// Desc: 
//----------------------------------------------------------------------------------------

void SetTileSetType(size_t Value )
{
    if( Value <= 7 )
        _GameConfig.TileSetType = Value;
}
