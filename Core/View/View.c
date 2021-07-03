//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <stdlib.h>
#include <string.h>

#include "Vector.h"
#include "Camera.h"
#include "View.h"
#include "GameTypes.h"
#include "Hardware.h"
#include "Shoot.h"

#include "State.h"
#include "StateMoveTank.h"

#include "LevelManager.h"
#include "PlayerManager.h"
#include "ObjectManager.h"
#include "MeshManager.h"

#include "DialogTankMove.h"
#include "DialogShotInfo.h"

#include <vector>

#define VIEW_THIRDPERSONDISTANCE    100.0f
#define VIEW_THIRDPERSONANGLE       30.0f
#define VIEW_THIRDPERSONHEIGHT      25.0f

#define VIEW_CHASEDISTANCE          400.0f
#define VIEW_CHASEANGLE             45.0f
#define VIEW_CHASEHEIGHT            25.0f

#define VIEW_BIRDSEYEDISTANCE       1024.0f


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

void (*UpdateView)(void);

extern size_t g_LandscapeMesh;
extern HardwareConfig_t g_HWConfig;

//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static Plane2_t _ViewFrustum[ 6 ];


//----------------------------------------------------------------------------------------
// Name: SetupViewTransform()
// Desc:
//----------------------------------------------------------------------------------------

void SetupViewTransform(void)
{
    Vector_t* CamRot;
    Vector_t ObjRot0, ObjRot1, ObjRot2;
    Vector_t* CamPos;
    CameraType_t Type;

    Type = GetCameraType();
    CamRot = GetCameraRotation();
    CamPos = GetCameraPosition();

    switch (Type)
    {
    case CAMERATYPE_MOUSELOOK :
    case CAMERATYPE_BIRDSEYE :
        glRotatef(-CamRot->X, 1.0f, 0.0f, 0.0f);
        glRotatef(-CamRot->Y, 0.0f, 1.0f, 0.0f);
        glTranslatef(-CamPos->X, -CamPos->Y, -CamPos->Z);
        break;

    default :
        glTranslatef(0.0f, -GetCameraViewHeight(), -GetCameraViewDistance());
        glRotatef(GetCameraViewAngle(), 1.0f, 0.0f, 0.0f);

        if (-1 != GetCameraAttachedPlayer())
        {
            GetObjectRotation(GetPlayerTank(GetCameraAttachedPlayer()), 0, &ObjRot0);
            glRotatef(ObjRot0.Y + 180.0f, 0.0f, 1.0f, 0.0f);
            glRotatef(ObjRot0.Z, 0.0f, 0.0f, 1.0f);
            glRotatef(ObjRot0.X, 1.0f, 0.0f, 0.0f);
        }
        else
        {
            GetObjectRotation(GetPlayerTank(GetCameraAttachedPlayerBarrell()), 0, &ObjRot0);
            GetObjectRotation(GetPlayerTank(GetCameraAttachedPlayerBarrell()), 1, &ObjRot1);
            GetObjectRotation(GetPlayerTank(GetCameraAttachedPlayerBarrell()), 2, &ObjRot2);

            glRotatef(-ObjRot2.X, 1.0f, 0.0f, 0.0f);
            glRotatef(ObjRot0.Y + ObjRot1.Y + 180.0f, 0.0f, 1.0f, 0.0f);
            glRotatef(ObjRot0.Z, 0.0f, 0.0f, 1.0f);
            glRotatef(ObjRot0.X, 1.0f, 0.0f, 0.0f);
        }

        glTranslatef(-CamPos->X, -CamPos->Y, -CamPos->Z);
        break;
    }
}


//----------------------------------------------------------------------------------------
// Name: ExtractViewFrustumPlanes()
// Desc:
//----------------------------------------------------------------------------------------

void ExtractViewFrustumPlanes(void)
{
    float CombinedMatrix[ 16 ];
    float ModelViewMatrix[ 16 ];
    size_t i;
    float Denom;

    glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix);

    // multiply the projection matrix by the model view matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMultMatrixf(ModelViewMatrix);

    // get the combined projection and model view matrix
    glGetFloatv(GL_PROJECTION_MATRIX, CombinedMatrix);
    glPopMatrix();

    // extract the 6 view frustum planes in world space

    // left
    _ViewFrustum[ 0 ].Normal.X = -(CombinedMatrix[  3 ] + CombinedMatrix[  0 ]);
    _ViewFrustum[ 0 ].Normal.Y = -(CombinedMatrix[  7 ] + CombinedMatrix[  4 ]);
    _ViewFrustum[ 0 ].Normal.Z = -(CombinedMatrix[ 11 ] + CombinedMatrix[  8 ]);
    _ViewFrustum[ 0 ].Distance = -(CombinedMatrix[ 15 ] + CombinedMatrix[ 12 ]);

    // right
    _ViewFrustum[ 1 ].Normal.X = -(CombinedMatrix[  3 ] - CombinedMatrix[  0 ]);
    _ViewFrustum[ 1 ].Normal.Y = -(CombinedMatrix[  7 ] - CombinedMatrix[  4 ]);
    _ViewFrustum[ 1 ].Normal.Z = -(CombinedMatrix[ 11 ] - CombinedMatrix[  8 ]);
    _ViewFrustum[ 1 ].Distance = -(CombinedMatrix[ 15 ] - CombinedMatrix[ 12 ]);

    // bottom
    _ViewFrustum[ 3 ].Normal.X = -(CombinedMatrix[  3 ] + CombinedMatrix[  1 ]);
    _ViewFrustum[ 3 ].Normal.Y = -(CombinedMatrix[  7 ] + CombinedMatrix[  5 ]);
    _ViewFrustum[ 3 ].Normal.Z = -(CombinedMatrix[ 11 ] + CombinedMatrix[  9 ]);
    _ViewFrustum[ 3 ].Distance = -(CombinedMatrix[ 15 ] + CombinedMatrix[ 13 ]);

    // top
    _ViewFrustum[ 2 ].Normal.X = -(CombinedMatrix[  3 ] - CombinedMatrix[  1 ]);
    _ViewFrustum[ 2 ].Normal.Y = -(CombinedMatrix[  7 ] - CombinedMatrix[  5 ]);
    _ViewFrustum[ 2 ].Normal.Z = -(CombinedMatrix[ 11 ] - CombinedMatrix[  9 ]);
    _ViewFrustum[ 2 ].Distance = -(CombinedMatrix[ 15 ] - CombinedMatrix[ 13 ]);

    // near
    _ViewFrustum[ 4 ].Normal.X = -(CombinedMatrix[  3 ] + CombinedMatrix[  2 ]);
    _ViewFrustum[ 4 ].Normal.Y = -(CombinedMatrix[  7 ] + CombinedMatrix[  6 ]);
    _ViewFrustum[ 4 ].Normal.Z = -(CombinedMatrix[ 11 ] + CombinedMatrix[ 10 ]);
    _ViewFrustum[ 4 ].Distance = -(CombinedMatrix[ 15 ] + CombinedMatrix[ 14 ]);

    // far
    _ViewFrustum[ 5 ].Normal.X = -(CombinedMatrix[  3 ] - CombinedMatrix[  2 ]);
    _ViewFrustum[ 5 ].Normal.Y = -(CombinedMatrix[  7 ] - CombinedMatrix[  6 ]);
    _ViewFrustum[ 5 ].Normal.Z = -(CombinedMatrix[ 11 ] - CombinedMatrix[ 10 ]);
    _ViewFrustum[ 5 ].Distance = -(CombinedMatrix[ 15 ] - CombinedMatrix[ 14 ]);

    for (i = 0; i < 6; i++)
    {
        Denom = 1.0f / VectorMagnitude(&_ViewFrustum[ i ].Normal);
        _ViewFrustum[ i ].Normal.X *= Denom;
        _ViewFrustum[ i ].Normal.Y *= Denom;
        _ViewFrustum[ i ].Normal.Z *= Denom;
        _ViewFrustum[ i ].Distance *= Denom;
    }

    glMatrixMode(GL_MODELVIEW);
}


//----------------------------------------------------------------------------------------
// Name: GetViewFrustum()
// Desc:
//----------------------------------------------------------------------------------------

Plane2_t* GetViewFrustum(void)
{
    return _ViewFrustum;
}


//----------------------------------------------------------------------------------------
// Name: SetFirstPersonView()
// Desc:
//----------------------------------------------------------------------------------------

void SetFirstPersonView(size_t PlayerIDTank, size_t PlayerIDBarrell)
{
    SetCameraState(CAMERASTATE_IDLE, CAMERATYPE_FIRSTPERSON);

    HidePlayer(PlayerIDTank);
    HidePlayer(PlayerIDBarrell);

    AttachCameraToPlayer(PlayerIDTank);
    AttachCameraToPlayerBarrell(PlayerIDBarrell);

    SetCameraViewDistance(0.0f);
    SetCameraViewAngle(0.0f);
    SetCameraViewHeight(0.0f);

    if (-1 != PlayerIDTank)
        DialogTankMove->Children[2]->Name = "Camera Mode: First Person";
    else
        DialogShotInfo->Children[8]->Name = "Camera Mode: First Person";

    UpdateView = UpdateFirstPersonView;

    EnableCrossHair();
}


//----------------------------------------------------------------------------------------
// Name: SetThirdPersonView()
// Desc:
//----------------------------------------------------------------------------------------

void SetThirdPersonView(size_t PlayerIDTank, size_t PlayerIDBarrell)
{
    SetCameraState(CAMERASTATE_IDLE, CAMERATYPE_THIRDPERSON);

    ShowPlayer(PlayerIDTank);
    ShowPlayer(PlayerIDBarrell);

    AttachCameraToPlayer(PlayerIDTank);
    AttachCameraToPlayerBarrell(PlayerIDBarrell);

    SetCameraViewDistance(VIEW_THIRDPERSONDISTANCE);
    SetCameraViewAngle(VIEW_THIRDPERSONANGLE);
    SetCameraViewHeight(VIEW_THIRDPERSONHEIGHT);

    if (-1 != PlayerIDTank)
        DialogTankMove->Children[2]->Name = "Camera Mode: Third Person";
    else
        DialogShotInfo->Children[8]->Name = "Camera Mode: Third Person";

    UpdateView = UpdateThirdPersonView;

    DisableCrossHair();
}


//----------------------------------------------------------------------------------------
// Name: SetChaseView()
// Desc:
//----------------------------------------------------------------------------------------

void SetChaseView(size_t PlayerIDTank, size_t PlayerIDBarrell)
{
    SetCameraState(CAMERASTATE_IDLE, CAMERATYPE_CHASE);

    ShowPlayer(PlayerIDTank);
    ShowPlayer(PlayerIDBarrell);

    AttachCameraToPlayer(PlayerIDTank);
    AttachCameraToPlayerBarrell(PlayerIDBarrell);

    SetCameraViewDistance(VIEW_CHASEDISTANCE);
    SetCameraViewAngle(VIEW_CHASEANGLE);
    SetCameraViewHeight(VIEW_CHASEHEIGHT);

    if (-1 != PlayerIDTank)
        DialogTankMove->Children[2]->Name = "Camera Mode: Chase";
    else
        DialogShotInfo->Children[8]->Name = "Camera Mode: Chase";

    UpdateView = UpdateChaseView;

    DisableCrossHair();
}


//----------------------------------------------------------------------------------------
// Name: SetBirdsEyeView()
// Desc:
//----------------------------------------------------------------------------------------

void SetBirdsEyeView(size_t PlayerIDTank, size_t PlayerIDBarrell)
{
    SetCameraState(CAMERASTATE_IDLE, CAMERATYPE_BIRDSEYE);

    ShowPlayer(PlayerIDTank);
    ShowPlayer(PlayerIDBarrell);

    AttachCameraToPlayer(PlayerIDTank);
    AttachCameraToPlayerBarrell(PlayerIDBarrell);

    SetCameraViewDistance(0.0f);
    SetCameraViewAngle(0.0f);
    SetCameraViewHeight(0.0f);

    if (-1 != PlayerIDTank)
        DialogTankMove->Children[2]->Name = "Camera Mode: Bird's Eye";
    else
        DialogShotInfo->Children[8]->Name = "Camera Mode: Bird's Eye";

    UpdateView = UpdateBirdsEyeView;

    DisableCrossHair();
}


//----------------------------------------------------------------------------------------
// Name: SetMouseLookView()
// Desc:
//----------------------------------------------------------------------------------------

void SetMouseLookView(size_t PlayerIDTank, size_t PlayerIDBarrell)
{
    CameraType_t Type;

    GetCameraState(nullptr, &Type);
    if (Type != CAMERATYPE_MOUSELOOK)
        SetCameraOverCurrentPlayer();

    SetCameraState(CAMERASTATE_MOUSELOOK, CAMERATYPE_MOUSELOOK);

    ShowPlayer(PlayerIDTank);
    ShowPlayer(PlayerIDBarrell);

    if (-1 != PlayerIDTank)
        DialogTankMove->Children[2]->Name = "Camera Mode: MouseLook";
    else
        DialogShotInfo->Children[8]->Name = "Camera Mode: MouseLook";

    UpdateView = nullptr;

    DisableCrossHair();
}


//----------------------------------------------------------------------------------------
// Name: UpdateFirstPersonView()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateFirstPersonView(void)
{
    Vector_t ObjPos;
    Vector_t CamPos;
    Vector_t Normal;

    // get the normal of the poly the player's sitting on.  use this
    // to project a point out from the poly to place the camera at.

    if (-1 != GetCameraAttachedPlayerBarrell())
    {
        GetObjectPosition(GetPlayerTank(GetCameraAttachedPlayerBarrell()), 0, &ObjPos);
        GetPlayerGroundNormal(GetCameraAttachedPlayerBarrell(), &Normal);
    }
    else
    {
        GetObjectPosition(GetPlayerTank(GetCameraAttachedPlayer()), 0, &ObjPos);
        GetPlayerGroundNormal(GetCameraAttachedPlayer(), &Normal);
    }

    CamPos.X = ObjPos.X + (TANK_BARRELLHEIGHT * Normal.X);
    CamPos.Y = ObjPos.Y + (TANK_BARRELLHEIGHT * Normal.Y);
    CamPos.Z = ObjPos.Z + (TANK_BARRELLHEIGHT * Normal.Z);
    SetCameraPosition(&CamPos);
}


//----------------------------------------------------------------------------------------
// Name: UpdateThirdPersonView()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateThirdPersonView(void)
{
    Vector_t ObjPos, Intersect;
    Vector_t* CamPos;
    Vector_t ObjRot0, ObjRot1, ObjRot2;
    Vector_t Normal;
    Vector_t Move;
    Polygon_t* Polygon;
    float Distance;
    size_t PlayerID, TankID;

    // get the normal of the poly the player's sitting on.  use this
    // to project a point out from the poly to place the camera at.

    if (-1 != GetCameraAttachedPlayerBarrell())
    {
        PlayerID = GetCameraAttachedPlayerBarrell();
        TankID = GetPlayerTank(PlayerID);

        GetObjectPosition(TankID, 0, &ObjPos);
        GetObjectRotation(TankID, 0, &ObjRot0);
        GetObjectRotation(TankID, 1, &ObjRot1);
        GetObjectRotation(TankID, 2, &ObjRot2);
        GetPlayerGroundNormal(PlayerID, &Normal);
    }
    else
    {
        PlayerID = GetCameraAttachedPlayer();
        TankID = GetPlayerTank(PlayerID);

        GetObjectPosition(TankID, 0, &ObjPos);
        GetObjectRotation(TankID, 0, &ObjRot0);
        GetPlayerGroundNormal(PlayerID, &Normal);
    }

    CamPos = GetCameraPosition();
    CamPos->X = ObjPos.X + (GetCameraViewHeight() * Normal.X);
    CamPos->Y = ObjPos.Y + (GetCameraViewHeight() * Normal.Y);
    CamPos->Z = ObjPos.Z + (GetCameraViewHeight() * Normal.Z);

    // now project that point in the opposite direction that the camera's facing
    // and see if it intersects with the landscape.

    if (-1 != GetCameraAttachedPlayerBarrell())
    {
        Move.X = 0.0f; Move.Y = 0.0f; Move.Z = 1.0f;
        VectorRotateX(-ObjRot2.X * DTOR, &Move);
        VectorRotateY(-(ObjRot0.Y + ObjRot1.Y) * DTOR, &Move);
        VectorRotateZ(-ObjRot0.Z * DTOR, &Move);
        VectorRotateX(-ObjRot0.X * DTOR, &Move);
    }
    else
        VectorGetMoveXZY(-ObjRot0.X * DTOR, -ObjRot0.Z * DTOR, -ObjRot0.Y * DTOR, &Move);

    ObjPos.X = CamPos->X - (Move.X * VIEW_THIRDPERSONDISTANCE);
    ObjPos.Y = CamPos->Y - (Move.Y * VIEW_THIRDPERSONDISTANCE);
    ObjPos.Z = CamPos->Z - (Move.Z * VIEW_THIRDPERSONDISTANCE);

    if (LineMeshIntersect(g_LandscapeMesh, CamPos, &ObjPos, &Polygon, &Intersect, &Distance) == IntersectionResult::IntersectsLandscape)
        SetCameraViewDistance(Distance);
    else
        SetCameraViewDistance(VIEW_THIRDPERSONDISTANCE);
}


//----------------------------------------------------------------------------------------
// Name: UpdateChaseView()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateChaseView(void)
{
    Vector_t ObjPos, Intersect;
    Vector_t* CamPos;
    Vector_t ObjRot0, ObjRot1, ObjRot2;
    Vector_t Normal;
    Vector_t Move;
    Polygon_t* Polygon;
    float Distance;
    size_t PlayerID, TankID;

    // get the normal of the poly the player's sitting on.  use this
    // to project a point out from the poly to place the camera at.

    if (-1 != GetCameraAttachedPlayerBarrell())
    {
        PlayerID = GetCameraAttachedPlayerBarrell();
        TankID = GetPlayerTank(PlayerID);

        GetObjectPosition(TankID, 0, &ObjPos);
        GetObjectRotation(TankID, 0, &ObjRot0);
        GetObjectRotation(TankID, 1, &ObjRot1);
        GetObjectRotation(TankID, 2, &ObjRot2);
        GetPlayerGroundNormal(PlayerID, &Normal);
    }
    else
    {
        PlayerID = GetCameraAttachedPlayer();
        TankID = GetPlayerTank(PlayerID);

        GetObjectPosition(TankID, 0, &ObjPos);
        GetObjectRotation(TankID, 0, &ObjRot0);
        GetPlayerGroundNormal(PlayerID, &Normal);
    }

    CamPos = GetCameraPosition();
    CamPos->X = ObjPos.X + (GetCameraViewHeight() * Normal.X);
    CamPos->Y = ObjPos.Y + (GetCameraViewHeight() * Normal.Y);
    CamPos->Z = ObjPos.Z + (GetCameraViewHeight() * Normal.Z);

    // now project that point in the opposite direction that the camera's facing
    // and see if it intersects with the landscape.

    if (-1 != GetCameraAttachedPlayerBarrell())
    {
        Move.X = 0.0f; Move.Y = 0.0f; Move.Z = 1.0f;
        VectorRotateX(-ObjRot2.X * DTOR, &Move);
        VectorRotateY(-(ObjRot0.Y + ObjRot1.Y) * DTOR, &Move);
        VectorRotateZ(-ObjRot0.Z * DTOR, &Move);
        VectorRotateX(-ObjRot0.X * DTOR, &Move);
    }
    else
        VectorGetMoveXZY(-ObjRot0.X * DTOR, -ObjRot0.Z * DTOR, -ObjRot0.Y * DTOR, &Move);

    ObjPos.X = CamPos->X - (Move.X * VIEW_CHASEDISTANCE);
    ObjPos.Y = CamPos->Y - (Move.Y * VIEW_CHASEDISTANCE);
    ObjPos.Z = CamPos->Z - (Move.Z * VIEW_CHASEDISTANCE);

    if (LineMeshIntersect(g_LandscapeMesh, CamPos, &ObjPos, &Polygon, &Intersect, &Distance) == IntersectionResult::IntersectsLandscape)
        SetCameraViewDistance(Distance);
    else
        SetCameraViewDistance(VIEW_CHASEDISTANCE);
}


//----------------------------------------------------------------------------------------
// Name: UpdateBirdsEyeView()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateBirdsEyeView(void)
{
    Vector_t ObjPos;
    Vector_t ObjRot0, ObjRot1;
    Vector_t* CamPos;
    Vector_t* CamRot;
    size_t PlayerID, TankID;

    // place the camera directly above the player point it straight
    // down. disregard the normal of the polygon the player's sitting on.

    CamPos = GetCameraPosition();
    CamRot = GetCameraRotation();

    if (-1 != GetCameraAttachedPlayerBarrell())
    {
        PlayerID = GetCameraAttachedPlayerBarrell();
        TankID = GetPlayerTank(PlayerID);

        GetObjectRotation(TankID, 0, &ObjRot0);
        GetObjectRotation(TankID, 1, &ObjRot1);
        GetObjectPosition(TankID, 0, &ObjPos);

        CamRot->Y = -(ObjRot0.Y + ObjRot1.Y) + 180.0f;
    }
    else
    {
        PlayerID = GetCameraAttachedPlayer();
        TankID = GetPlayerTank(PlayerID);

        GetObjectRotation(TankID, 0, &ObjRot0);
        GetObjectPosition(TankID, 0, &ObjPos);

        CamRot->Y = -ObjRot0.Y + 180.0f;
    }

    VectorCopy(ObjPos, (*CamPos));
    CamPos->Y += VIEW_BIRDSEYEDISTANCE;

    CamRot->X = 270.0f;
    CamRot->Z = 0.0f;
}


//----------------------------------------------------------------------------------------
// Name: IsLineOfSight()
// Desc:
//----------------------------------------------------------------------------------------

bool IsLineOfSight(const Vector_t& From, const Vector_t& To)
{
    Polygon_t *Polygon; Vector_t Intersect; float Smallest;
    return LineMeshIntersect(g_LandscapeMesh, (Vector_t*)&From, (Vector_t*)&To, &Polygon, &Intersect, &Smallest) == IntersectionResult::ClearLineOfSight;
}


//----------------------------------------------------------------------------------------
// Name: IsConeOfSight()
// Desc:
//----------------------------------------------------------------------------------------

bool IsConeOfSight(const Vector_t& From, const Vector_t& To, float ConeBaseRadius, size_t NumConeBasePoints, size_t& NumCollidingPoints)
{
    if (!IsLineOfSight(From, To))
        return false;

    Vector_t Right, Up, Forward;
    DirectionToBasis(From, To, Right, Up, Forward);

    Vector_t NewFrom;
    float AngleDegrees = 0.0f;
    float AngleInc = 360.0f / float(NumConeBasePoints);

    Vector_t RotateMe;
    RotateMe.X = Up.X * ConeBaseRadius;
    RotateMe.Y = Up.Y * ConeBaseRadius;
    RotateMe.Z = Up.Z * ConeBaseRadius;

    for (size_t i = 0; i < NumConeBasePoints; ++i)
    {
        NewFrom = RotatePointAroundVector(RotateMe, Forward, AngleDegrees); // Rotates around the origin

        NewFrom.X += From.X;    // Move it into world space
        NewFrom.Y += From.Y;
        NewFrom.Z += From.Z;

        if (!IsLineOfSight(NewFrom, To))
            return false;

        AngleDegrees += AngleInc;
    }

    return true;
}


//----------------------------------------------------------------------------------------
// Name: IsCylinderOfSight()
// Desc:
//----------------------------------------------------------------------------------------

bool IsCylinderOfSight(const Vector_t& From, const Vector_t& To, float CylinderRadius, size_t NumCylinderPoints, size_t& NumCollidingPoints)
{
    if (!IsLineOfSight(From, To))
        return false;

    Vector_t Right, Up, Forward;
    DirectionToBasis(From, To, Right, Up, Forward);

    Vector_t Direction;
    VectorSubtract(To, From, Direction);

    Vector_t NewFrom, NewTo;
    float AngleDegrees = 0.0f;
    float AngleInc = 360.0f / float(NumCylinderPoints);

    Vector_t FromRotateMe;
    FromRotateMe.X = Up.X * CylinderRadius;
    FromRotateMe.Y = Up.Y * CylinderRadius;
    FromRotateMe.Z = Up.Z * CylinderRadius;

    for (size_t i = 0; i < NumCylinderPoints; ++i)
    {
        NewFrom = RotatePointAroundVector(FromRotateMe, Forward, AngleDegrees); // Rotates around the origin

        NewFrom.X += From.X;    // Move it into world space
        NewFrom.Y += From.Y;
        NewFrom.Z += From.Z;

        NewTo.X = NewFrom.X + Direction.X;
        NewTo.Y = NewFrom.Y + Direction.Y;
        NewTo.Z = NewFrom.Z + Direction.Z;

        if (!IsLineOfSight(NewFrom, NewTo))
            return false;

        AngleDegrees += AngleInc;
    }

    return true;
}
