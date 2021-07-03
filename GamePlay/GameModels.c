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
#include "ModelManager.h"
#include "Error.h"
#include "LoadingScreen.h"
#include "GameTypes.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

size_t g_ModelSkySphere;

size_t g_ModelTank0LOD0;
size_t g_ModelTank0LOD1;
size_t g_ModelTank1LOD0;
size_t g_ModelTank1LOD1;

size_t g_ModelParachuteLOD0;
size_t g_ModelParachuteLOD1;
size_t g_ModelShieldLight;
size_t g_ModelShieldMedium;
size_t g_ModelShieldHeavy;
size_t g_ModelFuel;
size_t g_ModelAutoDefense;
size_t g_ModelBatteries;
size_t g_ModelHeatSeekingWeak;
size_t g_ModelHeatSeekingStrong;

size_t g_ModelMissileSmallLOD0;
size_t g_ModelMissileSmallLOD1;
size_t g_ModelMissileLargeLOD0;
size_t g_ModelMissileLargeLOD1;
size_t g_ModelNukeSmallLOD0;
size_t g_ModelNukeSmallLOD1;
size_t g_ModelNukeLargeLOD0;
size_t g_ModelNukeLargeLOD1;
size_t g_ModelFunkyBombLOD0;
size_t g_ModelFunkyBombLOD1;
size_t g_ModelRailgun;
size_t g_ModelNapalmSmallLOD0;
size_t g_ModelNapalmSmallLOD1;
size_t g_ModelNapalmLargeLOD0;
size_t g_ModelNapalmLargeLOD1;
size_t g_ModelRollerSmallLOD0;
size_t g_ModelRollerSmallLOD1;
size_t g_ModelRollerLargeLOD0;
size_t g_ModelRollerLargeLOD1;
size_t g_ModelAirStrikeSmallLOD0;
size_t g_ModelAirStrikeSmallLOD1;
size_t g_ModelAirStrikeLargeLOD0;
size_t g_ModelAirStrikeLargeLOD1;

size_t g_ModelAirStrikePlaneLOD0;
size_t g_ModelAirStrikePlaneLOD1;

size_t g_ModelExplosion;
size_t g_ModelShockwave;

size_t g_ModelShieldLightP0;
size_t g_ModelShieldLightP1;
size_t g_ModelShieldLightP2;

size_t g_ModelShieldMediumP0;
size_t g_ModelShieldMediumP1;
size_t g_ModelShieldMediumP2;

size_t g_ModelShieldHeavyP0;
size_t g_ModelShieldHeavyP1;
size_t g_ModelShieldHeavyP2;

size_t g_ModelDebrisRockDirtP0;
size_t g_ModelDebrisRockDirtP1;
size_t g_ModelDebrisRockGrassP0;
size_t g_ModelDebrisRockGrassP1;
size_t g_ModelDebrisRockRockP0;
size_t g_ModelDebrisRockRockP1;
size_t g_ModelDebrisRockSnowP0;
size_t g_ModelDebrisRockSnowP1;

size_t g_ModelDebrisGeneral0;
size_t g_ModelDebrisGeneral1;
size_t g_ModelDebrisGeneral2;
size_t g_ModelDebrisGeneral3;
size_t g_ModelDebrisGeneral4;

size_t g_ModelDebrisHellCatP0;
size_t g_ModelDebrisHellCatP1;
size_t g_ModelDebrisHellCatP2;
size_t g_ModelDebrisHellCatP3;
size_t g_ModelDebrisHellCatP4;
size_t g_ModelDebrisHellCatP5;

size_t g_ModelDebrisLocustP0;
size_t g_ModelDebrisLocustP1;
size_t g_ModelDebrisLocustP2;
size_t g_ModelDebrisLocustP3;
size_t g_ModelDebrisLocustP4;
size_t g_ModelDebrisLocustP5;

size_t g_ModelBuyFullVersion;


//-----------------------------------------------------------------------------
// Name: CreateModels()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateModels( void )
{
    LogWrite( "CreateModels() creating...\n" );
    BeginLoadingScreen( TextureGame, "Loading Models...", 0, 78 );

    g_ModelSkySphere = CreateModel( "LWO/SkySphere.lwo" );                              UpdateLoadingScreen( 1 );
    if( -1 == g_ModelSkySphere )
        return 0;

    g_ModelTank0LOD0 = CreateModel( "LWO/Tank 1943 M18HellCat LOD0.lwo" );              UpdateLoadingScreen( 2 );
    if( -1 == g_ModelTank0LOD0 )
        return 0;

    g_ModelTank0LOD1 = CreateModel( "LWO/Tank 1943 M18HellCat LOD1.lwo" );              UpdateLoadingScreen( 3 );
    if( -1 == g_ModelTank0LOD1 )
        return 0;

    g_ModelTank1LOD0 = CreateModel( "LWO/Tank 1943 M22Locust LOD0.lwo" );               UpdateLoadingScreen( 4 );
    if( -1 == g_ModelTank1LOD0 )
        return 0;

    g_ModelTank1LOD1 = CreateModel( "LWO/Tank 1943 M22Locust LOD1.lwo" );               UpdateLoadingScreen( 5 );
    if( -1 == g_ModelTank1LOD1 )
        return 0;

    g_ModelParachuteLOD0 = CreateModel( "LWO/ItemParachutesLOD0.lwo" );                 UpdateLoadingScreen( 6 );
    if( -1 == g_ModelParachuteLOD0 )
        return 0;

    g_ModelParachuteLOD1 = CreateModel( "LWO/ItemParachutesLOD1.lwo" );                 UpdateLoadingScreen( 7 );
    if( -1 == g_ModelParachuteLOD1 )
        return 0;

    g_ModelShieldLight = CreateModel( "LWO/ItemShieldsLight.lwo" );                     UpdateLoadingScreen( 8 );
    if( -1 == g_ModelShieldLight )
        return 0;

#if DEMO
    g_ModelShieldMedium = -1;
    g_ModelShieldHeavy = -1;
#else
    g_ModelShieldMedium = CreateModel( "LWO/ItemShieldsMedium.lwo" );                   UpdateLoadingScreen( 9 );
    if( -1 == g_ModelShieldMedium )
        return 0;

    g_ModelShieldHeavy = CreateModel( "LWO/ItemShieldsHeavy.lwo" );                     UpdateLoadingScreen( 10 );
    if( -1 == g_ModelShieldHeavy )
        return 0;
#endif

    g_ModelFuel = CreateModel( "LWO/ItemFuel.lwo" );                                    UpdateLoadingScreen( 11 );
    if( -1 == g_ModelFuel )
        return 0;

    g_ModelAutoDefense = CreateModel( "LWO/ItemAutomaticDefense.lwo" );                 UpdateLoadingScreen( 12 );
    if( -1 == g_ModelAutoDefense )
        return 0;

    g_ModelHeatSeekingWeak = CreateModel( "LWO/ItemHeatSeekWeak.lwo" );                 UpdateLoadingScreen( 13 );
    if( -1 == g_ModelHeatSeekingWeak )
        return 0;

    g_ModelBatteries = CreateModel( "LWO/ItemBatteries.lwo" );                          UpdateLoadingScreen( 14 );
    if( -1 == g_ModelBatteries )
        return 0;

#if DEMO
    g_ModelHeatSeekingStrong = -1;
#else
    g_ModelHeatSeekingStrong = CreateModel( "LWO/ItemHeatSeekStrong.lwo" );             UpdateLoadingScreen( 15 );
    if( -1 == g_ModelHeatSeekingStrong )
        return 0;
#endif

    g_ModelMissileSmallLOD0 = CreateModel( "LWO/WeaponMissileSmallLOD0.lwo" );          UpdateLoadingScreen( 16 );
    if( -1 == g_ModelMissileSmallLOD0 )
        return 0;

    g_ModelMissileSmallLOD1 = CreateModel( "LWO/WeaponMissileSmallLOD1.lwo" );          UpdateLoadingScreen( 17 );
    if( -1 == g_ModelMissileSmallLOD1 )
        return 0;

    g_ModelMissileLargeLOD0 = CreateModel( "LWO/WeaponMissileLargeLOD0.lwo" );          UpdateLoadingScreen( 18 );
    if( -1 == g_ModelMissileLargeLOD0 )
        return 0;

    g_ModelMissileLargeLOD1 = CreateModel( "LWO/WeaponMissileLargeLOD1.lwo" );          UpdateLoadingScreen( 19 );
    if( -1 == g_ModelMissileLargeLOD1 )
        return 0;

    g_ModelNukeSmallLOD0 = CreateModel( "LWO/WeaponNukeSmallLOD0.lwo" );                UpdateLoadingScreen( 20 );
    if( -1 == g_ModelNukeSmallLOD0 )
        return 0;

    g_ModelNukeSmallLOD1 = CreateModel( "LWO/WeaponNukeSmallLOD1.lwo" );                UpdateLoadingScreen( 21 );
    if( -1 == g_ModelNukeSmallLOD1 )
        return 0;

#if DEMO
    g_ModelNukeLargeLOD0 = -1;
    g_ModelNukeLargeLOD1 = -1;
#else
    g_ModelNukeLargeLOD0 = CreateModel( "LWO/WeaponNukeLargeLOD0.lwo" );                UpdateLoadingScreen( 22 );
    if( -1 == g_ModelNukeLargeLOD0 )
        return 0;

    g_ModelNukeLargeLOD1 = CreateModel( "LWO/WeaponNukeLargeLOD1.lwo" );                UpdateLoadingScreen( 23 );
    if( -1 == g_ModelNukeLargeLOD1 )
        return 0;
#endif

    g_ModelFunkyBombLOD0 = CreateModel( "LWO/WeaponFunkyBombLOD0.lwo" );                UpdateLoadingScreen( 24 );
    if( -1 == g_ModelFunkyBombLOD0 )
        return 0;

    g_ModelFunkyBombLOD1 = CreateModel( "LWO/WeaponFunkyBombLOD1.lwo" );                UpdateLoadingScreen( 25 );
    if( -1 == g_ModelFunkyBombLOD1 )
        return 0;

    g_ModelRailgun = CreateModel( "LWO/WeaponRailgun.lwo" );                            UpdateLoadingScreen( 26 );
    if( -1 == g_ModelRailgun )
        return 0;

    g_ModelNapalmSmallLOD0 = CreateModel( "LWO/WeaponNapalmSmallLOD0.lwo" );            UpdateLoadingScreen( 27 );
    if( -1 == g_ModelNapalmSmallLOD0 )
        return 0;

    g_ModelNapalmSmallLOD1 = CreateModel( "LWO/WeaponNapalmSmallLOD1.lwo" );            UpdateLoadingScreen( 28 );
    if( -1 == g_ModelNapalmSmallLOD1 )
        return 0;

#if DEMO
    g_ModelNapalmLargeLOD0 = -1;
    g_ModelNapalmLargeLOD1 = -1;
#else
    g_ModelNapalmLargeLOD0 = CreateModel( "LWO/WeaponNapalmLargeLOD0.lwo" );            UpdateLoadingScreen( 29 );
    if( -1 == g_ModelNapalmLargeLOD0 )
        return 0;

    g_ModelNapalmLargeLOD1 = CreateModel( "LWO/WeaponNapalmLargeLOD1.lwo" );            UpdateLoadingScreen( 30 );
    if( -1 == g_ModelNapalmLargeLOD1 )
        return 0;
#endif

    g_ModelRollerSmallLOD0 = CreateModel( "LWO/WeaponRollerSmallLOD0.lwo" );            UpdateLoadingScreen( 31 );
    if( -1 == g_ModelRollerSmallLOD0 )
        return 0;

    g_ModelRollerSmallLOD1 = CreateModel( "LWO/WeaponRollerSmallLOD1.lwo" );            UpdateLoadingScreen( 32 );
    if( -1 == g_ModelRollerSmallLOD1 )
        return 0;

#if DEMO
    g_ModelRollerLargeLOD0 = -1;
    g_ModelRollerLargeLOD1 = -1;
#else
    g_ModelRollerLargeLOD0 = CreateModel( "LWO/WeaponRollerLargeLOD0.lwo" );            UpdateLoadingScreen( 33 );
    if( -1 == g_ModelRollerLargeLOD0 )
        return 0;

    g_ModelRollerLargeLOD1 = CreateModel( "LWO/WeaponRollerLargeLOD1.lwo" );            UpdateLoadingScreen( 34 );
    if( -1 == g_ModelRollerLargeLOD1 )
        return 0;
#endif

    g_ModelAirStrikeSmallLOD0 = CreateModel( "LWO/WeaponAirStrikeSmallLOD0.lwo" );      UpdateLoadingScreen( 35 );
    if( -1 == g_ModelAirStrikeSmallLOD0 )
        return 0;

    g_ModelAirStrikeSmallLOD1 = CreateModel( "LWO/WeaponAirStrikeSmallLOD1.lwo" );      UpdateLoadingScreen( 36 );
    if( -1 == g_ModelAirStrikeSmallLOD1 )
        return 0;

#if DEMO
    g_ModelAirStrikeLargeLOD0 = -1;
    g_ModelAirStrikeLargeLOD1 = -1;
#else
    g_ModelAirStrikeLargeLOD0 = CreateModel( "LWO/WeaponAirStrikeLargeLOD0.lwo" );      UpdateLoadingScreen( 37 );
    if( -1 == g_ModelAirStrikeLargeLOD0 )
        return 0;

    g_ModelAirStrikeLargeLOD1 = CreateModel( "LWO/WeaponAirStrikeLargeLOD1.lwo" );      UpdateLoadingScreen( 38 );
    if( -1 == g_ModelAirStrikeLargeLOD1 )
        return 0;
#endif

    g_ModelAirStrikePlaneLOD0 = CreateModel( "LWO/WeaponAirStrikePlaneLOD0.lwo" );      UpdateLoadingScreen( 39 );
    if( -1 == g_ModelAirStrikePlaneLOD0 )
        return 0;

    g_ModelAirStrikePlaneLOD1 = CreateModel( "LWO/WeaponAirStrikePlaneLOD0.lwo" );      UpdateLoadingScreen( 40 );
    if( -1 == g_ModelAirStrikePlaneLOD1 )
        return 0;

    g_ModelExplosion = CreateModel( "LWO/Explosion.lwo" );                              UpdateLoadingScreen( 41 );
    if( -1 == g_ModelExplosion )
        return 0;

    g_ModelShockwave = CreateModel( "LWO/Shockwave.lwo" );                              UpdateLoadingScreen( 42 );
    if( -1 == g_ModelShockwave )
        return 0;

    g_ModelShieldLightP0 = CreateModel( "LWO/ItemShieldsLightPiece0.lwo" );             UpdateLoadingScreen( 43 );
    if( -1 == g_ModelShieldLightP0 )
        return 0;

    g_ModelShieldLightP1 = CreateModel( "LWO/ItemShieldsLightPiece1.lwo" );             UpdateLoadingScreen( 44 );
    if( -1 == g_ModelShieldLightP1 )
        return 0;

    g_ModelShieldLightP2 = CreateModel( "LWO/ItemShieldsLightPiece2.lwo" );             UpdateLoadingScreen( 45 );
    if( -1 == g_ModelShieldLightP2 )
        return 0;

#if DEMO
    g_ModelShieldMediumP0 = -1;
    g_ModelShieldMediumP1 = -1;
    g_ModelShieldMediumP2 = -1;
    g_ModelShieldHeavyP0 = -1;
    g_ModelShieldHeavyP1 = -1;
    g_ModelShieldHeavyP2 = -1;
#else
    g_ModelShieldMediumP0 = CreateModel( "LWO/ItemShieldsMediumPiece0.lwo" );           UpdateLoadingScreen( 46 );
    if( -1 == g_ModelShieldMediumP0 )
        return 0;

    g_ModelShieldMediumP1 = CreateModel( "LWO/ItemShieldsMediumPiece1.lwo" );           UpdateLoadingScreen( 47 );
    if( -1 == g_ModelShieldMediumP1 )
        return 0;

    g_ModelShieldMediumP2 = CreateModel( "LWO/ItemShieldsMediumPiece2.lwo" );           UpdateLoadingScreen( 48 );
    if( -1 == g_ModelShieldMediumP2 )
        return 0;

    g_ModelShieldHeavyP0 = CreateModel( "LWO/ItemShieldsHeavyPiece0.lwo" );             UpdateLoadingScreen( 49 );
    if( -1 == g_ModelShieldHeavyP0 )
        return 0;

    g_ModelShieldHeavyP1 = CreateModel( "LWO/ItemShieldsHeavyPiece1.lwo" );             UpdateLoadingScreen( 50 );
    if( -1 == g_ModelShieldHeavyP1 )
        return 0;

    g_ModelShieldHeavyP2 = CreateModel( "LWO/ItemShieldsHeavyPiece2.lwo" );             UpdateLoadingScreen( 51 );
    if( -1 == g_ModelShieldHeavyP2 )
        return 0;
#endif

    g_ModelDebrisRockDirtP0 = CreateModel( "LWO/DebrisRockDirt0.lwo" );                  UpdateLoadingScreen( 52 );
    if( -1 == g_ModelDebrisRockDirtP0 )
        return 0;

    g_ModelDebrisRockDirtP1 = CreateModel( "LWO/DebrisRockDirt1.lwo" );                  UpdateLoadingScreen( 53 );
    if( -1 == g_ModelDebrisRockDirtP1 )
        return 0;

    g_ModelDebrisRockGrassP0 = CreateModel( "LWO/DebrisRockGrass0.lwo" );                UpdateLoadingScreen( 54 );
    if( -1 == g_ModelDebrisRockGrassP0 )
        return 0;

    g_ModelDebrisRockGrassP1 = CreateModel( "LWO/DebrisRockGrass1.lwo" );                UpdateLoadingScreen( 55 );
    if( -1 == g_ModelDebrisRockGrassP1 )
        return 0;

    g_ModelDebrisRockRockP0 = CreateModel( "LWO/DebrisRockRock0.lwo" );                  UpdateLoadingScreen( 56 );
    if( -1 == g_ModelDebrisRockRockP0 )
        return 0;

    g_ModelDebrisRockRockP1 = CreateModel( "LWO/DebrisRockRock1.lwo" );                  UpdateLoadingScreen( 57 );
    if( -1 == g_ModelDebrisRockRockP1 )
        return 0;

    g_ModelDebrisRockSnowP0 = CreateModel( "LWO/DebrisRockSnow0.lwo" );                  UpdateLoadingScreen( 58 );
    if( -1 == g_ModelDebrisRockSnowP0 )
        return 0;

    g_ModelDebrisRockSnowP1 = CreateModel( "LWO/DebrisRockSnow1.lwo" );                  UpdateLoadingScreen( 59 );
    if( -1 == g_ModelDebrisRockSnowP1 )
        return 0;

    g_ModelDebrisGeneral0 = CreateModel( "LWO/DebrisGeneral0.lwo" );                     UpdateLoadingScreen( 60 );
    if( -1 == g_ModelDebrisGeneral0 )
        return 0;

    g_ModelDebrisGeneral1 = CreateModel( "LWO/DebrisGeneral1.lwo" );                     UpdateLoadingScreen( 61 );
    if( -1 == g_ModelDebrisGeneral1 )
        return 0;

    g_ModelDebrisGeneral2 = CreateModel( "LWO/DebrisGeneral2.lwo" );                     UpdateLoadingScreen( 62 );
    if( -1 == g_ModelDebrisGeneral2 )
        return 0;

    g_ModelDebrisGeneral3 = CreateModel( "LWO/DebrisGeneral3.lwo" );                     UpdateLoadingScreen( 63 );
    if( -1 == g_ModelDebrisGeneral3 )
        return 0;

    g_ModelDebrisGeneral4 = CreateModel( "LWO/DebrisGeneral4.lwo" );                     UpdateLoadingScreen( 64 );
    if( -1 == g_ModelDebrisGeneral4 )
        return 0;

    g_ModelDebrisHellCatP0 = CreateModel( "LWO/TankHellCatPiece0.lwo" );                 UpdateLoadingScreen( 65 );
    if( -1 == g_ModelDebrisHellCatP0 )
        return 0;

    g_ModelDebrisHellCatP1 = CreateModel( "LWO/TankHellCatPiece1.lwo" );                 UpdateLoadingScreen( 66 );
    if( -1 == g_ModelDebrisHellCatP1 )
        return 0;

    g_ModelDebrisHellCatP2 = CreateModel( "LWO/TankHellCatPiece2.lwo" );                 UpdateLoadingScreen( 67 );
    if( -1 == g_ModelDebrisHellCatP2 )
        return 0;

    g_ModelDebrisHellCatP3 = CreateModel( "LWO/TankHellCatPiece3.lwo" );                 UpdateLoadingScreen( 68 );
    if( -1 == g_ModelDebrisHellCatP3 )
        return 0;

    g_ModelDebrisHellCatP4 = CreateModel( "LWO/TankHellCatPiece4.lwo" );                 UpdateLoadingScreen( 69 );
    if( -1 == g_ModelDebrisHellCatP4 )
        return 0;

    g_ModelDebrisHellCatP5 = CreateModel( "LWO/TankHellCatPiece5.lwo" );                 UpdateLoadingScreen( 70 );
    if( -1 == g_ModelDebrisHellCatP5 )
        return 0;

    g_ModelDebrisLocustP0 = CreateModel( "LWO/TankLocustPiece0.lwo" );                   UpdateLoadingScreen( 71 );
    if( -1 == g_ModelDebrisLocustP0 )
        return 0;

    g_ModelDebrisLocustP1 = CreateModel( "LWO/TankLocustPiece1.lwo" );                   UpdateLoadingScreen( 72 );
    if( -1 == g_ModelDebrisLocustP1 )
        return 0;

    g_ModelDebrisLocustP2 = CreateModel( "LWO/TankLocustPiece2.lwo" );                   UpdateLoadingScreen( 73 );
    if( -1 == g_ModelDebrisLocustP2 )
        return 0;

    g_ModelDebrisLocustP3 = CreateModel( "LWO/TankLocustPiece3.lwo" );                   UpdateLoadingScreen( 74 );
    if( -1 == g_ModelDebrisLocustP3 )
        return 0;

    g_ModelDebrisLocustP4 = CreateModel( "LWO/TankLocustPiece4.lwo" );                   UpdateLoadingScreen( 75 );
    if( -1 == g_ModelDebrisLocustP4 )
        return 0;

    g_ModelDebrisLocustP5 = CreateModel( "LWO/TankLocustPiece5.lwo" );                   UpdateLoadingScreen( 76 );
    if( -1 == g_ModelDebrisLocustP5 )
        return 0;
#if DEMO
    g_ModelBuyFullVersion = CreateModel( "LWO/BuyFullVersion.lwo" );                     UpdateLoadingScreen( 77 );
    if( -1 == g_ModelBuyFullVersion )
        return 0;
#else
    g_ModelBuyFullVersion = -1;
#endif

    EndLoadingScreen( "Loading Models...Done", 78 );
    LogWrite( "CreateModels() created\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteModels()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteModels( void )
{
    LogWrite( "DeleteModels() deleting...\n" );

    DeleteModel( g_ModelBuyFullVersion );
    DeleteModel( g_ModelDebrisLocustP5 );
    DeleteModel( g_ModelDebrisLocustP4 );
    DeleteModel( g_ModelDebrisLocustP3 );
    DeleteModel( g_ModelDebrisLocustP2 );
    DeleteModel( g_ModelDebrisLocustP1 );
    DeleteModel( g_ModelDebrisLocustP0 );
    DeleteModel( g_ModelDebrisHellCatP5 );
    DeleteModel( g_ModelDebrisHellCatP4 );
    DeleteModel( g_ModelDebrisHellCatP3 );
    DeleteModel( g_ModelDebrisHellCatP2 );
    DeleteModel( g_ModelDebrisHellCatP1 );
    DeleteModel( g_ModelDebrisHellCatP0 );
    DeleteModel( g_ModelDebrisGeneral4 );
    DeleteModel( g_ModelDebrisGeneral3 );
    DeleteModel( g_ModelDebrisGeneral2 );
    DeleteModel( g_ModelDebrisGeneral1 );
    DeleteModel( g_ModelDebrisGeneral0 );
    DeleteModel( g_ModelShieldHeavyP2 );
    DeleteModel( g_ModelShieldHeavyP1 );
    DeleteModel( g_ModelShieldHeavyP0 );
    DeleteModel( g_ModelShieldMediumP2 );
    DeleteModel( g_ModelShieldMediumP1 );
    DeleteModel( g_ModelShieldMediumP0 );
    DeleteModel( g_ModelShieldLightP2 );
    DeleteModel( g_ModelShieldLightP1 );
    DeleteModel( g_ModelShieldLightP0 );
    DeleteModel( g_ModelShockwave );
    DeleteModel( g_ModelExplosion );
    DeleteModel( g_ModelAirStrikePlaneLOD1 );
    DeleteModel( g_ModelAirStrikePlaneLOD0 );
    DeleteModel( g_ModelAirStrikeLargeLOD1 );
    DeleteModel( g_ModelAirStrikeLargeLOD0 );
    DeleteModel( g_ModelAirStrikeSmallLOD1 );
    DeleteModel( g_ModelAirStrikeSmallLOD0 );
    DeleteModel( g_ModelRollerLargeLOD1 );
    DeleteModel( g_ModelRollerLargeLOD0 );
    DeleteModel( g_ModelRollerSmallLOD1 );
    DeleteModel( g_ModelRollerSmallLOD0 );
    DeleteModel( g_ModelNapalmLargeLOD1 );
    DeleteModel( g_ModelNapalmLargeLOD0 );
    DeleteModel( g_ModelNapalmSmallLOD1 );
    DeleteModel( g_ModelNapalmSmallLOD0 );
    DeleteModel( g_ModelRailgun );
    DeleteModel( g_ModelFunkyBombLOD1 );
    DeleteModel( g_ModelFunkyBombLOD0 );
    DeleteModel( g_ModelNukeLargeLOD1 );
    DeleteModel( g_ModelNukeLargeLOD0 );
    DeleteModel( g_ModelNukeSmallLOD1 );
    DeleteModel( g_ModelNukeSmallLOD0 );
    DeleteModel( g_ModelMissileLargeLOD1 );
    DeleteModel( g_ModelMissileLargeLOD0 );
    DeleteModel( g_ModelMissileSmallLOD1 );
    DeleteModel( g_ModelMissileSmallLOD0 );
    DeleteModel( g_ModelHeatSeekingStrong );
    DeleteModel( g_ModelBatteries );
    DeleteModel( g_ModelHeatSeekingWeak );
    DeleteModel( g_ModelAutoDefense );
    DeleteModel( g_ModelFuel );
    DeleteModel( g_ModelShieldHeavy );
    DeleteModel( g_ModelShieldMedium );
    DeleteModel( g_ModelShieldLight );
    DeleteModel( g_ModelParachuteLOD1 );
    DeleteModel( g_ModelParachuteLOD0 );
    DeleteModel( g_ModelTank1LOD1 );
    DeleteModel( g_ModelTank1LOD0 );
    DeleteModel( g_ModelTank0LOD1 );
    DeleteModel( g_ModelTank0LOD0 );
    DeleteModel( g_ModelSkySphere );

    LogWrite( "DeleteModels() deleted\n" );
}
