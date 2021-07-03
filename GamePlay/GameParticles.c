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
#include "ParticleMissileSmoke.h"
#include "ParticleGeneralSpark.h"
#include "ParticleDecalSmoke.h"
#include "ParticleFireball.h"
#include "ParticleAirStrikeBeacon.h"
#include "ParticleDamageSmoke.h"
#include "ParticleTankSpark.h"
#include "ParticleShieldLightSpark.h"
#include "ParticleShieldMediumSpark.h"
#include "ParticleShieldHeavySpark.h"
#include "ParticleGroundSmoke.h"
#include "ParticleRailgun.h"
#include "ParticleRailgunInner.h"
#include "ParticleSnowflake.h"
#include "ParticleCentralExplosion.h"
#include "ParticleExplFireball.h"
#include "ParticleExplSparks.h"
#include "ParticleShieldLightDebris.h"
#include "ParticleShieldMediumDebris.h"
#include "ParticleShieldHeavyDebris.h"
#include "ParticleDebrisGround.h"
#include "ParticleDebrisTank.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// Name: CreateParticles()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateParticles( void )
{
    LogWrite( "CreateParticles() creating...\n" );

    if( !CreateMissileSmoke() )
        return 0;

    if( !CreateGeneralSpark() )
        return 0;

    if( !CreateDecalSmoke() )
        return 0;

    if( !CreateFireball() )
        return 0;

    if( !CreateAirStrikeBeacon() )
        return 0;

    if( !CreateDamageSmoke() )
        return 0;

    if( !CreateTankSpark() )
        return 0;

    if( !CreateShieldLightSpark() )
        return 0;

    if( !CreateShieldMediumSpark() )
        return 0;

    if( !CreateShieldHeavySpark() )
        return 0;

    if( !CreateGroundSmoke() )
        return 0;

    if( !CreateRailgun() )
        return 0;

    if( !CreateRailgunInner() )
        return 0;

    if( !CreateSnowflake() )
        return 0;

    if( !CreateCentralExplosion() )
        return 0;

    if( !CreateExplFireball() )
        return 0;

    if( !CreateExplSparks() )
        return 0;

    if( !CreateShieldLightDebris() )
        return 0;

    if( !CreateShieldMediumDebris() )
        return 0;

    if( !CreateShieldHeavyDebris() )
        return 0;

    if( !CreateDebrisGround() )
        return 0;

    if( !CreateDebrisTank() )
        return 0;

    LogWrite( "CreateParticles() created\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteParticles()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteParticles( void )
{
    LogWrite( "DeleteParticles() deleting...\n" );

    DeleteDebrisTank();
    DeleteDebrisGround();
    DeleteShieldHeavyDebris();
    DeleteShieldMediumDebris();
    DeleteShieldLightDebris();
    DeleteExplSparks();
    DeleteExplFireball();
    DeleteCentralExplosion();
    DeleteSnowflake();
    DeleteRailgunInner();
    DeleteRailgun();
    DeleteGroundSmoke();
    DeleteShieldHeavySpark();
    DeleteShieldMediumSpark();
    DeleteShieldLightSpark();
    DeleteTankSpark();
    DeleteDamageSmoke();
    DeleteAirStrikeBeacon();
    DeleteFireball();
    DeleteDecalSmoke();
    DeleteGeneralSpark();
    DeleteMissileSmoke();

    LogWrite( "DeleteParticles() created\n" );
}
