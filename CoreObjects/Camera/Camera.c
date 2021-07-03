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
#include <stdlib.h>
#include <math.h>

#include "Vector.h"
#include "Sound.h"
#include "Utility.h"

#include "CameraTypes.h"
#include "Camera.h"
#include "CameraMouseLook.h"

#include "MeshManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "ShotManager.h"
#include "ObjectManager.h"
#include "BotShoot.h"
#include "StateWaitForShot.h"
#include "View.h"
#include "ShotManager.h"

#include "Timer.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

#define FLYBY_BBOXSIZE          4.0f
#define FLYBY_STEPSIZE          4.0f

#define MOVE_BBOXSIZE           32.0f
#define MOVE_LOOKATSTEPSIZE     32.0f
#define MOVE_LOOKFROMSTEPSIZE   16.0f

#define FOLLOWING_MINDISTANCE   512.0f
#define FOLLOWING_MAXDISTANCE   2048.0f


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;
extern LONGLONG FrameNowMs;
extern float FrameScale;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static const float _FlyByLookFromTimeoutMs = 20000.0f;
static const float _FlyByLookAtTimeoutMs = 30000.0f;
static const size_t _FlyByMinVectors = 10;

static Camera_t _Camera;

static std::vector<Vector_t> _FlyByLookFromPoints;
static std::vector<Vector_t> _FlyByLookAtPoints;
static LONGLONG _FlyByLookFromMs;
static LONGLONG _FlyByLookAtMs;

static std::vector<Vector_t> _CameraTrackingPointsA;
static std::vector<Vector_t> _CameraTrackingPointsB;
static std::vector<Vector_t> _CameraTrackingPointsC;
static Vector_t _CameraTrackingShotStartPoint;
static Vector_t _CameraTrackingShotFinishPoint;
static Vector_t _CameraTrackingShotDirection;
static Vector_t _CameraTrackingShotDirectionUnit;
static float _CameraTrackingShotMagnitude;
static bool _CameraTrackingShotChooseLeft = false;

static bool _LookAtPositionSet = false;
static Vector_t _LookAtPosition;
static Object_t* _LookAtObject = nullptr;

//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _CameraShakeCounter = 0;
static size_t _CameraShakeThreshold = 0;
static long _CameraShakeMin, _CameraShakeMax;


//----------------------------------------------------------------------------------------
// private functions
//----------------------------------------------------------------------------------------

static size_t FlyByThink(void);
static void FlyByMove(void);

static size_t FindTankThink(void);
static void FindTankMove(void);
static size_t IsPointWithinClippingArea(Vector_t* Point);
static void CameraShake(void);

static void PerpendicularStationaryInitialise(size_t ShotID);
static void PerpendicularStationaryThink();
static void PerpendicularStationaryMove();
static void PerpendicularPartialFollowInitialise(size_t ShotID);
static void PerpendicularPartialFollowThink();
static void PerpendicularPartialFollowMove();
static void PerpendicularCompleteFollowInitialise(size_t ShotID);
static void PerpendicularCompleteFollowThink();
static void PerpendicularCompleteFollowMove();
static void CornerStationaryInitialise(size_t ShotID);
static void CornerStationaryThink();
static void CornerStationaryMove();
static void CornerPartialFollowInitialise(size_t ShotID);
static void CornerPartialFollowThink();
static void CornerPartialFollowMove();
static void CornerCompleteFollowInitialise(size_t ShotID);
static void CornerCompleteFollowThink();
static void CornerCompleteFollowMove();
static void FollowShotDirectlyInitialise(size_t ShotID);
static void FollowShotDirectlyThink();
static void FollowShotDirectlyMove();
static void FollowShotWavilyInitialise(size_t ShotID);
static void FollowShotWavilyThink();
static void FollowShotWavilyMove();
static void DollyTrackPerpendicularAcrossPathInitialise(size_t ShotID);
static void DollyTrackPerpendicularAcrossPathThink();
static void DollyTrackPerpendicularAcrossPathMove();
static void DollyTrackConvexArcAcrossPathInitialise(size_t ShotID);
static void DollyTrackConvexArcAcrossPathThink();
static void DollyTrackConvexArcAcrossPathMove();
static void DollyTrackConcaveArcAcrossPathInitialise(size_t ShotID);
static void DollyTrackConcaveArcAcrossPathThink();
static void DollyTrackConcaveArcAcrossPathMove();
static void BeginStationaryEndStationaryInitialise(size_t ShotID);
static void BeginStationaryEndStationaryThink();
static void BeginStationaryEndStationaryMove();
static void BeginStationaryEndPerpendicularAcrossPathInitialise(size_t ShotID);
static void BeginStationaryEndPerpendicularAcrossPathThink();
static void BeginStationaryEndPerpendicularAcrossPathMove();
static void BeginStationaryEndConcexArcAcrossPathInitialise(size_t ShotID);
static void BeginStationaryEndConcexArcAcrossPathThink();
static void BeginStationaryEndConcexArcAcrossPathMove();
static void TrackAirStrikeInitialise(size_t ShotID);
static void TrackAirStrikeThink();
static void TrackAirStrikeMove();
static void TrackMultiProjectileInitialise(size_t ShotID);
static void TrackMultiProjectileThink();
static void TrackMultiProjectileMove();
static void TrackFunkyBombInitialise(size_t ShotID);
static void TrackFunkyBombThink();
static void TrackFunkyBombMove();
static void TrackNapalmInitialise(size_t ShotID);
static void TrackNapalmThink();
static void TrackNapalmMove();
static void RailgunPerpendicularAcrossPathInitialise(size_t ShotID);
static void RailgunPerpendicularAcrossPathThink();
static void RailgunPerpendicularAcrossPathMove();
static void RailgunConcaveArcAcrossPathInitialise(size_t ShotID);
static void RailgunConcaveArcAcrossPathThink();
static void RailgunConcaveArcAcrossPathMove();
static void TrackTankStationaryInitialise(size_t ShotID);
static void TrackTankStationaryThink();
static void TrackTankStationaryMove();
static void TrackTankConcexArcInitialise(size_t ShotID);
static void TrackTankConcexArcThink();
static void TrackTankConcexArcMove();
static void TrackTankFollowInitialise(size_t ShotID);
static void TrackTankFollowThink();
static void TrackTankFollowMove();


//----------------------------------------------------------------------------------------
// Name: IsPointWithinBox()
// Desc:
//----------------------------------------------------------------------------------------

static size_t IsPointWithinBox(Vector_t* Point, Vector_t* Min, Vector_t* Max)
{
    if (Point->X > Min->X && Point->Y > Min->Y && Point->Z > Min->Z &&
            Point->X < Max->X && Point->Y < Max->Y && Point->Z < Max->Z)
        return 1;

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: CreateUnitVector()
// Desc:
//----------------------------------------------------------------------------------------

static void CreateUnitVector(Vector_t* Src, Vector_t* Dst, Vector_t* UnitVec)
{
    VectorSubtract((*Dst), (*Src), (*UnitVec));
    VectorNormalise(UnitVec);
}


//----------------------------------------------------------------------------------------
// Name: ChooseNewAngle()
// Desc:
//----------------------------------------------------------------------------------------

static void ChooseNewAngle(void)
{
    size_t i, Angle;
    float Smallest;
    Vector_t Intersect;
    Vector_t Temp;
    Polygon_t* Polygon;

    // try looking at the shot from Points that form a circle
    // around the shot.  keep looping until we find a point from
    // which a line of sight to the shot is not obstructed.

    Angle = (rand() % 36) * 10;
    for (i = 0; i < 36; i++)
    {
        _Camera.LookFromDest.X = _Camera.PointToFollow->X + ((float)cos((float)Angle * DTOR) * FOLLOWING_MINDISTANCE);
        _Camera.LookFromDest.Z = _Camera.PointToFollow->Z + ((float)sin((float)Angle * DTOR) * FOLLOWING_MINDISTANCE);
        _Camera.LookFromDest.Y = _Camera.PointToFollow->Y + FOLLOWING_MINDISTANCE;

        ClipCameraFollowingPoint(&_Camera.LookFromDest);

        VectorCopy((*_Camera.PointToFollow), Temp);
        Temp.Y += 16.0f;

        if (LineMeshIntersect(g_LandscapeMesh, &_Camera.LookFromDest, &Temp, &Polygon, &Intersect, &Smallest) == IntersectionResult::ClearLineOfSight)
        {
            _Camera.Angle     = (float)Angle;
            _Camera.HaveAngle = 1;
            break;
        }

        Angle += 10;
    }
}


//----------------------------------------------------------------------------------------
// Name: CircleFollowShotMove()
// Desc:
//----------------------------------------------------------------------------------------

static void CircleFollowShotMove(void)
{
    float Smallest;
    Vector_t Intersect;
    Polygon_t* Polygon;
    Vector_t Temp;

    if (!_Camera.HaveAngle)
    {
        ChooseNewAngle();
        return;
    }

    // check if the current angle intersects the landscape
    _Camera.LookFromDest.X = _Camera.PointToFollow->X + ((float)cos(_Camera.Angle * DTOR) * FOLLOWING_MINDISTANCE);
    _Camera.LookFromDest.Z = _Camera.PointToFollow->Z + ((float)sin(_Camera.Angle * DTOR) * FOLLOWING_MINDISTANCE);
    _Camera.LookFromDest.Y = _Camera.PointToFollow->Y + FOLLOWING_MINDISTANCE;

    ClipCameraFollowingPoint(&_Camera.LookFromDest);

    VectorCopy((*_Camera.PointToFollow), Temp);
    Temp.Y += 16.0f;

    if (LineMeshIntersect(g_LandscapeMesh, &_Camera.LookFromDest, &Temp, &Polygon, &Intersect, &Smallest) == IntersectionResult::IntersectsLandscape)
        ChooseNewAngle();
}


//----------------------------------------------------------------------------------------
// Name: FollowingShotMove()
// Desc:
//----------------------------------------------------------------------------------------

static void FollowingShotMove(void)
{
    Vector_t Forward, Up, Right;
    Vector_t TankPosition, MidPoint;
    Vector_t Temp, Intersect;
    Vector_t Normal;
    float Mag, Distance, Smallest;
    Polygon_t* Polygon;

    // calc a perpendicular unit vector to the vector from the
    // shooter to the shot

    GetPlayerPosition(GetCurrentPlayer(), &TankPosition);
    VectorSubtract((*_Camera.PointToFollow), TankPosition, Forward);

    Up.X = Up.Z = 0.0f; Up.Y = 1.0f;
    VectorCrossProd(Forward, Up, Right);
    VectorNormalise(&Right);

    // calc the midpoint between the shooter and the shot
    Mag = fabsf(VectorMagnitude(&Forward)) / 2.0f;
    VectorCopy(Forward, Normal);
    VectorNormalise(&Normal);
    MidPoint.X = TankPosition.X + (Normal.X * Mag);
    MidPoint.Y = TankPosition.Y + (Normal.Y * Mag);
    MidPoint.Z = TankPosition.Z + (Normal.Z * Mag);

    // project a point along the normal from this midpoint
    if (Mag < FOLLOWING_MINDISTANCE)
        Distance = FOLLOWING_MINDISTANCE;
    else if (Mag > FOLLOWING_MAXDISTANCE)
        Distance = FOLLOWING_MAXDISTANCE;
    else
        Distance = Mag;

    _Camera.LookFromDest.X = MidPoint.X + (Right.X * Distance);
    _Camera.LookFromDest.Y = MidPoint.Y + (Right.Y * Distance);
    _Camera.LookFromDest.Z = MidPoint.Z + (Right.Z * Distance);

    ClipCameraFollowingPoint(&_Camera.LookFromDest);

    VectorCopy((*_Camera.PointToFollow), Temp);
    Temp.Y += 16.0f;

    if (LineMeshIntersect(g_LandscapeMesh, &_Camera.LookFromDest, &Temp, &Polygon, &Intersect, &Smallest) == IntersectionResult::IntersectsLandscape)
    {
        if (IsPointWithinClippingArea(_Camera.PointToFollow))
            CircleFollowShotMove();
        else
        {
            VectorCopy((*_Camera.PointToFollow), _Camera.Position);
            ClipCamera(&_Camera.Position);
            CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
            SetListenerOnCamera();
            return;
        }
    }

    // update this point so that changing to "looking at point" will be a smooth
    // camera transition
    VectorGetMoveYX(_Camera.Rotation.Y * DTOR, _Camera.Rotation.X * DTOR, &Forward);
    _Camera.LookAtCrnt.X = _Camera.Position.X - (Forward.X * 128.0f);
    _Camera.LookAtCrnt.Y = _Camera.Position.Y - (Forward.Y * 128.0f);
    _Camera.LookAtCrnt.Z = _Camera.Position.Z - (Forward.Z * 128.0f);

    SetListenerOnCamera();
}


//-----------------------------------------------------------------------------
// Name: CameraLookingAtPointThink()
// Desc:
//-----------------------------------------------------------------------------

static void CameraLookingAtPointThink(void)
{
    Vector_t Min, Max, V;

    Min.X = _Camera.PointToFollow->X - MOVE_BBOXSIZE; Min.Y = _Camera.PointToFollow->Y - MOVE_BBOXSIZE;
    Min.Z = _Camera.PointToFollow->Z - MOVE_BBOXSIZE; Max.X = _Camera.PointToFollow->X + MOVE_BBOXSIZE;
    Max.Y = _Camera.PointToFollow->Y + MOVE_BBOXSIZE; Max.Z = _Camera.PointToFollow->Z + MOVE_BBOXSIZE;

    if (!IsPointWithinBox(&_Camera.LookAtCrnt, &Min, &Max))
    {
        CreateUnitVector(&_Camera.LookAtCrnt, _Camera.PointToFollow, &V);
        _Camera.LookAtCrnt.X += (V.X * MOVE_LOOKATSTEPSIZE);
        _Camera.LookAtCrnt.Y += (V.Y * MOVE_LOOKATSTEPSIZE);
        _Camera.LookAtCrnt.Z += (V.Z * MOVE_LOOKATSTEPSIZE);
        return;
    }

    VectorCopy((*_Camera.PointToFollow), _Camera.LookAtCrnt);
}



//----------------------------------------------------------------------------------------
// Name: DistanceFollowThink()
// Desc:
//----------------------------------------------------------------------------------------

static void DistanceFollowThink(void)
{
    float Smallest;
    Vector_t Intersect;
    Polygon_t* Polygon;
    Vector_t Temp;

    if (!_Camera.HaveAngle)
    {
        ChooseNewAngle();
        VectorCopy(_Camera.LookFromDest, _Camera.Position);
        return;
    }

    // check if the current angle intersects the landscape
    _Camera.Position.X = _Camera.PointToFollow->X + ((float)cos(_Camera.Angle * DTOR) * FOLLOWING_MINDISTANCE);
    _Camera.Position.Z = _Camera.PointToFollow->Z + ((float)sin(_Camera.Angle * DTOR) * FOLLOWING_MINDISTANCE);
    _Camera.Position.Y = _Camera.PointToFollow->Y + FOLLOWING_MINDISTANCE;

    ClipCameraFollowingPoint(&_Camera.LookFromDest);

    VectorCopy((*_Camera.PointToFollow), Temp);
    Temp.Y += 16.0f;

    if (LineMeshIntersect(g_LandscapeMesh, &_Camera.Position, &Temp, &Polygon, &Intersect, &Smallest) == IntersectionResult::IntersectsLandscape)
    {
        ChooseNewAngle();
        VectorCopy(_Camera.LookFromDest, _Camera.Position);
    }

    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}


//----------------------------------------------------------------------------------------
// Name: GenerateRandomPointWithinTheLandscape()
// Desc:
//----------------------------------------------------------------------------------------

static Vector_t GenerateRandomPointWithinTheLandscape(bool OnLandscape)
{
    float MaxX, MaxY, MaxZ;
    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    int RandomValue = rand() % int(MaxX / 2.0f);
    float X = (MaxX / 4.0f) + float(RandomValue);

    RandomValue = rand() % int(MaxZ / 2.0f);
    float Z = (MaxZ / 4.0f) + float(RandomValue);

    float TileWidth, TileDepth;
    GetMeshTileSize(g_LandscapeMesh, &TileWidth, &TileDepth);
    float Y = GetMeshHeight(g_LandscapeMesh, X, Z, nullptr);
    if (!OnLandscape)
    {
        Y += 6.0f * TileWidth;
        if (Y > MaxY)
            Y = MaxY;
    }

    return { X, Y, Z };
}


//----------------------------------------------------------------------------------------
// Name: CameraThink()
// Desc:
//----------------------------------------------------------------------------------------

size_t CameraThink(void)
{
    switch (_Camera.State)
    {
    case CAMERASTATE_IDLE:
        break;
    case CAMERASTATE_FLYBY:
        return FlyByThink();
    case CAMERASTATE_MOUSELOOK:
        break;
    case CAMERASTATE_FOLLOWINGPOINT:
        CameraLookAt(&_Camera.LookFromDest, _Camera.PointToFollow);
        SetListenerOnCamera();
        break;
    case CAMERASTATE_LOOKINGATPOINT:
        CameraLookingAtPointThink();
        CameraLookAt(&_Camera.Position, &_Camera.LookAtCrnt);
        SetListenerOnCamera();
        break;
    case CAMERASTATE_FINDTANK:
        return FindTankThink();
    case CAMERASTATE_DISTANCEFOLLOW:
        DistanceFollowThink();
        break;
    case CAMERASTATE_PERPENDICULAR_STATIONARY:
        PerpendicularStationaryThink();
        break;
    case CAMERASTATE_PERPENDICULAR_PARTIAL_FOLLOW:
        PerpendicularPartialFollowThink();
        break;
    case CAMERASTATE_PERPENDICULAR_COMPLETE_FOLLOW:
        PerpendicularCompleteFollowThink();
        break;
    case CAMERASTATE_CORNER_STATIONARY:
        CornerStationaryThink();
        break;
    case CAMERASTATE_CORNER_PARTIAL_FOLLOW:
        CornerPartialFollowThink();
        break;
    case CAMERASTATE_CORNER_COMPLETE_FOLLOW:
        CornerCompleteFollowThink();
        break;
    case CAMERASTATE_FOLLOW_SHOT_DIRECTLY:
        FollowShotDirectlyThink();
        break;
    case CAMERASTATE_FOLLOW_SHOT_WAVILY:
        FollowShotWavilyThink();
        break;
    case CAMERASTATE_DOLLY_TRACK_PERPENDICULAR_ACROSS_PATH:
        DollyTrackPerpendicularAcrossPathThink();
        break;
    case CAMERASTATE_DOLLY_TRACK_CONVEX_ARC_ACROSS_PATH:
        DollyTrackConvexArcAcrossPathThink();
        break;
    case CAMERASTATE_DOLLY_TRACK_CONCAVE_ARC_ACROSS_PATH:
        DollyTrackConcaveArcAcrossPathThink();
        break;
    case CAMERASTATE_BEGIN_STATIONARY_END_STATIONARY:
        BeginStationaryEndStationaryThink();
        break;
    case CAMERASTATE_BEGIN_STATIONARY_END_PERPENDICULAR_ACROSS_PATH:
        BeginStationaryEndPerpendicularAcrossPathThink();
        break;
    case CAMERASTATE_BEGIN_STATIONARY_END_CONCEX_ARC_ACROSS_PATH:
        BeginStationaryEndConcexArcAcrossPathThink();
        break;
    case CAMERASTATE_TRACK_AIR_STRIKE:
        TrackAirStrikeThink();
        break;
    case CAMERASTATE_TRACK_MULTI_PROJECTILE:
        TrackMultiProjectileThink();
        break;
    case CAMERASTATE_TRACK_FUNKY_BOMB:
        TrackFunkyBombThink();
        break;
    case CAMERASTATE_TRACK_NAPALM:
        TrackNapalmThink();
        break;
    case CAMERASTATE_RAILGUN_PERPENDICULAR_ACROSS_PATH:
        RailgunPerpendicularAcrossPathThink();
        break;
    case CAMERASTATE_RAILGUN_CONCAVE_ARC_ACROSS_PATH:
        RailgunConcaveArcAcrossPathThink();
        break;
    case CAMERASTATE_TRACK_TANK_STATIONARY:
        TrackTankStationaryThink();
        break;
    case CAMERASTATE_TRACK_TANK_CONCEX_ARC:
        TrackTankConcexArcThink();
        break;
    case CAMERASTATE_TRACK_TANK_FOLLOW:
        TrackTankFollowThink();
        break;
    }

    return 0;
}


//----------------------------------------------------------------------------------------
// Name: CameraMove()
// Desc:
//----------------------------------------------------------------------------------------

void CameraMove(void)
{
    switch (_Camera.State)
    {
    case CAMERASTATE_IDLE:
        break;
    case CAMERASTATE_FLYBY:
        FlyByMove();
        break;
    case CAMERASTATE_MOUSELOOK:
        CameraMouseLook(&_Camera);
        break;
    case CAMERASTATE_FOLLOWINGPOINT:
        FollowingShotMove();
        break;
    case CAMERASTATE_LOOKINGATPOINT:
        break;
    case CAMERASTATE_FINDTANK:
        FindTankMove();
        break;
    case CAMERASTATE_DISTANCEFOLLOW:
        break;
    case CAMERASTATE_PERPENDICULAR_STATIONARY:
        PerpendicularStationaryMove();
        break;
    case CAMERASTATE_PERPENDICULAR_PARTIAL_FOLLOW:
        PerpendicularPartialFollowMove();
        break;
    case CAMERASTATE_PERPENDICULAR_COMPLETE_FOLLOW:
        PerpendicularCompleteFollowMove();
        break;
    case CAMERASTATE_CORNER_STATIONARY:
        CornerStationaryMove();
        break;
    case CAMERASTATE_CORNER_PARTIAL_FOLLOW:
        CornerPartialFollowMove();
        break;
    case CAMERASTATE_CORNER_COMPLETE_FOLLOW:
        CornerCompleteFollowMove();
        break;
    case CAMERASTATE_FOLLOW_SHOT_DIRECTLY:
        FollowShotDirectlyMove();
        break;
    case CAMERASTATE_FOLLOW_SHOT_WAVILY:
        FollowShotWavilyMove();
        break;
    case CAMERASTATE_DOLLY_TRACK_PERPENDICULAR_ACROSS_PATH:
        DollyTrackPerpendicularAcrossPathMove();
        break;
    case CAMERASTATE_DOLLY_TRACK_CONVEX_ARC_ACROSS_PATH:
        DollyTrackConvexArcAcrossPathMove();
        break;
    case CAMERASTATE_DOLLY_TRACK_CONCAVE_ARC_ACROSS_PATH:
        DollyTrackConcaveArcAcrossPathMove();
        break;
    case CAMERASTATE_BEGIN_STATIONARY_END_STATIONARY:
        BeginStationaryEndStationaryMove();
        break;
    case CAMERASTATE_BEGIN_STATIONARY_END_PERPENDICULAR_ACROSS_PATH:
        BeginStationaryEndPerpendicularAcrossPathMove();
        break;
    case CAMERASTATE_BEGIN_STATIONARY_END_CONCEX_ARC_ACROSS_PATH:
        BeginStationaryEndConcexArcAcrossPathMove();
        break;
    case CAMERASTATE_TRACK_AIR_STRIKE:
        TrackAirStrikeMove();
        break;
    case CAMERASTATE_TRACK_MULTI_PROJECTILE:
        TrackMultiProjectileMove();
        break;
    case CAMERASTATE_TRACK_FUNKY_BOMB:
        TrackFunkyBombMove();
        break;
    case CAMERASTATE_TRACK_NAPALM:
        TrackNapalmMove();
        break;
    case CAMERASTATE_RAILGUN_PERPENDICULAR_ACROSS_PATH:
        RailgunPerpendicularAcrossPathMove();
        break;
    case CAMERASTATE_RAILGUN_CONCAVE_ARC_ACROSS_PATH:
        RailgunConcaveArcAcrossPathMove();
        break;
    case CAMERASTATE_TRACK_TANK_STATIONARY:
        TrackTankStationaryMove();
        break;
    case CAMERASTATE_TRACK_TANK_CONCEX_ARC:
        TrackTankConcexArcMove();
        break;
    case CAMERASTATE_TRACK_TANK_FOLLOW:
        TrackTankFollowMove();
        break;
    }

    CameraShake();
}


//----------------------------------------------------------------------------------------
// Name: CameraCollide()
// Desc:
//----------------------------------------------------------------------------------------

void CameraCollide(void)
{
    switch (_Camera.State)
    {
    case CAMERASTATE_IDLE:
    case CAMERASTATE_MOUSELOOK:
    case CAMERASTATE_DISTANCEFOLLOW:
        break;
    default:
        ClipCamera(&_Camera.Position);
        break;
    }
}


//----------------------------------------------------------------------------------------
// Name: SetCameraState()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraState(CameraState_t State, CameraType_t Type)
{
    _Camera.State = State;
    _Camera.Type = Type;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraState()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraStateAndShotID(CameraState_t State, size_t ShotID)
{
    //_Camera.State = State;
    _Camera.State = CAMERASTATE_CORNER_COMPLETE_FOLLOW;
    _Camera.Type = CAMERATYPE_MOUSELOOK;

    //switch (_Camera.State)
    //{
    //case CAMERASTATE_PERPENDICULAR_STATIONARY:
    //    PerpendicularStationaryInitialise(ShotID);
    //    break;
    //case CAMERASTATE_PERPENDICULAR_PARTIAL_FOLLOW:
    //    PerpendicularPartialFollowInitialise(ShotID);
    //    break;
    //case CAMERASTATE_PERPENDICULAR_COMPLETE_FOLLOW:
    //    PerpendicularCompleteFollowInitialise(ShotID);
    //    break;
    //case CAMERASTATE_CORNER_STATIONARY:
        CornerStationaryInitialise(ShotID);
    //    break;
    //case CAMERASTATE_CORNER_PARTIAL_FOLLOW:
    //    CornerPartialFollowInitialise(ShotID);
    //    break;
    //case CAMERASTATE_CORNER_COMPLETE_FOLLOW:
        CornerCompleteFollowInitialise(ShotID);
    //    break;
    //case CAMERASTATE_FOLLOW_SHOT_DIRECTLY:
    //    FollowShotDirectlyInitialise(ShotID);
    //    break;
    //case CAMERASTATE_FOLLOW_SHOT_WAVILY:
    //    FollowShotWavilyInitialise(ShotID);
    //    break;
    //case CAMERASTATE_DOLLY_TRACK_PERPENDICULAR_ACROSS_PATH:
    //    DollyTrackPerpendicularAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_DOLLY_TRACK_CONVEX_ARC_ACROSS_PATH:
    //    DollyTrackConvexArcAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_DOLLY_TRACK_CONCAVE_ARC_ACROSS_PATH:
    //    DollyTrackConcaveArcAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_BEGIN_STATIONARY_END_STATIONARY:
    //    BeginStationaryEndStationaryInitialise(ShotID);
    //    break;
    //case CAMERASTATE_BEGIN_STATIONARY_END_PERPENDICULAR_ACROSS_PATH:
    //    BeginStationaryEndPerpendicularAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_BEGIN_STATIONARY_END_CONCEX_ARC_ACROSS_PATH:
    //    BeginStationaryEndConcexArcAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_AIR_STRIKE:
    //    TrackAirStrikeInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_MULTI_PROJECTILE:
    //    TrackMultiProjectileInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_FUNKY_BOMB:
    //    TrackFunkyBombInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_NAPALM:
    //    TrackNapalmInitialise(ShotID);
    //    break;
    //case CAMERASTATE_RAILGUN_PERPENDICULAR_ACROSS_PATH:
    //    RailgunPerpendicularAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_RAILGUN_CONCAVE_ARC_ACROSS_PATH:
    //    RailgunConcaveArcAcrossPathInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_TANK_STATIONARY:
    //    TrackTankStationaryInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_TANK_CONCEX_ARC:
    //    TrackTankConcexArcInitialise(ShotID);
    //    break;
    //case CAMERASTATE_TRACK_TANK_FOLLOW:
    //    TrackTankFollowInitialise(ShotID);
    //    break;
    //}
}


//----------------------------------------------------------------------------------------
// Name: GetCameraState()
// Desc:
//----------------------------------------------------------------------------------------

void GetCameraState(CameraState_t* State, CameraType_t* Type)
{
    if (State)
        (*State) = _Camera.State;

    if (Type)
        (*Type) = _Camera.Type;
}


//----------------------------------------------------------------------------------------
// Name: CameraLookAt()
// Desc:
//----------------------------------------------------------------------------------------

void CameraLookAt(Vector_t* Eye, Vector_t* Center)
{
    Vector_t V;

    //VectorSubtract( (*Center), (*Eye), V );
    //VectorInverse( V );
    VectorSubtract((*Eye), (*Center), V);               // this achieves the same thing as the previous 2 lines
    // tho is technically backwards...
    VectorNormalise(&V);
    VectorCopy((*Eye), _Camera.Position);

    VectorGetAnglesXY(&V, &_Camera.Rotation.X, &_Camera.Rotation.Y);
    _Camera.Rotation.Z = 0.0f;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraPosition()
// Desc:
//----------------------------------------------------------------------------------------

Vector_t* GetCameraPosition(void)
{
    return &_Camera.Position;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraRotation()
// Desc:
//----------------------------------------------------------------------------------------

Vector_t* GetCameraRotation(void)
{
    return &_Camera.Rotation;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraPosition()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraPosition(Vector_t* Position)
{
    VectorCopy((*Position), _Camera.Position);
}


//----------------------------------------------------------------------------------------
// Name: SetCameraRotation()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraRotation(Vector_t* Rotation)
{
    VectorCopy((*Rotation), _Camera.Rotation);
}


//-----------------------------------------------------------------------------
// Name: ClipCamera
// Desc:
//-----------------------------------------------------------------------------

void ClipCamera(Vector_t* Point)
{
    float MaxX, MaxY, MaxZ;
    float Quarter, Height;

    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    Quarter = MaxX / 4.0f;

    if (Point->X < Quarter)
        Point->X = Quarter;
    else if (Point->X > MaxX - Quarter)
        Point->X = MaxX - Quarter;

    Quarter = MaxZ / 4.0f;

    if (Point->Z < Quarter)
        Point->Z = Quarter;
    else if (Point->Z > MaxZ - Quarter)
        Point->Z = MaxZ - Quarter;

    if (Point->X < 0.0f || Point->X > MaxX ||
            Point->Z < 0.0f || Point->Z > MaxZ)
        Height = 0.0f;
    else
        Height = GetMeshHeight(g_LandscapeMesh, Point->X, Point->Z, nullptr);

    if (Point->Y < Height + 50.0f)
        Point->Y = Height + 50.0f;
    else if (Point->Y > MaxY)
        Point->Y = MaxY;

}


//-----------------------------------------------------------------------------
// Name: ClipCameraFollowingPoint
// Desc:
//-----------------------------------------------------------------------------

void ClipCameraFollowingPoint(Vector_t* Point)
{
    float MaxX, MaxY, MaxZ;
    float Quarter, Height;

    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    Quarter = MaxX / 4.0f;

    if (Point->X < Quarter)
        Point->X = Quarter;
    else if (Point->X > MaxX - Quarter)
        Point->X = MaxX - Quarter;

    Quarter = MaxZ / 4.0f;

    if (Point->Z < Quarter)
        Point->Z = Quarter;
    else if (Point->Z > MaxZ - Quarter)
        Point->Z = MaxZ - Quarter;

    if (Point->X < 0.0f || Point->X > MaxX ||
            Point->Z < 0.0f || Point->Z > MaxZ)
        Height = 0.0f;
    else
        Height = GetMeshHeight(g_LandscapeMesh, Point->X, Point->Z, nullptr);

    if (Height < 1024.0f)
        Height = 1024.0f;

    if (Point->Y < Height + 50.0f)
        Point->Y = Height + 50.0f;
    else if (Point->Y > MaxY)
        Point->Y = MaxY;
}


//-----------------------------------------------------------------------------
// Name: GetCameraDoneFlags()
// Desc:
//-----------------------------------------------------------------------------

void GetCameraDoneFlags(size_t* LookFrom, size_t* LookAt)
{
    if (LookFrom)
        (*LookFrom) = _Camera.LookFromDone;

    if (LookAt)
        (*LookAt) = _Camera.LookAtDone;
}


//----------------------------------------------------------------------------------------
// Name: CameraChooseRandomFlyByLookFromPoints()
// Desc:
//----------------------------------------------------------------------------------------

static void CameraChooseRandomFlyByLookFromPoints()
{
    _FlyByLookFromMs = GetNowMs();

    while (_FlyByLookFromPoints.size() < _FlyByMinVectors)
        _FlyByLookFromPoints.push_back(GenerateRandomPointWithinTheLandscape(false));
}


//----------------------------------------------------------------------------------------
// Name: CameraChooseRandomFlyByLookAtPoints()
// Desc:
//----------------------------------------------------------------------------------------

static void CameraChooseRandomFlyByLookAtPoints()
{
    _FlyByLookAtMs = GetNowMs();

    while (_FlyByLookAtPoints.size() < _FlyByMinVectors)
        _FlyByLookAtPoints.push_back(GenerateRandomPointWithinTheLandscape(false));
}


//----------------------------------------------------------------------------------------
// Name: InitialiseFlyByPoints()
// Desc:
//----------------------------------------------------------------------------------------

void InitialiseFlyByPoints(void)
{
    CameraChooseRandomFlyByLookFromPoints();
    CameraChooseRandomFlyByLookAtPoints();
}


//----------------------------------------------------------------------------------------
// Name: InitialiseLookAtPoints()
// Desc:
//----------------------------------------------------------------------------------------

void InitialiseLookAtPoints(void)
{
    Vector_t Move;

    _Camera.LookAtDone = 0;

    // project a point straight out infront of the player
    VectorGetMoveYX(_Camera.Rotation.Y * DTOR, _Camera.Rotation.X * DTOR, &Move);
    _Camera.LookAtCrnt.X = _Camera.Position.X - (Move.X * 128.0f);
    _Camera.LookAtCrnt.Y = _Camera.Position.Y - (Move.Y * 128.0f);
    _Camera.LookAtCrnt.Z = _Camera.Position.Z - (Move.Z * 128.0f);
}


//-----------------------------------------------------------------------------
// Name: FlyByThink
// Desc:
//-----------------------------------------------------------------------------

static size_t FlyByThink(void)
{
    auto DeltaMs = FrameNowMs - _FlyByLookAtMs;
    auto Percentage = DeltaMs / _FlyByLookAtTimeoutMs;

    _Camera.LookAtCrnt.X = CubicBezierInterp(_FlyByLookAtPoints[0].X, _FlyByLookAtPoints[1].X, _FlyByLookAtPoints[2].X, _FlyByLookAtPoints[3].X, _FlyByLookAtPoints[4].X, Percentage);
    _Camera.LookAtCrnt.Y = CubicBezierInterp(_FlyByLookAtPoints[0].Y, _FlyByLookAtPoints[1].Y, _FlyByLookAtPoints[2].Y, _FlyByLookAtPoints[3].Y, _FlyByLookAtPoints[4].Y, Percentage);
    _Camera.LookAtCrnt.Z = CubicBezierInterp(_FlyByLookAtPoints[0].Z, _FlyByLookAtPoints[1].Z, _FlyByLookAtPoints[2].Z, _FlyByLookAtPoints[3].Z, _FlyByLookAtPoints[4].Z, Percentage);

    SetListenerOnCamera();

    if (Percentage > 1.0f)
    {
        _FlyByLookAtPoints.erase(_FlyByLookAtPoints.begin());
        _FlyByLookAtPoints.erase(_FlyByLookAtPoints.begin());
        _FlyByLookAtPoints.erase(_FlyByLookAtPoints.begin());
        _FlyByLookAtPoints.erase(_FlyByLookAtPoints.begin());
        _FlyByLookAtPoints.erase(_FlyByLookAtPoints.begin());
        _FlyByLookAtPoints.insert(_FlyByLookAtPoints.begin(), _Camera.LookAtCrnt);
        CameraChooseRandomFlyByLookAtPoints();
    }

    CameraLookAt(&_Camera.Position, &_Camera.LookAtCrnt);
    SetListenerOnCamera();
    return 1;
}


//-----------------------------------------------------------------------------
// Name: FlyByMove()
// Desc:
//-----------------------------------------------------------------------------

static void FlyByMove(void)
{
    auto DeltaMs = FrameNowMs - _FlyByLookFromMs;
    auto Percentage = DeltaMs / _FlyByLookFromTimeoutMs;

    _Camera.Position.X = CubicBezierInterp(_FlyByLookFromPoints[0].X, _FlyByLookFromPoints[1].X, _FlyByLookFromPoints[2].X, _FlyByLookFromPoints[3].X, _FlyByLookFromPoints[4].X, Percentage);
    _Camera.Position.Y = CubicBezierInterp(_FlyByLookFromPoints[0].Y, _FlyByLookFromPoints[1].Y, _FlyByLookFromPoints[2].Y, _FlyByLookFromPoints[3].Y, _FlyByLookFromPoints[4].Y, Percentage);
    _Camera.Position.Z = CubicBezierInterp(_FlyByLookFromPoints[0].Z, _FlyByLookFromPoints[1].Z, _FlyByLookFromPoints[2].Z, _FlyByLookFromPoints[3].Z, _FlyByLookFromPoints[4].Z, Percentage);

    SetListenerOnCamera();

    if (Percentage > 1.0f)
    {
        _FlyByLookFromPoints.erase(_FlyByLookFromPoints.begin());
        _FlyByLookFromPoints.erase(_FlyByLookFromPoints.begin());
        _FlyByLookFromPoints.erase(_FlyByLookFromPoints.begin());
        _FlyByLookFromPoints.erase(_FlyByLookFromPoints.begin());
        _FlyByLookFromPoints.erase(_FlyByLookFromPoints.begin());
        _FlyByLookFromPoints.insert(_FlyByLookFromPoints.begin(), _Camera.Position);
        CameraChooseRandomFlyByLookFromPoints();
    }
}


//-----------------------------------------------------------------------------
// Name: InitialiseFindTankPoints()
// Desc:
//-----------------------------------------------------------------------------

void InitialiseFindTankPoints(Vector_t* TankPosition, Vector_t* GroundNormal)
{
    Vector_t Move;
    Vector_t Rotation;

    _Camera.LookAtDone = _Camera.LookFromDone = 0;
    _Camera.AttachedBarrell = -1;
    _Camera.AttachedTank    = -1;

    // project a point straight out infront of the player
    VectorGetMoveYX(_Camera.Rotation.Y * DTOR, _Camera.Rotation.X * DTOR, &Move);
    _Camera.LookAtCrnt.X = _Camera.Position.X - (Move.X * 256.0f);
    _Camera.LookAtCrnt.Y = _Camera.Position.Y - (Move.Y * 256.0f);
    _Camera.LookAtCrnt.Z = _Camera.Position.Z - (Move.Z * 256.0f);

    // setup the "destination" point structures
    VectorCopy((*GroundNormal), Rotation);
    Rotation.Y = 0.0f;
    VectorNormalise(&Rotation);
    Rotation.Y = 1.0f;

    _Camera.LookFromDest.X = TankPosition->X + (Rotation.X * 256.0f);
    _Camera.LookFromDest.Y = TankPosition->Y + (Rotation.Y * 256.0f);
    _Camera.LookFromDest.Z = TankPosition->Z + (Rotation.Z * 256.0f);
    ClipCamera(&_Camera.LookFromDest);

    VectorCopy((*TankPosition), _Camera.LookAtDest);
}


//-----------------------------------------------------------------------------
// Name: FindTankThink()
// Desc:
//-----------------------------------------------------------------------------

static size_t FindTankThink(void)
{
    Vector_t Min, Max;

    if (!_Camera.LookAtDone)
    {
        Min.X = _Camera.LookAtDest.X - MOVE_BBOXSIZE; Min.Y = _Camera.LookAtDest.Y - MOVE_BBOXSIZE;
        Min.Z = _Camera.LookAtDest.Z - MOVE_BBOXSIZE; Max.X = _Camera.LookAtDest.X + MOVE_BBOXSIZE;
        Max.Y = _Camera.LookAtDest.Y + MOVE_BBOXSIZE; Max.Z = _Camera.LookAtDest.Z + MOVE_BBOXSIZE;

        if (IsPointWithinBox(&_Camera.LookAtCrnt, &Min, &Max))
        {
            VectorCopy(_Camera.LookAtDest, _Camera.LookAtCrnt);
            _Camera.LookAtDone = 1;
        }
    }

    if (!_Camera.LookFromDone)
    {
        Min.X = _Camera.LookFromDest.X - MOVE_BBOXSIZE; Min.Y = _Camera.LookFromDest.Y - MOVE_BBOXSIZE;
        Min.Z = _Camera.LookFromDest.Z - MOVE_BBOXSIZE; Max.X = _Camera.LookFromDest.X + MOVE_BBOXSIZE;
        Max.Y = _Camera.LookFromDest.Y + MOVE_BBOXSIZE; Max.Z = _Camera.LookFromDest.Z + MOVE_BBOXSIZE;

        if (IsPointWithinBox(&_Camera.Position, &Min, &Max))
        {
            VectorCopy(_Camera.LookFromDest, _Camera.Position);
            _Camera.LookFromDone = 1;
        }
    }

    if (_Camera.LookAtDone && _Camera.LookFromDone)
        return 1;

    return 0;
}


//-----------------------------------------------------------------------------
// Name: FindTankMove()
// Desc:
//-----------------------------------------------------------------------------

static void FindTankMove(void)
{
    Vector_t V;

    if (!_Camera.LookFromDone)
    {
        CreateUnitVector(&_Camera.Position, &_Camera.LookFromDest, &V);
        _Camera.Position.X += (V.X * MOVE_LOOKFROMSTEPSIZE);
        _Camera.Position.Y += (V.Y * MOVE_LOOKFROMSTEPSIZE);
        _Camera.Position.Z += (V.Z * MOVE_LOOKFROMSTEPSIZE);

        SetListenerOnCamera();
    }

    if (!_Camera.LookAtDone)
    {
        CreateUnitVector(&_Camera.LookAtCrnt, &_Camera.LookAtDest, &V);
        _Camera.LookAtCrnt.X += (V.X * MOVE_LOOKATSTEPSIZE);
        _Camera.LookAtCrnt.Y += (V.Y * MOVE_LOOKATSTEPSIZE);
        _Camera.LookAtCrnt.Z += (V.Z * MOVE_LOOKATSTEPSIZE);
    }

    CameraLookAt(&_Camera.Position, &_Camera.LookAtCrnt);
    SetListenerOnCamera();
}


//-----------------------------------------------------------------------------
// Name: GetCameraType()
// Desc:
//-----------------------------------------------------------------------------

CameraType_t GetCameraType(void)
{
    return _Camera.Type;
}


//----------------------------------------------------------------------------------------
// Name: AttachedCameraToPlayer()
// Desc:
//----------------------------------------------------------------------------------------

void AttachCameraToPlayer(size_t PlayerID)
{
    _Camera.AttachedTank = PlayerID;
}


//----------------------------------------------------------------------------------------
// Name: AttachedCameraToPlayerBarrell()
// Desc:
//----------------------------------------------------------------------------------------

void AttachCameraToPlayerBarrell(size_t PlayerID)
{
    _Camera.AttachedBarrell = PlayerID;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraViewDistance()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraViewDistance(float ViewDistance)
{
    _Camera.ViewDistance = ViewDistance;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraViewDistance()
// Desc:
//----------------------------------------------------------------------------------------

float GetCameraViewDistance(void)
{
    return _Camera.ViewDistance;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraViewAngle()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraViewAngle(float ViewAngle)
{
    _Camera.ViewAngle = ViewAngle;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraViewAngle()
// Desc:
//----------------------------------------------------------------------------------------

float GetCameraViewAngle(void)
{
    return _Camera.ViewAngle;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraViewHeight()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraViewHeight(float ViewHeight)
{
    _Camera.ViewHeight = ViewHeight;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraViewHeight()
// Desc:
//----------------------------------------------------------------------------------------

float GetCameraViewHeight(void)
{
    return _Camera.ViewHeight;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraAttachedPlayer()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetCameraAttachedPlayer(void)
{
    return _Camera.AttachedTank;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraAttachedPlayerBarrell()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetCameraAttachedPlayerBarrell(void)
{
    return _Camera.AttachedBarrell;
}


//----------------------------------------------------------------------------------------
// Name: SetPointToFollow()
// Desc:
//----------------------------------------------------------------------------------------

void SetPointToFollow(Vector_t* Point)
{
    if (Point)
    {
        _Camera.HaveAngle = 0;
        _Camera.PointToFollow = Point;
    }
}


//----------------------------------------------------------------------------------------
// Name: SetCameraOverCurrentPlayer()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraOverCurrentPlayer(void)
{
    GetPlayerPosition(GetCurrentPlayer(), &_Camera.Position);
    _Camera.Position.Y += 128.0f;

    VectorSet(_Camera.Rotation, 0.0f);
}


//----------------------------------------------------------------------------------------
// Name: SetListenerOnCamera()
// Desc:
//----------------------------------------------------------------------------------------

void SetListenerOnCamera(void)
{
    Vector_t Forward, Right, Up;

    VectorGetMoveYX(_Camera.Rotation.Y * DTOR, _Camera.Rotation.X * DTOR, &Forward);
    Up.X = Up.Z = 0.0f; Up.Y = 1.0f;

    VectorCrossProd(Forward, Up, Right);
    VectorCrossProd(Forward, Right, Up);
    VectorNormalise(&Up);

    SetSoundListenerPosition(_Camera.Position.X, _Camera.Position.Y, _Camera.Position.Z,
                             Forward.X, Forward.Y, Forward.Z, Up.X, Up.Y, Up.Z);
}


//----------------------------------------------------------------------------------------
// Name: IsPointWithinClippingArea()
// Desc:
//----------------------------------------------------------------------------------------

static size_t IsPointWithinClippingArea(Vector_t* Point)
{
    float MaxX, MaxY, MaxZ;
    float Quarter;

    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    Quarter = MaxX / 4.0f;

    if (Point->X < Quarter)
        return 0;
    else if (Point->X > MaxX - Quarter)
        return 0;

    Quarter = MaxZ / 4.0f;

    if (Point->Z < Quarter)
        return 0;
    else if (Point->Z > MaxZ - Quarter)
        return 0;

    return 1;
}


static bool DoesCameraShakeBlockConeOfSight(float ShakeX, float ShakeY, float ShakeZ)
{
    if (!_LookAtObject && !_LookAtPositionSet)
        return false;

    Vector_t UpdatedCameraPosition;
    UpdatedCameraPosition.X = _Camera.Position.X + ShakeX;
    UpdatedCameraPosition.Y = _Camera.Position.Y + ShakeY;
    UpdatedCameraPosition.Z = _Camera.Position.Z + ShakeZ;
    ClipCamera(&UpdatedCameraPosition);

    size_t NumCollidingPoints;
    if (_LookAtPositionSet)
        return !IsConeOfSight(UpdatedCameraPosition, _LookAtPosition, 50.0f, 4, NumCollidingPoints);

    Vector_t SlightlyAbovePosition{ _LookAtObject->Positions[0] };
    SlightlyAbovePosition.Y += 5.0f;
    return !IsConeOfSight(UpdatedCameraPosition, SlightlyAbovePosition, 50.0f, 4, NumCollidingPoints);
}

//----------------------------------------------------------------------------------------
// Name: CameraShake()
// Desc:
//----------------------------------------------------------------------------------------

static void CameraShake(void)
{
    if (_CameraShakeThreshold <= 0)
        return;

    if (++_CameraShakeCounter > _CameraShakeThreshold)
    {
        _CameraShakeCounter   = 0;
        _CameraShakeThreshold = 0;
        return;
    }

    float ShakeX = (float)RandRange(_CameraShakeMin, _CameraShakeMax);
    float ShakeY = (float)RandRange(_CameraShakeMin, _CameraShakeMax);
    float ShakeZ = (float)RandRange(_CameraShakeMin, _CameraShakeMax);

    if (DoesCameraShakeBlockConeOfSight(ShakeX, ShakeY, ShakeZ))
        return;

    _Camera.Position.X += ShakeX;
    _Camera.Position.Y += ShakeY;
    _Camera.Position.Z += ShakeZ;
    ClipCamera(&_Camera.Position);
}


//----------------------------------------------------------------------------------------
// Name: AddCameraShake()
// Desc:
//----------------------------------------------------------------------------------------

void AddCameraShake(size_t NumFrames, long MinAmount, long MaxAmount)
{
    _CameraShakeThreshold = NumFrames;
    _CameraShakeMin = MinAmount;
    _CameraShakeMax = MaxAmount;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraShakeLookAtPosition()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraShakeLookAtPosition(const Vector_t& LookAtPosition)
{
    _LookAtPositionSet = true;
    _LookAtPosition = LookAtPosition;
}


//----------------------------------------------------------------------------------------
// Name: SetCameraShakeLookAtObject()
// Desc:
//----------------------------------------------------------------------------------------

void SetCameraShakeLookAtObject(Object_t* LookAtObject)
{
    _LookAtObject = LookAtObject;
}


//----------------------------------------------------------------------------------------
// Name: ClearAllCameraShakeLookAtInfo()
// Desc:
//----------------------------------------------------------------------------------------

void ClearAllCameraShakeLookAtInfo()
{
    _LookAtPositionSet = false;
    _LookAtObject = nullptr;
}


//----------------------------------------------------------------------------------------
// Name: GetCameraShakeLookAtPosition()
// Desc:
//----------------------------------------------------------------------------------------

bool GetCameraShakeLookAtPosition(Vector_t& Position)
{
    if (_LookAtPositionSet)
    {
        Position = _LookAtPosition;
        return true;
    }
    if (_LookAtObject)
    {
        Position = _LookAtObject->Positions[0];
        return true;
    }
    return false;
}


//----------------------------------------------------------------------------------------
// Name: StopCameraTrackingObject()
// Desc:
//----------------------------------------------------------------------------------------

void StopCameraTrackingObject(Vector_t* DeletedObjectPosition, const Vector_t& NewPositionToUse)
{
    if (_Camera.PointToFollow == DeletedObjectPosition)
    {
        _Camera.LookAtDest = NewPositionToUse;
        _Camera.PointToFollow = &_Camera.LookAtDest;
    }
}


static void CommonCameraTrackingSetup()
{
    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &_CameraTrackingShotStartPoint, &_CameraTrackingShotFinishPoint);

    VectorSubtract(_CameraTrackingShotFinishPoint, _CameraTrackingShotStartPoint, _CameraTrackingShotDirection);
    _CameraTrackingShotMagnitude = VectorMagnitude(&_CameraTrackingShotDirection);

    _CameraTrackingShotDirectionUnit = _CameraTrackingShotDirection;
    VectorNormalise(&_CameraTrackingShotDirectionUnit);

    _CameraTrackingShotChooseLeft = (rand() % 2 == 0);
}

static void CreatePointsNextToThisPointInDirectionOfShot(const Vector_t& MidPoint, bool DuplicatePointsAtLargerYCoord, std::vector<Vector_t>& Points)
{
    Vector_t SourcePoint{ MidPoint };

    // 1. Change the Y coord to be higher than the SourcePoint
    float TileWidth, TileDepth;
    GetMeshTileSize(g_LandscapeMesh, &TileWidth, &TileDepth);
    SourcePoint.Y = GetMeshHeight(g_LandscapeMesh, SourcePoint.X, SourcePoint.Z, nullptr) + (4 * TileWidth);

    // 2. Move perpendicularly to either side the this new point
    Vector_t Right, Up{ 0.0f, 1.0f, 0.0f };
    VectorCrossProd(_CameraTrackingShotDirectionUnit, Up, Right);
    if (rand() % 2 == 1)
        VectorInverse(Right);

    Points.clear();

    // 3. Create three Points on one side of the point and 3 on the other side. The 'side' is
    //    determined by the direction vector of the shot.
    auto Distance = 10.0f * TileWidth;
    Points.push_back(Vector_t
        {
            SourcePoint.X + (Right.X * Distance),
            SourcePoint.Y + (Right.Y * Distance),
            SourcePoint.Z + (Right.Z * Distance)
        });

    Points.push_back(Vector_t
        {
            Points[0].X + (_CameraTrackingShotDirectionUnit.X * Distance),
            Points[0].Y,
            Points[0].Z + (_CameraTrackingShotDirectionUnit.Z * Distance)
        });

    Points.push_back(Vector_t
        {
            Points[0].X + (_CameraTrackingShotDirectionUnit.X * -Distance),
            Points[0].Y,
            Points[0].Z + (_CameraTrackingShotDirectionUnit.Z * -Distance)
        });

    VectorInverse(Right);

    Points.push_back(Vector_t
        {
            SourcePoint.X + (Right.X * Distance),
            SourcePoint.Y + (Right.Y * Distance),
            SourcePoint.Z + (Right.Z * Distance) });

    Points.push_back(Vector_t
        {
            Points[3].X + (_CameraTrackingShotDirectionUnit.X * Distance),
            Points[3].Y,
            Points[3].Z + (_CameraTrackingShotDirectionUnit.Z * Distance)
        });

    Points.push_back(Vector_t
        {
            Points[3].X + (_CameraTrackingShotDirectionUnit.X * -Distance),
            Points[3].Y,
            Points[3].Z + (_CameraTrackingShotDirectionUnit.Z * -Distance)
        });

    if (DuplicatePointsAtLargerYCoord)
    {
        // 4. Copy those six Points and change the y coord to the be the maximum - 1 tile's width
        float MaxX, MaxY, MaxZ;
        GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);
        MaxY -= TileWidth;
        Points.push_back(Vector_t{ Points[0].X, MaxY, Points[0].Z, });
        Points.push_back(Vector_t{ Points[1].X, MaxY, Points[1].Z, });
        Points.push_back(Vector_t{ Points[2].X, MaxY, Points[2].Z, });
        Points.push_back(Vector_t{ Points[3].X, MaxY, Points[3].Z, });
        Points.push_back(Vector_t{ Points[4].X, MaxY, Points[4].Z, });
        Points.push_back(Vector_t{ Points[5].X, MaxY, Points[5].Z, });
    }

    // 5. Clip to the bounds of the world
    for (auto& Position : Points)
        ClipCamera(&Position);
}

static void PlaceCameraAtOneOfThesePoints(const std::vector<Vector_t>& Points, bool FallbackToSavedShotPositions)
{
    // Find a viewing location where there's a clear line of sight. If one can't be found then
    // just use the first one in the list.

    size_t NumCollidingPoints;
    bool Found = false;
    for (auto& Position : Points)
    {
        if (IsConeOfSight(Position, *_Camera.PointToFollow, 50.0f, 4, NumCollidingPoints))
        {
            _Camera.Position = Position;
            Found = true;
            break;
        }
    }

    if (!Found)
    {
        if (FallbackToSavedShotPositions)
        {
            // Check the saved shot positions
            auto savedShotPositions = GetSavedShotPositions(GetShotID());
            for (auto& Position : savedShotPositions)
            {
                if (IsConeOfSight(Position, *_Camera.PointToFollow, 50.0f, 4, NumCollidingPoints))
                {
                    _Camera.Position = Position;
                    Found = true;
                    break;
                }
            }
        }

        if (!Found)
            _Camera.Position = Points.front();  // We're left with blindly choosing a position
    }
}

#undef max

static void GenerateBezierCurveBetweenPoints(const Vector_t& Start, const Vector_t& Finish, std::vector<Vector_t>& Points)
{
    Vector_t Direction;
    VectorSubtract(Finish, Start, Direction);

    float Magnitude = VectorMagnitude(&Direction);

    float TileWidth, TileDepth;
    GetMeshTileSize(g_LandscapeMesh, &TileWidth, &TileDepth);

    float MaxX, MaxY, MaxZ;
    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    Vector_t MidPoint;
    MidPoint.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirection.X * 0.5f);
    MidPoint.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirection.Z * 0.5f);
    MidPoint.Y = MaxY;

    Vector_t OneQuarter;
    OneQuarter.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirection.X * 0.25f);
    OneQuarter.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirection.Z * 0.25f);
    float Y1 = GetMeshHeight(g_LandscapeMesh, OneQuarter.X, OneQuarter.Z, nullptr) + 3.0f * TileWidth;
    float Y2 = MaxY - (3.0f * TileWidth);
    float Y3 = std::max(Y1, Y2);
    OneQuarter.Y = std::max(Y3, MaxY);

    Vector_t ThreeQuarters;
    ThreeQuarters.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirection.X * 0.75f);
    ThreeQuarters.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirection.Z * 0.75f);
    Y1 = GetMeshHeight(g_LandscapeMesh, ThreeQuarters.X, ThreeQuarters.Z, nullptr) + 3.0f * TileWidth;
    Y2 = MaxY - (3.0f * TileWidth);
    Y3 = std::max(Y1, Y2);
    ThreeQuarters.Y = std::max(Y3, MaxY);

    Vector_t OneThird;
    OneThird.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirection.X * 0.33333f);
    OneThird.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirection.Z * 0.33333f);
    Y1 = GetMeshHeight(g_LandscapeMesh, OneThird.X, OneThird.Z, nullptr) + 5.0f * TileWidth;
    Y2 = MaxY - (5.0f * TileWidth);
    Y3 = std::max(Y1, Y2);
    OneThird.Y = std::max(Y3, MaxY);

    Vector_t TwoThirds;
    TwoThirds.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirection.X * 0.66666f);
    TwoThirds.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirection.Z * 0.66666f);
    Y1 = GetMeshHeight(g_LandscapeMesh, TwoThirds.X, TwoThirds.Z, nullptr) + 5.0f * TileWidth;
    Y2 = MaxY - (5.0f * TileWidth);
    Y3 = std::max(Y1, Y2);
    TwoThirds.Y = std::max(Y3, MaxY);

    Points.clear();
    Points.push_back(Start);

    if (Magnitude < 5.0f * TileWidth)
    {
        Points.push_back(MidPoint);
    }
    else if (Magnitude > 15.0f * TileWidth)
    {
        Points.push_back(OneQuarter);
        Points.push_back(MidPoint);
        Points.push_back(ThreeQuarters);
    }
    else
    {
        Points.push_back(OneThird);
        Points.push_back(TwoThirds);
    }

    Points.push_back(Finish);
}





static void PerpendicularStationaryInitialise(size_t ShotID)
{
    CommonCameraTrackingSetup();

    // Find the middle of the line segment
    Vector_t MidPoint;
    MidPoint.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirection.X * 0.5f);
    MidPoint.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirection.Z * 0.5f);

    // Generate some Points into private storage to be used by the camera.
    CreatePointsNextToThisPointInDirectionOfShot(MidPoint, true, _CameraTrackingPointsA);

    // Inform the camera to look at the player's shot
    auto ProjectileObjID = GetShotObject(ShotID);
    _Camera.PointToFollow = GetObjectPositionPtr(ProjectileObjID, 0);
}

static void PerpendicularStationaryThink()
{
    PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsA, true);
    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}

static void PerpendicularStationaryMove()
{
}





static void PerpendicularPartialFollowInitialise(size_t ShotID)
{
    CommonCameraTrackingSetup();

    Vector_t Quarter;
    Quarter.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirectionUnit.X * (_CameraTrackingShotMagnitude * 0.25f));
    Quarter.Y = _CameraTrackingShotStartPoint.Y + (_CameraTrackingShotDirectionUnit.Y * (_CameraTrackingShotMagnitude * 0.25f));
    Quarter.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirectionUnit.Z * (_CameraTrackingShotMagnitude * 0.25f));

    // Generate some Points into private storage to be used by the camera.
    CreatePointsNextToThisPointInDirectionOfShot(Quarter, true, _CameraTrackingPointsA);

    Vector_t ThreeQuarters;
    ThreeQuarters.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirectionUnit.X * (_CameraTrackingShotMagnitude * 0.75f));
    ThreeQuarters.Y = _CameraTrackingShotStartPoint.Y + (_CameraTrackingShotDirectionUnit.Y * (_CameraTrackingShotMagnitude * 0.75f));
    ThreeQuarters.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirectionUnit.Z * (_CameraTrackingShotMagnitude * 0.75f));

    // Generate some Points into private storage to be used by the camera.
    CreatePointsNextToThisPointInDirectionOfShot(ThreeQuarters, true, _CameraTrackingPointsB);

    GenerateBezierCurveBetweenPoints(_CameraTrackingShotStartPoint, _CameraTrackingShotFinishPoint, _CameraTrackingPointsC);

    // Inform the camera to look at the player's shot
    auto ProjectileObjID = GetShotObject(ShotID);
    _Camera.PointToFollow = GetObjectPositionPtr(ProjectileObjID, 0);
}

static void PerpendicularPartialFollowThink()
{
    // Determine what percentage of the way to the destination the shot is

    Vector_t Intersection; float Percentage;
    if (!MathPointAndLineIntersection(*_Camera.PointToFollow, _CameraTrackingShotStartPoint, _CameraTrackingShotFinishPoint, Intersection, &Percentage))
    {
        CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
        SetListenerOnCamera();
        return;
    }

    if (Percentage < 0.25f)
    {
        // Stay at the start Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsA, false);
    }
    else if (Percentage >= 0.75f)
    {
        // Stay at the finish Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsB, true);
    }
    else
    {
        switch (_CameraTrackingPointsC.size())
        {
        case 3:
            _Camera.Position.X = BezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, Percentage);
            _Camera.Position.Y = BezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, Percentage);
            _Camera.Position.Z = BezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, Percentage);
            break;
        case 4:
            _Camera.Position.X = QuadraticBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, Percentage);
            _Camera.Position.Y = QuadraticBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, Percentage);
            _Camera.Position.Z = QuadraticBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, Percentage);
            break;
        case 5:
            _Camera.Position.X = CubicBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, _CameraTrackingPointsC[4].X, Percentage);
            _Camera.Position.Y = CubicBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, _CameraTrackingPointsC[4].Y, Percentage);
            _Camera.Position.Z = CubicBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, _CameraTrackingPointsC[4].Z, Percentage);
            break;
        }

        // Move the camera out perpendicularly to the direction of the shot.
        Vector_t Up{ 0.0f, 1.0f, 0.0f }, Right;
        VectorCrossProd(_CameraTrackingShotDirectionUnit, Up, Right);

        float TileWidth, TileDepth;
        GetMeshTileSize(g_LandscapeMesh, &TileWidth, &TileDepth);

        float Multiplier = (_CameraTrackingShotChooseLeft ? -5.0f : 5.0f);

        _Camera.Position.X += (Right.X * TileWidth * Multiplier);
        _Camera.Position.Y += (Right.Y * TileWidth * Multiplier);
        _Camera.Position.Z += (Right.Z * TileWidth * Multiplier);
        ClipCamera(&_Camera.Position);
    }

    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}

static void PerpendicularPartialFollowMove()
{
}





static void PerpendicularCompleteFollowInitialise(size_t ShotID)
{
    PerpendicularPartialFollowInitialise(ShotID);
}

static void PerpendicularCompleteFollowThink()
{
    // Determine what percentage of the way to the destination the shot is

    Vector_t Intersection; float Percentage;
    if (!MathPointAndLineIntersection(*_Camera.PointToFollow, _CameraTrackingShotStartPoint, _CameraTrackingShotFinishPoint, Intersection, &Percentage))
    {
        CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
        SetListenerOnCamera();
        return;
    }

    if (Percentage >= 0.9f)
    {
        // Stay at the finish Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsB, true);
    }
    else
    {
        switch (_CameraTrackingPointsC.size())
        {
        case 3:
            _Camera.Position.X = BezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, Percentage);
            _Camera.Position.Y = BezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, Percentage);
            _Camera.Position.Z = BezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, Percentage);
            break;
        case 4:
            _Camera.Position.X = QuadraticBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, Percentage);
            _Camera.Position.Y = QuadraticBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, Percentage);
            _Camera.Position.Z = QuadraticBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, Percentage);
            break;
        case 5:
            _Camera.Position.X = CubicBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, _CameraTrackingPointsC[4].X, Percentage);
            _Camera.Position.Y = CubicBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, _CameraTrackingPointsC[4].Y, Percentage);
            _Camera.Position.Z = CubicBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, _CameraTrackingPointsC[4].Z, Percentage);
            break;
        }

        // Move the camera out perpendicularly to the direction of the shot.
        Vector_t Up{ 0.0f, 1.0f, 0.0f }, Right;
        VectorCrossProd(_CameraTrackingShotDirectionUnit, Up, Right);

        float TileWidth, TileDepth;
        GetMeshTileSize(g_LandscapeMesh, &TileWidth, &TileDepth);

        float Multiplier = (_CameraTrackingShotChooseLeft ? -5.0f : 5.0f);

        _Camera.Position.X += (Right.X * TileWidth * Multiplier);
        _Camera.Position.Y += (Right.Y * TileWidth * Multiplier);
        _Camera.Position.Z += (Right.Z * TileWidth * Multiplier);
        ClipCamera(&_Camera.Position);
    }

    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}

static void PerpendicularCompleteFollowMove()
{
}





static void CornerStationaryInitialise(size_t ShotID)
{
    CommonCameraTrackingSetup();

    float MaxX, MaxY, MaxZ;
    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    // Which corner is closest to start point?
    Vector_t Corner, Temp;
    std::vector<float> Magnitudes;
    std::vector<Vector_t> Points;

    Corner = { 0.0f, MaxY, 0.0f };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    Corner = { 0.0f, MaxY, MaxZ };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    Corner = { MaxX, MaxY, 0.0f };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    Corner = { MaxX, MaxY, MaxZ };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    _CameraTrackingPointsC.assign(1, Vector_t{ 0.0f, 0.0f, 0.0f });
    std::size_t i;
    float SmallestValue = 1000000.0f;
    for (i = 0; i < Magnitudes.size(); ++i)
    {
        if (fabsf(Magnitudes[i]) < SmallestValue)
        {
            SmallestValue = fabsf(Magnitudes[i]);
            _CameraTrackingPointsC[0] = Points[i];
        }
    }

    Vector_t ThreeQuarters;
    ThreeQuarters.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirectionUnit.X * (_CameraTrackingShotMagnitude * 0.75f));
    ThreeQuarters.Y = _CameraTrackingShotStartPoint.Y + (_CameraTrackingShotDirectionUnit.Y * (_CameraTrackingShotMagnitude * 0.75f));
    ThreeQuarters.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirectionUnit.Z * (_CameraTrackingShotMagnitude * 0.75f));

    // Generate some Points into private storage to be used by the camera.
    CreatePointsNextToThisPointInDirectionOfShot(ThreeQuarters, true, _CameraTrackingPointsB);

    // Inform the camera to look at the player's shot
    auto ProjectileObjID = GetShotObject(ShotID);
    _Camera.PointToFollow = GetObjectPositionPtr(ProjectileObjID, 0);
}

static void CornerStationaryThink()
{
    // Determine what percentage of the way to the destination the shot is

    Vector_t Intersection; float Percentage;
    if (!MathPointAndLineIntersection(*_Camera.PointToFollow, _CameraTrackingShotStartPoint, _CameraTrackingShotFinishPoint, Intersection, &Percentage))
    {
        CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
        SetListenerOnCamera();
        return;
    }

    if (Percentage >= 0.9f)
    {
        // Stay at the finish Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsB, true);
    }
    else
    {
        _Camera.Position = _CameraTrackingPointsC[0];
        ClipCamera(&_Camera.Position);
    }

    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}

static void CornerStationaryMove()
{
}





static void CornerPartialFollowInitialise(size_t ShotID)
{
    CommonCameraTrackingSetup();

    Vector_t Quarter;
    Quarter.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirectionUnit.X * (_CameraTrackingShotMagnitude * 0.25f));
    Quarter.Y = _CameraTrackingShotStartPoint.Y + (_CameraTrackingShotDirectionUnit.Y * (_CameraTrackingShotMagnitude * 0.25f));
    Quarter.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirectionUnit.Z * (_CameraTrackingShotMagnitude * 0.25f));

    // Generate some Points into private storage to be used by the camera.
    CreatePointsNextToThisPointInDirectionOfShot(Quarter, true, _CameraTrackingPointsA);

    Vector_t ThreeQuarters;
    ThreeQuarters.X = _CameraTrackingShotStartPoint.X + (_CameraTrackingShotDirectionUnit.X * (_CameraTrackingShotMagnitude * 0.75f));
    ThreeQuarters.Y = _CameraTrackingShotStartPoint.Y + (_CameraTrackingShotDirectionUnit.Y * (_CameraTrackingShotMagnitude * 0.75f));
    ThreeQuarters.Z = _CameraTrackingShotStartPoint.Z + (_CameraTrackingShotDirectionUnit.Z * (_CameraTrackingShotMagnitude * 0.75f));

    // Generate some Points into private storage to be used by the camera.
    CreatePointsNextToThisPointInDirectionOfShot(ThreeQuarters, true, _CameraTrackingPointsB);

    float MaxX, MaxY, MaxZ;
    GetMeshMaxDimensions(g_LandscapeMesh, &MaxX, &MaxY, &MaxZ);

    // Which corner is closest to start point?
    Vector_t Corner, Temp, Closest;
    std::vector<float> Magnitudes;
    std::vector<Vector_t> Points;

    Corner = { 0.0f, MaxY, 0.0f };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    Corner = { 0.0f, MaxY, MaxZ };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    Corner = { MaxX, MaxY, 0.0f };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    Corner = { MaxX, MaxY, MaxZ };
    VectorSubtract(Corner, _CameraTrackingShotStartPoint, Temp);
    Magnitudes.push_back(VectorMagnitude(&Temp));
    Points.push_back(Corner);

    std::size_t i;
    float SmallestValue = 1000000.0f;
    for (i = 0; i < Magnitudes.size(); ++i)
    {
        if (fabsf(Magnitudes[i]) < SmallestValue)
        {
            SmallestValue = fabsf(Magnitudes[i]);
            Closest = Points[i];
        }
    }

    GenerateBezierCurveBetweenPoints(Closest, _CameraTrackingShotFinishPoint, _CameraTrackingPointsC);

    // Inform the camera to look at the player's shot
    auto ProjectileObjID = GetShotObject(ShotID);
    _Camera.PointToFollow = GetObjectPositionPtr(ProjectileObjID, 0);
}

static void CornerPartialFollowThink()
{
    // Determine what percentage of the way to the destination the shot is

    Vector_t Intersection; float Percentage;
    if (!MathPointAndLineIntersection(*_Camera.PointToFollow, _CameraTrackingShotStartPoint, _CameraTrackingShotFinishPoint, Intersection, &Percentage))
    {
        CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
        SetListenerOnCamera();
        return;
    }

    if (Percentage < 0.25f)
    {
        // Stay at the start Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsA, false);
    }
    else if (Percentage >= 0.75f)
    {
        // Stay at the finish Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsB, true);
    }
    else
    {
        switch (_CameraTrackingPointsC.size())
        {
        case 3:
            _Camera.Position.X = BezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, Percentage);
            _Camera.Position.Y = BezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, Percentage);
            _Camera.Position.Z = BezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, Percentage);
            break;
        case 4:
            _Camera.Position.X = QuadraticBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, Percentage);
            _Camera.Position.Y = QuadraticBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, Percentage);
            _Camera.Position.Z = QuadraticBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, Percentage);
            break;
        case 5:
            _Camera.Position.X = CubicBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, _CameraTrackingPointsC[4].X, Percentage);
            _Camera.Position.Y = CubicBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, _CameraTrackingPointsC[4].Y, Percentage);
            _Camera.Position.Z = CubicBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, _CameraTrackingPointsC[4].Z, Percentage);
            break;
        }

        ClipCamera(&_Camera.Position);
    }

    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}

static void CornerPartialFollowMove()
{
}





static void CornerCompleteFollowInitialise(size_t ShotID)
{
    CornerPartialFollowInitialise(ShotID);
}

static void CornerCompleteFollowThink()
{
    // Determine what percentage of the way to the destination the shot is

    Vector_t Intersection; float Percentage;
    if (!MathPointAndLineIntersection(*_Camera.PointToFollow, _CameraTrackingShotStartPoint, _CameraTrackingShotFinishPoint, Intersection, &Percentage))
    {
        CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
        SetListenerOnCamera();
        return;
    }

    if (Percentage >= 0.9f)
    {
        // Stay at the finish Points and look at the shot
        PlaceCameraAtOneOfThesePoints(_CameraTrackingPointsB, true);
    }
    else
    {
        switch (_CameraTrackingPointsC.size())
        {
        case 3:
            _Camera.Position.X = BezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, Percentage);
            _Camera.Position.Y = BezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, Percentage);
            _Camera.Position.Z = BezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, Percentage);
            break;
        case 4:
            _Camera.Position.X = QuadraticBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, Percentage);
            _Camera.Position.Y = QuadraticBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, Percentage);
            _Camera.Position.Z = QuadraticBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, Percentage);
            break;
        case 5:
            _Camera.Position.X = CubicBezierInterp(_CameraTrackingPointsC[0].X, _CameraTrackingPointsC[1].X, _CameraTrackingPointsC[2].X, _CameraTrackingPointsC[3].X, _CameraTrackingPointsC[4].X, Percentage);
            _Camera.Position.Y = CubicBezierInterp(_CameraTrackingPointsC[0].Y, _CameraTrackingPointsC[1].Y, _CameraTrackingPointsC[2].Y, _CameraTrackingPointsC[3].Y, _CameraTrackingPointsC[4].Y, Percentage);
            _Camera.Position.Z = CubicBezierInterp(_CameraTrackingPointsC[0].Z, _CameraTrackingPointsC[1].Z, _CameraTrackingPointsC[2].Z, _CameraTrackingPointsC[3].Z, _CameraTrackingPointsC[4].Z, Percentage);
            break;
        }

        ClipCamera(&_Camera.Position);
    }

    CameraLookAt(&_Camera.Position, _Camera.PointToFollow);
    SetListenerOnCamera();
}

static void CornerCompleteFollowMove()
{
}





static void FollowShotDirectlyInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void FollowShotDirectlyThink()
{
}

static void FollowShotDirectlyMove()
{
}





static void FollowShotWavilyInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void FollowShotWavilyThink()
{
}

static void FollowShotWavilyMove()
{
}





static void DollyTrackPerpendicularAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void DollyTrackPerpendicularAcrossPathThink()
{
}

static void DollyTrackPerpendicularAcrossPathMove()
{
}





static void DollyTrackConvexArcAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void DollyTrackConvexArcAcrossPathThink()
{
}

static void DollyTrackConvexArcAcrossPathMove()
{
}





static void DollyTrackConcaveArcAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void DollyTrackConcaveArcAcrossPathThink()
{
}

static void DollyTrackConcaveArcAcrossPathMove()
{
}





static void BeginStationaryEndStationaryInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void BeginStationaryEndStationaryThink()
{
}

static void BeginStationaryEndStationaryMove()
{
}





static void BeginStationaryEndPerpendicularAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void BeginStationaryEndPerpendicularAcrossPathThink()
{
}

static void BeginStationaryEndPerpendicularAcrossPathMove()
{
}





static void BeginStationaryEndConcexArcAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void BeginStationaryEndConcexArcAcrossPathThink()
{
}

static void BeginStationaryEndConcexArcAcrossPathMove()
{
}





static void TrackAirStrikeInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackAirStrikeThink()
{
}

static void TrackAirStrikeMove()
{
}





static void TrackMultiProjectileInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackMultiProjectileThink()
{
}

static void TrackMultiProjectileMove()
{
}





static void TrackFunkyBombInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackFunkyBombThink()
{
}

static void TrackFunkyBombMove()
{
}





static void TrackNapalmInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackNapalmThink()
{
}

static void TrackNapalmMove()
{
}





static void RailgunPerpendicularAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void RailgunPerpendicularAcrossPathThink()
{
}

static void RailgunPerpendicularAcrossPathMove()
{
}





static void RailgunConcaveArcAcrossPathInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void RailgunConcaveArcAcrossPathThink()
{
}

static void RailgunConcaveArcAcrossPathMove()
{
}





static void TrackTankStationaryInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackTankStationaryThink()
{
}

static void TrackTankStationaryMove()
{
}





static void TrackTankConcexArcInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackTankConcexArcThink()
{
}

static void TrackTankConcexArcMove()
{
}





static void TrackTankFollowInitialise(size_t ShotID)
{
    auto Weapon = GetShotWeapon(ShotID);

    Vector_t ObjPos;
    GetObjectPosition(GetPlayerTank(GetCurrentPlayer()), 0, &ObjPos);

    float Heading, Pitch, Power;
    GetPlayerShotDetails(GetCurrentPlayer(), &Heading, &Pitch, &Power);

    Vector_t ShootVector, StartPoint, FinishPoint;
    CalculateProjectShotDetails(Heading, Pitch, Power, &ShootVector, &StartPoint, &FinishPoint);
}

static void TrackTankFollowThink()
{
}

static void TrackTankFollowMove()
{
}
