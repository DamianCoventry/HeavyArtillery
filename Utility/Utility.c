//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 2 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "Utility.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static char Text[ 1024 ];


//-----------------------------------------------------------------------------
// Name: CosineInterp()
// Desc:
//-----------------------------------------------------------------------------

float CosineInterp(float A, float B, float Percent)
{
    float Angle;
    float Value;

    Angle = Percent * PI;
    Value = (1.0f - (float)cos(Angle)) * 0.5f;

    return A * (1.0f - Value) + B * Value;
}
/*
double CosineInterpolate(
   double y1,double y2,
   double mu)
{
   double mu2;

   mu2 = (1-cos(mu*PI))/2;
   return(y1*(1-mu2)+y2*mu2);
}
*/
/*
double CubicInterpolate(
   double y0,double y1,
   double y2,double y3,
   double mu)
{
   double a0,a1,a2,a3,mu2;

   mu2 = mu*mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;

   return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}
*/


//-----------------------------------------------------------------------------
// Name: LinearInterp()
// Desc:
//-----------------------------------------------------------------------------

float LinearInterp(float A, float B, float Percent)
{
    return A + ((B - A) * Percent);
}


//-----------------------------------------------------------------------------
// Name: BezierInterp()
// Desc:
//-----------------------------------------------------------------------------

float BezierInterp(float V0, float V1, float V2, float Percent)
{
    float R = LinearInterp(V0, V1, Percent);
    R = LinearInterp(R, V1, Percent);
    return LinearInterp(R, V2, Percent);
}


//-----------------------------------------------------------------------------
// Name: QuadraticBezierInterp()
// Desc:
//-----------------------------------------------------------------------------

float QuadraticBezierInterp(float V0, float V1, float V2, float V3, float Percent)
{
    float R = LinearInterp(V0, V1, Percent);
    R = LinearInterp(R, V1, Percent);
    R = LinearInterp(R, V2, Percent);
    return LinearInterp(R, V3, Percent);
}


//-----------------------------------------------------------------------------
// Name: CubicBezierInterp()
// Desc:
//-----------------------------------------------------------------------------

float CubicBezierInterp(float V0, float V1, float V2, float V3, float V4, float Percent)
{
    float R = LinearInterp(V0, V1, Percent);
    R = LinearInterp(R, V1, Percent);
    R = LinearInterp(R, V2, Percent);
    R = LinearInterp(R, V3, Percent);
    return LinearInterp(R, V4, Percent);
}


//-----------------------------------------------------------------------------
// Name: RandRange()
// Desc:
//-----------------------------------------------------------------------------

long RandRange(long Min, long Max)
{
    return (long)((rand() % (Max - Min)) + Min);
}


//-----------------------------------------------------------------------------
// Name: GetHeading()
// Desc:
//-----------------------------------------------------------------------------

float GetHeading(Vector_t* Point1, Vector_t* Point2)
{
    float Opp, Adj;
    float Angle;

    Opp = Point2->X - Point1->X;
    Adj = Point2->Z - Point1->Z;

    Angle = (float)atan(Opp / Adj);

    if (Opp >= 0.0f)
    {
        if (Adj >= 0.0f)
            ; // leave it alone
        else
            Angle = _180DEGASRADS + Angle;
    }
    else
    {
        if (Adj >= 0.0f)
            Angle = _360DEGASRADS + Angle;
        else
            Angle = _180DEGASRADS + Angle;
    }

    return Angle;
}

//-----------------------------------------------------------------------------
// Name: GetAngle2D()
// Desc:
//-----------------------------------------------------------------------------

float GetAngle2D(float X1, float Y1, float X2, float Y2)
{
    float Angle;
    float Ang;
    float Hyp;
    float Len1, Len2;

    Len1 = (float)fabs(X2 - X1);
    Len2 = (float)fabs(Y2 - Y1);

    Hyp = (float)sqrt((Len1 * Len1) + (Len2 * Len2));
    Ang = (float)asin((float)(Len2 / Hyp));

    Angle = (float)Ang * RTOD;

    if (Y1 > Y2)
    {
        if (X1 > X2)
            Angle =  90.0f + Angle;
        else
            Angle = 270.0f - Angle;
    }
    else
    {
        if (X1 > X2)
            Angle =  90.0f - Angle;
        else
        {
            // don't touch
        }
    }

    return Angle;
}


//-----------------------------------------------------------------------------
// Name: AngleDiff()
// Desc:
//-----------------------------------------------------------------------------

float AngleDiff(float AngleDegrees1, float AngleDegrees2)
{
    float Sum;

    // calc the smallest diff between the two Angles
    if (AngleDegrees1 > AngleDegrees2)
    {
        Sum = (360 - AngleDegrees1) + AngleDegrees2;
        if (Sum > 180)
        {
            Sum = (float)fabs(AngleDegrees1 - AngleDegrees2);

            // flag this negative to indicate that the caller
            // should turn counter clockwise to get to this
            // Angle the fastest
            Sum = -Sum;
        }
    }
    else
    {
        Sum = (360 - AngleDegrees2) + AngleDegrees1;
        if (Sum > 180)
            Sum = (float)fabs(AngleDegrees1 - AngleDegrees2);
        else
            // see previous note
            Sum = -Sum;
    }

    return Sum;
}
