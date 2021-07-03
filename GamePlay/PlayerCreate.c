//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 29 June 2002
//
//========================================================================================


#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <vector>

#include "GameTypes.h"


//----------------------------------------------------------------------------------------
// private data
//----------------------------------------------------------------------------------------

static std::vector<PlayerCreate_t> _PlayerCreate;


//----------------------------------------------------------------------------------------
// Name: ResetAllPlayers()
// Desc:
//----------------------------------------------------------------------------------------

void ResetAllPlayers(void)
{
    _PlayerCreate.clear();
}


//----------------------------------------------------------------------------------------
// Name: AddPlayer()
// Desc:
//----------------------------------------------------------------------------------------

bool AddPlayer(const std::string& Name, size_t Team, BotType_t Type)
{
    if (_PlayerCreate.size() >= MAXPLAYERS || Name.empty() || Team >= MAXPLAYERS)
        return false;
    _PlayerCreate.push_back({ Name, Team, Type });
    return true;
}


//----------------------------------------------------------------------------------------
// Name: RemovePlayer()
// Desc:
//----------------------------------------------------------------------------------------

bool RemovePlayer(size_t Index)
{
    if (Index >= _PlayerCreate.size())
        return false;

    auto itor = _PlayerCreate.begin();
    std::advance(itor, Index);
    _PlayerCreate.erase(itor);

    return true;
}


//----------------------------------------------------------------------------------------
// Name: EditPlayer()
// Desc:
//----------------------------------------------------------------------------------------

bool EditPlayer(size_t Index, const std::string& Name, size_t Team, BotType_t Type)
{
    if (Index >= _PlayerCreate.size())
        return false;

    auto itor = _PlayerCreate.begin();
    std::advance(itor, Index);
    *itor = { Name, Team, Type };

    return true;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCreateCount()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerCreateCount(void)
{
    return _PlayerCreate.size();
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCreateName()
// Desc:
//----------------------------------------------------------------------------------------

std::string GetPlayerCreateName(size_t Index)
{
    if (Index >= _PlayerCreate.size())
        return {};
    auto itor = _PlayerCreate.begin();
    std::advance(itor, Index);
    return itor->Name;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCreateTeam()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetPlayerCreateTeam(size_t Index)
{
    if (Index >= _PlayerCreate.size())
        return -1;
    auto itor = _PlayerCreate.begin();
    std::advance(itor, Index);
    return itor->Team;
}


//----------------------------------------------------------------------------------------
// Name: SetPlayerCreateBotType()
// Desc:
//----------------------------------------------------------------------------------------

void SetPlayerCreateBotType(size_t Index, BotType_t Type)
{
    if (Index >= _PlayerCreate.size())
        return;
    auto itor = _PlayerCreate.begin();
    std::advance(itor, Index);
    itor->BotType = Type;
}


//----------------------------------------------------------------------------------------
// Name: GetPlayerCreateBotType()
// Desc:
//----------------------------------------------------------------------------------------

BotType_t GetPlayerCreateBotType(size_t Index)
{
    if (Index >= _PlayerCreate.size())
        return BOTTYPE_RANDOM;
    auto itor = _PlayerCreate.begin();
    std::advance(itor, Index);
    return itor->BotType;
}


//----------------------------------------------------------------------------------------
// Name: GetBotType()
// Desc:
//----------------------------------------------------------------------------------------

size_t GetNumberOfTeams(void)
{
    size_t Teams[MAXPLAYERS];
    size_t Total, i;

    memset(Teams, 0, sizeof(size_t) * MAXPLAYERS);

    for (const auto& player : _PlayerCreate)
        Teams[player.Team] = 1;

    Total = 0;

    for (i = 0; i < MAXPLAYERS; i++)
        Total += Teams[i];

    return Total;
}
