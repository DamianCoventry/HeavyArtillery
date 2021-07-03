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
#include "Camera.h"

#include "GameItems.h"
#include "GameSounds.h"

#include "PlayerManager.h"
#include "SoundManager.h"

#include "DialogProperties.h"
#include "DialogMyTank.h"


//----------------------------------------------------------------------------------------
// Name: UseBatteries()
// Desc: 
//----------------------------------------------------------------------------------------

void UseBatteries( Item_t *Item, Backpack_t *Backpack, size_t PlayerID )
{
    float Power = BATTERY_POWER;
    float TankPower, ShieldPower;
    size_t Used = 0;
    Vector_t *Position;

    TankPower = GetPlayerTankPower( PlayerID );
    if( TankPower < 100.0f )
    {
        BackpackUse( Backpack, BPITEM_TYPEITEM, (void *)&ItemBatteries );
        Used = 1;

        TankPower += Power;
        if( TankPower > 100.0f )
        {
            Power = TankPower - 100.0f;
            TankPower = 100.0f;
        }
    }
    SetPlayerTankPower( PlayerID, TankPower );

    ShieldPower = GetPlayerShieldPower( PlayerID );
    if( GetPlayerShieldType( PlayerID ) != SHIELDTYPE_NONE && ShieldPower < 100.0f )
    {
        if( !Used )
            BackpackUse( Backpack, BPITEM_TYPEITEM, (void *)&ItemBatteries );

        ShieldPower += ( Power * GetPlayerShieldType( PlayerID ) );
        if( ShieldPower > 100.0f )
            ShieldPower = 100.0f;
    }
    SetPlayerShieldPower( PlayerID, ShieldPower );

    PropertiesUpdate( DialogProperties );
    DialogUpdateMyTank( DialogMyTank );

    Position = GetCameraPosition();
    SetSoundPosition( SoundItemActivated0, Position->X, Position->Y, Position->Z );
    PlaySound3D( SoundItemActivated0, 0 );
}
