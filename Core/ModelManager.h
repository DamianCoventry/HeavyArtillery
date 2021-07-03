//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "Vector.h"
#include "ObjectTypes.h"

#include <string>

#ifndef _ROTATETYPE_
#define _ROTATETYPE_

#define MODELROTATE_XZY     0
#define MODELROTATE_YXZ     1

#endif


extern size_t CreateModel(const std::string& FileName);
extern void DeleteModel(size_t ID);

extern size_t GetModelPointCount(size_t ID, size_t Layer);

extern float GetModelBSphereRadius(size_t ID);

extern void DrawNormalModel(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                            Color_t* Colors[], size_t RotateType, float Ambient);

extern void DrawShadedModel(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                            Color_t* Colors[], size_t RotateType, float Ambient);

extern void DrawShadedModelTrans(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                                 Color_t* Colors[], size_t RotateType, float Ambient);

extern void DrawLine(size_t ID, Vector_t* Positions, Vector_t* Rotations,
                     Color_t* Colors[], size_t RotateType, float Ambient);

extern void CalcObjectPointColors(size_t ModelID, Vector_t* LightDirection, float Ambient,
                                  Vector_t* Positions, Vector_t* Rotations, Color_t* Colors[]);

extern void DrawExplosionModel(size_t ID, Vector_t* Positions, Vector_t* Rotations, Vector_t* Scales,
                               Color_t* Color, size_t RotateType, float Ambient);
