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
#include <stdlib.h>
#include <math.h>

#include "ObjectTypes.h"
#include "LightSource.h"

#include "ShotManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "SoundManager.h"

#include "Projectile.h"
#include "Shoot.h"
#include "StateWaitForShot.h"

#include "GameWeapons.h"
#include "GameSounds.h"


//----------------------------------------------------------------------------------------
// Name: MIRVThink()
// Desc: 
//----------------------------------------------------------------------------------------

void MIRVThink( Object_t *Projectile, size_t IsHiRes )
{
    size_t i, ShotID;
    float Angle, Power;
    Vector_t ShootVector;

    FaceDirection( Projectile, IsHiRes );

    // split the shot into five large missiles when the shot reaches apogee

    if( Projectile->Physics.Move.Y <= 0.0f )
    {
        StopSmokeTrail( Projectile->Data );
        ReturnShot( Projectile->Data );
        KillObject( Projectile->ID );

        // create 5 large missiles
        VectorGetAnglesXY( &Projectile->Physics.Move, nullptr, &Angle );

        Power  = fabsf( VectorMagnitude( &Projectile->Physics.Move ) ) / SHOOT_POWERMODIFIER;
        Angle -= (float)( ( (size_t)WeaponMIRV.ProjectileCount / (size_t)2 ) * (size_t)WeaponMIRV.AngleIncrement );

        SetSoundPosition( SoundShotEmit0, Projectile->Positions[ 0 ].X, Projectile->Positions[ 0 ].Y, Projectile->Positions[ 0 ].Z );
        PlaySound3D( SoundShotEmit0, 0 );

        for( i = 0; i < WeaponMIRV.ProjectileCount; i++ )
        {
            VectorGetMoveYX( Angle * DTOR, Projectile->Physics.Move.X * DTOR, &ShootVector );
            ShotID = WeaponMissileLarge.Shoot( &WeaponMissileLarge, &Projectile->Positions[ 0 ], &ShootVector, Power, 0 );

            Angle += WeaponMIRV.AngleIncrement;

            if( i == WeaponMIRV.ProjectileCount / 2 )
                UpdateCameraToFollowShot( ShotID );
        }
    }
}
