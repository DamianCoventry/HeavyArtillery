//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

#include "LWObject.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ObjectManager.h"

#define MAXMODELS               128

#define INVALIDID( _id )        ( (_id) < 0 || (_id) >= MAXMODELS || !_ModelPool[ _id ].Used )


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _ModelIndex = 0;
static LWObject_t _ModelPool[ MAXMODELS ];


//----------------------------------------------------------------------------------------
// Name: GetFreeModel()
// Desc:
//----------------------------------------------------------------------------------------

static size_t GetFreeModel(void)
{
    size_t i;
    size_t ID;

    i = _ModelIndex;

    while (_ModelPool[ i ].Used)
    {
        i++;
        if (i >= MAXMODELS)
            i = 0;

        if (i == _ModelIndex)
            return -1;        // pool is full
    }

    _ModelPool[ i ].Used = 1;
    ID = i;

    _ModelIndex = i;
    if (++_ModelIndex >= MAXMODELS)
        _ModelIndex = 0;

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: ReturnModel()
// Desc:
//----------------------------------------------------------------------------------------

static void ReturnModel(size_t ID)
{
    if (INVALIDID(ID))
        return;

    _ModelPool[ ID ].Used = 0;
}


//----------------------------------------------------------------------------------------
// Name: CreateModel()
// Desc:
//----------------------------------------------------------------------------------------

size_t CreateModel(const std::string& FileName)
{
    size_t ID;

    ID = GetFreeModel();
    if (-1 == ID)
        return -1;

    if (!LWOLoad(FileName, &_ModelPool[ ID ]))
    {
        ReturnModel(ID);
        return -1;
    }

    return ID;
}


//----------------------------------------------------------------------------------------
// Name: DeleteModel()
// Desc:
//----------------------------------------------------------------------------------------

void DeleteModel(size_t ID)
{
    if (INVALIDID(ID))
        return;

    LWOUnload(&_ModelPool[ ID ]);

    ReturnModel(ID);
}


//----------------------------------------------------------------------------------------
// Name: GetModelBSphereRadius()
// Desc:
//----------------------------------------------------------------------------------------

float GetModelBSphereRadius(size_t ID)
{
    if (INVALIDID(ID))
        return 0.0f;

    return _ModelPool[ ID ].BSphereRadius;
}


//----------------------------------------------------------------------------------------
// Name: DrawNormalModel()
// Desc:
//----------------------------------------------------------------------------------------

void DrawNormalModel(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                     Color_t* Colors[], size_t RotateType, float Ambient)
{
    size_t LayerIdx, PolygonIdx;
    LWOPolygon_t* Polygon;
    LWOLayer_t* Layer;
    LWObject_t* LWO;

    if (INVALIDID(ID))
        return;

    LWO = &_ModelPool[ ID ];

    glBindTexture(GL_TEXTURE_2D, GetTextureNumber(LWO->TextureID));
    glDisable(GL_BLEND);
    glColor3f(Ambient, Ambient, Ambient);

    for (LayerIdx = 0; LayerIdx < LWO->LayerCount; LayerIdx++)
    {
        Layer = &LWO->Layers[ LayerIdx ];

        glTranslatef(Positions[ LayerIdx ].X, Positions[ LayerIdx ].Y, Positions[ LayerIdx ].Z);

        if (RotateType == MODELROTATE_XZY)
        {
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
        }
        /*
                if( 0 == LayerIdx )
                {
                    glDisable( GL_TEXTURE_2D );
                    glColor3f( 1.0f, 1.0f, 0.0f );
                    glutWireSphere( GetModelBSphereRadius( ID ), 8.0f, 8.0f );
                    glEnable( GL_TEXTURE_2D );
                }
        */
        glBegin(GL_TRIANGLES);

        for (PolygonIdx = 0; PolygonIdx < Layer->PolygonCount; PolygonIdx++)
        {
            Polygon = &Layer->Polygons[ PolygonIdx ];

            glTexCoord2fv((float*)&Polygon->T1); glVertex3fv((float*)&Layer->Points[ Polygon->P1 ]);
            glTexCoord2fv((float*)&Polygon->T2); glVertex3fv((float*)&Layer->Points[ Polygon->P2 ]);
            glTexCoord2fv((float*)&Polygon->T3); glVertex3fv((float*)&Layer->Points[ Polygon->P3 ]);
        }

        glEnd();
    }
}


//----------------------------------------------------------------------------------------
// Name: DrawShadedModel()
// Desc:
//----------------------------------------------------------------------------------------

void DrawShadedModel(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                     Color_t* Colors[], size_t RotateType, float Ambient)
{
    size_t LayerIdx, PolygonIdx;
    LWOPolygon_t* Polygon;
    LWOLayer_t* Layer;
    LWObject_t* LWO;

    if (INVALIDID(ID))
        return;

    LWO = &_ModelPool[ ID ];

    glBindTexture(GL_TEXTURE_2D, GetTextureNumber(LWO->TextureID));
    glDisable(GL_BLEND);

    for (LayerIdx = 0; LayerIdx < LWO->LayerCount; LayerIdx++)
    {
        Layer = &LWO->Layers[ LayerIdx ];

        glTranslatef(Positions[ LayerIdx ].X, Positions[ LayerIdx ].Y, Positions[ LayerIdx ].Z);

        if (RotateType == MODELROTATE_XZY)
        {
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
        }
        /*
                if( 0 == LayerIdx )
                {
                    glDisable( GL_TEXTURE_2D );
                    glColor3f( 1.0f, 1.0f, 0.0f );
                    glutWireSphere( GetModelBSphereRadius( ID ), 8.0f, 8.0f );
                    glEnable( GL_TEXTURE_2D );
                }
        */
        for (PolygonIdx = 0; PolygonIdx < Layer->PolygonCount; PolygonIdx++)
        {
            Polygon = &Layer->Polygons[ PolygonIdx ];

            glBegin(GL_TRIANGLES);
            glTexCoord2fv((float*)&Polygon->T1);
            glColor4fv((float*)&Colors[ LayerIdx ][ Polygon->P1 ]);
            glVertex3fv((float*)&Layer->Points[ Polygon->P1 ]);

            glTexCoord2fv((float*)&Polygon->T2);
            glColor4fv((float*)&Colors[ LayerIdx ][ Polygon->P2 ]);
            glVertex3fv((float*)&Layer->Points[ Polygon->P2 ]);

            glTexCoord2fv((float*)&Polygon->T3);
            glColor4fv((float*)&Colors[ LayerIdx ][ Polygon->P3 ]);
            glVertex3fv((float*)&Layer->Points[ Polygon->P3 ]);
            glEnd();
        }
    }
}


//----------------------------------------------------------------------------------------
// Name: DrawExplosionModel()
// Desc:
//----------------------------------------------------------------------------------------

void DrawExplosionModel(size_t ID, Vector_t* Positions, Vector_t* Rotations, Vector_t* Scales,
                        Color_t* Color, size_t RotateType, float Ambient)
{
    size_t LayerIdx, PolygonIdx;
    LWOPolygon_t* Polygon;
    LWOLayer_t* Layer;
    LWObject_t* LWO;

    if (INVALIDID(ID))
        return;

    LWO = &_ModelPool[ ID ];

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, GetTextureNumber(LWO->TextureID));
    glColor4f(Color->R, Color->G, Color->B, Ambient);

    for (LayerIdx = 0; LayerIdx < LWO->LayerCount; LayerIdx++)
    {
        Layer = &LWO->Layers[ LayerIdx ];

        //glScalef( 1.0f, 1.0f, 1.0f );
        glTranslatef(Positions[ LayerIdx ].X, Positions[ LayerIdx ].Y, Positions[ LayerIdx ].Z);

        glScalef(Scales[ LayerIdx ].X, Scales[ LayerIdx ].Y, Scales[ LayerIdx ].Z);

        if (RotateType == MODELROTATE_XZY)
        {
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
        }
        /*
                if( 0 == LayerIdx )
                {
                    glDisable( GL_TEXTURE_2D );
                    glColor3f( 1.0f, 1.0f, 0.0f );
                    glutWireSphere( GetModelBSphereRadius( ID ), 8.0f, 8.0f );
                    glEnable( GL_TEXTURE_2D );
                }
        */

        glBegin(GL_TRIANGLES);

        for (PolygonIdx = 0; PolygonIdx < Layer->PolygonCount; PolygonIdx++)
        {
            Polygon = &Layer->Polygons[ PolygonIdx ];

            glTexCoord2fv((float*)&Polygon->T1); glVertex3fv((float*)&Layer->Points[ Polygon->P1 ]);
            glTexCoord2fv((float*)&Polygon->T2); glVertex3fv((float*)&Layer->Points[ Polygon->P2 ]);
            glTexCoord2fv((float*)&Polygon->T3); glVertex3fv((float*)&Layer->Points[ Polygon->P3 ]);
        }

        glEnd();
    }

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}


//----------------------------------------------------------------------------------------
// Name: DrawShadedModelTrans()
// Desc:
//----------------------------------------------------------------------------------------

void DrawShadedModelTrans(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                          Color_t* Colors[], size_t RotateType, float Ambient)
{
    size_t LayerIdx, PolygonIdx;
    LWOPolygon_t* Polygon;
    LWOLayer_t* Layer;
    LWObject_t* LWO;

    if (INVALIDID(ID))
        return;

    LWO = &_ModelPool[ ID ];

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, GetTextureNumber(LWO->TextureID));

    for (LayerIdx = 0; LayerIdx < LWO->LayerCount; LayerIdx++)
    {
        Layer = &LWO->Layers[ LayerIdx ];

        glTranslatef(Positions[ LayerIdx ].X, Positions[ LayerIdx ].Y, Positions[ LayerIdx ].Z);

        if (RotateType == MODELROTATE_XZY)
        {
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            glRotatef(-Rotations[ LayerIdx ].Y, 0.0f, 1.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].X, 1.0f, 0.0f, 0.0f);
            glRotatef(-Rotations[ LayerIdx ].Z, 0.0f, 0.0f, 1.0f);
        }
        /*
                if( 0 == LayerIdx )
                {
                    glDisable( GL_TEXTURE_2D );
                    glColor3f( 1.0f, 1.0f, 0.0f );
                    glutWireSphere( GetModelBSphereRadius( ID ), 8.0f, 8.0f );
                    glEnable( GL_TEXTURE_2D );
                }
        */
        for (PolygonIdx = 0; PolygonIdx < Layer->PolygonCount; PolygonIdx++)
        {
            Polygon = &Layer->Polygons[ PolygonIdx ];

            glBegin(GL_TRIANGLES);
            glTexCoord2fv((float*)&Polygon->T1);
            glColor4fv((float*)&Colors[ LayerIdx ][ Polygon->P1 ]);
            glVertex3fv((float*)&Layer->Points[ Polygon->P1 ]);

            glTexCoord2fv((float*)&Polygon->T2);
            glColor4fv((float*)&Colors[ LayerIdx ][ Polygon->P2 ]);
            glVertex3fv((float*)&Layer->Points[ Polygon->P2 ]);

            glTexCoord2fv((float*)&Polygon->T3);
            glColor4fv((float*)&Colors[ LayerIdx ][ Polygon->P3 ]);
            glVertex3fv((float*)&Layer->Points[ Polygon->P3 ]);
            glEnd();
        }
    }

    glDisable(GL_BLEND);
}


//----------------------------------------------------------------------------------------
// Name: DrawLine()
// Desc:
//----------------------------------------------------------------------------------------

void DrawLine(size_t ID, Vector_t* Positions, Vector_t* Rotations,
              Color_t* Colors[], size_t RotateType, float Ambient)
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);
    glVertex3fv((float*)&Positions[ 0 ]);
    glVertex3fv((float*)&Positions[ 1 ]);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}


//----------------------------------------------------------------------------------------
// Name: GetModelPointCount()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetModelPointCount(size_t ID, size_t Layer)
{
    if (INVALIDID(ID))
        return 0;

    return _ModelPool[ ID ].Layers[ Layer ].PointCount;
}


//----------------------------------------------------------------------------------------
// Name: CalcObjectPointColors()
// Desc:
//----------------------------------------------------------------------------------------

extern void CalcObjectPointColors(size_t ModelID, Vector_t* LightDirection, float Ambient,
                                  Vector_t* Positions, Vector_t* Rotations, Color_t* Colors[])
{
    LWOLayer_t* LWOLayer;
    Vector_t Direction;
    size_t LayerIdx, PointIdx;
    float Dot;

    if (INVALIDID(ModelID))
        return;

    VectorCopy((*LightDirection), Direction);

    for (LayerIdx = 0; LayerIdx < _ModelPool[ ModelID ].LayerCount; LayerIdx++)
    {
        LWOLayer = &_ModelPool[ ModelID ].Layers[ LayerIdx ];

        VectorRotateX(Rotations[ LayerIdx ].X * DTOR, &Direction);
        VectorRotateZ(Rotations[ LayerIdx ].Z * DTOR, &Direction);
        VectorRotateY(Rotations[ LayerIdx ].Y * DTOR, &Direction);

        for (PointIdx = 0; PointIdx < LWOLayer->PointCount; PointIdx++)
        {
            Dot = -VectorDotProd(LWOLayer->PointNormals[ PointIdx ], Direction);
            if (Dot < 0.0f)
                Dot = 0.0f;

            Colors[ LayerIdx ][ PointIdx ].R = Dot + Ambient;
            if (Colors[ LayerIdx ][ PointIdx ].R > 1.0f)
                Colors[ LayerIdx ][ PointIdx ].R = 1.0f;

            Colors[ LayerIdx ][ PointIdx ].G = Colors[ LayerIdx ][ PointIdx ].R;
            Colors[ LayerIdx ][ PointIdx ].B = Colors[ LayerIdx ][ PointIdx ].R;
            Colors[ LayerIdx ][ PointIdx ].A = 1.0f;
        }
    }
}
