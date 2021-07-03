//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Friday, 28 June 2002
//
//========================================================================================


#ifndef _OBJECT_H_
#define _OBJECT_H_

#define OBJECT_MAXLAYERS        4

#include "Vector.h"
#include "ParticleTypes.h"


//----------------------------------------------------------------------------------------
// public flags
//----------------------------------------------------------------------------------------

#define OBJECT_CANBEDRAWN           0x00000001
#define OBJECT_NEEDSPHYSICS         0x00000002
#define OBJECT_CANCOLLIDEWITHTANK   0x00000004
#define OBJECT_ISTANK               0x00000008

#define OBJECTSTATE_DEAD            0
#define OBJECTSTATE_DYING           1
#define OBJECTSTATE_ALIVE           2
#define OBJECTSTATE_INACTIVE        3

#define PHYSICS_IGNOREGRAVITY       0x00000001
#define PHYSICS_IGNOREFRICTION      0x00000002
#define PHYSICS_CLIPTOWORLD         0x00000004
#define PHYSICS_ONGROUNDBEFOREMOVE  0x00000008
#define PHYSICS_ONGROUNDAFTERMOVE   0x00000010
#define PHYSICS_HASCOLLIDED         0x00000020

#define DEFAULT_GRAVITY             0.050f
#define DEFAULT_FRICTIONAIR         0.005f
#define DEFAULT_FRICTIONGROUND      0.075f
#define DEFAULT_MASS                1.000f
#define DEFAULT_BOUNCEREDUCTION     0.850f


//----------------------------------------------------------------------------------------
// public types
//----------------------------------------------------------------------------------------

typedef enum BBoxType_s
{
    BBOX_SPHERE,
    BBOX_CYLINDER,
    BBOX_BOX
}
BBoxType_t;

typedef struct BBoxSphere_s
{
    float Radius;
}
BBoxSphere_t;

typedef struct BBoxCylinder_s
{
    float Radius;
    float Height;
}
BBoxCylinder_t;

typedef struct BBoxBox_s
{
    Vector_t Min, Max;
}
BBoxBox_t;

#ifndef _COLOR_STRUCT_
#define _COLOR_STRUCT_

typedef struct Color_s
{
    float R, G, B, A;
}
Color_t;

#endif

struct BBox_s
{
    BBoxSphere_t Sphere;
    BBoxCylinder_t Cylinder;
    BBoxBox_t Box;
};

typedef struct Physics_s
{
    size_t Flags;

    float Mass;
    float Gravity;
    float BounceReduction;

    float GroundHeight;
    size_t PolyNormal;

    Vector_t Move;
    BBoxType_t BBoxType;

    BBox_s BBox;
}
Physics_t;

typedef struct Object_s
{
    size_t Used;
    size_t ID;

    size_t State;
    size_t ModelIDLoRes;
    size_t ModelIDHiRes;
    size_t RotateType;

    long Flags;
    size_t Data;

    float BlendFactor;

    Physics_t Physics;

    size_t ThinkCounter1, ThinkCounter2;
    size_t ThinkCounter3, ThinkCounter4;

    Vector_t RotationDeltas;

    Vector_t Scales[ OBJECT_MAXLAYERS ];
    Vector_t Positions[ OBJECT_MAXLAYERS ];
    Vector_t Rotations[ OBJECT_MAXLAYERS ];

    Color_t BaseColor;
    Color_t *PointColorsLoRes[ OBJECT_MAXLAYERS ];
    Color_t *PointColorsHiRes[ OBJECT_MAXLAYERS ];

    void (*Draw)( size_t ModelID, Vector_t *Positions, Vector_t *Rotations, Color_t *Colors[], size_t RotateType, float Ambient );
    void (*Draw2)( size_t ModelID, Vector_t *Positions, Vector_t *Rotations, Vector_t *Scales, Color_t *Colors, size_t RotateType, float Ambient );
    void (*CollideMesh)( struct Object_s *Self, size_t MeshID );

    void (*OnMove)( struct Object_s *Self );
    void (*OnThink)( struct Object_s *Self, size_t IsHiResModel );
    void (*OnCollideMesh)( struct Object_s *Self );
    void (*OnCollideObject)( struct Object_s *Self, struct Object_s *OtherObj );
    void (*OnCollideParticle)( struct Object_s *Self, Particle_t *Particle );
    void (*OnFallingDamage)( struct Object_s *Self );
}
Object_t;


#endif
