//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#include "ObjectTypes.h"
#include "ParticleTypes.h"
#include "Vector.h"


extern size_t GetFreeObject( void );
extern void ReturnObject( size_t ID );

extern void ActivateObject( size_t ID );
extern void DeactivateObject( size_t ID );
extern void KillObject( size_t ID );
extern void ResetObject( size_t ID );

extern void SetObjectModels( size_t ObjectID, size_t ModelIDHiRes, size_t ModelIDLoRes );
extern void GetObjectModels( size_t ObjectID, size_t *ModelIDHiRes, size_t *ModelIDLoRes );

extern void SetObjectDrawFunction( size_t ID, void (*Draw)( size_t, Vector_t *, Vector_t *, Color_t *[], size_t, float ) );
extern void SetObjectDrawFunction2( size_t ID, void (*Draw)( size_t, Vector_t *, Vector_t *, Vector_t *, Color_t *, size_t, float ) );
extern void SetObjectCollideMeshFunction( size_t ID, void (*Collide)( Object_t *, size_t ) );

extern void SetObjectOnMoveFunction( size_t ID, void (*Move)( Object_t * ) );
extern void SetObjectOnThinkFunction( size_t ID, void (*Think)( Object_t *, size_t IsHiRes ) );
extern void SetObjectOnCollideMeshFunction( size_t ID, void (*Collide)( Object_t * ) );
extern void SetObjectOnCollideObjectFunction( size_t ID, void (*Collide)( Object_t *, Object_t * ) );
extern void SetObjectOnFallingDamageFunction( size_t ID, void (*FallingDamage)( Object_t * ) );

extern size_t DoesObjectHaveFallingDamageFunction( size_t ID );

extern void AddObjectFlag( size_t ID, size_t Flag );
extern void RemoveObjectFlag( size_t ID, size_t Flag );

extern void ObjectManagerThink( void );
extern void ObjectManagerMove( void );
extern void ObjectManagerCollide( size_t MeshID );
extern void ObjectManagerDraw( Plane2_t *ViewFrustum );
extern void ObjectManagerRemoveDead( void );

extern void DrawObject( size_t ID );

extern void SetObjectLOD( size_t Distance );     // 0 = near, 1 = medium, 2 = far
extern size_t GetObjectLOD( void );

extern size_t IsObjectDead( size_t ID );
extern size_t IsObjectActivated( size_t ID );

extern void SetObjectRotation( size_t ID, size_t PieceIndex, Vector_t *Vector );
extern void GetObjectRotation( size_t ID, size_t PieceIndex, Vector_t *Vector );

extern void SetObjectPosition( size_t ID, size_t PieceIndex, Vector_t *Point );
extern void GetObjectPosition( size_t ID, size_t PieceIndex, Vector_t *Point );
extern Vector_t *GetObjectPositionPtr( size_t ID, size_t PieceIndex );

extern void HideObject( size_t ID );
extern void ShowObject( size_t ID );

extern void ApplyForceToObject( size_t ID, Vector_t *Force );
extern void CapObjectToTermVelo( size_t ID );

extern Vector_t *GetObjectMovement( size_t ID );
extern void SetObjectMovement( size_t ID, Vector_t *Move );

extern void SetObjectData( size_t ID, size_t Data );
extern size_t GetObjectData( size_t ID );

extern void SetObjectGroundPolygon( size_t ID, size_t PolyIndex );
extern size_t GetObjectGroundPolygon( size_t ID );

extern void SetTankPhysics( size_t ID );
extern void SetShieldParachutePhysics( size_t ID );
extern void SetRailgunPhysics( size_t ID );

extern void SetObjectRotateType( size_t ID, size_t Type );
extern void SetObjectThinkCounter( size_t ID, size_t CounterIndex, size_t Value );

extern void CollideParticleWithObjects( ParticleSystem_t *System, Particle_t *Particle );

extern void SetObjectBlendFactor( size_t ID, float Blend );
extern float GetObjectBlendFactor( size_t ID );

extern void SetObjectScale( size_t ID, size_t PieceIndex, Vector_t *Point );
extern void GetObjectScale( size_t ID, size_t PieceIndex, Vector_t *Point );

extern void SetObjectBaseColor( size_t ID, Color_t *Color );
extern void GetObjectBaseColor( size_t ID, Color_t *Color );

extern void CalcObjectPointColors2( size_t ID );

extern void GetObjectRotationDeltas( size_t ID, Vector_t *RotationDeltas );
extern void SetObjectRotationDeltas( size_t ID, Vector_t *RotationDeltas );

extern size_t IsObjectOnGround2( size_t ID );
extern void ClearObjectPhysics( size_t ID );
