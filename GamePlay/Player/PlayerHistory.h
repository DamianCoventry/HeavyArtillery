//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 1 July 2002
//
//========================================================================================


#ifndef _PLAYERHISTORY_H_
#define _PLAYERHISTORY_H_

#include "ObjectTypes.h"
#include "GameTypes.h"


//----------------------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------------------

#define MAXHISTORYENTRIES       10


//----------------------------------------------------------------------------------------
// typedefs
//----------------------------------------------------------------------------------------

typedef struct PlayerTurn_s
{
    float DamageTaken[ MAXPLAYERS ];
}
PlayerTurn_t;

typedef struct PlayerHistory_s
{
    size_t Index;
    PlayerTurn_t Turns[ MAXHISTORYENTRIES ];

    // these variables are for the stats dialog
    size_t NumShotsFired;
    size_t NumShotsHit;
    size_t NumShotsMissed;
    float DamageToEnemys;
    float DamageToSelf;
    float DamageToTeammates;
    float FallingDamage;
    size_t EnemyKills;
    size_t SelfKills;
    size_t TeammateKills;
    size_t SurviveBonus;
    size_t TeamWinBonus;
    size_t CashWon;

    size_t HasHitThisTurn;
}
PlayerHistory_t;


//----------------------------------------------------------------------------------------
// public functions
//----------------------------------------------------------------------------------------

PlayerTurn_t *GetCurrentTurn( PlayerHistory_t *History );
PlayerTurn_t *GetPreviousTurn( PlayerHistory_t *History, size_t TurnsAgo );

void IncrementHistory( PlayerHistory_t *History );


#endif
