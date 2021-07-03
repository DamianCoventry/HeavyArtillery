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
#include "GameSounds.h"
#include "Sound.h"
#include "SoundManager.h"
#include "LoadingScreen.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

size_t SoundShootHitScan0;
size_t SoundShootMissile0;
size_t SoundShootBomb0;
size_t SoundShotEmit0;
size_t SoundNapalm0;
size_t SoundAirRaid0;
size_t SoundAeroplaneEngine0;
size_t SoundAeroplaneEmit0;
size_t SoundBombExplode0;
size_t SoundBombExplode1;
size_t SoundBombExplode2;
size_t SoundMissileExplode0;
size_t SoundMissileExplode1;
size_t SoundMissileExplode2;
size_t SoundTankExplode0;
size_t SoundTankExplode1;
size_t SoundTankExplode2;
size_t SoundHitScanExplode0;
size_t SoundGuidance0;
size_t SoundShieldUp0;
size_t SoundShieldDown0;
size_t SoundItemActivated0;
size_t SoundItemDeactivated0;
size_t SoundParachute0;
size_t SoundShieldShatter0;
size_t SoundTankHit0;
size_t SoundTankHit1;
size_t SoundShieldHit0;
size_t SoundShieldHit1;
size_t SoundShieldHit2;
size_t SoundTankImpact0;
size_t SoundTankImpact1;
size_t SoundObjectImpact0;
size_t SoundObjectImpact1;
size_t SoundApplause0;
size_t SoundTankEngineStart0;
size_t SoundTankEngine0;
size_t SoundAmbientNoises[ MAXAMBIENTNOISES ];

//-----------------------------------------------------------------------------
// Name: CreateSounds()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateSounds( void )
{
    if( !IsSoundAvailable() )
        return 1;

    LogWrite( "CreateSounds() loading...\n" );
    BeginLoadingScreen( TextureGame, "Loading Sounds...", 0, 47 );

    SoundShootHitScan0 = CreateSound( "WAV/ShootHitscan0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShootHitScan0 )
        return 0;

    UpdateLoadingScreen( 1 );

    SoundShootMissile0 = CreateSound( "WAV/ShootMissile0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShootMissile0 )
        return 0;

    UpdateLoadingScreen( 2 );

    SoundShootBomb0 = CreateSound( "WAV/ShootBomb0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShootBomb0 )
        return 0;

    UpdateLoadingScreen( 3 );

    SoundShotEmit0 = CreateSound( "WAV/ShotEmit0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShotEmit0 )
        return 0;

    UpdateLoadingScreen( 4 );

    SoundNapalm0 = CreateSound( "WAV/Napalm0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundNapalm0 )
        return 0;

    UpdateLoadingScreen( 5 );

    SoundAirRaid0 = CreateSound( "WAV/AirRaid0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAirRaid0 )
        return 0;

    UpdateLoadingScreen( 6 );

    SoundAeroplaneEngine0 = CreateSound( "WAV/AirPlaneEngine0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAeroplaneEngine0 )
        return 0;

    UpdateLoadingScreen( 7 );

    SoundAeroplaneEmit0 = CreateSound( "WAV/AirPlaneEmit0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAeroplaneEmit0 )
        return 0;

    UpdateLoadingScreen( 8 );

    SoundBombExplode0 = CreateSound( "WAV/BombExplode0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundBombExplode0 )
        return 0;

    UpdateLoadingScreen( 9 );

    SoundBombExplode1 = CreateSound( "WAV/BombExplode1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundBombExplode1 )
        return 0;

    UpdateLoadingScreen( 10 );

    SoundBombExplode2 = CreateSound( "WAV/BombExplode2.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundBombExplode2 )
        return 0;

    UpdateLoadingScreen( 11 );

    SoundMissileExplode0 = CreateSound( "WAV/MissileExplode0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundMissileExplode0 )
        return 0;

    UpdateLoadingScreen( 12 );

    SoundMissileExplode1 = CreateSound( "WAV/MissileExplode1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundMissileExplode1 )
        return 0;

    UpdateLoadingScreen( 13 );

    SoundMissileExplode2 = CreateSound( "WAV/MissileExplode2.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundMissileExplode2 )
        return 0;

    UpdateLoadingScreen( 14 );

    SoundTankExplode0 = CreateSound( "WAV/TankExplode0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankExplode0 )
        return 0;

    UpdateLoadingScreen( 15 );

    SoundTankExplode1 = CreateSound( "WAV/TankExplode1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankExplode1 )
        return 0;

    UpdateLoadingScreen( 16 );

    SoundTankExplode2 = CreateSound( "WAV/TankExplode2.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankExplode2 )
        return 0;

    UpdateLoadingScreen( 17 );

    SoundHitScanExplode0 = CreateSound( "WAV/HitScanExplode0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundHitScanExplode0 )
        return 0;

    UpdateLoadingScreen( 18 );

    SoundGuidance0 = CreateSound( "WAV/Guidance0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundGuidance0 )
        return 0;

    UpdateLoadingScreen( 19 );

    SoundShieldUp0 = CreateSound( "WAV/ShieldUp0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShieldUp0 )
        return 0;

    UpdateLoadingScreen( 20 );

    SoundShieldDown0 = CreateSound( "WAV/ShieldDown0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShieldDown0 )
        return 0;

    UpdateLoadingScreen( 21 );

    SoundItemActivated0 = CreateSound( "WAV/ItemActivate0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundItemActivated0 )
        return 0;

    UpdateLoadingScreen( 22 );

    SoundItemDeactivated0 = CreateSound( "WAV/ItemDeactivate0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundItemDeactivated0 )
        return 0;

    UpdateLoadingScreen( 23 );

    SoundParachute0 = CreateSound( "WAV/Parachute0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundParachute0 )
        return 0;

    UpdateLoadingScreen( 24 );

    SoundShieldShatter0 = CreateSound( "WAV/ShieldShatter0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShieldShatter0 )
        return 0;

    UpdateLoadingScreen( 25 );

    SoundTankHit0 = CreateSound( "WAV/TankHit0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankHit0 )
        return 0;

    UpdateLoadingScreen( 26 );

    SoundTankHit1 = CreateSound( "WAV/TankHit1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankHit1 )
        return 0;

    UpdateLoadingScreen( 27 );

    SoundShieldHit0 = CreateSound( "WAV/ShieldHit0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShieldHit0 )
        return 0;

    UpdateLoadingScreen( 28 );

    SoundShieldHit1 = CreateSound( "WAV/ShieldHit1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShieldHit1 )
        return 0;

    UpdateLoadingScreen( 29 );

    SoundShieldHit2 = CreateSound( "WAV/ShieldHit2.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundShieldHit2 )
        return 0;

    UpdateLoadingScreen( 30 );

    SoundTankImpact0 = CreateSound( "WAV/TankImpact0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankImpact0 )
        return 0;

    UpdateLoadingScreen( 31 );

    SoundTankImpact1 = CreateSound( "WAV/TankImpact1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankImpact1 )
        return 0;

    UpdateLoadingScreen( 32 );

    SoundObjectImpact0 = CreateSound( "WAV/ObjectImpact0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundObjectImpact0 )
        return 0;

    UpdateLoadingScreen( 33 );

    SoundObjectImpact1 = CreateSound( "WAV/ObjectImpact1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundObjectImpact1 )
        return 0;

    UpdateLoadingScreen( 34 );

    SoundApplause0 = CreateSound( "WAV/Applause0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundApplause0 )
        return 0;

    UpdateLoadingScreen( 35 );

    SoundAmbientNoises[ 0 ] = CreateSound( "WAV/AmbientNoise0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 0 ] )
        return 0;

    UpdateLoadingScreen( 36 );

    SoundAmbientNoises[ 1 ] = CreateSound( "WAV/AmbientNoise1.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 1 ] )
        return 0;

    UpdateLoadingScreen( 37 );

    SoundAmbientNoises[ 2 ] = CreateSound( "WAV/AmbientNoise2.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 2 ] )
        return 0;

    UpdateLoadingScreen( 38 );

    SoundAmbientNoises[ 3 ] = CreateSound( "WAV/AmbientNoise3.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 3 ] )
        return 0;

    UpdateLoadingScreen( 39 );

    SoundAmbientNoises[ 4 ] = CreateSound( "WAV/AmbientNoise4.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 4 ] )
        return 0;

    UpdateLoadingScreen( 40 );

    SoundAmbientNoises[ 5 ] = CreateSound( "WAV/AmbientNoise5.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 5 ] )
        return 0;

    UpdateLoadingScreen( 41 );

    SoundAmbientNoises[ 6 ] = CreateSound( "WAV/AmbientNoise6.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 6 ] )
        return 0;

    UpdateLoadingScreen( 42 );

    SoundAmbientNoises[ 7 ] = CreateSound( "WAV/AmbientNoise7.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 7 ] )
        return 0;

    UpdateLoadingScreen( 43 );

    SoundAmbientNoises[ 8 ] = CreateSound( "WAV/AmbientNoise8.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 8 ] )
        return 0;

    UpdateLoadingScreen( 44 );

    SoundAmbientNoises[ 9 ] = CreateSound( "WAV/AmbientNoise9.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundAmbientNoises[ 9 ] )
        return 0;

    UpdateLoadingScreen( 45 );

    SoundTankEngineStart0 = CreateSound( "WAV/TankEngineStart0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankEngineStart0 )
        return 0;

    UpdateLoadingScreen( 46 );

    SoundTankEngine0 = CreateSound( "WAV/TankEngine0.wav", 1000.0f, 1000000.0f );
    if( -1 == SoundTankEngine0 )
        return 0;

    EndLoadingScreen( "Loading Sounds...done", 47 );
    LogWrite( "CreateSounds() loaded\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteSounds()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteSounds( void )
{
    LogWrite( "DeleteSounds() unloading...\n" );

    DeleteSound( SoundTankEngine0 );
    DeleteSound( SoundTankEngineStart0 );
    DeleteSound( SoundAmbientNoises[ 9 ] );
    DeleteSound( SoundAmbientNoises[ 8 ] );
    DeleteSound( SoundAmbientNoises[ 7 ] );
    DeleteSound( SoundAmbientNoises[ 6 ] );
    DeleteSound( SoundAmbientNoises[ 5 ] );
    DeleteSound( SoundAmbientNoises[ 4 ] );
    DeleteSound( SoundAmbientNoises[ 3 ] );
    DeleteSound( SoundAmbientNoises[ 2 ] );
    DeleteSound( SoundAmbientNoises[ 1 ] );
    DeleteSound( SoundAmbientNoises[ 0 ] );
    DeleteSound( SoundApplause0 );
    DeleteSound( SoundObjectImpact1 );
    DeleteSound( SoundObjectImpact0 );
    DeleteSound( SoundTankImpact1 );
    DeleteSound( SoundTankImpact0 );
    DeleteSound( SoundShieldHit2 );
    DeleteSound( SoundShieldHit1 );
    DeleteSound( SoundShieldHit0 );
    DeleteSound( SoundTankHit1 );
    DeleteSound( SoundTankHit0 );
    DeleteSound( SoundShieldShatter0 );
    DeleteSound( SoundParachute0 );
    DeleteSound( SoundItemDeactivated0 );
    DeleteSound( SoundItemActivated0 );
    DeleteSound( SoundShieldDown0 );
    DeleteSound( SoundShieldUp0 );
    DeleteSound( SoundGuidance0 );
    DeleteSound( SoundHitScanExplode0 );
    DeleteSound( SoundTankExplode2 );
    DeleteSound( SoundTankExplode1 );
    DeleteSound( SoundTankExplode0 );
    DeleteSound( SoundMissileExplode2 );
    DeleteSound( SoundMissileExplode1 );
    DeleteSound( SoundMissileExplode0 );
    DeleteSound( SoundBombExplode2 );
    DeleteSound( SoundBombExplode1 );
    DeleteSound( SoundBombExplode0 );
    DeleteSound( SoundAeroplaneEmit0 );
    DeleteSound( SoundAeroplaneEngine0 );
    DeleteSound( SoundAirRaid0 );
    DeleteSound( SoundNapalm0 );
    DeleteSound( SoundShotEmit0 );
    DeleteSound( SoundShootBomb0 );
    DeleteSound( SoundShootMissile0 );
    DeleteSound( SoundShootHitScan0 );

    LogWrite( "DeleteSounds() unloaded\n" );
}
