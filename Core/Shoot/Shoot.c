//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 6 July 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>

#include "WeaponTypes.h"
#include "StateWaitForShot.h"

#include "Vector.h"
#include "Backpack.h"
#include "Guidance.h"

#include "GameItems.h"
#include "GameTypes.h"
#include "GameSounds.h"

#include "Shoot.h"
#include "Hardware.h"
#include "Camera.h"

#include "ParticleRailgun.h"
#include "ParticleRailgunInner.h"

#include "ObjectManager.h"
#include "LevelManager.h"
#include "ModelManager.h"
#include "ShotManager.h"
#include "PlayerManager.h"
#include "MeshManager.h"
#include "ExplosionManager.h"
#include "SoundManager.h"

#define CROSSHAIR_MAXSIZE       10.0f
#define CROSSHAIR_LINESIZE       2.0f


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;
extern HardwareConfig_t g_HWConfig;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _CrossHair = 0;
static Vector_t _RailLookAtPoint;


//----------------------------------------------------------------------------------------
// Name: ShootProjectile()
// Desc:
//----------------------------------------------------------------------------------------

size_t ShootProjectile(Weapon_t* Weapon, Vector_t* StartPoint, Vector_t* ShootVector, float Power, size_t AllowGuidance)
{
    size_t ShotID, ObjectID;
    size_t Index;
    Vector_t Move;
    size_t (*Guidance)(size_t, Vector_t*, Vector_t*);

    // setup a new object and activate it
    ObjectID = GetFreeObject();
    ResetObject(ObjectID);

    SetObjectModels(ObjectID, Weapon->ProjectileModelIDLOD0, Weapon->ProjectileModelIDLOD1);
    SetObjectDrawFunction(ObjectID, DrawShadedModel);
    SetObjectCollideMeshFunction(ObjectID, Weapon->CollideMesh);

    SetObjectOnThinkFunction(ObjectID, Weapon->OnThink);
    SetObjectOnCollideMeshFunction(ObjectID, Weapon->OnCollideMesh);
    SetObjectOnCollideObjectFunction(ObjectID, Weapon->OnCollideObject);

    SetObjectPosition(ObjectID, 0, StartPoint);
    AddObjectFlag(ObjectID, OBJECT_CANCOLLIDEWITHTANK);
    SetObjectRotateType(ObjectID, MODELROTATE_YXZ);

    Guidance = nullptr;

    if (AllowGuidance)
    {
        Index = BackpackGetAbsoluteIndex(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEITEM, (void*)&ItemHeatSeekWeak);
        if (-1 != Index && IsBackpackItemActivated(GetPlayerBackpack(GetCurrentPlayer()), Index))
        {
            Guidance = WeakGuidance;
            InitialiseGuidance(0);
        }

        Index = BackpackGetAbsoluteIndex(GetPlayerBackpack(GetCurrentPlayer()), BPITEM_TYPEITEM, (void*)&ItemHeatSeekStrong);
        if (-1 != Index && IsBackpackItemActivated(GetPlayerBackpack(GetCurrentPlayer()), Index))
        {
            Guidance = StrongGuidance;
            InitialiseGuidance(1);
        }
    }

    ShotID = GetFreeShot();
    SetupShot(ShotID, Weapon, ObjectID, Guidance);

    Power *= SHOOT_POWERMODIFIER;
    VectorScale((*ShootVector), Power, Move);
    ApplyForceToObject(ObjectID, &Move);

    SetObjectData(ObjectID, ShotID);
    ActivateObject(ObjectID);
    ShowObject(ObjectID);

    if (Weapon->StartSmokeTrail && Weapon->StartSparkTrail)
        SetSmokeTrailID(ShotID, Weapon->StartSmokeTrail(ObjectID), Weapon->StartSparkTrail(ObjectID));

    return ShotID;
}


//----------------------------------------------------------------------------------------
// Name: ShootHitScan()
// Desc:
//----------------------------------------------------------------------------------------

size_t ShootHitScan(Weapon_t* Weapon, Vector_t* StartPoint, Vector_t* ShootVector, float Power, size_t AllowGuidance)
{
    size_t ShotID, ObjectID;
    size_t i, ModelID;
    size_t DamagedPlayerID;
    float Smallest;
    Vector_t Intersect, EndPoint;
    Vector_t Force;
    Polygon_t* Polygon;
    PlayerHistory_t* History;

    // setup a new object and activate it
    ObjectID = GetFreeObject();
    ResetObject(ObjectID);

    //SetObjectDrawFunction( ObjectID, DrawLine );
    SetObjectOnThinkFunction(ObjectID, Weapon->OnThink);
    SetRailgunPhysics(ObjectID);

    EndPoint.X = StartPoint->X + (ShootVector->X * 8000.0f);
    EndPoint.Y = StartPoint->Y + (ShootVector->Y * 8000.0f);
    EndPoint.Z = StartPoint->Z + (ShootVector->Z * 8000.0f);

    if (LineMeshIntersect(g_LandscapeMesh, StartPoint, &EndPoint, &Polygon, &Intersect, &Smallest) == IntersectionResult::IntersectsLandscape)
        VectorCopy(Intersect, EndPoint);

    SetObjectPosition(ObjectID, 0, StartPoint);
    SetObjectPosition(ObjectID, 1, &EndPoint);

    ShotID = GetFreeShot();
    SetupShot(ShotID, Weapon, ObjectID, nullptr);

    SetObjectData(ObjectID, ShotID);
    ActivateObject(ObjectID);
    ShowObject(ObjectID);

    VectorScale((*ShootVector), (2.0f * TERMINALVELOCITY), Force);

    DamagedPlayerID = -1;

    History = GetPlayerHistory(GetCurrentPlayer());

    // damage all tanks along the rail's path
    for (i = 0; i < GetPlayerCount(); i++)
    {
        if (!IsPlayerDead(i) && i != GetCurrentPlayer())
        {
            GetPlayerPosition(i, &Intersect);
            GetObjectModels(GetPlayerTank(i), &ModelID, nullptr);

            if (MathLineSphereIntersect(&Intersect, GetModelBSphereRadius(ModelID), StartPoint, &EndPoint, nullptr, nullptr))
            {
                History->NumShotsHit++;
                History->HasHitThisTurn = 1;

                PlayerTakeDamage(i, RAILGUN_DAMAGE);
                ApplyForceToObject(GetPlayerTank(i), &Force);

                DamagedPlayerID = i;
            }
        }
    }

    if (-1 == DamagedPlayerID)
    {
        History->NumShotsMissed++;
        History->HasHitThisTurn = 1;

        // no tanks were damaged, lookat the point where the rail ended
        VectorCopy(EndPoint, _RailLookAtPoint);
    }
    else
    {
        // lookat the last tank that was damaged
        GetPlayerPosition(DamagedPlayerID, &_RailLookAtPoint);
    }

    SetPointToFollow(&_RailLookAtPoint);
    SetHitScanShot();

    StartRailgun(StartPoint, &EndPoint);
    StartRailgunInner(StartPoint, &EndPoint);

    StartExplosion(&EndPoint, GetCurrentPlayer(), Weapon->ExplType);

    SetSoundPosition(SoundHitScanExplode0, EndPoint.X, EndPoint.Y, EndPoint.Z);
    PlaySound3D(SoundHitScanExplode0, 0);

    return ShotID;
}


//----------------------------------------------------------------------------------------
// Name: EnableCrossHair()
// Desc:
//----------------------------------------------------------------------------------------

void EnableCrossHair(void)
{
    _CrossHair = 1;
}


//----------------------------------------------------------------------------------------
// Name: DisableCrossHair()
// Desc:
//----------------------------------------------------------------------------------------

void DisableCrossHair(void)
{
    _CrossHair = 0;
}


//----------------------------------------------------------------------------------------
// Name: DrawCrossHair()
// Desc:
//----------------------------------------------------------------------------------------

void DrawCrossHair(void)
{
    float CenterX, CenterY;

    if (!_CrossHair)
        return;

    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    CenterX = g_HWConfig.GfxWidth  / 2.0f;
    CenterY = g_HWConfig.GfxHeight / 2.0f;

    glBegin(GL_LINES);

    glVertex2f(CenterX - CROSSHAIR_MAXSIZE,  CenterY - CROSSHAIR_MAXSIZE);
    glVertex2f(CenterX - CROSSHAIR_LINESIZE, CenterY - CROSSHAIR_LINESIZE);

    glVertex2f(CenterX + CROSSHAIR_LINESIZE, CenterY - CROSSHAIR_LINESIZE);
    glVertex2f(CenterX + CROSSHAIR_MAXSIZE,  CenterY - CROSSHAIR_MAXSIZE);

    glVertex2f(CenterX - CROSSHAIR_MAXSIZE,  CenterY + CROSSHAIR_MAXSIZE);
    glVertex2f(CenterX - CROSSHAIR_LINESIZE, CenterY + CROSSHAIR_LINESIZE);

    glVertex2f(CenterX + CROSSHAIR_LINESIZE, CenterY + CROSSHAIR_LINESIZE);
    glVertex2f(CenterX + CROSSHAIR_MAXSIZE,  CenterY + CROSSHAIR_MAXSIZE);

    glEnd();

    glEnable(GL_TEXTURE_2D);
}
