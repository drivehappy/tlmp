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
  void ServerOnClientDisconnect(void *arg);
  // --

  // Client Events
  void ClientOnConnect(void *);
  // --

  // Direct Hook function for handling rendering the window without focus
  static void OgreIsActive STDARG
  {
    e->calloriginal = false;
    e->retval = 1;
  };

  static void OgreAddResourceLocation STDARG;

  // Post Event for character save state creation
  void CharacterSaveState_ReadFromFile(CCharacterSaveState* saveState, PVOID file, u32 unk);

  // Pre Event for character initialization
  void CreateMonster(CMonster* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0, bool & calloriginal);
  void CreatePlayer(CPlayer* character, CResourceManager* resourceManager, wchar_t* type, u32 unk, bool & calloriginal);

  // Pre Event for Monster AI and Idle
  void Monster_Idle(CMonster* monster, float dTime, bool & calloriginal);
  void Monster_ProcessAI(CMonster* monster, float dTime, bool & calloriginal);

  // Pre Event for Character SetDestination
  void Character_Dtor(CCharacter*);
  void Character_SetAlignment(CCharacter*, u32);
  void Character_SetActionPre(CCharacter*, u32, bool & calloriginal);
  void Character_SetDestination(CCharacter*, CLevel*, float, float);
  void Character_UseSkillPre(CCharacter*, u64, bool & calloriginal);
  void Character_UseSkillPost(CCharacter*, u64, bool & calloriginal);
  void Character_SetTarget(CCharacter*, CCharacter*, bool & calloriginal);
  void Character_AttackPre(CCharacter*, bool & calloriginal);
  void Character_PickupEquipmentPre(CCharacter* character, CEquipment* equipment, CLevel* level, bool&);
  void Character_PickupEquipmentPost(CCharacter* character, CEquipment* equipment, CLevel* level, bool&);
  void Character_Character_UpdatePre(CCharacter*, PVOID, float*, float, bool&);
  void Character_SetOrientationPre(CCharacter*, Vector3*, float, bool&);
  void Character_SetupSkillsPre(CCharacter*, CDataGroup*, u32, bool&);
  void Character_AddSkillPre(CCharacter*, wstring*, u32, bool&);

  // Skill Manager
  void SkillManager_AddSkillPre(CSkillManager*, CSkill*, u32, u32, bool&);

  // Post Event for equipment initialization
  void Equipment_Dtor(CEquipment*);
  void CreateEquipmentPre(CEquipment*, CResourceManager*, u64, u32, u32, u32, bool&);
  void CreateEquipmentPost(CEquipment*, CResourceManager*, u64, u32, u32, u32, bool&);
  void EquipmentInitialize(CEquipment* equipment, CItemSaveState* itemSaveState, bool & calloriginal);
  void EquipmentAddStackCountPost(CEquipment*, u32);
  void EquipmentAddGemPre(CEquipment*, CEquipment*, bool&);

  // Pre event for enchant menu
  void EnchantMenuEnchantItemPre(CEnchantMenu*, bool&);
  
  // Pre Event for player initialization
  void Level_CharacterInitialize(CCharacter*, CLevel*, CCharacter*, Vector3*, u32, bool&);
  void Level_DropEquipmentPre(CLevel* level, CEquipment* equipment, Vector3 & position, bool unk0, bool&);
  void Level_DropEquipmentPost(CLevel* level, CEquipment* equipment, Vector3 & position, bool unk0, bool&);

  // Event for Inventory
  void Inventory_AddEquipmentPre(CEquipment*, CInventory*, CEquipment*, u32&, u32, bool&);
  void Inventory_AddEquipmentPost(CEquipment*, CInventory*, CEquipment*, u32&, u32, bool&);
  void Inventory_RemoveEquipmentPre(CInventory*, CEquipment*);
  void Inventory_RemoveEquipmentPost(CInventory* inventory, CEquipment* equipment);

  // Pre Event for MainMenu Event
  void MainMenuEventPre(CMainMenu*, u32, wstring, bool&);

  // Event for Game
  void Game_Ctor(CGame* game);

  // Pre Event for the main GameClient loop
  void GameClient_Ctor(CGameClient*);
  void GameClient_CreateLevelPre(CGameClient*, wstring unk0, wstring unk1, u32 unk2, u32 unk3, u32 unk4, wstring unk5, bool & calloriginal);
  void GameClient_CreateLevelPost(CGameClient* client, wstring unk0, wstring unk1, u32 unk2, u32 unk3, u32 unk4, wstring unk5, bool & calloriginal);
  void GameClient_LoadLevelPre(CGameClient*, bool & calloriginal);
  void GameClient_LoadLevelPost(CGameClient* client, bool & calloriginal);
  void GameClient_LoadMapPre(PVOID retval, CGameClient*, u32 unk0, bool & calloriginal);
  void GameClientSaveGamePre(CGameClient *gameClient, u32 unk0, u32 unk1, bool & callOriginal);
  void GameClientGamePausedPost(bool& retval, CGameClient *gameClient, bool & calloriginal);

  // Pre Event for suppressing the paused game
  void GameUI_TriggerPausePre(CGameUI *gameUI, bool & calloriginal);

  // Pre Event for the GameUI Keyboard Input
  void GameUI_HandleKeyboardInputPre(CGameUI*, u32, u32, u32, bool&);
  void GameUI_HandleKeyboardInputPost(CGameUI*, u32, u32, u32, bool&);

  // Post Event for GameUI Window Resize
  void GameUI_WindowResizedPost(CGameUI*, bool&);

  // Pre Event for KeyManager input
  void KeyManager_HandleInputPre(CKeyManager*, u32, u32, bool&);

  // Pre Event for MouseManager input
  void MouseManager_HandleInputPre(CMouseManager*, u32 wParam, u32 mouseButton, bool& calloriginal);

  // Post Event for the main GameClient loop
  void GameClient_ProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);
  void GameClient_TitleProcessObjects(CGameClient *client, float dTime, PVOID unk1, PVOID unk2);

  // Helpers
  void SendInventoryAddEquipmentToServer(CCharacter* owner, CEquipment* equipment, u32 slot, u32 unk);
  
};
