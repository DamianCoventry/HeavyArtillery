//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 15 July 2002
//
//========================================================================================


#include "GameTypes.h"


extern size_t ReachedDestination( size_t PlyrIndex, BotInfo_t *BotInfo );
extern void TurnTowardsDestination( size_t PlyrIndex, BotInfo_t *BotInfo );

extern void ChooseMoveDestination( BotInfo_t *BotInfo );
extern size_t UpdateBotFuel( size_t PlyrIndex );

extern void UpdateBotView( size_t PlyrIndex );
