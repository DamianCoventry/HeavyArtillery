//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Tuesday, 2 April 2002
//-----------------------------------------------------------------------------


#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Vector.h"


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

float CosineInterp(float A, float B, float Percent);
float LinearInterp(float A, float B, float Percent);
float BezierInterp(float V0, float V1, float V2, float Percent);
float QuadraticBezierInterp(float V0, float V1, float V2, float V3, float Percent);
float CubicBezierInterp(float V0, float V1, float V2, float V3, float V4, float Percent);

long RandRange(long Min, long Max);

float GetHeading(Vector_t* Point1, Vector_t* Point2);
float GetAngle2D(float X1, float Y1, float X2, float Y2);
float AngleDiff(float AngleDegrees1, float AngleDegrees2);


#endif
