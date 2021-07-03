//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 2 April 2002
//-----------------------------------------------------------------------------


#ifndef _VECTOR_H_
#define _VECTOR_H_


#define PI                  3.1415926535897932384626433832795f

#define RTOD                57.295779513082320876798154814105f
#define DTOR                0.017453292519943295769236907684886f
#define DTOR_HALF           0.008726646259971647884618453842443f

#define _030DEGASRADS       0.52359877559829887307710723054658f
#define _045DEGASRADS       0.78539816339744830961566084581988f
#define _070DEGASRADS       1.221730476396030703846583537942f
#define _090DEGASRADS       1.5707963267948966192313216916398f
#define _135DEGASRADS       2.3561944901923449288469825374596f
#define _180DEGASRADS       3.1415926535897932384626433832795f
#define _270DEGASRADS       4.7123889803846898576939650749193f
#define _315DEGASRADS       5.4977871437821381673096259207391f
#define _360DEGASRADS       6.283185307179586476925286766559f

#define EPSILON             0.001f

#define INFRONT             0
#define BEHIND              1
#define ONPLANE             2

#define _Decrease( _speed, _amount )            \
    if( _speed < -EPSILON )                     \
    {                                           \
        _speed += _amount;                      \
        if( _speed >= -EPSILON )                \
            _speed = 0.0f;                      \
    }                                           \
    else if( _speed > EPSILON )                 \
    {                                           \
        _speed -= _amount;                      \
        if( _speed <= EPSILON )                 \
            _speed = 0.0f;                      \
    }                                           \
    else                                        \
        _speed = 0.0f;



//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

struct Vector_t
{
    Vector_t(float Value = 0.0f)
        : X(Value), Y(Value), Z(Value) {}

    Vector_t(float X_, float Y_, float Z_)
        : X(X_), Y(Y_), Z(Z_) {}

    Vector_t(const Vector_t& Other)
        : X(Other.X), Y(Other.Y), Z(Other.Z) {}

    void operator=(const Vector_t& Other)
    {
        X = Other.X, Y = Other.Y, Z = Other.Z;
    }

    bool operator==(const Vector_t& Other) const
    {
        return X == Other.X && Y == Other.Y && Z == Other.Z;
    }

    float X, Y, Z;
};

typedef struct Point2D_s
{
    float X, Y;
}
Point2D_t;

typedef struct Polygon_s
{
    size_t P1, P2, P3;
    Vector_t Normal;
    size_t TextureNumber;
}
Polygon_t;

typedef struct Plane_s
{
    Vector_t Normal;
    Vector_t Point;
}
Plane_t;

typedef struct Plane2_s
{
    Vector_t Normal;
    float Distance;
}
Plane2_t;

typedef struct Polygon2_s
{
    Vector_t Points[ 3 ];
    Vector_t Normal;
}
Polygon2_t;

typedef struct TwoPoints_s
{
    Vector_t Point1;
    Vector_t Point2;
    Vector_t Point3;
}
TwoPoints_t;


//-----------------------------------------------------------------------------
// public macros
//-----------------------------------------------------------------------------

#define Min( _f1, _f2 )                         ( (_f1) < (_f2) ? (_f1) : (_f2) )
#define Max( _f1, _f2 )                         ( (_f1) > (_f2) ? (_f1) : (_f2) )

//#define fabsf( _f )                             ( (_f) < 0.0f ? -(_f) : (_f) )

#define VectorDotProd( V1, V2 )                 ( V1.X * V2.X + V1.Y * V2.Y + V1.Z * V2.Z )
#define VectorCrossProd( V1, V2, Out )          { Out.X = V1.Y * V2.Z - V1.Z * V2.Y; Out.Y = V1.Z * V2.X - V1.X * V2.Z; Out.Z = V1.X * V2.Y - V1.Y * V2.X; }

#define VectorSubtract( V1, V2, Out )           { Out.X = V1.X - V2.X; Out.Y = V1.Y - V2.Y; Out.Z = V1.Z - V2.Z; }
#define VectorAdd( V1, V2, Out )                { Out.X = V1.X + V2.X; Out.Y = V1.Y + V2.Y; Out.Z = V1.Z + V2.Z; }
#define VectorDivide( V, F, Out )               { Out.X = V.X / F; Out.Y = V.Y / F; Out.Z = V.Z / F; }
#define VectorCopy( Src, Dst )                  { Dst.X = Src.X; Dst.Y = Src.Y; Dst.Z = Src.Z; }
#define VectorCopyInverse( Src, Dst )           { Dst.X = -Src.X; Dst.Y = -Src.Y; Dst.Z = -Src.Z; }
#define VectorScale( V, Scale, Out )            { Out.X = (Scale) * V.X; Out.Y = (Scale) * V.Y; Out.Z = (Scale) * V.Z; }
#define VectorInverse( V )                      { V.X = -V.X; V.Y = -V.Y; V.Z = -V.Z; }
#define VectorInverse2( V, Out )                { Out.X = -V.X; Out.Y = -V.Y; Out.Z = -V.Z; }
#define VectorSet( V, F )                       { V.X = (F); V.Y = (F); V.Z = (F); }

#define VectorFuzzyCompare( V1, V2, Tolerance ) ( ( V1.X >= ( V2.X - (Tolerance) ) && V1.X <= ( V2.X + (Tolerance) ) ) &&       \
                                                  ( V1.Y >= ( V2.Y - (Tolerance) ) && V1.Y <= ( V2.Y + (Tolerance) ) ) &&       \
                                                  ( V1.Z >= ( V2.Z - (Tolerance) ) && V1.Z <= ( V2.Z + (Tolerance) ) ) )

#define VectorDecrease( V, F )                  { _Decrease( V.X, F ); _Decrease( V.Y, F ); _Decrease( V.Z, F ); }


//-----------------------------------------------------------------------------
// public Functions
//-----------------------------------------------------------------------------

size_t VectorCompare(Vector_t* V1, Vector_t* V2);
void VectorReflect(Vector_t* ReflectMe, Vector_t* UnitVec, Vector_t* Out);

float VectorMagnitude(Vector_t* V);
float VectorMagnitude2(Vector_t* P1, Vector_t* P2);
float VectorDotProdAngle(Vector_t* V1, Vector_t* V2);

void VectorNormalise(Vector_t* V);
void VectorNormalisedCrossProd(Vector_t* V1, Vector_t* V2, Vector_t* Out);

float MathPlaneEquation(Vector_t* OnPlane, Vector_t* Normal, Vector_t* Test);

float MathLinePlaneIntersect(Vector_t* OnPlane,
                             Vector_t* Normal,
                             Vector_t* StartPoint,
                             Vector_t* FinishPoint,
                             Vector_t* Intersect);

size_t MathLineTriangleIntersect(Vector_t* Triangle1,
                                 Vector_t* Triangle2,
                                 Vector_t* Triangle3,
                                 Vector_t* TriangleNormal,
                                 Vector_t* LineSegment_1,
                                 Vector_t* LineSegment_2);

size_t MathLineSphereIntersect(Vector_t* SphereCenter
                               , float SphereRadius
                               , Vector_t* LineSegment1
                               , Vector_t* LineSegment2
                               , Vector_t* Intersect1
                               , Vector_t* Intersect2);

size_t MathDistanceBetweenPointAndLine(Vector_t* Point,
                                       Vector_t* LineSegment1,
                                       Vector_t* LineSegment2,
                                       float* Distance);

bool MathPointAndLineIntersection(const Vector_t& Point,
                                  const Vector_t& LineSegment1,
                                  const Vector_t& LineSegment2,
                                  Vector_t& Intersection,
                                  float* Percentage = nullptr);

// each of these functions returns a unit vector pointing in the direction
// of the given rotations.  rotations must be radians.
void VectorGetMoveXY(float XRotation, float YRotation, Vector_t* Move);
void VectorGetMoveXZ(float XRotation, float ZRotation, Vector_t* Move);
void VectorGetMoveYX(float YRotation, float XRotation, Vector_t* Move);
void VectorGetMoveYZ(float YRotation, float ZRotation, Vector_t* Move);
void VectorGetMoveZX(float ZRotation, float XRotation, Vector_t* Move);
void VectorGetMoveZY(float ZRotation, float YRotation, Vector_t* Move);
void VectorGetMoveXYZ(float XRotation, float YRotation, float ZRotation, Vector_t* Move);
void VectorGetMoveXZY(float XRotation, float ZRotation, float YRotation, Vector_t* Move);
void VectorGetMoveYXZ(float YRotation, float XRotation, float ZRotation, Vector_t* Move);
void VectorGetMoveYZX(float YRotation, float ZRotation, float XRotation, Vector_t* Move);
void VectorGetMoveZXY(float ZRotation, float XRotation, float YRotation, Vector_t* Move);
void VectorGetMoveZYX(float ZRotation, float YRotation, float XRotation, Vector_t* Move);

void VectorRotateX(float XRotation, Vector_t* Move);
void VectorRotateY(float YRotation, Vector_t* Move);
void VectorRotateZ(float ZRotation, Vector_t* Move);

// x rotation will be from -90 ->  90 inclusive
// y rotation will be from   0 -> 359 inclusive
void VectorGetAnglesXY(Vector_t* UnitVector, float* XRotation, float* YRotation);

size_t VectorClassifyPoint(Vector_t* Point, Plane_t* Plane);

size_t VectorGetIntersect(Vector_t* LineStart, Vector_t* LineFinish, Vector_t* PointOnPlane
                          , Vector_t* PlaneNormal, Vector_t* Intersection, float* Percentage);

size_t VectorIsAABBInFrustum(Plane2_t* ViewFrustum
                             , float MinX, float MinY, float MinZ
                             , float MaxX, float MaxY, float MaxZ);

size_t VectorIsSphereInFrustum(Plane2_t* ViewFrustum, Vector_t* SphereCenter
                               , float SphereRadius);

size_t ClassifyPoint(Vector_t* Point, Plane_t* Plane);


void DirectionToBasis(const Vector_t& From, const Vector_t& To, Vector_t& Right, Vector_t& Up, Vector_t& Forward);
void DirectionToBasis(const Vector_t& Direction, Vector_t& Right, Vector_t& Up, Vector_t& Forward);

Vector_t RotatePointAroundVector(const Vector_t& RotateMe, const Vector_t& RotateAroundMe, float AngleDegrees);


#endif
