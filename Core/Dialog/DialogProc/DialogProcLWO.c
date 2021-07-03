//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include <stdlib.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <math.h>

#include "DialogProcLWO.h"
#include "DialogProcDefault.h"
#include "Dialog.h"
#include "DialogScheme.h"
#include "DialogDraw.h"

#include "Hardware.h"
#include "Graphics.h"
#include "Input.h"
#include "Vector.h"

#include "ObjectManager.h"


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern HardwareConfig_t g_HWConfig;


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static float Angle = 0.0f;
static float _ViewingDistance;


//-----------------------------------------------------------------------------
// Name: DialogProcLWO()
// Desc: 
//-----------------------------------------------------------------------------

size_t DialogProcLWO( Dialog_t *Dialog, size_t Msg, void* Param1, void* Param2 )
{
    DialogObject_t *DialogObject;
    Vector_t V;
    float X, Y;

    V.Z = 0.0f;

    switch( Msg )
    {
        case WM_CREATE :
            _ViewingDistance = ( (float)g_HWConfig.GfxHeight / 2.0f ) / (float)tan( DTOR * ( g_HWConfig.GLVFOV / 2.0f ) );
            return DIALOG_MSGHANDLED;

        case WM_DISPLAYCHANGE :
            _ViewingDistance = ( (float)g_HWConfig.GfxHeight / 2.0f ) / (float)tan( DTOR * ( g_HWConfig.GLVFOV / 2.0f ) );
            return DIALOG_MSGHANDLED;

        case WM_DESTROY :
            return DIALOG_MSGHANDLED;

        case WM_PAINT :
            DialogClientToScreen( Dialog, &X, &Y );

            if( Dialog->Flags & DIALOG_BORDER )
                DialogDrawRectSunken( X, Y, Dialog->W, Dialog->H );

            DialogObject = (DialogObject_t *)Dialog->Data;
            if( nullptr == DialogObject )
                return DIALOG_MSGHANDLED;

            Angle += 0.25f;
            if( Angle >= 360.0f )
                Angle -= 360.0f;

            X += Dialog->W / 2.0f;
            Y += Dialog->H / 2.0f;

            glEnable( GL_DEPTH_TEST );
            //glDisable( GL_FOG );

            glClear( GL_DEPTH_BUFFER_BIT );

            Set3D();
            glPushMatrix();

                glTranslatef( X-((float)g_HWConfig.GfxWidth / 2.0f), -Y+((float)g_HWConfig.GfxHeight / 2.0f), -_ViewingDistance );
                glScalef( DialogObject->Scale, DialogObject->Scale, DialogObject->Scale );

                V.X = -30.0f;
                V.Y = Angle;
                SetObjectRotation( DialogObject->ObjectID, 0, &V );
                DrawObject( DialogObject->ObjectID );

            glPopMatrix();

            Set2D();
            glDisable( GL_DEPTH_TEST );
            //glEnable( GL_FOG );

            return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault( Dialog, Msg, Param1, Param2 );
}
