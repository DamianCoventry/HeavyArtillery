//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Sunday, 30 June 2002
//
//========================================================================================


#include "Backpack.h"
#include "Vector.h"
#include "GameTypes.h"
#include "PlayerHistory.h"


extern void CreatePlayers(void);
extern void DeletePlayers(void);

extern void SetupPlayersForNewGame(void);
extern void SetupPlayersForNewLevel(void);

extern size_t GetPlayerCount(void);

extern std::string GetPlayerName(size_t PlyrIndex);
extern size_t GetPlayerTeam(size_t PlyrIndex);

extern size_t IsPlayerDead(size_t PlyrIndex);

extern size_t IsPlayerABot(size_t PlyrIndex);
extern size_t GetPlayerCash(size_t PlyrIndex);
extern void AddToPlayerCash(size_t PlyrIndex, size_t Cash);
extern void SubtractFromPlayerCash(size_t PlyrIndex, size_t Cash);

extern void AllowBotToThink(size_t PlyrIndex);

extern Backpack_t* GetPlayerBackpack(size_t PlyrIndex);

extern size_t GetPlayerTank(size_t PlyrIndex);
extern size_t GetPlayerParachute(size_t PlyrIndex);
extern size_t GetPlayerShield(size_t PlyrIndex);

extern float GetPlayerTankPower(size_t PlyrIndex);
extern float GetPlayerShieldPower(size_t PlyrIndex);
extern float GetPlayerShieldType(size_t PlyrIndex);

extern void SetPlayerTankPower(size_t PlyrIndex, float TankPower);
extern void SetPlayerShieldPower(size_t PlyrIndex, float ShieldPower);

extern size_t GetPlayerCurrentWeapon(size_t PlyrIndex);
extern void SetPlayerCurrentWeapon(size_t PlyrIndex, size_t Weapon);

extern BotType_t GetPlayerBotType(size_t PlyrIndex);

extern void GetPlayerShotDetails(size_t PlyrIndex, float* Heading, float* Pitch, float* Power);

extern void StopPlayerReceivingFallingDamage(size_t PlyrIndex);
extern void AllowPlayerToReceiveFallingDamage(size_t PlyrIndex);
extern size_t IsPlayerReceivingFallingDamage(size_t PlyrIndex);

extern void RepairPlayersTank(size_t PlyrIndex);
extern void PlacePlayersOnLandscape(void);

extern void GetPlayerPosition(size_t PlyrIndex, Vector_t* Point);
extern void GetPlayerGroundNormal(size_t PlyrIndex, Vector_t* Normal);

extern void ModifyTankBarrell(size_t PlyrIndex, float HeadingDelta, float PitchDelta);
extern void ModifyTankShotPower(size_t PlyrIndex, float PowerDelta);
extern void ModifyTankRotation(size_t PlyrIndex, float HeadingDelta);
extern void SetTankShotPower(size_t PlyrIndex, float Power);

extern void HidePlayer(size_t PlyrIndex);
extern void ShowPlayer(size_t PlyrIndex);

extern void MovePlayerForward(size_t PlyrIndex, float MoveScale);
extern void MovePlayerBackward(size_t PlyrIndex, float MoveScale);

extern size_t IsPlayerMoving(size_t PlyrIndex);
extern size_t IsPlayerOnGround(size_t PlyrIndex);

extern void ActivatePlayerShield(size_t PlyrIndex, float ShieldType);
extern void DeactivatePlayerShield(size_t PlyrIndex);

extern float PlayerTakeDamage(size_t PlyrIndex, float Damage);
extern void ExplodePlayer(size_t PlyrIndex);

extern void PlayerTakeCollisionDamage(size_t ObjectID, float Damage);

extern BotInfo_t* GetBotInfo(size_t PlyrIndex);
extern void InitaliseBotTurn(size_t PlyrIndex);
extern PlayerHistory_t* GetPlayerHistory(size_t PlyrIndex);

extern void SetPlayerDamageSmokeID(size_t PlyrIndex, size_t ParticleID);
extern size_t GetPlayerDamageSmokeID(size_t PlyrIndex);
