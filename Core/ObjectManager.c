//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#include "ObjectTypes.h"
#include "Object.h"
#include "LWObject.h"

#include "LightSource.h"
#include "Vector.h"
#include "Camera.h"
#include "GameTypes.h"
#include "BotShoot.h"

#include "ObjectManager.h"
#include "ModelManager.h"
#include "MeshManager.h"
#include "ParticleManager.h"

#include "ParticleGroundSmoke.h"


#define MAXOBJECTS              256

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXOBJECTS || !_ObjectPool[ _id ].Used )


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern size_t g_LandscapeMesh;


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _ObjectIndex = 0;
static Object_t _ObjectPool[ MAXOBJECTS ];

static size_t _LODDistance;
float _LODSettings[ 3 ] = { 1000.0f, 1500.0f, 2000.0f };


//----------------------------------------------------------------------------------------
// private functions
//----------------------------------------------------------------------------------------

static void CollideObjectWithObjects( Object_t *Object );


//----------------------------------------------------------------------------------------
// private functions
//----------------------------------------------------------------------------------------

static void ClipObjectToWorld( size_t ID );


//----------------------------------------------------------------------------------------
// Name: GetFreeObject()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetFreeObject( void )
{
    size_t i;
    size_t ID;

    i = _ObjectIndex;

    while( _ObjectPool[ i ].Used )
    {
        i++;
        if( i >= MAXOBJECTS )
            i = 0;

        if( i == _ObjectIndex )
            return -1;        // pool is full
    }

    _ObjectPool[ i ].Used = 1;
    ID = i;

    _ObjectIndex = i;
    if( ++_ObjectIndex >= MAXOBJECTS )
        _ObjectIndex = 0;

    _ObjectPool[ ID ].ID = ID;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnObject()
// Desc: 
//----------------------------------------------------------------------------------------

void ReturnObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: ObjectManagerThink()
// Desc: 
//----------------------------------------------------------------------------------------

void ObjectManagerThink( void )
{
    size_t i, IsHiResModel;
    Vector_t *CameraPosition;
    float XDistance, ZDistance;

    CameraPosition = GetCameraPosition();

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && ( _ObjectPool[ i ].State != OBJECTSTATE_DEAD ) && _ObjectPool[ i ].OnThink )
        {
            XDistance = fabsf( _ObjectPool[ i ].Positions[ 0 ].X - CameraPosition->X );
            ZDistance = fabsf( _ObjectPool[ i ].Positions[ 0 ].Z - CameraPosition->Z );
            IsHiResModel = ( XDistance < _LODSettings[ _LODDistance ] && ZDistance < _LODSettings[ _LODDistance ] );

            // let the think function know if the manager thinks that it should use a hi-res model.  handle the
            // case of this object only having a hi-res model.
            _ObjectPool[ i ].OnThink( &_ObjectPool[ i ], _ObjectPool[ i ].ModelIDLoRes != -1 ? IsHiResModel : 1 );
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: ObjectManagerMove()
// Desc: 
//----------------------------------------------------------------------------------------

void ObjectManagerMove( void )
{
    size_t i;
    float Friction;

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used )
            _ObjectPool[ i ].Physics.Flags &= ~PHYSICS_HASCOLLIDED;
    }

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && _ObjectPool[ i ].State == OBJECTSTATE_ALIVE )
        {
            if( IsObjectOnGround( &_ObjectPool[ i ] ) )
            {
                _ObjectPool[ i ].Physics.Flags |= PHYSICS_ONGROUNDBEFOREMOVE;
                Friction = DEFAULT_FRICTIONGROUND;
            }
            else
            {
                _ObjectPool[ i ].Physics.Flags &= ~PHYSICS_ONGROUNDBEFOREMOVE;
                Friction = DEFAULT_FRICTIONAIR;
            }

            MoveObject( &_ObjectPool[ i ], Friction );

            if( _ObjectPool[ i ].Physics.Flags & PHYSICS_CLIPTOWORLD )
                ClipObjectToWorld( i );

            if( IsObjectOnGround( &_ObjectPool[ i ] ) )
                _ObjectPool[ i ].Physics.Flags |= PHYSICS_ONGROUNDAFTERMOVE;
            else
                _ObjectPool[ i ].Physics.Flags &= ~PHYSICS_ONGROUNDAFTERMOVE;

            if( _ObjectPool[ i ].OnMove )
                _ObjectPool[ i ].OnMove( &_ObjectPool[ i ] );
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: ObjectManagerCollide()
// Desc: 
//----------------------------------------------------------------------------------------

void ObjectManagerCollide( size_t MeshID )
{
    size_t i;

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && ( _ObjectPool[ i ].State == OBJECTSTATE_ALIVE ) && _ObjectPool[ i ].CollideMesh )
        {
            _ObjectPool[ i ].CollideMesh( &_ObjectPool[ i ], MeshID );

            if( !( _ObjectPool[ i ].Physics.Flags & PHYSICS_ONGROUNDBEFOREMOVE ) &&
                 ( _ObjectPool[ i ].Physics.Flags & PHYSICS_ONGROUNDAFTERMOVE  ) )
            {
                StartGroundSmoke( &_ObjectPool[ i ].Positions[ 0 ] );

                if( _ObjectPool[ i ].OnFallingDamage )
                    _ObjectPool[ i ].OnFallingDamage( &_ObjectPool[ i ] );
            }

            if( ( _ObjectPool[ i ].State == OBJECTSTATE_ALIVE ) && ( _ObjectPool[ i ].Flags & OBJECT_CANCOLLIDEWITHTANK ) )
                CollideObjectWithObjects( &_ObjectPool[ i ] );
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: ObjectManagerDraw()
// Desc: 
//----------------------------------------------------------------------------------------

void ObjectManagerDraw( Plane2_t *ViewFrustum )
{
    size_t i;
    float Ambient;
    Vector_t *CameraPosition;
    float XDistance, ZDistance;

    CameraPosition = GetCameraPosition();
    GetLightSourceAmbient( &Ambient );

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && ( _ObjectPool[ i ].Draw || _ObjectPool[ i ].Draw2 ) &&
            ( _ObjectPool[ i ].State == OBJECTSTATE_ALIVE ) && ( _ObjectPool[ i ].Flags & OBJECT_CANBEDRAWN ) )
        {
            if( _ObjectPool[ i ].Draw2 )
            {
                glPushMatrix();

                _ObjectPool[ i ].Draw2( _ObjectPool[ i ].ModelIDHiRes, _ObjectPool[ i ].Positions,
                                        _ObjectPool[ i ].Rotations, _ObjectPool[ i ].Scales, &_ObjectPool[ i ].BaseColor,
                                        _ObjectPool[ i ].RotateType, GetObjectBlendFactor( i ) );

                glPopMatrix();
            }
            else if( VectorIsSphereInFrustum( ViewFrustum, &_ObjectPool[ i ].Positions[ 0 ], _ObjectPool[ i ].Physics.BBox.Sphere.Radius ) )
            {
                glPushMatrix();

                if( -1 == _ObjectPool[ i ].ModelIDLoRes )
                {
                    _ObjectPool[ i ].Draw( _ObjectPool[ i ].ModelIDHiRes, _ObjectPool[ i ].Positions,
                                           _ObjectPool[ i ].Rotations, _ObjectPool[ i ].PointColorsHiRes,
                                           _ObjectPool[ i ].RotateType, Ambient );
                }
                else
                {
                    XDistance = fabsf( _ObjectPool[ i ].Positions[ 0 ].X - CameraPosition->X );
                    ZDistance = fabsf( _ObjectPool[ i ].Positions[ 0 ].Z - CameraPosition->Z );

                    // draw either the hi or lo res model
                    if( XDistance < _LODSettings[ _LODDistance ] && ZDistance < _LODSettings[ _LODDistance ] )
                        _ObjectPool[ i ].Draw( _ObjectPool[ i ].ModelIDHiRes, _ObjectPool[ i ].Positions,
                                               _ObjectPool[ i ].Rotations, _ObjectPool[ i ].PointColorsHiRes,
                                               _ObjectPool[ i ].RotateType, Ambient );
                    else
                        _ObjectPool[ i ].Draw( _ObjectPool[ i ].ModelIDLoRes, _ObjectPool[ i ].Positions,
                                               _ObjectPool[ i ].Rotations, _ObjectPool[ i ].PointColorsLoRes,
                                               _ObjectPool[ i ].RotateType, Ambient );
                }

                glPopMatrix();
            }
        }
    }

}


//----------------------------------------------------------------------------------------
// Name: ObjectManagerRemoveDead()
// Desc: 
//----------------------------------------------------------------------------------------

void ObjectManagerRemoveDead( void )
{
    size_t i;

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && _ObjectPool[ i ].State & OBJECTSTATE_DEAD )
            ReturnObject( i );
    }
}


//----------------------------------------------------------------------------------------
// Name: CollideObjectWithObjects()
// Desc: 
//----------------------------------------------------------------------------------------

static void CollideObjectWithObjects( Object_t *Object )
{
    size_t i;

    // collide this object with any tanks that are alive
    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && ( _ObjectPool[ i ].State == OBJECTSTATE_ALIVE ) &&
            &_ObjectPool[ i ] != Object && ( _ObjectPool[ i ].Flags & OBJECT_ISTANK ) )
            CollideObjects( Object, &_ObjectPool[ i ] );
    }
}


//----------------------------------------------------------------------------------------
// Name: ResetObject()
// Desc: 
//----------------------------------------------------------------------------------------

void ResetObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    InitialiseObject( &_ObjectPool[ ID ] );
}


//----------------------------------------------------------------------------------------
// Name: SetObjectModels()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectModels( size_t ObjectID, size_t ModelIDHiRes, size_t ModelIDLoRes )
{
    if( INVALIDID( ObjectID ) )
        return;

    FreePointColors( &_ObjectPool[ ObjectID ] );

    _ObjectPool[ ObjectID ].ModelIDHiRes = ModelIDHiRes;
    _ObjectPool[ ObjectID ].ModelIDLoRes = ModelIDLoRes;

    _ObjectPool[ ObjectID ].Physics.BBoxType = BBOX_SPHERE;
    _ObjectPool[ ObjectID ].Physics.BBox.Sphere.Radius = GetModelBSphereRadius( ModelIDHiRes );

    AllocatePointColors( &_ObjectPool[ ObjectID ] );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectModels()
// Desc: 
//----------------------------------------------------------------------------------------

void GetObjectModels( size_t ObjectID, size_t *ModelIDHiRes, size_t *ModelIDLoRes )
{
    if( INVALIDID( ObjectID ) )
        return;

    if( ModelIDHiRes )
        (*ModelIDHiRes) = _ObjectPool[ ObjectID ].ModelIDHiRes;

    if( ModelIDLoRes )
        (*ModelIDLoRes) = _ObjectPool[ ObjectID ].ModelIDLoRes;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectDrawFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectDrawFunction( size_t ID, void (*Draw)( size_t, Vector_t *, Vector_t *, Color_t *Colors[], size_t, float ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Draw = Draw;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectDrawFunction2()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectDrawFunction2( size_t ID, void (*Draw)( size_t, Vector_t *, Vector_t *, Vector_t *, Color_t *Color, size_t, float ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Draw2 = Draw;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectCollideMeshFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectCollideMeshFunction( size_t ID, void (*Collide)( Object_t *, size_t ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].CollideMesh = Collide;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectOnMoveFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectOnMoveFunction( size_t ID, void (*Move)( Object_t * ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].OnMove = Move;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectOnThinkFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectOnThinkFunction( size_t ID, void (*Think)( Object_t *, size_t IsHiRes ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].OnThink = Think;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectOnCollideMeshFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectOnCollideMeshFunction( size_t ID, void (*Collide)( Object_t * ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].OnCollideMesh = Collide;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectOnCollideObjectFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectOnCollideObjectFunction( size_t ID, void (*Collide)( Object_t *, Object_t * ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].OnCollideObject = Collide;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectOnFallingDamageFunction()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectOnFallingDamageFunction( size_t ID, void (*FallingDamage)( Object_t * ) )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].OnFallingDamage = FallingDamage;
}


//----------------------------------------------------------------------------------------
// Name: DoesObjectHaveFallingDamageFunction()
// Desc: 
//----------------------------------------------------------------------------------------

size_t DoesObjectHaveFallingDamageFunction( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    return( nullptr != _ObjectPool[ ID ].OnFallingDamage );
}


//----------------------------------------------------------------------------------------
// Name: ActivateObject()
// Desc: 
//----------------------------------------------------------------------------------------

void AddObjectFlag( size_t ID, size_t Flag )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Flags |= Flag;
}


//----------------------------------------------------------------------------------------
// Name: ActivateObject()
// Desc: 
//----------------------------------------------------------------------------------------

void RemoveObjectFlag( size_t ID, size_t Flag )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Flags &= ~Flag;
}


//----------------------------------------------------------------------------------------
// Name: ActivateObject()
// Desc: 
//----------------------------------------------------------------------------------------

void ActivateObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].State = OBJECTSTATE_ALIVE;
}


//----------------------------------------------------------------------------------------
// Name: DeactivateObject()
// Desc: 
//----------------------------------------------------------------------------------------

void DeactivateObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].State = OBJECTSTATE_INACTIVE;
}


//----------------------------------------------------------------------------------------
// Name: KillObject()
// Desc: 
//----------------------------------------------------------------------------------------

void KillObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].State = OBJECTSTATE_DEAD;
}


//----------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//----------------------------------------------------------------------------------------

void DrawObject( size_t ID )
{
    float Ambient;
    Vector_t Direction;

    if( INVALIDID( ID ) )
        return;

    if( nullptr == _ObjectPool[ ID ].Draw )
        return;

    GetLightSourceAmbient( &Ambient );
    GetLightSourceDirection( &Direction.X, &Direction.Y, &Direction.Z );

    if( _ObjectPool[ ID ].Draw == DrawShadedModel || _ObjectPool[ ID ].Draw == DrawShadedModelTrans )
        CalcObjectPointColors( _ObjectPool[ ID ].ModelIDHiRes, &Direction, Ambient,
                               _ObjectPool[ ID ].Positions, _ObjectPool[ ID ].Rotations,
                               _ObjectPool[ ID ].PointColorsHiRes );

    _ObjectPool[ ID ].Draw( _ObjectPool[ ID ].ModelIDHiRes, _ObjectPool[ ID ].Positions,
                            _ObjectPool[ ID ].Rotations, _ObjectPool[ ID ].PointColorsHiRes,
                            _ObjectPool[ ID ].RotateType, Ambient );
}


//----------------------------------------------------------------------------------------
// Name: SetObjectLOD()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectLOD( size_t Distance )
{
    switch( Distance )
    {
        case 0 : case 1 : case 2 :
            _LODDistance = Distance;
            break;

        default :
            break;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetObjectLOD()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetObjectLOD( void )
{
    return _LODDistance;
}


//----------------------------------------------------------------------------------------
// Name: IsObjectDead()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IsObjectDead( size_t ID )
{
    if( INVALIDID( ID ) )
        return 1;

    return( _ObjectPool[ ID ].State == OBJECTSTATE_DEAD );
}


//----------------------------------------------------------------------------------------
// Name: IsObjectActivated()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IsObjectActivated( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    return( _ObjectPool[ ID ].State != OBJECTSTATE_INACTIVE );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectRotation()
// Desc: 
//----------------------------------------------------------------------------------------

void GetObjectRotation( size_t ID, size_t PieceIndex, Vector_t *Vector )
{
    if( INVALIDID( ID ) )
        return;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS || nullptr == Vector )
        return;

    VectorCopy( _ObjectPool[ ID ].Rotations[ PieceIndex ], (*Vector) );
}


//----------------------------------------------------------------------------------------
// Name: SetObjectRotation()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectRotation( size_t ID, size_t PieceIndex, Vector_t *Vector )
{
    if( INVALIDID( ID ) )
        return;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS || nullptr == Vector )
        return;

    VectorCopy( (*Vector), _ObjectPool[ ID ].Rotations[ PieceIndex ] );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectPosition()
// Desc: 
//----------------------------------------------------------------------------------------

void GetObjectPosition( size_t ID, size_t PieceIndex, Vector_t *Point )
{
    if( INVALIDID( ID ) )
        return;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS || nullptr == Point )
        return;

    VectorCopy( _ObjectPool[ ID ].Positions[ PieceIndex ], (*Point) );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectPositionPtr()
// Desc: 
//----------------------------------------------------------------------------------------

Vector_t *GetObjectPositionPtr( size_t ID, size_t PieceIndex )
{
    if( INVALIDID( ID ) )
        return nullptr;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS )
        return nullptr;

    return &_ObjectPool[ ID ].Positions[ PieceIndex ];
}


//----------------------------------------------------------------------------------------
// Name: SetObjectPosition()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectPosition( size_t ID, size_t PieceIndex, Vector_t *Point )
{
    if( INVALIDID( ID ) || !Point )
        return;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS || nullptr == Point )
        return;

    VectorCopy( (*Point), _ObjectPool[ ID ].Positions[ PieceIndex ] );
}


//----------------------------------------------------------------------------------------
// Name: HideObject()
// Desc: 
//----------------------------------------------------------------------------------------

void HideObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Flags &= ~OBJECT_CANBEDRAWN;
}


//----------------------------------------------------------------------------------------
// Name: ShowObject()
// Desc: 
//----------------------------------------------------------------------------------------

void ShowObject( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Flags |= OBJECT_CANBEDRAWN;
}


//----------------------------------------------------------------------------------------
// Name: ApplyForceToObject()
// Desc: 
//----------------------------------------------------------------------------------------

void ApplyForceToObject( size_t ID, Vector_t *Force )
{
    if( INVALIDID( ID ) )
        return;

    // VectorScale( (*Force), _ObjectPool[ ID ].Physics.Mass, (*Force) );
    VectorAdd( _ObjectPool[ ID ].Physics.Move, (*Force), _ObjectPool[ ID ].Physics.Move );
}


//----------------------------------------------------------------------------------------
// Name: CapObjectToTermVelo()
// Desc: 
//----------------------------------------------------------------------------------------

void CapObjectToTermVelo( size_t ID )
{
    float Mag;

    if( INVALIDID( ID ) )
        return;

	Mag = VectorMagnitude( &_ObjectPool[ ID ].Physics.Move );
    if( Mag < -TERMINALVELOCITY )
    {
        VectorNormalise( &_ObjectPool[ ID ].Physics.Move );
        VectorScale( _ObjectPool[ ID ].Physics.Move, -TERMINALVELOCITY, _ObjectPool[ ID ].Physics.Move );
    }
    else if( Mag > TERMINALVELOCITY )
    {
        VectorNormalise( &_ObjectPool[ ID ].Physics.Move );
        VectorScale( _ObjectPool[ ID ].Physics.Move, TERMINALVELOCITY, _ObjectPool[ ID ].Physics.Move );
    }
}


//----------------------------------------------------------------------------------------
// Name: GetObjectMovement()
// Desc: 
//----------------------------------------------------------------------------------------

Vector_t *GetObjectMovement( size_t ID )
{
    if( INVALIDID( ID ) )
        return nullptr;

    return &_ObjectPool[ ID ].Physics.Move;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectMovement()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectMovement( size_t ID, Vector_t *Move )
{
    if( INVALIDID( ID ) )
        return;

    VectorCopy( (*Move), _ObjectPool[ ID ].Physics.Move );
}


//----------------------------------------------------------------------------------------
// Name: ClipObjectToWorld()
// Desc: 
//----------------------------------------------------------------------------------------

static void ClipObjectToWorld( size_t ID )
{
    float MaxX, MaxY, MaxZ;
    float Quarter, Height;

    if( INVALIDID( ID ) )
        return;

    GetMeshMaxDimensions( g_LandscapeMesh, &MaxX, &MaxY, &MaxZ );

    Quarter = MaxX / 4.0f;

    if( _ObjectPool[ ID ].Positions[ 0 ].X < Quarter )
        _ObjectPool[ ID ].Positions[ 0 ].X = Quarter;
    else if( _ObjectPool[ ID ].Positions[ 0 ].X > MaxX - Quarter )
        _ObjectPool[ ID ].Positions[ 0 ].X = MaxX - Quarter;

    Quarter = MaxZ / 4.0f;

    if( _ObjectPool[ ID ].Positions[ 0 ].Z < Quarter )
        _ObjectPool[ ID ].Positions[ 0 ].Z = Quarter;
    else if( _ObjectPool[ ID ].Positions[ 0 ].Z > MaxZ - Quarter )
        _ObjectPool[ ID ].Positions[ 0 ].Z = MaxZ - Quarter;

    if( _ObjectPool[ ID ].Positions[ 0 ].X < 0.0f || _ObjectPool[ ID ].Positions[ 0 ].X > MaxX ||
        _ObjectPool[ ID ].Positions[ 0 ].Z < 0.0f || _ObjectPool[ ID ].Positions[ 0 ].Z > MaxZ )
        Height = 0.0f;
    else
        Height = GetMeshHeight( g_LandscapeMesh, _ObjectPool[ ID ].Positions[ 0 ].X, _ObjectPool[ ID ].Positions[ 0 ].Z, nullptr );

    if( _ObjectPool[ ID ].Positions[ 0 ].Y < Height )
        _ObjectPool[ ID ].Positions[ 0 ].Y = Height;
    else if( _ObjectPool[ ID ].Positions[ 0 ].Y > MaxY )
        _ObjectPool[ ID ].Positions[ 0 ].Y = MaxY;

}


//----------------------------------------------------------------------------------------
// Name: SetObjectData()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectData( size_t ID, size_t Data )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Data = Data;
}


//----------------------------------------------------------------------------------------
// Name: GetObjectData()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetObjectData( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    return _ObjectPool[ ID ].Data;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectGroundPolygon()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectGroundPolygon( size_t ID, size_t PolyIndex )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Physics.PolyNormal = PolyIndex;
}


//----------------------------------------------------------------------------------------
// Name: GetObjectGroundPolygon()
// Desc: 
//----------------------------------------------------------------------------------------

size_t GetObjectGroundPolygon( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    return _ObjectPool[ ID ].Physics.PolyNormal;
}


//----------------------------------------------------------------------------------------
// Name: SetTankPhysics()
// Desc: 
//----------------------------------------------------------------------------------------

void SetTankPhysics( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Physics.Flags = PHYSICS_CLIPTOWORLD;
    _ObjectPool[ ID ].Physics.Mass  = 2.0f;
}


//----------------------------------------------------------------------------------------
// Name: SetTankPhysics()
// Desc: 
//----------------------------------------------------------------------------------------

void SetShieldParachutePhysics( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Physics.Flags = PHYSICS_IGNOREGRAVITY | PHYSICS_IGNOREFRICTION;
}


//----------------------------------------------------------------------------------------
// Name: SetTankPhysics()
// Desc: 
//----------------------------------------------------------------------------------------

void SetRailgunPhysics( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].Physics.Flags = PHYSICS_IGNOREGRAVITY | PHYSICS_IGNOREFRICTION;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectRotateType()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectRotateType( size_t ID, size_t Type )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].RotateType = Type;
}


//----------------------------------------------------------------------------------------
// Name: SetObjectThinkCounter()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectThinkCounter( size_t ID, size_t CounterIndex, size_t Value )
{
    if( INVALIDID( ID ) )
        return;

    switch( CounterIndex )
    {
        case 0 : _ObjectPool[ ID ].ThinkCounter1 = Value; break;
        case 1 : _ObjectPool[ ID ].ThinkCounter2 = Value; break;
        case 2 : _ObjectPool[ ID ].ThinkCounter3 = Value; break;
        case 3 : _ObjectPool[ ID ].ThinkCounter4 = Value; break;
    }
}


//----------------------------------------------------------------------------------------
// Name: SetObjectThinkCounter()
// Desc: 
//----------------------------------------------------------------------------------------

void CollideParticleWithObjects( ParticleSystem_t *System, Particle_t *Particle )
{
    size_t i;
    float DistanceBetween;
    Vector_t V;

    if( nullptr == System->OnCollideObject )
        return;

    for( i = 0; i < MAXOBJECTS; i++ )
    {
        if( _ObjectPool[ i ].Used && ( _ObjectPool[ i ].State == OBJECTSTATE_ALIVE ) && ( _ObjectPool[ i ].Flags & OBJECT_ISTANK ) )
        {
            VectorSubtract( Particle->Position, _ObjectPool[ i ].Positions[ 0 ], V );
            DistanceBetween = fabsf( VectorMagnitude( &V ) );

            if( DistanceBetween <= Particle->BSphereSize + GetModelBSphereRadius( _ObjectPool[ i ].ModelIDHiRes ) )
            {
                if( System->OnCollideObject )
                    System->OnCollideObject( System, Particle, &_ObjectPool[ i ] );

                if( _ObjectPool[ i ].OnCollideParticle )
                    _ObjectPool[ i ].OnCollideParticle( &_ObjectPool[ i ], Particle );
            }
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: SetObjectBlendFactor()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectBlendFactor( size_t ID, float Blend )
{
    if( INVALIDID( ID ) )
        return;

    _ObjectPool[ ID ].BlendFactor = Blend;
}


//----------------------------------------------------------------------------------------
// Name: GetObjectBlendFactor()
// Desc: 
//----------------------------------------------------------------------------------------

float GetObjectBlendFactor( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0.0f;

    return _ObjectPool[ ID ].BlendFactor;
}


//----------------------------------------------------------------------------------------
// Name: GetObjectScale()
// Desc: 
//----------------------------------------------------------------------------------------

void GetObjectScale( size_t ID, size_t PieceIndex, Vector_t *Scale )
{
    if( INVALIDID( ID ) )
        return;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS || nullptr == Scale )
        return;

    VectorCopy( _ObjectPool[ ID ].Scales[ PieceIndex ], (*Scale) );
}


//----------------------------------------------------------------------------------------
// Name: SetObjectScale()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectScale( size_t ID, size_t PieceIndex, Vector_t *Scale )
{
    if( INVALIDID( ID ) || !Scale )
        return;

    if( PieceIndex < 0 || PieceIndex >= LWO_MAXLAYERS || nullptr == Scale )
        return;

    VectorCopy( (*Scale), _ObjectPool[ ID ].Scales[ PieceIndex ] );
}


//----------------------------------------------------------------------------------------
// Name: SetObjectBaseColor()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectBaseColor( size_t ID, Color_t *Color )
{
    if( INVALIDID( ID ) )
        return;

    memcpy( &_ObjectPool[ ID ].BaseColor, Color, sizeof( Color_t ) );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectBaseColor()
// Desc: 
//----------------------------------------------------------------------------------------

void GetObjectBaseColor( size_t ID, Color_t *Color )
{
    if( INVALIDID( ID ) )
        return;

    memcpy( Color, &_ObjectPool[ ID ].BaseColor, sizeof( Color_t ) );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectBaseColor()
// Desc: 
//----------------------------------------------------------------------------------------

void CalcObjectPointColors2( size_t ID )
{
    Object_t *Object;
    Vector_t LightDirection;
    float Ambient;

    if( INVALIDID( ID ) )
        return;

    Object = &_ObjectPool[ ID ];

    GetLightSourceDirection( &LightDirection.X, &LightDirection.Y, &LightDirection.Z );
    GetLightSourceAmbient( &Ambient );

    CalcObjectPointColors( Object->ModelIDHiRes, &LightDirection, Ambient,
                           Object->Positions, Object->Rotations, Object->PointColorsHiRes );
}


//----------------------------------------------------------------------------------------
// Name: GetObjectRotationDeltas()
// Desc: 
//----------------------------------------------------------------------------------------

void GetObjectRotationDeltas( size_t ID, Vector_t *RotationDeltas )
{
    if( INVALIDID( ID ) )
        return;

    VectorCopy( _ObjectPool[ ID ].RotationDeltas, (*RotationDeltas) );
}


//----------------------------------------------------------------------------------------
// Name: SetObjectRotationDeltas()
// Desc: 
//----------------------------------------------------------------------------------------

void SetObjectRotationDeltas( size_t ID, Vector_t *RotationDeltas )
{
    if( INVALIDID( ID ) )
        return;

    VectorCopy( (*RotationDeltas), _ObjectPool[ ID ].RotationDeltas );
}


//----------------------------------------------------------------------------------------
// Name: IsObjectOnGround2()
// Desc: 
//----------------------------------------------------------------------------------------

size_t IsObjectOnGround2( size_t ID )
{
    if( INVALIDID( ID ) )
        return 0;

    _ObjectPool[ ID ].Physics.GroundHeight = GetMeshHeight( g_LandscapeMesh, _ObjectPool[ ID ].Positions[ 0 ].X,
                                                  _ObjectPool[ ID ].Positions[ 0 ].Z, &_ObjectPool[ ID ].Physics.PolyNormal );

    return( _ObjectPool[ ID ].Positions[ 0 ].Y <= _ObjectPool[ ID ].Physics.GroundHeight + DAMAGEHEIGHT );
}


//----------------------------------------------------------------------------------------
// Name: IsObjectOnGround2()
// Desc: 
//----------------------------------------------------------------------------------------

void ClearObjectPhysics( size_t ID )
{
    if( INVALIDID( ID ) )
        return;

    VectorSet( _ObjectPool[ ID ].Physics.Move, 0.0f );
}
