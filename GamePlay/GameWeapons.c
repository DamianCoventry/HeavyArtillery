//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "Dialog.h"
#include "WeaponTypes.h"
#include "LoadingScreen.h"

#include "GameModels.h"
#include "GameExplosions.h"
#include "GameSounds.h"

#include "Shoot.h"
#include "Object.h"

#include "Projectile.h"
#include "LeapFrog.h"
#include "MIRV.h"
#include "DeathsHead.h"
#include "FunkyBomb.h"
#include "AirStrike.h"
#include "Napalm.h"

#include "ParticleMissileSmoke.h"
#include "ParticleGeneralSpark.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

Weapon_t WeaponMissileSmall;
Weapon_t WeaponMissileLarge;
Weapon_t WeaponNukeSmall;
Weapon_t WeaponNukeLarge;
Weapon_t WeaponLeapFrogSmall;
Weapon_t WeaponLeapFrogLarge;
Weapon_t WeaponFunkyBomb;
Weapon_t WeaponMIRV;
Weapon_t WeaponDeathsHead;
Weapon_t WeaponRailgun;
Weapon_t WeaponNapalmSmall;
Weapon_t WeaponNapalmLarge;
Weapon_t WeaponRollerSmall;
Weapon_t WeaponRollerLarge;
Weapon_t WeaponBouncerSmall;
Weapon_t WeaponBouncerLarge;
Weapon_t WeaponAirStrikeSmall;
Weapon_t WeaponAirStrikeLarge;


//-----------------------------------------------------------------------------
// Name: CreateWeapons()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateWeapons( void )
{
    LogWrite( "CreateWeapons() creating...\n" );
    BeginLoadingScreen( TextureGame, "Creating Weapons...", 0, 18 );

    WeaponMissileSmall.Name                 = "Small Missile";
    WeaponMissileSmall.Price                = 0;
    WeaponMissileSmall.BundleSize           = 10;
    WeaponMissileSmall.Line0                = "This weapon has a single projectile";
    WeaponMissileSmall.Line1                = "that does minor damage. It detonates";
    WeaponMissileSmall.Line2                = "on impact as a single explosion with";
    WeaponMissileSmall.Line3                = "a small blast radius.";
    WeaponMissileSmall.ProjectileCount      = 1;
    WeaponMissileSmall.AeroplaneCount        = 0;
    WeaponMissileSmall.Scale                = 9.0f;
    WeaponMissileSmall.AngleIncrement       = 0.0f;
    WeaponMissileSmall.BounceReduction      = 0.0f;
    WeaponMissileSmall.ExplType             = &g_ExplosionSmall;
    WeaponMissileSmall.DecalID              = 0;        // TODO
    WeaponMissileSmall.StartSmokeTrail      = StartMissileSmoke;
    WeaponMissileSmall.StartSparkTrail      = StartGeneralSpark;
    WeaponMissileSmall.ParticleEmitterID    = 0;        // TODO
    WeaponMissileSmall.ProjectileModelIDLOD0= g_ModelMissileSmallLOD0;
    WeaponMissileSmall.ProjectileModelIDLOD1= g_ModelMissileSmallLOD1;
    WeaponMissileSmall.PortraitModelID      = g_ModelMissileSmallLOD0;
    WeaponMissileSmall.Shoot                = ShootProjectile;
    WeaponMissileSmall.CollideMesh          = CollideMeshStick;
    WeaponMissileSmall.OnThink              = FaceDirection;
    WeaponMissileSmall.OnCollideMesh        = CollideMeshExplode;
    WeaponMissileSmall.OnCollideObject      = CollideObjectExplode;
    WeaponMissileSmall.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 1 );

    WeaponMissileLarge.Name                 = "Large Missile";
    WeaponMissileLarge.Price                = 3500;
    WeaponMissileLarge.BundleSize           = 5;
    WeaponMissileLarge.Line0                = "This weapon has a single projectile";
    WeaponMissileLarge.Line1                = "that does average damage. It detonates";
    WeaponMissileLarge.Line2                = "on impact as a single explosion with";
    WeaponMissileLarge.Line3                = "an average sized blast radius.";
    WeaponMissileLarge.ProjectileCount      = 1;
    WeaponMissileLarge.AeroplaneCount        = 0;
    WeaponMissileLarge.Scale                = 9.0f;
    WeaponMissileLarge.AngleIncrement       = 0.0f;
    WeaponMissileLarge.BounceReduction      = 0.0f;
    WeaponMissileLarge.ExplType             = &g_ExplosionMedium;
    WeaponMissileLarge.DecalID              = 0;        // TODO
    WeaponMissileLarge.StartSmokeTrail      = StartMissileSmoke;
    WeaponMissileLarge.StartSparkTrail      = StartGeneralSpark;
    WeaponMissileLarge.ParticleEmitterID    = 0;        // TODO
    WeaponMissileLarge.ProjectileModelIDLOD0= g_ModelMissileLargeLOD0;
    WeaponMissileLarge.ProjectileModelIDLOD1= g_ModelMissileLargeLOD1;
    WeaponMissileLarge.PortraitModelID      = g_ModelMissileLargeLOD0;
    WeaponMissileLarge.Shoot                = ShootProjectile;
    WeaponMissileLarge.CollideMesh          = CollideMeshStick;
    WeaponMissileLarge.OnThink              = FaceDirection;
    WeaponMissileLarge.OnCollideMesh        = CollideMeshExplode;
    WeaponMissileLarge.OnCollideObject      = CollideObjectExplode;
    WeaponMissileLarge.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 2 );

    WeaponNukeSmall.Name                 = "Small Nuke";
    WeaponNukeSmall.Price                = 10000;
    WeaponNukeSmall.BundleSize           = 3;
    WeaponNukeSmall.Line0                = "This weapon has a single projectile";
    WeaponNukeSmall.Line1                = "that does a lot of damage. It detonates";
    WeaponNukeSmall.Line2                = "on impact as a single explosion with";
    WeaponNukeSmall.Line3                = "a large blast radius.";
    WeaponNukeSmall.ProjectileCount      = 1;
    WeaponNukeSmall.AeroplaneCount        = 0;
    WeaponNukeSmall.Scale                = 8.0f;
    WeaponNukeSmall.AngleIncrement       = 0.0f;
    WeaponNukeSmall.BounceReduction      = 0.0f;
    WeaponNukeSmall.ExplType             = &g_ExplosionLarge;
    WeaponNukeSmall.DecalID              = 0;        // TODO
    WeaponNukeSmall.StartSmokeTrail      = StartMissileSmoke;
    WeaponNukeSmall.StartSparkTrail      = StartGeneralSpark;
    WeaponNukeSmall.ParticleEmitterID    = 0;        // TODO
    WeaponNukeSmall.ProjectileModelIDLOD0= g_ModelNukeSmallLOD0;
    WeaponNukeSmall.ProjectileModelIDLOD1= g_ModelNukeSmallLOD1;
    WeaponNukeSmall.PortraitModelID      = g_ModelNukeSmallLOD0;
    WeaponNukeSmall.Shoot                = ShootProjectile;
    WeaponNukeSmall.CollideMesh          = CollideMeshStick;
    WeaponNukeSmall.OnThink              = FaceDirection;
    WeaponNukeSmall.OnCollideMesh        = CollideMeshExplode;
    WeaponNukeSmall.OnCollideObject      = CollideObjectExplode;
    WeaponNukeSmall.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 3 );

    WeaponNukeLarge.Name                 = "Large Nuke";
    WeaponNukeLarge.Price                = 12000;
    WeaponNukeLarge.BundleSize           = 1;
    WeaponNukeLarge.Line0                = "This weapon has a single projectile that";
    WeaponNukeLarge.Line1                = "does huge damage. It has a huge explosion";
    WeaponNukeLarge.Line2                = "that obliterates everything caught in the";
    WeaponNukeLarge.Line3                = "blast radius.";
    WeaponNukeLarge.ProjectileCount      = 1;
    WeaponNukeLarge.AeroplaneCount        = 0;
    WeaponNukeLarge.Scale                = 6.0f;
    WeaponNukeLarge.AngleIncrement       = 0.0f;
    WeaponNukeLarge.BounceReduction      = 0.0f;
    WeaponNukeLarge.ExplType             = &g_ExplosionHuge;
    WeaponNukeLarge.DecalID              = 0;        // TODO
    WeaponNukeLarge.StartSmokeTrail      = StartMissileSmoke;
    WeaponNukeLarge.StartSparkTrail      = StartGeneralSpark;
    WeaponNukeLarge.ParticleEmitterID    = 0;        // TODO
    WeaponNukeLarge.ProjectileModelIDLOD0= g_ModelNukeLargeLOD0;
    WeaponNukeLarge.ProjectileModelIDLOD1= g_ModelNukeLargeLOD1;
    WeaponNukeLarge.PortraitModelID      = g_ModelNukeLargeLOD0;
    WeaponNukeLarge.Shoot                = ShootProjectile;
    WeaponNukeLarge.CollideMesh          = CollideMeshStick;
    WeaponNukeLarge.OnThink              = FaceDirection;
    WeaponNukeLarge.OnCollideMesh        = CollideMeshExplode;
    WeaponNukeLarge.OnCollideObject      = CollideObjectExplode;
    WeaponNukeLarge.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 4 );

    WeaponLeapFrogSmall.Name                 = "Small Leap Frog";
    WeaponLeapFrogSmall.Price                = 10000;
    WeaponLeapFrogSmall.BundleSize           = 3;
    WeaponLeapFrogSmall.Line0                = "This weapon uses a Small Missile that on";
    WeaponLeapFrogSmall.Line1                = "impact fires another Small Missile that";
    WeaponLeapFrogSmall.Line2                = "leaps forward.  This pattern continues for";
    WeaponLeapFrogSmall.Line3                = "3 projectiles.";
    WeaponLeapFrogSmall.ProjectileCount      = 3;
    WeaponLeapFrogSmall.AeroplaneCount        = 0;
    WeaponLeapFrogSmall.Scale                = 9.0f;
    WeaponLeapFrogSmall.AngleIncrement       = 0.0f;
    WeaponLeapFrogSmall.BounceReduction      = 0.6f;
    WeaponLeapFrogSmall.ExplType             = &g_ExplosionSmall;
    WeaponLeapFrogSmall.DecalID              = 0;        // TODO
    WeaponLeapFrogSmall.StartSmokeTrail      = StartMissileSmoke;
    WeaponLeapFrogSmall.StartSparkTrail      = StartGeneralSpark;
    WeaponLeapFrogSmall.ParticleEmitterID    = 0;        // TODO
    WeaponLeapFrogSmall.ProjectileModelIDLOD0= g_ModelMissileSmallLOD0;
    WeaponLeapFrogSmall.ProjectileModelIDLOD1= g_ModelMissileSmallLOD1;
    WeaponLeapFrogSmall.PortraitModelID      = g_ModelMissileSmallLOD0;
    WeaponLeapFrogSmall.Shoot                = ShootProjectile;
    WeaponLeapFrogSmall.CollideMesh          = CollideMeshStick;
    WeaponLeapFrogSmall.OnThink              = FaceDirection;
    WeaponLeapFrogSmall.OnCollideMesh        = CollideMeshLeapFrog;
    WeaponLeapFrogSmall.OnCollideObject      = CollideObjectLeapFrog;
    WeaponLeapFrogSmall.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 5 );

    WeaponLeapFrogLarge.Name                 = "Large Leap Frog";
    WeaponLeapFrogLarge.Price                = 15000;
    WeaponLeapFrogLarge.BundleSize           = 2;
    WeaponLeapFrogLarge.Line0                = "This weapon uses a Large Missile that on";
    WeaponLeapFrogLarge.Line1                = "impact fires another Large Missile that leaps";
    WeaponLeapFrogLarge.Line2                = "forward.  This pattern continues for 5";
    WeaponLeapFrogLarge.Line3                = "projectiles.";
    WeaponLeapFrogLarge.ProjectileCount      = 5;
    WeaponLeapFrogLarge.AeroplaneCount        = 0;
    WeaponLeapFrogLarge.Scale                = 9.0f;
    WeaponLeapFrogLarge.AngleIncrement       = 0.0f;
    WeaponLeapFrogLarge.BounceReduction      = 0.5f;
    WeaponLeapFrogLarge.ExplType             = &g_ExplosionMedium;
    WeaponLeapFrogLarge.DecalID              = 0;        // TODO
    WeaponLeapFrogLarge.StartSmokeTrail      = StartMissileSmoke;
    WeaponLeapFrogLarge.StartSparkTrail      = StartGeneralSpark;
    WeaponLeapFrogLarge.ParticleEmitterID    = 0;        // TODO
    WeaponLeapFrogLarge.ProjectileModelIDLOD0= g_ModelMissileLargeLOD0;
    WeaponLeapFrogLarge.ProjectileModelIDLOD1= g_ModelMissileLargeLOD1;
    WeaponLeapFrogLarge.PortraitModelID      = g_ModelMissileLargeLOD0;
    WeaponLeapFrogLarge.Shoot                = ShootProjectile;
    WeaponLeapFrogLarge.CollideMesh          = CollideMeshStick;
    WeaponLeapFrogLarge.OnThink              = FaceDirection;
    WeaponLeapFrogLarge.OnCollideMesh        = CollideMeshLeapFrog;
    WeaponLeapFrogLarge.OnCollideObject      = CollideObjectLeapFrog;
    WeaponLeapFrogLarge.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 6 );

    WeaponFunkyBomb.Name                 = "Funky Bomb";
    WeaponFunkyBomb.Price                = 7000;
    WeaponFunkyBomb.BundleSize           = 2;
    WeaponFunkyBomb.Line0                = "This weapon is a multi-projectile weapon";
    WeaponFunkyBomb.Line1                = "that on impact spurts random bursts of Small";
    WeaponFunkyBomb.Line2                = "and Large Missiles, and randomly chooses";
    WeaponFunkyBomb.Line3                = "whether or not to emit another Funky Bomb.";
    WeaponFunkyBomb.ProjectileCount      = 3;
    WeaponFunkyBomb.AeroplaneCount        = 0;
    WeaponFunkyBomb.Scale                = 4.0f;
    WeaponFunkyBomb.AngleIncrement       = 0.0f;
    WeaponFunkyBomb.BounceReduction      = 0.0f;
    WeaponFunkyBomb.ExplType             = &g_ExplosionMedium;
    WeaponFunkyBomb.DecalID              = 0;        // TODO
    WeaponFunkyBomb.StartSmokeTrail      = 0;        // TODO
    WeaponFunkyBomb.ParticleEmitterID    = 0;        // TODO
    WeaponFunkyBomb.ProjectileModelIDLOD0= g_ModelFunkyBombLOD0;
    WeaponFunkyBomb.ProjectileModelIDLOD1= g_ModelFunkyBombLOD1;
    WeaponFunkyBomb.PortraitModelID      = g_ModelFunkyBombLOD0;
    WeaponFunkyBomb.Shoot                = ShootProjectile;
    WeaponFunkyBomb.CollideMesh          = CollideMeshStick;
    WeaponFunkyBomb.OnThink              = FunkyBombThink;
    WeaponFunkyBomb.OnCollideMesh        = FunkyBombMeshExplode;
    WeaponFunkyBomb.OnCollideObject      = FunkyBombObjectExplode;
    WeaponFunkyBomb.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 7 );

    WeaponMIRV.Name                 = "MIRV";
    WeaponMIRV.Price                = 15000;
    WeaponMIRV.BundleSize           = 3;
    WeaponMIRV.Line0                = "This is a multi-projectile weapon that";
    WeaponMIRV.Line1                = "splits into 5 Large Missiles at apogee.";
    WeaponMIRV.Line2                = "One projectile continues foward and two";
    WeaponMIRV.Line3                = "split left and right at 15 degree increments.";
    WeaponMIRV.ProjectileCount      = 5;
    WeaponMIRV.AeroplaneCount        = 0;
    WeaponMIRV.Scale                = 9.0f;
    WeaponMIRV.AngleIncrement       = 15.0f;
    WeaponMIRV.BounceReduction      = 0.0f;
    WeaponMIRV.ExplType             = &g_ExplosionMedium;
    WeaponMIRV.DecalID              = 0;        // TODO
    WeaponMIRV.StartSmokeTrail      = StartMissileSmoke;
    WeaponMIRV.StartSparkTrail      = StartGeneralSpark;
    WeaponMIRV.ParticleEmitterID    = 0;        // TODO
    WeaponMIRV.ProjectileModelIDLOD0= g_ModelMissileLargeLOD0;
    WeaponMIRV.ProjectileModelIDLOD1= g_ModelMissileLargeLOD1;
    WeaponMIRV.PortraitModelID      = g_ModelMissileLargeLOD0;
    WeaponMIRV.Shoot                = ShootProjectile;
    WeaponMIRV.CollideMesh          = CollideMeshStick;
    WeaponMIRV.OnThink              = MIRVThink;
    WeaponMIRV.OnCollideMesh        = CollideMeshExplode;
    WeaponMIRV.OnCollideObject      = CollideObjectExplode;
    WeaponMIRV.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 8 );

    WeaponDeathsHead.Name                 = "Deaths Head";
    WeaponDeathsHead.Price                = 20000;
    WeaponDeathsHead.BundleSize           = 1;
    WeaponDeathsHead.Line0                = "This is a multi-projectile weapon that splits";
    WeaponDeathsHead.Line1                = "into 9 Large Nukes at apogee. One projectile";
    WeaponDeathsHead.Line2                = "continues forward and 4 go either side at 25";
    WeaponDeathsHead.Line3                = "degree increments. Use with caution.";
    WeaponDeathsHead.ProjectileCount      = 9;
    WeaponDeathsHead.AeroplaneCount        = 0;
    WeaponDeathsHead.Scale                = 6.0f;
    WeaponDeathsHead.AngleIncrement       = 25.0f;
    WeaponDeathsHead.BounceReduction      = 0.0f;
    WeaponDeathsHead.ExplType             = &g_ExplosionHuge;
    WeaponDeathsHead.DecalID              = 0;        // TODO
    WeaponDeathsHead.StartSmokeTrail      = StartMissileSmoke;
    WeaponDeathsHead.StartSparkTrail      = StartGeneralSpark;
    WeaponDeathsHead.ParticleEmitterID    = 0;        // TODO
    WeaponDeathsHead.ProjectileModelIDLOD0= g_ModelNukeLargeLOD0;
    WeaponDeathsHead.ProjectileModelIDLOD1= g_ModelNukeLargeLOD1;
    WeaponDeathsHead.PortraitModelID      = g_ModelNukeLargeLOD0;
    WeaponDeathsHead.Shoot                = ShootProjectile;
    WeaponDeathsHead.CollideMesh          = CollideMeshStick;
    WeaponDeathsHead.OnThink              = DeathsHeadThink;
    WeaponDeathsHead.OnCollideMesh        = CollideMeshExplode;
    WeaponDeathsHead.OnCollideObject      = CollideObjectExplode;
    WeaponDeathsHead.ShootSoundID         = SoundShootMissile0;
    UpdateLoadingScreen( 9 );

    WeaponRailgun.Name                 = "Railgun";
    WeaponRailgun.Price                = 8500;
    WeaponRailgun.BundleSize           = 3;
    WeaponRailgun.Line0                = "This weapon fires a single beam of energy";
    WeaponRailgun.Line1                = "in a straight line. This weapon features";
    WeaponRailgun.Line2                = "pin-point accuracy and is useful for line";
    WeaponRailgun.Line3                = "of sight shots.";
    WeaponRailgun.ProjectileCount      = 1;
    WeaponRailgun.AeroplaneCount        = 0;
    WeaponRailgun.Scale                = 8.0f;
    WeaponRailgun.AngleIncrement       = 0.0f;
    WeaponRailgun.BounceReduction      = 0.0f;
    WeaponRailgun.ExplType             = &g_ExplosionTiny;
    WeaponRailgun.DecalID              = 0;        // TODO
    WeaponRailgun.StartSmokeTrail      = 0;        // TODO
    WeaponRailgun.ParticleEmitterID    = 0;        // TODO
    WeaponRailgun.ProjectileModelIDLOD0= g_ModelRailgun;
    WeaponRailgun.ProjectileModelIDLOD1= 0;
    WeaponRailgun.PortraitModelID      = g_ModelRailgun;
    WeaponRailgun.Shoot                = ShootHitScan;
    WeaponRailgun.CollideMesh          = nullptr;
    WeaponRailgun.OnThink              = RailgunThink;
    WeaponRailgun.OnCollideMesh        = nullptr;
    WeaponRailgun.OnCollideObject      = nullptr;
    WeaponRailgun.ShootSoundID         = SoundShootHitScan0;
    UpdateLoadingScreen( 10 );

    WeaponNapalmSmall.Name                 = "Small Napalm";
    WeaponNapalmSmall.Price                = 6000;
    WeaponNapalmSmall.BundleSize           = 5;
    WeaponNapalmSmall.Line0                = "This weapon fires a small barrell of";
    WeaponNapalmSmall.Line1                = "hot napalm goo.  When the barrell hits";
    WeaponNapalmSmall.Line2                = "the ground it explodes into globs of";
    WeaponNapalmSmall.Line3                = "napalm which cause damage for 2 seconds.";
    WeaponNapalmSmall.ProjectileCount      = 1;
    WeaponNapalmSmall.AeroplaneCount        = 0;
    WeaponNapalmSmall.Scale                = 8.0f;
    WeaponNapalmSmall.AngleIncrement       = 0.0f;
    WeaponNapalmSmall.BounceReduction      = 0.50f;
    WeaponNapalmSmall.ExplType             = &g_ExplosionTiny;
    WeaponNapalmSmall.DecalID              = 0;        // TODO
    WeaponNapalmSmall.StartSmokeTrail      = 0;        // TODO
    WeaponNapalmSmall.ParticleEmitterID    = 0;        // TODO
    WeaponNapalmSmall.ProjectileModelIDLOD0= g_ModelNapalmSmallLOD0;
    WeaponNapalmSmall.ProjectileModelIDLOD1= g_ModelNapalmSmallLOD1;
    WeaponNapalmSmall.PortraitModelID      = g_ModelNapalmSmallLOD0;
    WeaponNapalmSmall.Shoot                = ShootProjectile;
    WeaponNapalmSmall.CollideMesh          = CollideMeshBounce;
    WeaponNapalmSmall.OnThink              = NapalmThink;
    WeaponNapalmSmall.OnCollideMesh        = nullptr;
    WeaponNapalmSmall.OnCollideObject      = CollideObjectBounce;
    WeaponNapalmSmall.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 11 );

    WeaponNapalmLarge.Name                 = "Large Napalm";
    WeaponNapalmLarge.Price                = 8000;
    WeaponNapalmLarge.BundleSize           = 3;
    WeaponNapalmLarge.Line0                = "This weapon fires a large barrell of";
    WeaponNapalmLarge.Line1                = "hot napalm goo.  When the barrell hits";
    WeaponNapalmLarge.Line2                = "the ground it explodes into globs of";
    WeaponNapalmLarge.Line3                = "napalm which cause damage for 4 seconds.";
    WeaponNapalmLarge.ProjectileCount      = 1;
    WeaponNapalmLarge.AeroplaneCount        = 0;
    WeaponNapalmLarge.Scale                = 8.0f;
    WeaponNapalmLarge.AngleIncrement       = 0.0f;
    WeaponNapalmLarge.BounceReduction      = 0.55f;
    WeaponNapalmLarge.ExplType             = &g_ExplosionTiny;
    WeaponNapalmLarge.DecalID              = 0;        // TODO
    WeaponNapalmLarge.StartSmokeTrail      = 0;        // TODO
    WeaponNapalmLarge.ParticleEmitterID    = 0;        // TODO
    WeaponNapalmLarge.ProjectileModelIDLOD0= g_ModelNapalmLargeLOD0;
    WeaponNapalmLarge.ProjectileModelIDLOD1= g_ModelNapalmLargeLOD1;
    WeaponNapalmLarge.PortraitModelID      = g_ModelNapalmLargeLOD0;
    WeaponNapalmLarge.Shoot                = ShootProjectile;
    WeaponNapalmLarge.CollideMesh          = CollideMeshBounce;
    WeaponNapalmLarge.OnThink              = NapalmThink;
    WeaponNapalmLarge.OnCollideMesh        = nullptr;
    WeaponNapalmLarge.OnCollideObject      = CollideObjectBounce;
    WeaponNapalmLarge.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 12 );

    WeaponRollerSmall.Name                 = "Small Roller";
    WeaponRollerSmall.Price                = 5000;
    WeaponRollerSmall.BundleSize           = 10;
    WeaponRollerSmall.Line0                = "This weapon has a single projectile that";
    WeaponRollerSmall.Line1                = "on impact rolls.  When the projectile";
    WeaponRollerSmall.Line2                = "comes to rest it detonates equivalent to";
    WeaponRollerSmall.Line3                = "a Small Missile.";
    WeaponRollerSmall.ProjectileCount      = 1;
    WeaponRollerSmall.AeroplaneCount        = 0;
    WeaponRollerSmall.Scale                = 9.0f;
    WeaponRollerSmall.AngleIncrement       = 0.0f;
    WeaponRollerSmall.BounceReduction      = 0.0f;
    WeaponRollerSmall.ExplType             = &g_ExplosionSmall;
    WeaponRollerSmall.DecalID              = 0;        // TODO
    WeaponRollerSmall.StartSmokeTrail      = 0;        // TODO
    WeaponRollerSmall.ParticleEmitterID    = 0;        // TODO
    WeaponRollerSmall.ProjectileModelIDLOD0= g_ModelRollerSmallLOD0;
    WeaponRollerSmall.ProjectileModelIDLOD1= g_ModelRollerSmallLOD1;
    WeaponRollerSmall.PortraitModelID      = g_ModelRollerSmallLOD0;
    WeaponRollerSmall.Shoot                = ShootProjectile;
    WeaponRollerSmall.CollideMesh          = CollideMeshSlide;
    WeaponRollerSmall.OnThink              = RollerBouncerThink;
    WeaponRollerSmall.OnCollideMesh        = nullptr;
    WeaponRollerSmall.OnCollideObject      = CollideObjectBounce;
    WeaponRollerSmall.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 13 );

    WeaponRollerLarge.Name                 = "Large Roller";
    WeaponRollerLarge.Price                = 7500;
    WeaponRollerLarge.BundleSize           = 6;
    WeaponRollerLarge.Line0                = "This weapon has a single projectile that";
    WeaponRollerLarge.Line1                = "on impact rolls.  When the projectile comes";
    WeaponRollerLarge.Line2                = "to rest it detonates equivalent to Large";
    WeaponRollerLarge.Line3                = "Missile.";
    WeaponRollerLarge.ProjectileCount      = 1;
    WeaponRollerLarge.AeroplaneCount        = 0;
    WeaponRollerLarge.Scale                = 9.0f;
    WeaponRollerLarge.AngleIncrement       = 0.0f;
    WeaponRollerLarge.BounceReduction      = 0.0f;
    WeaponRollerLarge.ExplType             = &g_ExplosionMedium;
    WeaponRollerLarge.DecalID              = 0;        // TODO
    WeaponRollerLarge.StartSmokeTrail      = 0;        // TODO
    WeaponRollerLarge.ParticleEmitterID    = 0;        // TODO
    WeaponRollerLarge.ProjectileModelIDLOD0= g_ModelRollerLargeLOD0;
    WeaponRollerLarge.ProjectileModelIDLOD1= g_ModelRollerLargeLOD1;
    WeaponRollerLarge.PortraitModelID      = g_ModelRollerLargeLOD0;
    WeaponRollerLarge.Shoot                = ShootProjectile;
    WeaponRollerLarge.CollideMesh          = CollideMeshSlide;
    WeaponRollerLarge.OnThink              = RollerBouncerThink;
    WeaponRollerLarge.OnCollideMesh        = nullptr;
    WeaponRollerLarge.OnCollideObject      = CollideObjectBounce;
    WeaponRollerLarge.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 14 );

    WeaponBouncerSmall.Name                 = "Small Bouncer";
    WeaponBouncerSmall.Price                = 7000;
    WeaponBouncerSmall.BundleSize           = 10;
    WeaponBouncerSmall.Line0                = "This weapon has a single projectile that on";
    WeaponBouncerSmall.Line1                = "impact bounces.  When the projectile comes";
    WeaponBouncerSmall.Line2                = "to rest it detonates equivalent to a Small";
    WeaponBouncerSmall.Line3                = "Missile.";
    WeaponBouncerSmall.ProjectileCount      = 1;
    WeaponBouncerSmall.AeroplaneCount        = 0;
    WeaponBouncerSmall.Scale                = 9.0f;
    WeaponBouncerSmall.AngleIncrement       = 0.0f;
    WeaponBouncerSmall.BounceReduction      = 0.6f;
    WeaponBouncerSmall.ExplType             = &g_ExplosionSmall;
    WeaponBouncerSmall.DecalID              = 0;        // TODO
    WeaponBouncerSmall.StartSmokeTrail      = 0;        // TODO
    WeaponBouncerSmall.ParticleEmitterID    = 0;        // TODO
    WeaponBouncerSmall.ProjectileModelIDLOD0= g_ModelRollerSmallLOD0;
    WeaponBouncerSmall.ProjectileModelIDLOD1= g_ModelRollerSmallLOD1;
    WeaponBouncerSmall.PortraitModelID      = g_ModelRollerSmallLOD0;
    WeaponBouncerSmall.Shoot                = ShootProjectile;
    WeaponBouncerSmall.CollideMesh          = CollideMeshSlide;
    WeaponBouncerSmall.OnThink              = RollerBouncerThink;
    WeaponBouncerSmall.OnCollideMesh        = nullptr;
    WeaponBouncerSmall.OnCollideObject      = CollideObjectBounce;
    WeaponBouncerSmall.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 15 );

    WeaponBouncerLarge.Name                 = "Large Bouncer";
    WeaponBouncerLarge.Price                = 11000;
    WeaponBouncerLarge.BundleSize           = 5;
    WeaponBouncerLarge.Line0                = "This weapon has a single projectile that on";
    WeaponBouncerLarge.Line1                = "impact bounces.  When the projectile comes";
    WeaponBouncerLarge.Line2                = "to rest it detonates equivalent to a Large";
    WeaponBouncerLarge.Line3                = "Missile.";
    WeaponBouncerLarge.ProjectileCount      = 1;
    WeaponBouncerLarge.AeroplaneCount        = 0;
    WeaponBouncerLarge.Scale                = 9.0f;
    WeaponBouncerLarge.AngleIncrement       = 0.0f;
    WeaponBouncerLarge.BounceReduction      = 0.5f;
    WeaponBouncerLarge.ExplType             = &g_ExplosionMedium;
    WeaponBouncerLarge.DecalID              = 0;        // TODO
    WeaponBouncerLarge.StartSmokeTrail      = 0;        // TODO
    WeaponBouncerLarge.ParticleEmitterID    = 0;        // TODO
    WeaponBouncerLarge.ProjectileModelIDLOD0= g_ModelRollerLargeLOD0;
    WeaponBouncerLarge.ProjectileModelIDLOD1= g_ModelRollerLargeLOD1;
    WeaponBouncerLarge.PortraitModelID      = g_ModelRollerLargeLOD0;
    WeaponBouncerLarge.Shoot                = ShootProjectile;
    WeaponBouncerLarge.CollideMesh          = CollideMeshBounce;
    WeaponBouncerLarge.OnThink              = RollerBouncerThink;
    WeaponBouncerLarge.OnCollideMesh        = nullptr;
    WeaponBouncerLarge.OnCollideObject      = CollideObjectBounce;
    WeaponBouncerLarge.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 16 );

    WeaponAirStrikeSmall.Name                 = "Small Air Strike";
    WeaponAirStrikeSmall.Price                = 8500;
    WeaponAirStrikeSmall.BundleSize           = 5;
    WeaponAirStrikeSmall.Line0                = "This weapon has a single projectile that";
    WeaponAirStrikeSmall.Line1                = "on impact bounces.  When at rest 4 planes";
    WeaponAirStrikeSmall.Line2                = "will fly in and bomb the area of the";
    WeaponAirStrikeSmall.Line3                = "projectile with 3 Small Missiles Each.";
    WeaponAirStrikeSmall.ProjectileCount      = 3;
    WeaponAirStrikeSmall.AeroplaneCount        = 3;
    WeaponAirStrikeSmall.Scale                = 9.0f;
    WeaponAirStrikeSmall.AngleIncrement       = 0.0f;
    WeaponAirStrikeSmall.BounceReduction      = 0.50f;
    WeaponAirStrikeSmall.ExplType             = &g_ExplosionTiny;
    WeaponAirStrikeSmall.DecalID              = 0;        // TODO
    WeaponAirStrikeSmall.StartSmokeTrail      = 0;        // TODO
    WeaponAirStrikeSmall.ParticleEmitterID    = 0;        // TODO
    WeaponAirStrikeSmall.ProjectileModelIDLOD0= g_ModelAirStrikeSmallLOD0;
    WeaponAirStrikeSmall.ProjectileModelIDLOD1= g_ModelAirStrikeSmallLOD1;
    WeaponAirStrikeSmall.PortraitModelID      = g_ModelAirStrikeSmallLOD0;
    WeaponAirStrikeSmall.Shoot                = ShootProjectile;
    WeaponAirStrikeSmall.CollideMesh          = CollideMeshBounce;
    WeaponAirStrikeSmall.OnThink              = AirStrikeThink;
    WeaponAirStrikeSmall.OnCollideMesh        = nullptr;
    WeaponAirStrikeSmall.OnCollideObject      = CollideObjectBounce;
    WeaponAirStrikeSmall.ShootSoundID         = SoundShootBomb0;
    UpdateLoadingScreen( 17 );

    WeaponAirStrikeLarge.Name                 = "Large Air Strike";
    WeaponAirStrikeLarge.Price                = 12000;
    WeaponAirStrikeLarge.BundleSize           = 2;
    WeaponAirStrikeLarge.Line0                = "This weapon has a single projectile that";
    WeaponAirStrikeLarge.Line1                = "on impact bounces.  When at rest 7 planes";
    WeaponAirStrikeLarge.Line2                = "will fly in and bomb the area of the";
    WeaponAirStrikeLarge.Line3                = "projectile with 3 Large Missiles Each.";
    WeaponAirStrikeLarge.ProjectileCount      = 3;
    WeaponAirStrikeLarge.AeroplaneCount        = 5;
    WeaponAirStrikeLarge.Scale                = 9.0f;
    WeaponAirStrikeLarge.AngleIncrement       = 0.0f;
    WeaponAirStrikeLarge.BounceReduction      = 0.55f;
    WeaponAirStrikeLarge.ExplType             = &g_ExplosionTiny;
    WeaponAirStrikeLarge.DecalID              = 0;        // TODO
    WeaponAirStrikeLarge.StartSmokeTrail      = 0;        // TODO
    WeaponAirStrikeLarge.ParticleEmitterID    = 0;        // TODO
    WeaponAirStrikeLarge.ProjectileModelIDLOD0= g_ModelAirStrikeLargeLOD0;
    WeaponAirStrikeLarge.ProjectileModelIDLOD1= g_ModelAirStrikeLargeLOD1;
    WeaponAirStrikeLarge.PortraitModelID      = g_ModelAirStrikeLargeLOD0;
    WeaponAirStrikeLarge.Shoot                = ShootProjectile;
    WeaponAirStrikeLarge.CollideMesh          = CollideMeshBounce;
    WeaponAirStrikeLarge.OnThink              = AirStrikeThink;
    WeaponAirStrikeLarge.OnCollideMesh        = nullptr;
    WeaponAirStrikeLarge.OnCollideObject      = CollideObjectBounce;
    WeaponAirStrikeLarge.ShootSoundID         = SoundShootBomb0;

    EndLoadingScreen( "Creating Weapons...Done", 18 );
    LogWrite( "CreateWeapons() created\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteWeapons()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteWeapons( void )
{
    LogWrite( "DeleteWeapons() deleting...\n" );

    LogWrite( "DeleteWeapons() deleted\n" );
}


//-----------------------------------------------------------------------------
// Name: DeleteWeapons()
// Desc: 
//-----------------------------------------------------------------------------

void AddGameWeaponsToListBox( Dialog_t *ListBox )
{
    DialogSend( ListBox, LB_RESETCONTENT, 0, 0 );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponMissileLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)0, &WeaponMissileLarge );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponNukeSmall.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)1, &WeaponNukeSmall );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponNukeLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)2, &WeaponNukeLarge );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponLeapFrogSmall.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)3, &WeaponLeapFrogSmall );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponLeapFrogLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)4, &WeaponLeapFrogLarge );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponFunkyBomb.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)5, &WeaponFunkyBomb );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponMIRV.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)6, &WeaponMIRV );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponDeathsHead.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)7, &WeaponDeathsHead );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponRailgun.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)8, &WeaponRailgun );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponNapalmSmall.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)9, &WeaponNapalmSmall );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponNapalmLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)10, &WeaponNapalmLarge );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponRollerSmall.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)11, &WeaponRollerSmall );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponRollerLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)12, &WeaponRollerLarge );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponBouncerSmall.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)13, &WeaponBouncerSmall );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponBouncerLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)14, &WeaponBouncerLarge );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponAirStrikeSmall.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)15, &WeaponAirStrikeSmall );

    DialogSend( ListBox, LB_ADDSTRING, (void*)&WeaponAirStrikeLarge.Name, nullptr );
    DialogSend( ListBox, LB_SETITEMDATA, (void*)16, &WeaponAirStrikeLarge );
}
