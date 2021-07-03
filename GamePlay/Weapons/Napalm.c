//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 6 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <math.h>
#include "ObjectTypes.h"
#include "GameTypes.h"
#include "LightSource.h"

#include "ShotManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "ExplosionManager.h"
#include "MeshManager.h"
#include "SoundManager.h"

#include "Projectile.h"

#include "GameExplosions.h"
#include "GameWeapons.h"
#include "GameSounds.h"

#include "ParticleDecalSmoke.h"
#include "ParticleFireball.h"
#include "ParticleDebrisGround.h"


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------
// Name: RollerBouncerThink()
// Desc: 
//----------------------------------------------------------------------------------------

void NapalmThink( Object_t *Projectile, size_t IsHiRes )
{
    Weapon_t *Weapon;
    Vector_t Position;
    size_t ParticleID;
    Vector_t Normal;

    UpdateShotShading( Projectile, IsHiRes );

    Projectile->Rotations[ 0 ].Y += 2.0f;
    Projectile->Rotations[ 0 ].Z += 1.0f;

    if( fabsf( VectorMagnitude( &Projectile->Physics.Move ) ) <= EPSILON )
    {
        GetObjectPosition( Projectile->ID, 0, &Position );

        Weapon = GetShotWeapon( Projectile->Data );

        ParticleID = StartFireball( &Position, Weapon == &WeaponNapalmSmall );
        StartNapalmExplosion( &Position, GetCurrentPlayer(), Weapon->ExplType, ParticleID );

        GetMeshNormal( g_LandscapeMesh, Position.X, Position.Z, &Normal );
        StartDebrisGround( &Position, &Normal );

        SetSoundPosition( SoundNapalm0, Position.X, Position.Y, Position.Z );
        PlaySound3D( SoundNapalm0, 0 );

        ReturnShot( Projectile->Data );
        KillObject( Projectile->ID );
    }

    SaveShotPosition(Projectile->Data, Projectile->Positions[0]);
}
