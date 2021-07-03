//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 26 June 2002
//
//========================================================================================


#define MAXAMBIENTNOISES        10

//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern size_t SoundShootHitScan0;
extern size_t SoundShootMissile0;
extern size_t SoundShootBomb0;
extern size_t SoundShotEmit0;
extern size_t SoundNapalm0;
extern size_t SoundAirRaid0;
extern size_t SoundAeroplaneEngine0;
extern size_t SoundAeroplaneEmit0;
extern size_t SoundBombExplode0;
extern size_t SoundBombExplode1;
extern size_t SoundBombExplode2;
extern size_t SoundMissileExplode0;
extern size_t SoundMissileExplode1;
extern size_t SoundMissileExplode2;
extern size_t SoundTankExplode0;
extern size_t SoundTankExplode1;
extern size_t SoundTankExplode2;
extern size_t SoundHitScanExplode0;
extern size_t SoundGuidance0;
extern size_t SoundShieldUp0;
extern size_t SoundShieldDown0;
extern size_t SoundItemActivated0;
extern size_t SoundItemDeactivated0;
extern size_t SoundParachute0;
extern size_t SoundShieldShatter0;
extern size_t SoundTankHit0;
extern size_t SoundTankHit1;
extern size_t SoundShieldHit0;
extern size_t SoundShieldHit1;
extern size_t SoundShieldHit2;
extern size_t SoundTankImpact0;
extern size_t SoundTankImpact1;
extern size_t SoundObjectImpact0;
extern size_t SoundObjectImpact1;
extern size_t SoundApplause0;
extern size_t SoundTankEngineStart0;
extern size_t SoundTankEngine0;
extern size_t SoundAmbientNoises[ MAXAMBIENTNOISES ];


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

extern size_t CreateSounds( void );
extern void DeleteSounds( void );
