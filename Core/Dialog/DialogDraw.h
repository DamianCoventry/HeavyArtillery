//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// FridaY, 5 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOGDRAW_H_
#define _DIALOGDRAW_H_


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

void DialogDrawLine( float X1, float Y1, float X2, float Y2, float R, float G, float B );
void DialogDrawRect( float X, float Y, float W, float H, float R, float G, float B );

void DialogDrawRectRaised( float X, float Y, float W, float H );
void DialogDrawRectSunken( float X, float Y, float W, float H );
void DialogDrawRectFocus( float X, float Y, float W, float H );
void DialogDrawRectControl( float X, float Y, float W, float H );
void DialogDrawRectTabTop( float X, float Y, float W, float H );
void DialogDrawRectTabBottom( float X, float Y, float W, float H );


#endif
