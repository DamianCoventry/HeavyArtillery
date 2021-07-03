//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vector.h"


//----------------------------------------------------------------------------------------
// public structures
//----------------------------------------------------------------------------------------

typedef enum CameraState_e
{
    CAMERASTATE_IDLE = 0,
    CAMERASTATE_FLYBY,
    CAMERASTATE_MOUSELOOK,
    CAMERASTATE_FOLLOWINGPOINT,
    CAMERASTATE_LOOKINGATPOINT,
    CAMERASTATE_FINDTANK,
    CAMERASTATE_DISTANCEFOLLOW,

    CAMERASTATE_PERPENDICULAR_STATIONARY,
    CAMERASTATE_PERPENDICULAR_PARTIAL_FOLLOW,
    CAMERASTATE_PERPENDICULAR_COMPLETE_FOLLOW,

    CAMERASTATE_CORNER_STATIONARY,
    CAMERASTATE_CORNER_PARTIAL_FOLLOW,
    CAMERASTATE_CORNER_COMPLETE_FOLLOW,

    CAMERASTATE_FOLLOW_SHOT_DIRECTLY,
    CAMERASTATE_FOLLOW_SHOT_WAVILY,

    CAMERASTATE_DOLLY_TRACK_PERPENDICULAR_ACROSS_PATH,
    CAMERASTATE_DOLLY_TRACK_CONVEX_ARC_ACROSS_PATH,
    CAMERASTATE_DOLLY_TRACK_CONCAVE_ARC_ACROSS_PATH,

    CAMERASTATE_BEGIN_STATIONARY_END_STATIONARY,
    CAMERASTATE_BEGIN_STATIONARY_END_PERPENDICULAR_ACROSS_PATH,
    CAMERASTATE_BEGIN_STATIONARY_END_CONCEX_ARC_ACROSS_PATH,

    CAMERASTATE_TRACK_AIR_STRIKE,
    CAMERASTATE_TRACK_MULTI_PROJECTILE,
    CAMERASTATE_TRACK_FUNKY_BOMB,
    CAMERASTATE_TRACK_NAPALM,

    CAMERASTATE_RAILGUN_PERPENDICULAR_ACROSS_PATH,
    CAMERASTATE_RAILGUN_CONCAVE_ARC_ACROSS_PATH,

    CAMERASTATE_TRACK_TANK_STATIONARY,
    CAMERASTATE_TRACK_TANK_CONCEX_ARC,
    CAMERASTATE_TRACK_TANK_FOLLOW,
}
CameraState_t;

typedef enum CameraType_e
{
    CAMERATYPE_FIRSTPERSON,
    CAMERATYPE_THIRDPERSON,
    CAMERATYPE_CHASE,
    CAMERATYPE_BIRDSEYE,
    CAMERATYPE_MOUSELOOK
}
CameraType_t;

typedef struct Camera_s
{
    CameraState_t State;
    CameraType_t Type;

    Vector_t Position;
    Vector_t Rotation;

    size_t AttachedTank;
    size_t AttachedBarrell;

    float ViewAngle;
    float ViewDistance;
    float ViewHeight;

    size_t HaveAngle;
    float Angle;
    Vector_t *PointToFollow;

    size_t LookAtDone, LookFromDone;
    Vector_t LookAtDest;
    Vector_t LookAtCrnt;
    Vector_t LookFromDest;
}
Camera_t;


#endif
