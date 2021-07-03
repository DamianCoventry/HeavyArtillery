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


extern void ChooseTarget( size_t PlyrIndex, BotInfo_t *BotInfo );
extern void CalculateHeadingPitchPower( size_t PlyrIndex, BotInfo_t *BotInfo );
extern size_t RotateTowardsTarget( size_t PlyrIndex, BotInfo_t *BotInfo );

extern void SetupBotShotNoise( size_t PlyrIndex, BotInfo_t *BotInfo );

extern void CalculateProjectShotDetails(float Heading, float Pitch, float Power, Vector_t *ShootVector, Vector_t *StartPoint, Vector_t *FinishPoint);
