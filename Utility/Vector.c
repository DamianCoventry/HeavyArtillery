//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 2 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "Vector.h"

#include <math.h>
#include <string.h>


//-----------------------------------------------------------------------------
// private macros
//-----------------------------------------------------------------------------

#define Square( num_ )          ( (num_) * (num_) )


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static float VectorGetHeading(float Opp, float Adj);


//-----------------------------------------------------------------------------
// Name: VectorMagnitude()
// Desc:
//-----------------------------------------------------------------------------

float VectorMagnitude(Vector_t* V)
{
    return (float)sqrt(V->X * V->X + V->Y * V->Y + V->Z * V->Z);
}


//-----------------------------------------------------------------------------
// Name: VectorMagnitude2()
// Desc:
//-----------------------------------------------------------------------------

float VectorMagnitude2(Vector_t* p1, Vector_t* p2)
{
    Vector_t V;

    V.X = p2->X - p1->X;
    V.Y = p2->Y - p1->Y;
    V.Z = p2->Z - p1->Z;

    return (float)sqrt(V.X * V.X + V.Y * V.Y + V.Z * V.Z);
}


//-----------------------------------------------------------------------------
// Name: VectorDotProdAngle()
// Desc:
//-----------------------------------------------------------------------------

float VectorDotProdAngle(Vector_t* V1, Vector_t* V2)
{
    // re-arranged form Of the Dot prod:
    //  Ang = Cos-1 * ( A · B ) / ( |a| * |b| )
    return (float)acos(((V1->X * V2->X) + (V1->Y * V2->Y) + (V1->Z * V2->Z)) /
                       (VectorMagnitude(V1) * VectorMagnitude(V2)));
}


//-----------------------------------------------------------------------------
// Name: VectorNormalise()
// Desc:
//-----------------------------------------------------------------------------

void VectorNormalise(Vector_t* V)
{
    float D;

    D = (float)sqrt(V->X * V->X + V->Y * V->Y + V->Z * V->Z);

    if (0.0f == D)
        D = 0.00000001f;

    V->X /= D;
    V->Y /= D;
    V->Z /= D;
}


//-----------------------------------------------------------------------------
// Name: VectorNormalisedCrossProd()
// Desc:
//-----------------------------------------------------------------------------

void VectorNormalisedCrossProd(Vector_t* V1, Vector_t* V2, Vector_t* Out)
{
    float D;

    // Cross prod
    Out->X = V1->Y * V2->Z - V1->Z * V2->Y;
    Out->Y = V1->Z * V2->X - V1->X * V2->Z;
    Out->Z = V1->X * V2->Y - V1->Y * V2->X;

    // Normalise it
    D = (float)sqrt(Out->X * Out->X + Out->Y * Out->Y + Out->Z * Out->Z);

    if (0.0f == D)
        D = 0.00000001f;

    Out->X /= D;
    Out->Y /= D;
    Out->Z /= D;
}


//-----------------------------------------------------------------------------
// Name: Vectorcompare()
// Desc:
//-----------------------------------------------------------------------------

size_t VectorCompare(Vector_t* V1, Vector_t* V2)
{
    float Result = (float)fabs((V1->X - V2->X) + (V1->Y - V2->Y) + (V1->Z - V2->Z));
    return (Result < 0.1f);
}


//-----------------------------------------------------------------------------
// Name: VectorReflect()
// Desc:
//-----------------------------------------------------------------------------

void VectorReflect(Vector_t* ReflectMe, Vector_t* UnitVec, Vector_t* Out)
{
    float Dot;
    Vector_t V;

    VectorCopy((*ReflectMe), V);
    VectorInverse(V);

    Dot = VectorDotProd((*UnitVec), V);

    Out->X = 2.0f * UnitVec->X * Dot - V.X;
    Out->Y = 2.0f * UnitVec->Y * Dot - V.Y;
    Out->Z = 2.0f * UnitVec->Z * Dot - V.Z;
}


//-----------------------------------------------------------------------------
// Name: Vectorangles()
// Desc:
//-----------------------------------------------------------------------------

void Vectorangles(Vector_t* V, float* XRotation, float* YRotation, float* ZRotation)
{
    if (XRotation)
        (*XRotation) = VectorGetHeading(V->Y, V->Z);

    if (YRotation)
        (*YRotation) = VectorGetHeading(V->X, V->Z);

    if (ZRotation)
        (*ZRotation) = VectorGetHeading(V->X, V->Y);
}


//-----------------------------------------------------------------------------
// Name: MathPlaneEquation()
// Desc:
//-----------------------------------------------------------------------------

float MathPlaneEquation(Vector_t* OnPlane, Vector_t* Normal, Vector_t* Test)
{
    float D;

    // get the D Component for the plane equation
    D = -(OnPlane->X * Normal->X + OnPlane->Y * Normal->Y + OnPlane->Z * Normal->Z);

    // plane equation Ax + By + Cz + D = 0
    return Normal->X * Test->X + Normal->Y * Test->Y + Normal->Z * Test->Z + D;
}


//-----------------------------------------------------------------------------
// Name: MathLinePlaneIntersect()
// Desc:
//-----------------------------------------------------------------------------

float MathLinePlaneIntersect(Vector_t* OnPlane
                             , Vector_t* Normal
                             , Vector_t* StartPoint
                             , Vector_t* FinishPoint
                             , Vector_t* Intersect)
{
    float D, Denom;
    float Percent;
    float Ret;

    // get the d component for the plane equation
    D = -(OnPlane->X * Normal->X + OnPlane->Y * Normal->Y + OnPlane->Z * Normal->Z);

    // calculate the position on the line that intersects the plane
    Denom = Normal->X * (FinishPoint->X - StartPoint->X) +
            Normal->Y * (FinishPoint->Y - StartPoint->Y) +
            Normal->Z * (FinishPoint->Z - StartPoint->Z);

    Ret = (float)fabs(Denom);
    if (Ret >= -EPSILON && Ret <= EPSILON)
        return 0.0f;    // line is parallel to plane

    Percent = -(D + Normal->X * StartPoint->X + Normal->Y * StartPoint->Y + Normal->Z * StartPoint->Z) / Denom;

    // get the exact coords of the intersection
    Intersect->X = StartPoint->X + Percent * (FinishPoint->X - StartPoint->X);
    Intersect->Y = StartPoint->Y + Percent * (FinishPoint->Y - StartPoint->Y);
    Intersect->Z = StartPoint->Z + Percent * (FinishPoint->Z - StartPoint->Z);

    return Percent;
}


//-----------------------------------------------------------------------------
// Name: MathLineTriangleIntersect()
// Desc:
//-----------------------------------------------------------------------------

size_t MathLineTriangleIntersect(Vector_t* Triangle1,
                                 Vector_t* Triangle2,
                                 Vector_t* Triangle3,
                                 Vector_t* TriangleNormal,
                                 Vector_t* LineSegment1,
                                 Vector_t* LineSegment2)
{
    float Side1, Side2;
    size_t Behind, OnPlane, Infront;
    Vector_t V1, V2, Normal;

    Side1 = MathPlaneEquation(Triangle1, TriangleNormal, LineSegment1);
    Side2 = MathPlaneEquation(Triangle1, TriangleNormal, LineSegment2);

    if (Side1 < EPSILON && Side2 < EPSILON)
        return 0;
    if (Side1 > EPSILON && Side2 > EPSILON)
        return 0;

    Behind  = 0;
    OnPlane = 0;
    Infront = 0;

    V1.X = Triangle2->X - Triangle1->X;
    V1.Y = Triangle2->Y - Triangle1->Y;
    V1.Z = Triangle2->Z - Triangle1->Z;
    V2.X = LineSegment1->X - Triangle1->X;
    V2.Y = LineSegment1->Y - Triangle1->Y;
    V2.Z = LineSegment1->Z - Triangle1->Z;
    VectorNormalisedCrossProd(&V1, &V2, &Normal);
    Side1 = MathPlaneEquation(LineSegment1, &Normal, LineSegment2);
    if (Side1 < EPSILON)
        Behind++;
    else if (Side1 > EPSILON)
        Infront++;
    else
        OnPlane++;

    V1.X = Triangle3->X - Triangle2->X;
    V1.Y = Triangle3->Y - Triangle2->Y;
    V1.Z = Triangle3->Z - Triangle2->Z;
    V2.X = LineSegment1->X - Triangle2->X;
    V2.Y = LineSegment1->Y - Triangle2->Y;
    V2.Z = LineSegment1->Z - Triangle2->Z;
    VectorNormalisedCrossProd(&V1, &V2, &Normal);
    Side1 = MathPlaneEquation(LineSegment1, &Normal, LineSegment2);
    if (Side1 < EPSILON)
        Behind++;
    else if (Side1 > EPSILON)
        Infront++;
    else
        OnPlane++;

    V1.X = Triangle1->X - Triangle3->X;
    V1.Y = Triangle1->Y - Triangle3->Y;
    V1.Z = Triangle1->Z - Triangle3->Z;
    V2.X = LineSegment1->X - Triangle3->X;
    V2.Y = LineSegment1->Y - Triangle3->Y;
    V2.Z = LineSegment1->Z - Triangle3->Z;
    VectorNormalisedCrossProd(&V1, &V2, &Normal);
    Side1 = MathPlaneEquation(LineSegment1, &Normal, LineSegment2);
    if (Side1 < EPSILON)
        Behind++;
    else if (Side1 > EPSILON)
        Infront++;
    else
        OnPlane++;

    if (OnPlane > 0)
        return 1;

    if (3 == Behind || 3 == Infront)
        return 1;

    return 0;
}


//-----------------------------------------------------------------------------
// Name: MathLineSphereIntersect()
// Desc:
//-----------------------------------------------------------------------------

size_t MathLineSphereIntersect(Vector_t* SphereCenter
                               , float SphereRadius
                               , Vector_t* LineSegment1
                               , Vector_t* LineSegment2
                               , Vector_t* Intersect1
                               , Vector_t* Intersect2)
{
    float A, B, C, MU, i;

    // first we'll check if the sphere's center is between the two points of the
    // line segment
    A = (SphereCenter->X - LineSegment1->X) * (LineSegment2->X - LineSegment1->X) +
        (SphereCenter->Y - LineSegment1->Y) * (LineSegment2->Y - LineSegment1->Y) +
        (SphereCenter->Z - LineSegment1->Z) * (LineSegment2->Z - LineSegment1->Z);

    B = (LineSegment2->X - LineSegment1->X) * (LineSegment2->X - LineSegment1->X) +
        (LineSegment2->Y - LineSegment1->Y) * (LineSegment2->Y - LineSegment1->Y) +
        (LineSegment2->Z - LineSegment1->Z) * (LineSegment2->Z - LineSegment1->Z);

    MU = A / B;

    if (MU < -EPSILON || MU > 1.0f + EPSILON)
        return 0;       // closest point from sphere center to line is NOT between the line segment's end points

    // continue with testing the line (not the line segment)
    A = Square(LineSegment2->X - LineSegment1->X) +
        Square(LineSegment2->Y - LineSegment1->Y) +
        Square(LineSegment2->Z - LineSegment1->Z);

    B = 2.0f * ((LineSegment2->X - LineSegment1->X) * (LineSegment1->X - SphereCenter->X)
                + (LineSegment2->Y - LineSegment1->Y) * (LineSegment1->Y - SphereCenter->Y)
                + (LineSegment2->Z - LineSegment1->Z) * (LineSegment1->Z - SphereCenter->Z)) ;

    C = Square(SphereCenter->X) + Square(SphereCenter->Y) + Square(SphereCenter->Z) +
        Square(LineSegment1->X) + Square(LineSegment1->Y) + Square(LineSegment1->Z) -
        2.0f * (SphereCenter->X * LineSegment1->X +
                SphereCenter->Y * LineSegment1->Y +
                SphereCenter->Z * LineSegment1->Z) - Square(SphereRadius);

    i = B * B - 4 * A * C ;

    if (i < -EPSILON)                       // no intersection
        return 0;

    if (i >= -EPSILON && i <= EPSILON)      // one intersection
    {
        if (Intersect1)
        {
            MU = -B / (2.0f * A);
            Intersect1->X = LineSegment1->X + MU * (LineSegment2->X - LineSegment1->X);
            Intersect1->Y = LineSegment1->Y + MU * (LineSegment2->Y - LineSegment1->Y);
            Intersect1->Z = LineSegment1->Z + MU * (LineSegment2->Z - LineSegment1->Z);
        }
        return 1;
    }

    if (i > EPSILON)                        // two intersections
    {
        if (Intersect1)
        {
            // first intersection
            MU = (-B + (float)sqrt(Square(B) - 4.0f * A * C)) / (2.0f * A);
            Intersect1->X = LineSegment1->X + MU * (LineSegment2->X - LineSegment1->X);
            Intersect1->Y = LineSegment1->Y + MU * (LineSegment2->Y - LineSegment1->Y);
            Intersect1->Z = LineSegment1->Z + MU * (LineSegment2->Z - LineSegment1->Z);
        }
        if (Intersect2)
        {
            // second intersection
            MU = (-B - (float)sqrt(Square(B) - 4.0f * A * C)) / (2.0f * A);
            Intersect2->X = LineSegment1->X + MU * (LineSegment2->X - LineSegment1->X);
            Intersect2->Y = LineSegment1->Y + MU * (LineSegment2->Y - LineSegment1->Y);
            Intersect2->Z = LineSegment1->Z + MU * (LineSegment2->Z - LineSegment1->Z);
        }
        return 2;
    }

    return 0;       // should never get here
}


//-----------------------------------------------------------------------------
// Name: MathGetAlignment()
// Desc:
//-----------------------------------------------------------------------------

void MathGetAlignment(Vector_t* Normal, float* XRotation, float* YRotation, float* ZRotation)
{
    Vector_t V;

    if (XRotation)
    {
        V.X = 1.0f; V.Y = 0.0f; V.Z = 0.0f;
        (*XRotation) = VectorDotProdAngle(&V, Normal);
    }

    if (YRotation)
    {
        V.X = 0.0f; V.Y = 1.0f; V.Z = 0.0f;
        (*YRotation) = VectorDotProdAngle(&V, Normal);
    }

    if (ZRotation)
    {
        V.X = 0.0f; V.Y = 0.0f; V.Z = 1.0f;
        (*ZRotation) = VectorDotProdAngle(&V, Normal);
    }
}


//-----------------------------------------------------------------------------
// Name: VectorGetHeading()
// Desc:
//-----------------------------------------------------------------------------

static float VectorGetHeading(float Opp, float Adj)
{
    float Ang;

    if (Adj == 0.0f)
        Adj = 0.00000001f;

    Ang = (float)fabs((float)atan(Opp / Adj));

    if (Adj > 0.0f)
    {
        if (Opp > 0.0f)
            Ang = _360DEGASRADS - Ang;
        //ang = _090DEGASRADS - Ang;
        else
            Ang = Ang;
        //ang = _090DEGASRADS + Ang;
    }
    else
    {
        if (Opp > 0.0f)
            Ang = _180DEGASRADS + Ang;
        //ang = _270DEGASRADS + Ang;
        else
        {
            Ang = _180DEGASRADS - Ang;
            //ang = _270DEGASRADS - Ang;
        }
    }

    return Ang;
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveXY()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveXY(float XRotation, float YRotation, Vector_t* Move)
{
    Move->X = (float)sin((float)YRotation);
    Move->Y = (float)cos((float)YRotation) * -(float)sin((float)XRotation);
    Move->Z = (float)cos((float)YRotation) * (float)cos((float)XRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveYX()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveYX(float YRotation, float XRotation, Vector_t* Move)
{
    Move->X = (float)cos((float)XRotation) * (float)sin((float)YRotation);
    Move->Y = -(float)sin((float)XRotation);
    Move->Z = (float)cos((float)XRotation) * (float)cos((float)YRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveXZ()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveXZ(float XRotation, float ZRotation, Vector_t* Move)
{
    Move->X =  0.0f;
    Move->Y = -(float)sin((float)XRotation);
    Move->Z = (float)cos((float)XRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveZX()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveZX(float ZRotation, float XRotation, Vector_t* Move)
{
    Move->X = -(float)sin((float)XRotation) * -(float)sin((float)ZRotation);
    Move->Y = -(float)sin((float)XRotation) * (float)cos((float)ZRotation);
    Move->Z = (float)cos((float)XRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveYZ()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveYZ(float YRotation, float ZRotation, Vector_t* Move)
{
    Move->X = (float)sin((float)YRotation);
    Move->Y = 0.0f;
    Move->Z = (float)cos((float)YRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveZY()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveZY(float ZRotation, float YRotation, Vector_t* Move)
{
    Move->X = (float)sin((float)YRotation) * (float)cos((float)ZRotation);
    Move->Y = (float)sin((float)YRotation) * (float)sin((float)ZRotation);
    Move->Z = (float)cos((float)YRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveXYZ()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveXYZ(float XRotation, float YRotation, float ZRotation, Vector_t* Move)
{
    Vector_t FirstMove;

    VectorGetMoveYZ(YRotation, ZRotation, &FirstMove);

    Move->X = FirstMove.X;
    Move->Y = FirstMove.Z * -(float)sin((float)XRotation);
    Move->Z = FirstMove.Z * (float)cos((float)XRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveXZY()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveXZY(float XRotation, float ZRotation, float YRotation, Vector_t* Move)
{
    Vector_t FirstMove;

    VectorGetMoveZY(ZRotation, YRotation, &FirstMove);

    Move->X = FirstMove.X;
    Move->Y = FirstMove.Y * (float)cos((float)XRotation) + FirstMove.Z * -(float)sin((float)XRotation);
    Move->Z = FirstMove.Y * (float)sin((float)XRotation) + FirstMove.Z * (float)cos((float)XRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveYXZ()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveYXZ(float YRotation, float XRotation, float ZRotation, Vector_t* Move)
{
    Vector_t FirstMove;

    VectorGetMoveXZ(XRotation, ZRotation, &FirstMove);

    Move->X = FirstMove.Z * (float)sin((float)YRotation);
    Move->Y = FirstMove.Y;
    Move->Z = FirstMove.Z * (float)cos((float)YRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveYZX()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveYZX(float YRotation, float ZRotation, float XRotation, Vector_t* Move)
{
    Vector_t FirstMove;

    VectorGetMoveZX(ZRotation, XRotation, &FirstMove);

    Move->X = FirstMove.X * (float)cos((float)YRotation) + FirstMove.Z * (float)sin((float)YRotation);
    Move->Y = FirstMove.Y;
    Move->Z = FirstMove.X * -(float)sin((float)YRotation) + FirstMove.Z * (float)cos((float)YRotation);
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveZXY()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveZXY(float ZRotation, float XRotation, float YRotation, Vector_t* Move)
{
    Vector_t FirstMove;

    VectorGetMoveXY(XRotation, YRotation, &FirstMove);

    Move->X = FirstMove.X * (float)cos((float)ZRotation) + FirstMove.Y * -(float)sin((float)ZRotation);
    Move->Y = FirstMove.X * (float)sin((float)ZRotation) + FirstMove.Y * (float)cos((float)ZRotation);
    Move->Z = FirstMove.Z;
}


//-----------------------------------------------------------------------------
// Name: VectorGetMoveZYX()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetMoveZYX(float ZRotation, float YRotation, float XRotation, Vector_t* Move)
{
    Vector_t FirstMove;

    VectorGetMoveYX(YRotation, XRotation, &FirstMove);

    Move->X = FirstMove.X * (float)cos((float)ZRotation) + FirstMove.Y * -(float)sin((float)ZRotation);
    Move->Y = FirstMove.X * (float)sin((float)ZRotation) + FirstMove.Y * (float)cos((float)ZRotation);
    Move->Z = FirstMove.Z;
}


//-----------------------------------------------------------------------------
// Name: VectorRotateX()
// Desc:
//-----------------------------------------------------------------------------

void VectorRotateX(float XRotation, Vector_t* Move)
{
    Vector_t V;

    V.X = Move->X;
    V.Y = (float)cos(XRotation) * Move->Y - (float)sin(XRotation) * Move->Z;
    V.Z = (float)sin(XRotation) * Move->Y + (float)cos(XRotation) * Move->Z;

    memcpy(Move, &V, sizeof(Vector_t));
}


//-----------------------------------------------------------------------------
// Name: VectorRotateY()
// Desc:
//-----------------------------------------------------------------------------

void VectorRotateY(float YRotation, Vector_t* Move)
{
    Vector_t V;

    V.X = (float)cos(YRotation) * Move->X + (float)sin(YRotation) * Move->Z;
    V.Y = Move->Y;
    V.Z = -(float)sin(YRotation) * Move->X + (float)cos(YRotation) * Move->Z;

    memcpy(Move, &V, sizeof(Vector_t));
}


//-----------------------------------------------------------------------------
// Name: VectorRotateZ()
// Desc:
//-----------------------------------------------------------------------------

void VectorRotateZ(float ZRotation, Vector_t* Move)
{
    Vector_t V;

    V.X = (float)cos(ZRotation) * Move->X - (float)sin(ZRotation) * Move->Y;
    V.Y = (float)sin(ZRotation) * Move->X + (float)cos(ZRotation) * Move->Y;
    V.Z = Move->Z;

    memcpy(Move, &V, sizeof(Vector_t));
}


//-----------------------------------------------------------------------------
// Name: VectorGetAnglesYX()
// Desc:
//-----------------------------------------------------------------------------

void VectorGetAnglesXY(Vector_t* UnitVector, float* XRotation, float* YRotation)
{
    if (XRotation)
        (*XRotation) = RTOD * -(float)sin(UnitVector->Y);

    if (YRotation)
    {
        if (UnitVector->X >= 0.0f)
        {
            if (UnitVector->Z >= 0.0f)
            {
                if (0.0f == UnitVector->Z)      // prevent DBZ
                    (*YRotation) = 90.0f;
                else
                    (*YRotation) = RTOD * (float)atan(UnitVector->X / UnitVector->Z);
            }
            else
            {
                if (0.0f == UnitVector->X)      // prevent DBZ
                    (*YRotation) = 0.0f;
                else
                    (*YRotation) =  90.0f - (RTOD * (float)atan(UnitVector->Z / UnitVector->X));
            }
        }
        else
        {
            if (UnitVector->Z >= 0.0f)
            {
                if (0.0f == UnitVector->Z)      // prevent DBZ
                    (*YRotation) = -90.0f;
                else
                    (*YRotation) = RTOD * (float)atan(UnitVector->X / UnitVector->Z);
            }
            else
            {
                if (0.0f == UnitVector->X)      // prevent DBZ
                    (*YRotation) = 180.0f;
                else
                    (*YRotation) = 270.0f - (RTOD * (float)atan(UnitVector->Z / UnitVector->X));
            }
        }

        /*
                if( UnitVector->X > 0.0f )
                {
                    if( UnitVector->Z > 0.0f )
                    {
                        (*YRotation) = 90.0f + ( RTOD * (float)atan( UnitVector->X / UnitVector->Z ) );
                    }
                    else
                    {
                        // atan() should return a -ve number here
                        (*YRotation) = 90.0f + ( RTOD * (float)atan( UnitVector->Z / UnitVector->X ) );
                    }
                }
                else
                {
                    if( UnitVector->Z > 0.0f )
                    {
                        // atan() should return a -ve number here
                        (*YRotation) = 270.0f + ( RTOD * (float)atan( UnitVector->X / UnitVector->Z ) );
                    }
                    else
                    {
                        (*YRotation) = 270.0f + ( RTOD * (float)atan( UnitVector->Z / UnitVector->X ) );
                    }
                }
        */
    }
}


//-----------------------------------------------------------------------------
// Name: VectorClassifyPoint()
// Desc:
//-----------------------------------------------------------------------------

size_t VectorClassifyPoint(Vector_t* Point, Plane_t* Plane)
{
    float Dot;
    Vector_t V;

    VectorSubtract(Plane->Point, (*Point), V);
    Dot = VectorDotProd(V, Plane->Normal);

    if (Dot < -EPSILON)
        return INFRONT;

    if (Dot > EPSILON)
        return BEHIND;

    return ONPLANE;
}


//-----------------------------------------------------------------------------
// Name: VectorGetIntersect()
// Desc:
//-----------------------------------------------------------------------------

size_t VectorGetIntersect(Vector_t* LineStart, Vector_t* LineFinish, Vector_t* PointOnPlane
                          , Vector_t* PlaneNormal, Vector_t* Intersection, float* Percentage)
{
    Vector_t Direction;
    Vector_t Point;
    float LineLength;
    float Distance;

    Direction.X = LineFinish->X - LineStart->X;
    Direction.Y = LineFinish->Y - LineStart->Y;
    Direction.Z = LineFinish->Z - LineStart->Z;

    LineLength = VectorDotProd(Direction, (*PlaneNormal));
    if ((float)fabs(LineLength) < EPSILON)
        return 0;

    Point.X = PointOnPlane->X - LineStart->X;
    Point.Y = PointOnPlane->Y - LineStart->Y;
    Point.Z = PointOnPlane->Z - LineStart->Z;

    Distance = VectorDotProd(Point, (*PlaneNormal));

    if (Percentage)
    {
        (*Percentage) = Distance / LineLength;

        // does the line segment actually cross the plane?
        if ((*Percentage) < 0.0f || (*Percentage) > 1.0f)
            return 0;

        if (Intersection)
        {
            // add the percentage of the line to line start
            Intersection->X = LineStart->X + Direction.X * (*Percentage);
            Intersection->Y = LineStart->Y + Direction.Y * (*Percentage);
            Intersection->Z = LineStart->Z + Direction.Z * (*Percentage);
        }
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: VectorIsAABBInFrustum()
// Desc:
//-----------------------------------------------------------------------------

size_t VectorIsAABBInFrustum(Plane2_t* ViewFrustum
                             , float MinX, float MinY, float MinZ
                             , float MaxX, float MaxY, float MaxZ)
{
    Vector_t NearPoint;
    Plane2_t* Plane;
    size_t i;

    Plane = ViewFrustum;

    for (i = 0; i < 6; i++)
    {
        if (Plane->Normal.X > 0.0f)
        {
            if (Plane->Normal.Y > 0.0f)
            {
                if (Plane->Normal.Z > 0.0f)
                {
                    NearPoint.X = MinX; NearPoint.Y = MinY; NearPoint.Z = MinZ;
                }
                else
                {
                    NearPoint.X = MinX; NearPoint.Y = MinY; NearPoint.Z = MaxZ;
                }
            }
            else
            {
                if (Plane->Normal.Z > 0.0f)
                {
                    NearPoint.X = MinX; NearPoint.Y = MaxY; NearPoint.Z = MinZ;
                }
                else
                {
                    NearPoint.X = MinX; NearPoint.Y = MaxY; NearPoint.Z = MaxZ;
                }
            }
        }
        else
        {
            if (Plane->Normal.Y > 0.0f)
            {
                if (Plane->Normal.Z > 0.0f)
                {
                    NearPoint.X = MaxX; NearPoint.Y = MinY; NearPoint.Z = MinZ;
                }
                else
                {
                    NearPoint.X = MaxX; NearPoint.Y = MinY; NearPoint.Z = MaxZ;
                }
            }
            else
            {
                if (Plane->Normal.Z > 0.0f)
                {
                    NearPoint.X = MaxX; NearPoint.Y = MaxY; NearPoint.Z = MinZ;
                }
                else
                {
                    NearPoint.X = MaxX; NearPoint.Y = MaxY; NearPoint.Z = MaxZ;
                }
            }
        }

        // put the point thru the plane equation
        if (VectorDotProd(Plane->Normal, NearPoint) + Plane->Distance > 0.0f)
            return 0;

        Plane++;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: VectorIsSphereInFrustum()
// Desc:
//-----------------------------------------------------------------------------

size_t VectorIsSphereInFrustum(Plane2_t* ViewFrustum, Vector_t* SphereCenter
                               , float SphereRadius)
{
    size_t i;

    for (i = 0; i < 6; i++)
    {
        // put the point thru the plane equation
        if (VectorDotProd(ViewFrustum[ i ].Normal, (*SphereCenter)) + ViewFrustum[ i ].Distance > SphereRadius)
            return 0;
    }

    return 1;
}


//-----------------------------------------------------------------------------
// Name: MathDistanceBetweenPointAndLine()
// Desc:
//-----------------------------------------------------------------------------

size_t MathDistanceBetweenPointAndLine(Vector_t* Point,
                                       Vector_t* LineSegment1,
                                       Vector_t* LineSegment2,
                                       float* Distance)
{
    Vector_t Intersection;
    if (!MathPointAndLineIntersection(*Point, *LineSegment1, *LineSegment2, Intersection))
        return 0;

    if (Distance)
        (*Distance) = VectorMagnitude2(Point, &Intersection);

    return 1;
}


//-----------------------------------------------------------------------------
// Name: MathPointAndLineIntersection()
// Desc:
//-----------------------------------------------------------------------------

bool MathPointAndLineIntersection(const Vector_t& Point, const Vector_t& LineSegment1, const Vector_t& LineSegment2, Vector_t& Intersection, float* Percentage)
{
    float LineSegmentLength;
    float U;

    LineSegmentLength = VectorMagnitude2((Vector_t*)&LineSegment1, (Vector_t*)&LineSegment2);

    U = (((Point.X - LineSegment1.X) * (LineSegment2.X - LineSegment1.X)) +
         ((Point.Y - LineSegment1.Y) * (LineSegment2.Y - LineSegment1.Y)) +
         ((Point.Z - LineSegment1.Z) * (LineSegment2.Z - LineSegment1.Z))) /
        (LineSegmentLength * LineSegmentLength);

    if (Percentage)
        *Percentage = U;

    if (U < 0.0f || U > 1.0f)
        return false;   // closest point does not fall within the line segment

    Intersection.X = LineSegment1.X + U * (LineSegment2.X - LineSegment1.X);
    Intersection.Y = LineSegment1.Y + U * (LineSegment2.Y - LineSegment1.Y);
    Intersection.Z = LineSegment1.Z + U * (LineSegment2.Z - LineSegment1.Z);

    return true;
}


//-----------------------------------------------------------------------------
// Name: ClassifyPoint()
// Desc:
//-----------------------------------------------------------------------------

size_t ClassifyPoint(Vector_t* Point, Plane_t* Plane)
{
    float Dot;
    Vector_t V;

    VectorSubtract(Plane->Point, (*Point), V);
    Dot = VectorDotProd(V, Plane->Normal);

    if (Dot < -EPSILON) return INFRONT;
    if (Dot >  EPSILON) return BEHIND;
    return ONPLANE;
}


//-----------------------------------------------------------------------------
// Name: DirectionToBasis()
// Desc:
//-----------------------------------------------------------------------------

void DirectionToBasis(const Vector_t& From, const Vector_t& To, Vector_t& Right, Vector_t& Up, Vector_t& Forward)
{
    VectorSubtract(From, To, Forward);
    DirectionToBasis(Forward, Right, Up, Forward);
}


//-----------------------------------------------------------------------------
// Name: DirectionToBasis()
// Desc:
//-----------------------------------------------------------------------------
//
// Resetting the model, view and perpsective matrices puts the camera at the origin
// looking along the negative z axis. Therefore, for this use case, our basis is:
//
//  Right   = { 1.0f, 0.0f, 0.0f }
//  Up      = { 0.0f, 1.0f, 0.0f }
//  Forward = { 0.0f, 0.0f, -1.0f }
//
// Source: http://www.songho.ca/opengl/gl_projectionmatrix.html
//
//      "Note that the eye coordinates are defined in the right-handed coordinate
//       system, but NDC uses the left-handed coordinate system. That is, the camera
//       at the origin is looking along -Z axis in eye space, but it is looking
//       along +Z axis in NDC."
//
void DirectionToBasis(const Vector_t& Direction, Vector_t& Right, Vector_t& Up, Vector_t& Forward)
{
    Forward = Direction;
    VectorNormalise(&Forward);
    if (Forward == Vector_t{ -1.0f, 0.0f, 0.0f })
    {
        Right = { 0.0f, 0.0f, -1.0f };
        Up = { 0.0f, 1.0f, 0.0f };
    }
    else if (Forward == Vector_t{ 1.0f, 0.0f, 0.0f })
    {
        Right = { 0.0f, 0.0f, 1.0f };
        Up = { 0.0f, 1.0f, 0.0f };
    }
    else if (Forward == Vector_t{ 0.0f, -1.0f, 0.0f })
    {
        Right = { 1.0f, 0.0f, 0.0f };
        Up = { 0.0f, 0.0f, -1.0f };
    }
    else if (Forward == Vector_t{ 0.0f, 1.0f, 0.0f })
    {
        Right = { 1.0f, 0.0f, 0.0f };
        Up = { 0.0f, 0.0f, 1.0f };
    }
    else if (Forward == Vector_t{ 0.0f, 0.0f, -1.0f })
    {
        Right = { 1.0f, 0.0f, 0.0f };
        Up = { 0.0f, 1.0f, 0.0f };
    }
    else if (Forward == Vector_t{ 0.0f, 0.0f, 1.0f })
    {
        Right = { -1.0f, 0.0f, 0.0f };
        Up = { 0.0f, 1.0f, 0.0f };
    }
    else
    {
        Vector_t Temp{ 0.0f, 1.0f, 0.0f };
        VectorCrossProd(Forward, Temp, Right);
        VectorCrossProd(Forward, Right, Up);
    }
}

Vector_t RotatePointAroundVector(const Vector_t& RotateMe, const Vector_t& RotateAroundMe, float AngleDegrees)
{
    // Start by creating a 4x4 matrix out of the vector 'RotateAroundMe' and the angle 'AngleDegrees'.

    float MatrixRow0[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
    float MatrixRow1[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
    float MatrixRow2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float MatrixRow3[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    float AngleRadians = DTOR * AngleDegrees;
    const float Cosine = cosf(AngleRadians);
    const float Sine = sinf(AngleRadians);

    Vector_t Axis{ RotateAroundMe };
    VectorNormalise(&Axis);

    Vector_t Temp;
    Temp.X = (1.0f - Cosine) * Axis.X;
    Temp.Y = (1.0f - Cosine) * Axis.Y;
    Temp.Z = (1.0f - Cosine) * Axis.Z;

    float Rotation0[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
    float Rotation1[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
    float Rotation2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float Rotation3[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    Rotation0[0] = Cosine + Temp.X * Axis.X;
    Rotation0[1] = Temp.X * Axis.Y + Sine * Axis.Z;
    Rotation0[2] = Temp.X * Axis.Z - Sine * Axis.Y;

    Rotation1[0] = Temp.Y * Axis.X - Sine * Axis.Z;
    Rotation1[1] = Cosine + Temp.Y * Axis.Y;
    Rotation1[2] = Temp.Y * Axis.Z + Sine * Axis.X;

    Rotation2[0] = Temp.Z * Axis.X + Sine * Axis.Y;
    Rotation2[1] = Temp.Z * Axis.Y - Sine * Axis.X;
    Rotation2[2] = Cosine + Temp.Z * Axis.Z;

    float FinalMatrix0[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
    float FinalMatrix1[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
    float FinalMatrix2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float FinalMatrix3[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    FinalMatrix0[0] = (MatrixRow0[0] * Rotation0[0]) + (MatrixRow1[0] * Rotation0[1]) + (MatrixRow2[0] * Rotation0[2]);
    FinalMatrix0[1] = (MatrixRow0[1] * Rotation0[0]) + (MatrixRow1[1] * Rotation0[1]) + (MatrixRow2[1] * Rotation0[2]);
    FinalMatrix0[2] = (MatrixRow0[2] * Rotation0[0]) + (MatrixRow1[2] * Rotation0[1]) + (MatrixRow2[2] * Rotation0[2]);

    FinalMatrix1[0] = (MatrixRow0[0] * Rotation1[0]) + (MatrixRow1[0] * Rotation1[1]) + (MatrixRow2[0] * Rotation1[2]);
    FinalMatrix1[1] = (MatrixRow0[1] * Rotation1[0]) + (MatrixRow1[1] * Rotation1[1]) + (MatrixRow2[1] * Rotation1[2]);
    FinalMatrix1[2] = (MatrixRow0[2] * Rotation1[0]) + (MatrixRow1[2] * Rotation1[1]) + (MatrixRow2[2] * Rotation1[2]);

    FinalMatrix2[0] = (MatrixRow0[0] * Rotation2[0]) + (MatrixRow1[0] * Rotation2[1]) + (MatrixRow2[0] * Rotation2[2]);
    FinalMatrix2[1] = (MatrixRow0[1] * Rotation2[0]) + (MatrixRow1[1] * Rotation2[1]) + (MatrixRow2[1] * Rotation2[2]);
    FinalMatrix2[2] = (MatrixRow0[2] * Rotation2[0]) + (MatrixRow1[2] * Rotation2[1]) + (MatrixRow2[2] * Rotation2[2]);

    FinalMatrix3[0] = (MatrixRow0[0] * Rotation3[0]) + (MatrixRow1[0] * Rotation3[1]) + (MatrixRow2[0] * Rotation3[2]);
    FinalMatrix3[1] = (MatrixRow0[1] * Rotation3[0]) + (MatrixRow1[1] * Rotation3[1]) + (MatrixRow2[1] * Rotation3[2]);
    FinalMatrix3[2] = (MatrixRow0[2] * Rotation3[0]) + (MatrixRow1[2] * Rotation3[1]) + (MatrixRow2[2] * Rotation3[2]);

    // Now that we've created the 4x4 matrix 'FinalMatrix', multiply the vector 'RotateMe' with it

    Vector_t RotatedPoint
    {
        (FinalMatrix0[0] * RotateMe.X) + (FinalMatrix1[0] * RotateMe.Y) + (FinalMatrix2[0] * RotateMe.Z) + FinalMatrix3[0],
        (FinalMatrix0[1] * RotateMe.X) + (FinalMatrix1[1] * RotateMe.Y) + (FinalMatrix2[1] * RotateMe.Z) + FinalMatrix3[1],
        (FinalMatrix0[2] * RotateMe.X) + (FinalMatrix1[2] * RotateMe.Y) + (FinalMatrix2[2] * RotateMe.Z) + FinalMatrix3[2]
    };

    return RotatedPoint;
}
