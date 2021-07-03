//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 29 June 2002
//
//========================================================================================


#ifndef _GAMETYPES_H_
#define _GAMETYPES_H_



//----------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------

#define DEMO                    0

#if DEMO
#define GAME_NAME               "Heavy Artillery v1.10 DEMO"
#else
#define GAME_NAME               "Heavy Artillery v1.10"
#endif

#define MAXPLAYERS              10
#define MAXNAMECHARACTERS       20

#define SHIELDTYPE_NONE         0.00f
#define SHIELDTYPE_LIGHT        1.00f
#define SHIELDTYPE_MEDIUM       0.75f
#define SHIELDTYPE_HEAVY        0.50f

#define ITEMTYPE_OTHER          0
#define ITEMTYPE_GUIDANCE       1
#define ITEMTYPE_SHIELD         2
#define ITEMTYPE_POSSESS        3

#define TANK_BARRELLHEIGHT      15.0f
#define TERMINALVELOCITY        5.0f

#define BATTERY_POWER           10.0f

#define RAILGUN_THRESHOLD       150
#define RAILGUN_DAMAGE          70.0f

#define NUMFRAMESPERLITRE       60
#define DAMAGEHEIGHT            4.0f

#include "Vector.h"
#include <string>


//----------------------------------------------------------------------------------------
// enums
//----------------------------------------------------------------------------------------

typedef enum BotType_e
{
    BOTTYPE_NOTABOT = 0,
    BOTTYPE_INEPT,
    BOTTYPE_VINDICTIVE,
    BOTTYPE_COMPETETIVE,
    BOTTYPE_GREEDY,
    BOTTYPE_CRUEL,
    BOTTYPE_RANDOM
}
BotType_t;


typedef enum BotState_e
{
    BOTSTATE_CHOOSINGTURN = 0,
    BOTSTATE_POLLMOVE,
    BOTSTATE_POLLSHOOT
}
BotState_t;


typedef enum BotShootState_e
{
    BOTSHOOTSTATE_CHOOSINGTARGET = 0,
    BOTSHOOTSTATE_CALCULATINGSHOT,
    BOTSHOOTSTATE_ROTATINGBARRELL
}
BotShootState_t;


//----------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------

typedef struct PlayerCreate_s
{
    std::string Name;
    size_t Team;
    BotType_t BotType;
}
PlayerCreate_t;


typedef struct BotInfo_s
{
    BotState_t State;
    BotShootState_t ShotState;

    size_t DestTileX, DestTileZ;
    size_t TargetPlyrIndex;

    float DestHeading, DestPitch;
    float DestPower;

    Vector_t TargetPrevPosition;
    Vector_t MyPrevPosition;

    size_t ShotNoiseCounter;
    size_t ShotNoiseThreshold;
    long ShotNoiseMin, ShotNoiseMax;

    void (*ChooseNewTarget)( size_t, struct BotInfo_s * );
    void (*ReconsiderTarget)( size_t, struct BotInfo_s * );
}
BotInfo_t;


#endif
