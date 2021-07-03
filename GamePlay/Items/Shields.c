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
#include "LightSource.h"

#include "ObjectTypes.h"
#include "ItemTypes.h"

#include "GameTypes.h"
#include "GameItems.h"
#include "GameSounds.h"

#include "PlayerManager.h"
#include "ModelManager.h"
#include "ObjectManager.h"
#include "SoundManager.h"

#include "DialogMyTank.h"
#include "DialogProperties.h"


//----------------------------------------------------------------------------------------
// Name: ActivateShields()
// Desc: 
//----------------------------------------------------------------------------------------

void ActivateShields( Item_t *Item, Backpack_t *Backpack, size_t PlayerID )
{
    size_t Index;
    Vector_t Position;

    Index = BackpackGetAbsoluteIndex( Backpack, BPITEM_TYPEITEM, (void *)Item );

    switch( BackpackToggleActivate( Backpack, Index ) )
    {
        case BACKPACK_DEACTIVATED :
            DeactivatePlayerShield( PlayerID );

            GetPlayerPosition( PlayerID, &Position );
            SetSoundPosition( SoundShieldDown0, Position.X, Position.Y, Position.Z );
            PlaySound3D( SoundShieldDown0, 0 );
            break;

        case BACKPACK_ACTIVATED :
            BackpackUse( Backpack, BPITEM_TYPEITEM, Item );

            GetPlayerPosition( PlayerID, &Position );
            SetSoundPosition( SoundShieldUp0, Position.X, Position.Y, Position.Z );
            PlaySound3D( SoundShieldUp0, 0 );

            if( Item == &ItemShieldsHeavy )
                ActivatePlayerShield( PlayerID, SHIELDTYPE_HEAVY );

            else if( Item == &ItemShieldsMedium )
                ActivatePlayerShield( PlayerID, SHIELDTYPE_MEDIUM );

            else if( Item == &ItemShieldsLight )
                ActivatePlayerShield( PlayerID, SHIELDTYPE_LIGHT );

            break;
    }

    DialogUpdateMyTank( DialogMyTank );
    PropertiesUpdate( DialogProperties );
}


//----------------------------------------------------------------------------------------
// Name: ShieldsThink()
// Desc: 
//----------------------------------------------------------------------------------------

void ShieldsThink( Object_t *Object, size_t IsHiResModel )
{
    Vector_t LightDirection;
    Vector_t Rotation;
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

            GetObjectRotation( Object->ID, 0, &Rotation );
            Rotation.Y += 1.0f;
            SetObjectRotation( Object->ID, 0, &Rotation );
            break;
    }
}
