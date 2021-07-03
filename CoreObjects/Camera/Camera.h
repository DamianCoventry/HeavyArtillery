//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "CameraTypes.h"
#include "ObjectTypes.h"
#include "Vector.h"


//----------------------------------------------------------------------------------------
// public functions
//----------------------------------------------------------------------------------------

extern size_t CameraThink(void);
extern void CameraMove(void);
extern void CameraCollide(void);

extern void CameraLookAt(Vector_t* Eye, Vector_t* Center);

extern void SetCameraState(CameraState_t State, CameraType_t Type);
extern void SetCameraStateAndShotID(CameraState_t State, size_t ShotID);
extern void GetCameraState(CameraState_t* State, CameraType_t* Type);

extern Vector_t* GetCameraPosition(void);
extern Vector_t* GetCameraRotation(void);

extern void SetCameraPosition(Vector_t* Position);
extern void SetCameraRotation(Vector_t* Rotation);

extern void ClipCamera(Vector_t* Point);
extern void ClipCameraFollowingPoint(Vector_t* Point);

extern void InitialiseFlyByPoints(void);
extern void InitialiseLookAtPoints(void);

extern void InitialiseFindTankPoints(Vector_t* TankPosition, Vector_t* GroundNormal);

extern void GetCameraDoneFlags(size_t* LookFrom, size_t* LookAt);

extern CameraType_t GetCameraType(void);

extern void SetCameraViewDistance(float ViewDistance);
extern float GetCameraViewDistance(void);

extern void SetCameraViewAngle(float ViewAngle);
extern float GetCameraViewAngle(void);

extern void SetCameraViewHeight(float ViewHeight);
extern float GetCameraViewHeight(void);

extern void AttachCameraToPlayer(size_t PlayerID);
extern void AttachCameraToPlayerBarrell(size_t PlayerID);

extern size_t GetCameraAttachedPlayer(void);
extern size_t GetCameraAttachedPlayerBarrell(void);

extern void SetPointToFollow(Vector_t* Point);
extern void SetCameraOverCurrentPlayer(void);

extern void SetListenerOnCamera(void);

extern void AddCameraShake(size_t NumFrames, long MinAmount, long MaxAmount);

extern void SetCameraShakeLookAtPosition(const Vector_t& LookAtPosition);
extern void SetCameraShakeLookAtObject(Object_t* LookAtObject);
extern void ClearAllCameraShakeLookAtInfo();
extern bool GetCameraShakeLookAtPosition(Vector_t& Position);

extern void StopCameraTrackingObject(Vector_t* DeletedObjectPosition, const Vector_t& NewPositionToUse);
