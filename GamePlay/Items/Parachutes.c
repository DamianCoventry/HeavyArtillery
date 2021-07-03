//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 8 July 2002
//
//========================================================================================


#include "stdafx.h"
#include "Backpack.h"
#include "ItemTypes.h"
#include "ObjectTypes.h"
#include "Camera.h"

#include "GameSounds.h"

#include "LightSource.h"
#include "ModelManager.h"
#include "SoundManager.h"


//----------------------------------------------------------------------------------------
// Name: ActivateParachutes()
// Desc: 
//----------------------------------------------------------------------------------------

void ActivateParachutes( Item_t *Item, Backpack_t *Backpack, size_t PlayerID )
{
    size_t SoundID;
    Vector_t *Position;
    size_t Index = BackpackGetAbsoluteIndex( Backpack, BPITEM_TYPEITEM, (void *)Item );

    if( BACKPACK_DEACTIVATED == BackpackToggleActivate( Backpack, Index ) )
        SoundID = SoundItemDeactivated0;
    else
        SoundID = SoundItemActivated0;

    Position = GetCameraPosition();
    SetSoundPosition( SoundID, Position->X, Position->Y, Position->Z );
    PlaySound3D( SoundID, 0 );
}


//----------------------------------------------------------------------------------------
// Name: ParachutesThink()
// Desc: 
//----------------------------------------------------------------------------------------

void ParachutesThink( Object_t *Object, size_t IsHiResModel )
{
    Vector_t LightDirection;
    float Ambient;

    GetLightSourceDirection( &LightDirection.X, &LightDirection.Y, &LightDirection.Z );
    GetLightSourceAmbient( &Ambient );

    switch( Object->State )
    {
        case OBJECTSTATE_DEAD :
            break;

        case OBJECTSTATE_DYING :
            break;

        case OBJECTSTATE_ALIVE :
            if( IsHiResModel )
                CalcObjectPointColors( Object->ModelIDHiRes, &LightDirection, Ambient,
                                       Object->Positions, Object->Rotations, Object->PointColorsHiRes );
            else
                CalcObjectPointColors( Object->ModelIDLoRes, &LightDirection, Ambient,
                                       Object->Positions, Object->Rotations, Object->PointColorsLoRes );

            break;
    }
}
