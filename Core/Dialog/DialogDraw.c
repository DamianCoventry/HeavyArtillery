//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 5 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogDraw.h"
#include "DialogScheme.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/GL.h>


//-----------------------------------------------------------------------------
// Name: DialogDrawLine()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawLine( float X1, float Y1, float X2, float Y2, float R, float G, float B )
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_LINES );
        glColor3f( R, G, B );
        glVertex2f( X1, Y1 );
        glVertex2f( X2, Y2 );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRect()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRect( float X, float Y, float W, float H, float R, float G, float B )
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_QUADS );
        glColor3f( R, G, B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );
        glVertex2f( X+W, Y+H );
        glVertex2f( X+W, Y );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRectRaised()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRectRaised( float X, float Y, float W, float H )
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_QUADS );
        glColor3f( DialogScheme->Colors.DialogBack.R, DialogScheme->Colors.DialogBack.G, DialogScheme->Colors.DialogBack.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );
        glVertex2f( X+W, Y+H );
        glVertex2f( X+W, Y );
    glEnd();

    glBegin( GL_LINES );
        glColor3f( DialogScheme->Colors.Sunken.R, DialogScheme->Colors.Sunken.G, DialogScheme->Colors.Sunken.B );
        glVertex2f( X+W, Y );
        glVertex2f( X+W, Y+H );

        glVertex2f( X, Y+H );
        glVertex2f( X+W+1.0f, Y+H );

        glColor3f( DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );

        glVertex2f( X, Y );
        glVertex2f( X+W, Y );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRectSunken()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRectSunken( float X, float Y, float W, float H )
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_QUADS );
        glColor3f( DialogScheme->Colors.DialogBack.R, DialogScheme->Colors.DialogBack.G, DialogScheme->Colors.DialogBack.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );
        glVertex2f( X+W, Y+H );
        glVertex2f( X+W, Y );
    glEnd();

    glBegin( GL_LINES );
        glColor3f( DialogScheme->Colors.Sunken.R, DialogScheme->Colors.Sunken.G, DialogScheme->Colors.Sunken.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );

        glVertex2f( X, Y );
        glVertex2f( X+W, Y );

        glColor3f( DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );
        glVertex2f( X+W, Y );
        glVertex2f( X+W, Y+H );

        glVertex2f( X, Y+H );
        glVertex2f( X+W+1.0f, Y+H );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRectFocus()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRectFocus( float X, float Y, float W, float H )
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_LINES );
        glColor3f( DialogScheme->Colors.Focus.R, DialogScheme->Colors.Focus.G, DialogScheme->Colors.Focus.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );

        glVertex2f( X, Y );
        glVertex2f( X+W, Y );

        glVertex2f( X+W, Y );
        glVertex2f( X+W, Y+H );

        glVertex2f( X, Y+H );
        glVertex2f( X+W+1.0f, Y+H );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRectControl()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRectControl( float X, float Y, float W, float H )
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_QUADS );
        glColor3f( DialogScheme->Colors.ControlBack.R, DialogScheme->Colors.ControlBack.G, DialogScheme->Colors.ControlBack.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );
        glVertex2f( X+W, Y+H );
        glVertex2f( X+W, Y );
    glEnd();

    glBegin( GL_LINES );
        glColor3f( DialogScheme->Colors.Sunken.R, DialogScheme->Colors.Sunken.G, DialogScheme->Colors.Sunken.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );

        glVertex2f( X, Y );
        glVertex2f( X+W, Y );

        glColor3f( DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );
        glVertex2f( X+W, Y );
        glVertex2f( X+W, Y+H );

        glVertex2f( X, Y+H );
        glVertex2f( X+W+1.0f, Y+H );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRectTabTop()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRectTabTop( float X, float Y, float W, float H )
{
    H += 1.0f;

    glDisable( GL_TEXTURE_2D );
    glBegin( GL_QUADS );
        glColor3f( DialogScheme->Colors.DialogBack.R, DialogScheme->Colors.DialogBack.G, DialogScheme->Colors.DialogBack.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );
        glVertex2f( X+W, Y+H );
        glVertex2f( X+W, Y );
    glEnd();

    glBegin( GL_LINES );
        glColor3f( DialogScheme->Colors.Raised.R, DialogScheme->Colors.Raised.G, DialogScheme->Colors.Raised.B );
        glVertex2f( X, Y );
        glVertex2f( X, Y+H );

        glVertex2f( X, Y );
        glVertex2f( X+W, Y );

        glColor3f( DialogScheme->Colors.Sunken.R, DialogScheme->Colors.Sunken.G, DialogScheme->Colors.Sunken.B );
        glVertex2f( X+W, Y );
        glVertex2f( X+W, Y+H );
    glEnd();
    glEnable( GL_TEXTURE_2D );
}


//-----------------------------------------------------------------------------
// Name: DialogDrawRectTabBottom()
// Desc: 
//-----------------------------------------------------------------------------

void DialogDrawRectTabBottom( float X, float Y, float W, float H )
{
    // shrink it slightly so that it's noticeable that this tab isn't
    // currently selected
    DialogDrawRectTabTop( X+1.0f, Y+1.0f, W-2.0f, H-2.0f );
}
