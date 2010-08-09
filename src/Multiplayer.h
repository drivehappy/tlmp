#pragma once

#include "Events.h"
#include "Common.h"

#include "CPlayer.h"
#include "Globals.h"

#include "Logger.h"
using namespace TLAPI;

#include "UI.h"


namespace TLMP
{

  extern Logger            multiplayerLogger;
  extern CGameClient      *gameClient;

  void SetupNetwork();

  // Post Event for character initialization
  void CreateCharacter(CCharacter* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0);

  // Post Event for equipment initialization
  void CreateEquipment(CEquipment*, CResourceManager*, u64, u32, u32, u32);
  
  // Post Event for player initialization
  void Level_CharacterInitialize(CCharacter*, CLevel*, CCharacter*, Vector3*, u32);

  // Post Event for the main GameClient loop
  void GameClient_ProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);
  void GameClient_TitleProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);
  
};
