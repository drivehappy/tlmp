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

  // Server Events
  void ServerOnClientConnected(void *);
  // --

  // Post Event for character save state creation
  void CharacterSaveState_ReadFromFile(CCharacterSaveState* saveState, PVOID file, u32 unk);

  // Post Event for character initialization
  void CreateCharacter(CCharacter* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0);

  // Post Event for equipment initialization
  void CreateEquipment(CEquipment*, CResourceManager*, u64, u32, u32, u32);
  
  // Post Event for player initialization
  void Level_CharacterInitialize(CCharacter*, CLevel*, CCharacter*, Vector3*, u32);

  // Pre Event for MainMenu Event
  void MainMenuEventPre(CMainMenu*, u32, wstring, bool&);

  // Pre Event for the main GameClient loop
  void GameClient_CreateLevelPre(CGameClient*, wstring unk0, wstring unk1, u32 unk2, u32 unk3, u32 unk4, wstring unk5, bool & calloriginal);
  void GameClient_LoadLevelPre(CGameClient*, bool & calloriginal);
  void GameClient_LoadMapPre(PVOID retval, CGameClient*, u32 unk0, bool & calloriginal);
  // Post Event for the main GameClient loop
  void GameClient_ProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);
  void GameClient_TitleProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);
  
};
