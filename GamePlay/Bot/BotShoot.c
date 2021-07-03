//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 15 July 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>
#include <math.h>

#include "GameTypes.h"
#include "GameWeapons.h"

#include "Utility.h"
#include "Shoot.h"
#include "Vector.h"

#include "BotUtility.h"
#include "BotBuy.h"

#include "ObjectManager.h"
#include "LevelManager.h"
#include "PlayerManager.h"
#include "ModelManager.h"
#include "MeshManager.h"
#include "BotShoot.h"

#define HEADING_CHANGESPEED     5.0f
#define PITCH_CHANGESPEED       2.0f

#define RAILGUN_VFOV            20.0f
#define RAILGUN_HFOV            30.0f
#define RAILGUN_HALFVFOV        10.0f
#define RAILGUN_HALFHFOV        15.0f
#define RAILGUN_NUMSTEPS        60


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private functions
//----------------------------------------------------------------------------------------

static void LineUpHitscanShot(size_t PlyrIndex, BotInfo_t* BotInfo);
static void LineUpProjectileShot(size_t PlyrIndex, BotInfo_t* BotInfo);

static void TraceScanningProjectileShot(Vector_t* TargetPosition, BotInfo_t* BotInfo);
static void TraceRefiningProjectileShot(Vector_t* TargetPosition, BotInfo_t* BotInfo);
static void TraceProjectileShot(float Heading, float Pitch, float Power, Vector_t* FinishPoint);

static void TraceScanningHitScanShot(Vector_t* TargetPosition, BotInfo_t* BotInfo);
static void TraceRefiningHitScanShot(Vector_t* TargetPosition, BotInfo_t* BotInfo);

static size_t GetBestWeapon(size_t PlyrIndex, Weapon_t** WeaponList);


//----------------------------------------------------------------------------------------
// Name: ChooseTarget()
// Desc:
//----------------------------------------------------------------------------------------

void ChooseTarget(size_t PlyrIndex, BotInfo_t* BotInfo)
{
    if (-1 == BotInfo->TargetPlyrIndex)
    {
        // choose a new target
        if (BotInfo->ChooseNewTarget)
            BotInfo->ChooseNewTarget(PlyrIndex, BotInfo);
    }
    else
    {
        // decide whether or not to shoot again at this target
        if (BotInfo->ReconsiderTarget)
            BotInfo->ReconsiderTarget(PlyrIndex, BotInfo);
    }

    BotInfo->ShotState = BOTSHOOTSTATE_CALCULATINGSHOT;
}


//----------------------------------------------------------------------------------------
// Name: CalculateHeadingPitchPower()
// Desc:
//----------------------------------------------------------------------------------------

void CalculateHeadingPitchPower(size_t PlyrIndex, BotInfo_t* BotInfo)
{
    // calculate values for BotInfo->DestHeading, BotInfo->DestPitch and BotInfo->DestPower

    if (CanPlayerSeePlayer(PlyrIndex, BotInfo->TargetPlyrIndex) && -1 != BackpackGetRelativeIndex(GetPlayerBackpack(PlyrIndex), BPITEM_TYPEWEAPON, (void*)&WeaponRailgun))
        LineUpHitscanShot(PlyrIndex, BotInfo);
    else
        LineUpProjectileShot(PlyrIndex, BotInfo);

    if (BotInfo->ShotNoiseMin < BotInfo->ShotNoiseMax)
    {
        if (++BotInfo->ShotNoiseCounter >= BotInfo->ShotNoiseThreshold)
        {
            BotInfo->ShotNoiseCounter = 0;

            BotInfo->ShotNoiseMin++;
            BotInfo->ShotNoiseMax--;
        }
    }

    if (BotInfo->ShotNoiseMax > 1)
    {
        BotInfo->DestHeading += (float)RandRange(BotInfo->ShotNoiseMin, BotInfo->ShotNoiseMax);
        if (BotInfo->DestHeading < 0.0f)
            BotInfo->DestHeading += 360.0f;
        else if (BotInfo->DestHeading >= 360.0f)
            BotInfo->DestHeading -= 360.0f;

        BotInfo->DestPitch += (float)RandRange(BotInfo->ShotNoiseMin, BotInfo->ShotNoiseMax);
        if (BotInfo->DestPitch < 0.0f)
            BotInfo->DestPitch = 0.0f;
        else if (BotInfo->DestPitch > 90.0f)
            BotInfo->DestPitch = 90.0f;

        BotInfo->DestPower += (float)RandRange(BotInfo->ShotNoiseMin / 2, BotInfo->ShotNoiseMax / 2);
        if (BotInfo->DestPower < 0.0f)
            BotInfo->DestPower = 0.0f;
        else if (BotInfo->DestPower > 100.0f)
            BotInfo->DestPower = 100.0f;
    }
}


//----------------------------------------------------------------------------------------
// Name: RotateTowardsTarget()
// Desc:
//----------------------------------------------------------------------------------------

size_t RotateTowardsTarget(size_t PlyrIndex, BotInfo_t* BotInfo)
{
    size_t HeadingDone, PitchDone;
    float Angle;
    Vector_t Rotation;

    HeadingDone = PitchDone = 0;

    // change the heading
    GetObjectRotation(GetPlayerTank(PlyrIndex), 1, &Rotation);
    Angle = AngleDiff(BotInfo->DestHeading, Rotation.Y);
    if (fabsf(Angle) <= HEADING_CHANGESPEED)
    {
        Rotation.Y = BotInfo->DestHeading;
        HeadingDone = 1;
    }
    else if (Angle < 0.0f)
    {
        Rotation.Y += HEADING_CHANGESPEED;
        if (Rotation.Y >= 360.0f)
            Rotation.Y -= 360.0f;
    }
    else
    {
        Rotation.Y -= HEADING_CHANGESPEED;
        if (Rotation.Y < 0.0f)
            Rotation.Y += 360.0f;
    }
    SetObjectRotation(GetPlayerTank(PlyrIndex), 1, &Rotation);

    // change the pitch
    GetObjectRotation(GetPlayerTank(PlyrIndex), 2, &Rotation);
    Angle = AngleDiff(BotInfo->DestPitch, Rotation.X);
    if (fabsf(Angle) <= PITCH_CHANGESPEED)
    {
        Rotation.X = BotInfo->DestPitch;
        PitchDone = 1;
    }
    else if (Angle < 0.0f)
    {
        Rotation.X += PITCH_CHANGESPEED;
        if (Rotation.X >= 360.0f)
            Rotation.X -= 360.0f;
    }
    else
    {
        Rotation.X -= PITCH_CHANGESPEED;
        if (Rotation.X < 0.0f)
            Rotation.X += 360.0f;
    }
    SetObjectRotation(GetPlayerTank(PlyrIndex), 2, &Rotation);

    if (HeadingDone && PitchDone)
    {
        SetTankShotPower(PlyrIndex, BotInfo->DestPower);
        return 1;
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: LineUpHitscanShot()
// Desc:
//----------------------------------------------------------------------------------------

static void LineUpHitscanShot(size_t PlyrIndex, BotInfo_t* BotInfo)
{
    Vector_t TargetPosition;

    GetPlayerPosition(BotInfo->TargetPlyrIndex, &TargetPosition);

    TraceScanningHitScanShot(&TargetPosition, BotInfo);
    TraceRefiningHitScanShot(&TargetPosition, BotInfo);

    SetPlayerCurrentWeapon(PlyrIndex, BackpackGetRelativeIndex(GetPlayerBackpack(PlyrIndex), BPITEM_TYPEWEAPON, (void*)&WeaponRailgun));

    BotInfo->ShotState = BOTSHOOTSTATE_ROTATINGBARRELL;
    BotInfo->DestPower = -1;    // set this flag so that the next projectile shot re-calcs from the start
}


//----------------------------------------------------------------------------------------
// Name: LineUpProjectileShot()
// Desc:
//----------------------------------------------------------------------------------------

static void LineUpProjectileShot(size_t PlyrIndex, BotInfo_t* BotInfo)
{
    Vector_t TargetPosition;
    Vector_t MyPosition;
    float Mag;
    Weapon_t** WeaponList;

    GetPlayerPosition(BotInfo->TargetPlyrIndex, &TargetPosition);
    GetPlayerPosition(PlyrIndex, &MyPosition);

    if (-1 == BotInfo->DestPower)
        TraceScanningProjectileShot(&TargetPosition, BotInfo);
    else
    {
        Mag = fabsf(VectorMagnitude2(&BotInfo->TargetPrevPosition, &TargetPosition));
        if (Mag >= 1000.0f)
            TraceScanningProjectileShot(&TargetPosition, BotInfo);
        else
        {
            Mag = fabsf(VectorMagnitude2(&BotInfo->MyPrevPosition, &MyPosition));
            if (Mag >= 1000.0f)
                TraceScanningProjectileShot(&TargetPosition, BotInfo);
            else
                TraceRefiningProjectileShot(&TargetPosition, BotInfo);
        }
    }

    VectorCopy(TargetPosition, BotInfo->TargetPrevPosition);
    VectorCopy(MyPosition, BotInfo->MyPrevPosition);

    switch (rand() % 3)
    {
    case 0 : WeaponList = _WeaponsSortedByDamage; break;
    case 1 : WeaponList = _WeaponsSortedByProjectile; break;
    case 2 : WeaponList = _WeaponsSortedByPrice; break;
    }

    SetPlayerCurrentWeapon(PlyrIndex, GetBestWeapon(PlyrIndex, WeaponList));

    BotInfo->ShotState = BOTSHOOTSTATE_ROTATINGBARRELL;
}


//----------------------------------------------------------------------------------------
// Name: CalculateShotParameters()
// Desc:
//----------------------------------------------------------------------------------------

static void CalcShootVectorAndStartPoint(float Heading, float Pitch, Vector_t* ShootVector, Vector_t* StartPoint)
{
    Vector_t ObjRot0;
    Vector_t Normal;
    Vector_t Position;
    float BSphere;
    size_t TankID, ModelID;

    ShootVector->X = 0.0f;
    ShootVector->Y = 0.0f;
    ShootVector->Z = 1.0f;

    TankID = GetPlayerTank(GetCurrentPlayer());
    GetObjectRotation(TankID, 0, &ObjRot0);
    GetObjectPosition(TankID, 0, &Position);

    // get the world rotation
    VectorRotateX(-Pitch * DTOR, ShootVector);
    VectorRotateY(-(ObjRot0.Y + Heading) * DTOR, ShootVector);
    VectorRotateZ(-ObjRot0.Z * DTOR, ShootVector);
    VectorRotateX(-ObjRot0.X * DTOR, ShootVector);

    // move the point out past the bounding sphere, adding a bit
    // more so that the shot projectile doesn't collide on firing

    GetObjectModels(TankID, &ModelID, nullptr);
    BSphere = GetModelBSphereRadius(ModelID) + 50.0f;

    StartPoint->X = Position.X + (BSphere * ShootVector->X);
    StartPoint->Y = Position.Y + (BSphere * ShootVector->Y);
    StartPoint->Z = Position.Z + (BSphere * ShootVector->Z);

    // raise the point up to the barrell's height
    GetPlayerGroundNormal(GetCurrentPlayer(), &Normal);
    StartPoint->X += Normal.X * TANK_BARRELLHEIGHT;
    StartPoint->Y += Normal.Y * TANK_BARRELLHEIGHT;
    StartPoint->Z += Normal.Z * TANK_BARRELLHEIGHT;
}


//----------------------------------------------------------------------------------------
// Name: TraceProjectileShot()
// Desc:
//----------------------------------------------------------------------------------------

static void TraceProjectileShot(float Heading, float Pitch, float Power, Vector_t* FinishPoint)
{
    Vector_t Move;

    CalcShootVectorAndStartPoint(Heading, Pitch, &Move, FinishPoint);

    Power *= SHOOT_POWERMODIFIER;
    VectorScale(Move, Power, Move);

    while (FinishPoint->Y > GetMeshHeight(g_LandscapeMesh, FinishPoint->X, FinishPoint->Z, nullptr))
    {
        Move.Y -= DEFAULT_GRAVITY;

        VectorDecrease(Move, DEFAULT_FRICTIONAIR);

        VectorAdd(Move, (*FinishPoint), (*FinishPoint));
    }
}


//----------------------------------------------------------------------------------------
// Name: TraceScanningProjectileShot()
// Desc:
//----------------------------------------------------------------------------------------

static void TraceScanningProjectileShot(Vector_t* TargetPosition, BotInfo_t* BotInfo)
{
    size_t HeadingIdx, PitchIdx, PowerIdx;
    float Heading, Pitch, Power;
    float Mag, Closest = 1000000.0f;
    Vector_t FinishPoint;

    // fire shots around the shooter and store heading, pitch and power of the shot
    // that lands the closest to the target.

    Heading = 0.0f;
    for (HeadingIdx = 0; HeadingIdx < 8; HeadingIdx++)
    {
        Pitch = 0.0f;
        for (PitchIdx = 0; PitchIdx < 3; PitchIdx++)
        {
            Power = 10.0f;
            for (PowerIdx = 0; PowerIdx < 4; PowerIdx++)
            {
                TraceProjectileShot(Heading, Pitch, Power, &FinishPoint);

                Mag = fabsf(VectorMagnitude2(TargetPosition, &FinishPoint));
                if (Mag < Closest)
                {
                    Closest = Mag;

                    BotInfo->DestHeading  = Heading;
                    BotInfo->DestPitch    = Pitch;
                    BotInfo->DestPower    = Power;
                }

                Power += 20.0f;
            }

            Pitch += 30.0f;
        }

        Heading += 45.0f;
    }
}


//----------------------------------------------------------------------------------------
// Name: TraceRefiningProjectileShot()
// Desc:
//----------------------------------------------------------------------------------------

static void TraceRefiningProjectileShot(Vector_t* TargetPosition, BotInfo_t* BotInfo)
{
    size_t HeadingIdx, PitchIdx, PowerIdx;
    float Heading, Pitch, Power;
    float OrigHeading, OrigPitch, OrigPower;
    float Mag, Closest = 1000000.0f;
    Vector_t FinishPoint;
    size_t i;

    float HeadingStep = 22.5f;
    float PitchStep = 10.0f;
    float PowerStep = 30.0f;

    // refine the heading, pitch and power stored in the BotInfo structure by
    // firing shots around the stored heading, pitch and power.  save the new
    // heading, pitch and power of the shot that lands the closest to the target.

    OrigHeading = BotInfo->DestHeading;
    OrigPitch   = BotInfo->DestPitch;
    OrigPower   = BotInfo->DestPower;

    for (i = 0; i < 4; i++)
    {
        Heading = OrigHeading + (-3.0f * (HeadingStep / 6.0f));
        for (HeadingIdx = 0; HeadingIdx < 6; HeadingIdx++)
        {
            Pitch = OrigPitch + (-3.0f * (PitchStep / 6.0f));
            for (PitchIdx = 0; PitchIdx < 6; PitchIdx++)
            {
                Power = OrigPower + (-3.0f * (PowerStep / 6.0f));
                for (PowerIdx = 0; PowerIdx < 6; PowerIdx++)
                {
                    TraceProjectileShot(Heading, Pitch, Power, &FinishPoint);

                    Mag = fabsf(VectorMagnitude2(TargetPosition, &FinishPoint));
                    if (Mag < Closest)
                    {
                        Closest = Mag;

                        BotInfo->DestHeading  = Heading;
                        BotInfo->DestPitch    = Pitch;
                        BotInfo->DestPower    = Power;
                    }

                    Power += PowerStep / 6.0f;
                }

                Pitch += PitchStep / 6.0f;
            }

            Heading += HeadingStep / 6.0f;
        }

        HeadingStep /= 2.0f;
        PitchStep /= 2.0f;
        PowerStep /= 2.0f;
    }

}


//----------------------------------------------------------------------------------------
// Name: GetBestWeapon()
// Desc:
//----------------------------------------------------------------------------------------

static size_t GetBestWeapon(size_t PlyrIndex, Weapon_t** WeaponList)
{
    size_t i, Index;

    for (i = 0; WeaponList[ i ]; i++)
    {
        if (&WeaponRailgun != WeaponList[ i ])
        {
            Index = BackpackGetRelativeIndex(GetPlayerBackpack(PlyrIndex), BPITEM_TYPEWEAPON, (void*)WeaponList[ i ]);
            if (-1 != Index)
                return Index;
        }
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: TraceScanningHitScanShot()
// Desc:
//----------------------------------------------------------------------------------------

static void TraceScanningHitScanShot(Vector_t* TargetPosition, BotInfo_t* BotInfo)
{
    size_t HeadingIdx, PitchIdx;
    size_t P1, P2;
    float Heading, Pitch;
    float Distance, Closest = 1000000.0f;
    Vector_t LineStart, LineEnd, Intersect;
    Vector_t Move;
    Plane_t Plane;

    // fire shots around the shooter and store which heading, pitch and power results
    // in the closest shot to the target.

    VectorCopy((*TargetPosition), Plane.Point);

    GetPlayerPosition(GetCurrentPlayer(), &LineStart);
    VectorSubtract(LineStart, (*TargetPosition), Plane.Normal);
    VectorNormalise(&Plane.Normal);

    Heading = 0.0f;
    for (HeadingIdx = 0; HeadingIdx < 8; HeadingIdx++)
    {
        Pitch = 0.0f;
        for (PitchIdx = 0; PitchIdx < 3; PitchIdx++)
        {
            CalcShootVectorAndStartPoint(Heading, Pitch, &Move, &LineStart);

            LineEnd.X = LineStart.X + (8000.0f * Move.X);
            LineEnd.Y = LineStart.Y + (8000.0f * Move.Y);
            LineEnd.Z = LineStart.Z + (8000.0f * Move.Z);

            MathLinePlaneIntersect(&Plane.Point, &Plane.Normal, &LineStart, &LineEnd, &Intersect);

            P1 = ClassifyPoint(&LineStart, &Plane);
            P2 = ClassifyPoint(&LineEnd, &Plane);

            if (P1 != P2)
            {
                Distance = fabsf(VectorMagnitude2(TargetPosition, &Intersect));

                if (Distance < Closest)
                {
                    Closest = Distance;

                    BotInfo->DestHeading  = Heading;
                    BotInfo->DestPitch    = Pitch;
                }
            }

            Pitch += 30.0f;
        }

        Heading += 45.0f;
    }
}


//----------------------------------------------------------------------------------------
// Name: TraceRefiningHitScanShot()
// Desc:
//----------------------------------------------------------------------------------------

static void TraceRefiningHitScanShot(Vector_t* TargetPosition, BotInfo_t* BotInfo)
{
    size_t HeadingIdx, PitchIdx;
    size_t i, P1, P2;
    float Heading, Pitch;
    float Distance, Closest = 1000000.0f;
    Vector_t LineStart, LineEnd, Intersect;
    Vector_t Move;
    Plane_t Plane;
    float OrigPitch;

    // refine the heading and pitch stored in the BotInfo structure by
    // firing shots around the stored heading and pitch.  store which
    // heading and pitch results in the closest shot to the target.

    VectorCopy((*TargetPosition), Plane.Point);

    GetPlayerPosition(GetCurrentPlayer(), &LineStart);
    VectorSubtract(LineStart, (*TargetPosition), Plane.Normal);
    VectorNormalise(&Plane.Normal);

    for (i = 0; i < 20; i++)
    {
        OrigPitch = BotInfo->DestPitch;

        Heading = BotInfo->DestHeading - RAILGUN_HALFHFOV;
        for (HeadingIdx = 0; HeadingIdx < RAILGUN_NUMSTEPS; HeadingIdx++)
        {
            Pitch = OrigPitch - RAILGUN_HALFVFOV;
            for (PitchIdx = 0; PitchIdx < RAILGUN_NUMSTEPS; PitchIdx++)
            {
                CalcShootVectorAndStartPoint(Heading, Pitch, &Move, &LineStart);

                LineEnd.X = LineStart.X + (8000.0f * Move.X);
                LineEnd.Y = LineStart.Y + (8000.0f * Move.Y);
                LineEnd.Z = LineStart.Z + (8000.0f * Move.Z);

                MathLinePlaneIntersect(&Plane.Point, &Plane.Normal, &LineStart, &LineEnd, &Intersect);

                P1 = ClassifyPoint(&LineStart, &Plane);
                P2 = ClassifyPoint(&LineEnd, &Plane);

                if (P1 != P2)
                {
                    Distance = fabsf(VectorMagnitude2(TargetPosition, &Intersect));

                    if (Distance < Closest)
                    {
                        Closest = Distance;

                        BotInfo->DestHeading  = Heading;
                        BotInfo->DestPitch    = Pitch;
                    }
                }

                Pitch += RAILGUN_VFOV / (float)RAILGUN_NUMSTEPS;
            }

            Heading += RAILGUN_HFOV / (float)RAILGUN_NUMSTEPS;
        }
    }

}


//----------------------------------------------------------------------------------------
// Name: SetupBotShotNoise()
// Desc:
//----------------------------------------------------------------------------------------

void SetupBotShotNoise(size_t PlyrIndex, BotInfo_t* BotInfo)
{
    BotType_t Type;

    Type = GetPlayerBotType(PlyrIndex);

    if (BOTTYPE_RANDOM == Type)
        Type = (BotType_t)((size_t)rand() % 5 + 1);

    switch (Type)
    {
    case BOTTYPE_INEPT :
        BotInfo->ShotNoiseThreshold = 20;
        BotInfo->ShotNoiseMin = 32;
        BotInfo->ShotNoiseMax = -32;
        break;

    case BOTTYPE_VINDICTIVE :
        BotInfo->ShotNoiseThreshold = 15;
        BotInfo->ShotNoiseMin = 16;
        BotInfo->ShotNoiseMax = -16;
        break;

    case BOTTYPE_COMPETETIVE :
        BotInfo->ShotNoiseThreshold = 10;
        BotInfo->ShotNoiseMin = 8;
        BotInfo->ShotNoiseMax = -8;
        break;

    case BOTTYPE_GREEDY :
        BotInfo->ShotNoiseThreshold = 5;
        BotInfo->ShotNoiseMin = 4;
        BotInfo->ShotNoiseMax = -4;
        break;

    case BOTTYPE_CRUEL :
        BotInfo->ShotNoiseThreshold = 2;
        BotInfo->ShotNoiseMin = -2;
        BotInfo->ShotNoiseMax = 2;
        break;
    }

    BotInfo->ShotNoiseCounter = 0;
}

void CalculateProjectShotDetails(float Heading, float Pitch, float Power, Vector_t* ShootVector, Vector_t* StartPoint, Vector_t* FinishPoint)
{
    CalcShootVectorAndStartPoint(Heading, Pitch, ShootVector, StartPoint);
    TraceProjectileShot(Heading, Pitch, Power, FinishPoint);
}
