//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Saturday, 29 June 2002
//
//========================================================================================


#include "GameTypes.h"


extern void ResetAllPlayers(void);

extern bool AddPlayer(const std::string& Name, size_t Team, BotType_t Type);
extern bool RemovePlayer(size_t Index);
extern bool EditPlayer(size_t Index, const std::string& Name, size_t Team, BotType_t Type);

extern size_t GetPlayerCreateCount(void);

extern std::string GetPlayerCreateName(size_t Index);
extern size_t GetPlayerCreateTeam(size_t Index);

extern void SetPlayerCreateBotType(size_t Index, BotType_t Type);
extern BotType_t GetPlayerCreateBotType(size_t Index);

extern size_t GetNumberOfTeams(void);
