#pragma once

#include "tlapi.h"

#include "Logger.h"
using namespace TLAPI;

#include "UI.h"


namespace TLMP
{

  void SetupNetwork();

  // Server Events
  void ServerOnClientConnected(void *);
  // --

  // Direct Hook function for handling rendering the window without focus
  static void OgreIsActive STDARG
  {
    e->calloriginal = false;
    e->retval = 1;
  };

  // Post Event for character save state creation
  void CharacterSaveState_ReadFromFile(CCharacterSaveState* saveState, PVOID file, u32 unk);

  // Pre Event for character initialization
  void CreateMonster(CMonster* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0, bool & calloriginal);

  // Pre Event for Monster AI and Idle
  void Monster_Idle(CMonster* monster, float dTime, bool & calloriginal);
  void Monster_ProcessAI(CMonster* monster, float dTime, bool & calloriginal);

  // Pre Event for Character SetDestination
  void Character_Ctor(CCharacter*);
  void Character_SetAlignment(CCharacter*, u32);
  void Character_SetDestination(CCharacter*, CLevel*, float, float);
  void Character_PickupEquipmentPre(CCharacter* character, CEquipment* equipment, CLevel* level, bool&);
  void Character_PickupEquipmentPost(CCharacter* character, CEquipment* equipment, CLevel* level, bool&);

  // Post Event for equipment initialization
  void Equipment_Ctor(CEquipment*);
  void CreateEquipmentPre(CEquipment*, CResourceManager*, u64, u32, u32, u32, bool&);
  void CreateEquipmentPost(CEquipment*, CResourceManager*, u64, u32, u32, u32, bool&);
  void EquipmentInitialize(CEquipment* equipment, CItemSaveState* itemSaveState, bool & calloriginal);
  
  // Pre Event for player initialization
  void Level_CharacterInitialize(CCharacter*, CLevel*, CCharacter*, Vector3*, u32, bool&);
  void Level_DropEquipmentPre(CLevel* level, CEquipment* equipment, Vector3 & position, bool unk0, bool&);
  void Level_DropEquipmentPost(CLevel* level, CEquipment* equipment, Vector3 & position, bool unk0, bool&);

  // Event for Inventory
  void Inventory_AddEquipment(CEquipment*, CInventory*, CEquipment*, u32, u32, bool&);
  void Inventory_RemoveEquipmentPre(CInventory*, CEquipment*);

  // Pre Event for MainMenu Event
  void MainMenuEventPre(CMainMenu*, u32, wstring, bool&);

  // Event for Game
  void Game_Ctor(CGame* game);

  // Pre Event for the main GameClient loop
  void GameClient_Ctor(CGameClient*);
  void GameClient_CreateLevelPre(CGameClient*, wstring unk0, wstring unk1, u32 unk2, u32 unk3, u32 unk4, wstring unk5, bool & calloriginal);
  void GameClient_LoadLevelPre(CGameClient*, bool & calloriginal);
  void GameClient_LoadMapPre(PVOID retval, CGameClient*, u32 unk0, bool & calloriginal);

  // Post Event for the main GameClient loop
  void GameClient_ProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);
  void GameClient_TitleProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);

  // Helpers
  void SendInventoryAddEquipmentToServer(CCharacter* owner, CEquipment* equipment, u32 slot, u32 unk);
  
};
