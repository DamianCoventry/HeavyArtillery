//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include <string.h>
#include <stdlib.h>

#include "Camera.h"
#include "Utility.h"

#include "GameTypes.h"
#include "GameConfig.h"
#include "GameSounds.h"

#include "PlayerManager.h"
#include "LevelManager.h"
#include "SoundManager.h"

#define CASH_DAMAGETOENEMYS         50
#define CASH_DAMAGETOSELF           5
#define CASH_DAMAGETOTEAMMATES      10

#define CASH_ENEMYKILLS             5000
#define CASH_SELFKILLS              1000
#define CASH_TEAMMATEKILLS          2000

#define CASH_SURVIVEBONUS           7500
#define CASH_TEAMWINBONUS           5000
#define CASH_MINIMUMBONUS           5000

#define AMBIENT_MINTHRESHOLD        1500
#define AMBIENT_MAXTHRESHOLD        2000


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static size_t _CurrentLevel;
static size_t _CurrentPlayer;
static size_t _WinningTeam;
static size_t _PlayOrder[ MAXPLAYERS ];

static size_t _AmbientCounter   = 0;
static size_t _AmbientThreshold = AMBIENT_MINTHRESHOLD;


//----------------------------------------------------------------------------------------
// Name: BeginNewGame()
// Desc:
//----------------------------------------------------------------------------------------

void BeginNewGame(void)
{
    _CurrentLevel = 0;
}


//----------------------------------------------------------------------------------------
// Name: BeginNewLevel()
// Desc:
//----------------------------------------------------------------------------------------

void BeginNewLevel(void)
{
    _CurrentPlayer = 0;
}


//----------------------------------------------------------------------------------------
// Name: EndLevel()
// Desc:
//----------------------------------------------------------------------------------------

size_t EndLevel(void)
{
    if (++_CurrentLevel >= GetLevelCount())
        return 0;

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: IsEndOfLevel()
// Desc:
//----------------------------------------------------------------------------------------

size_t IsEndOfLevel(void)
{
    size_t i;
    size_t TeamsAlive, PlayersAlive;
    size_t Teams[ MAXPLAYERS ];

    // if there's < 2 players left or if there's only
    // one team left then the level is over

    // count how many players are still alive
    for (i = 0, PlayersAlive = 0; i < GetPlayerCount(); i++)
    {
        if (!IsPlayerDead(i))
            PlayersAlive++;
    }

    SetWinningTeam(-1);

    // count how many teams are left
    memset(Teams, 0, sizeof(size_t) * MAXPLAYERS);
    for (i = 0; i < GetPlayerCount(); i++)
    {
        if (!IsPlayerDead(i))
        {
            Teams[ GetPlayerTeam(i) ] = 1;
            SetWinningTeam(GetPlayerTeam(i));
        }
    }

    TeamsAlive = 0;
    for (i = 0; i < MAXPLAYERS; i++)
        TeamsAlive += Teams[ i ];

    // should this level keep going?
    if (PlayersAlive >= 2 && TeamsAlive >= 2)
        return 0;       // not end of level

    return 1;
}


//----------------------------------------------------------------------------------------
// Name: GetCurrentLevel()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetCurrentLevel(void)
{
    return _CurrentLevel;
}


//----------------------------------------------------------------------------------------
// Name: GenerateNewPlayOrder()
// Desc:
//----------------------------------------------------------------------------------------

void GenerateNewPlayOrder(void)
{
    size_t i, j;
    size_t Done, Player;

    for (Player = 0; Player < GetPlayerCount(); Player++)
    {
        Done = 0;
        while (!Done)
        {
            i = rand() % GetPlayerCount();

            for (j = 0; j < Player; j++)
            {
                if (i == _PlayOrder[ j ])
                    break;
            }

            if (j >= Player)
                Done = 1;
        }
        _PlayOrder[ Player ] = i;
    }
}


//----------------------------------------------------------------------------------------
// Name: GetCurrentPlayer()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetCurrentPlayer(void)
{
    return _PlayOrder[ _CurrentPlayer ];
}


//----------------------------------------------------------------------------------------
// Name: GetCurrentPlayerIndex()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetCurrentPlayerIndex(void)
{
    return _CurrentPlayer;
}


//----------------------------------------------------------------------------------------
// Name: GetCurrentPlayer()
// Desc:
//----------------------------------------------------------------------------------------

void SetNextPlayer(void)
{
    for (; ;)
    {
        if (++_CurrentPlayer >= GetPlayerCount())
            _CurrentPlayer = 0;

        if (!IsPlayerDead(_PlayOrder[ _CurrentPlayer ]))
            break;
    }

}


//----------------------------------------------------------------------------------------
// Name: SetWinningTeam()
// Desc:
//----------------------------------------------------------------------------------------

void SetWinningTeam(size_t Team)
{
    _WinningTeam = Team;
}


//----------------------------------------------------------------------------------------
// Name: GetWinningTeam()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetWinningTeam(void)
{
    return _WinningTeam;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayOrderPlayer()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayOrderPlayer(size_t PlayOrderIndex)
{
    if (PlayOrderIndex < GetPlayerCount())
        return _PlayOrder[ PlayOrderIndex ];

    return -1;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayOrderPlayer()
// Desc:
//----------------------------------------------------------------------------------------

void AwardEndOfLevelCash(void)
{
    size_t PlyrIndex;
    PlayerHistory_t* History;

    for (PlyrIndex = 0; PlyrIndex < GetPlayerCount(); PlyrIndex++)
    {
        History = GetPlayerHistory(PlyrIndex);

        History->CashWon = 0;

        // award cash for surviving to the end
        if (IsPlayerDead(PlyrIndex))
            History->SurviveBonus = 0;
        else
            History->SurviveBonus = CASH_SURVIVEBONUS;

        History->CashWon += History->SurviveBonus;

        // award cash if this player's team has won
        if (GetPlayerTeam(PlyrIndex) == _WinningTeam)
            History->TeamWinBonus = CASH_TEAMWINBONUS;
        else
            History->TeamWinBonus = 0;

        History->CashWon += History->TeamWinBonus;

        // award cash based on how much damage this player caused
        History->CashWon += (CASH_DAMAGETOENEMYS * (size_t)History->DamageToEnemys);

        // award cash based on how many kills this player caused
        History->CashWon += (CASH_ENEMYKILLS * (size_t)History->EnemyKills);

        // punish for killing self + team mates
        size_t negativeCash = (CASH_DAMAGETOSELF      * (size_t)History->DamageToSelf) +
                              (CASH_DAMAGETOTEAMMATES * (size_t)History->DamageToTeammates);

        // punish for damaging self + team mates
        negativeCash += (CASH_SELFKILLS     * (size_t)History->SelfKills) +
                        (CASH_TEAMMATEKILLS * (size_t)History->TeammateKills);

        if (negativeCash >= History->CashWon)
            History->CashWon = 0;
        else
            History->CashWon -= negativeCash;

        // cap the lowest amount of cash a player can win
        if (History->CashWon < CASH_MINIMUMBONUS)
            History->CashWon = CASH_MINIMUMBONUS;

        AddToPlayerCash(PlyrIndex, History->CashWon);
    }
}


//----------------------------------------------------------------------------------------
// Name: UpdateAmbientSounds()
// Desc:
//----------------------------------------------------------------------------------------

void UpdateAmbientSounds(void)
{
    size_t SoundIdx = rand() % MAXAMBIENTNOISES;
    Vector_t* Position = GetCameraPosition();

    if (++_AmbientCounter > _AmbientThreshold)
    {
        _AmbientCounter = 0;
        _AmbientThreshold = RandRange(AMBIENT_MINTHRESHOLD, AMBIENT_MAXTHRESHOLD);

        SetSoundPosition(SoundAmbientNoises[ SoundIdx ], Position->X, Position->Y, Position->Z);
        PlaySound2D(SoundAmbientNoises[ SoundIdx ], 0);
    }
}
