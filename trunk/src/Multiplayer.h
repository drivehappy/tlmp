#pragma once

#include "tlapi.h"

#include "Logger.h"
using namespace TLAPI;

#include "UI.h"


namespace TLMP
{
  // Debugging client slowdown
  static Timer LevelUpdateTime;
  static map<CCharacter*, Timer> CharacterUpdateTime;

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

  extern bool LevelLoading;

  void WndProcPre(HWND, UINT, WPARAM, LPARAM);

  // Path
  void Path_GetNextNodePre(CPath*, Vector3*, float);
  void Path_GetNextNodePost(CPath*, Vector3*, float);

  // Post Event for character save state creation
  void CharacterSaveState_ReadFromFile(CCharacterSaveState* saveState, PVOID file, u32 unk);

  // Pre Event for character initialization
  void CreateMonsterPre(CMonster* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0, bool & calloriginal);
  void CreateMonsterPost(CMonster* character, CResourceManager* resourceManager, u64 guid, u32 level, bool unk0, bool &);
  void CreatePlayer(CPlayer* character, CResourceManager* resourceManager, wchar_t* type, u32 unk, bool & calloriginal);

  // EffectManager
  void Effect_EffectManagerCreateEffectPre(CEffect* retval, CEffectManager* effectManager);
  void Effect_EffectManagerCreateEffectPost(CEffect* retval, CEffectManager* effectManager);
  void EffectManager_AddEffectToEquipmentPre(CEffectManager*, CEquipment*, CEffect*);
  void EffectManager_AddEffectToEquipmentPost(CEffectManager*, CEquipment*, CEffect*);

  // Effect event, debugging client, it crashes on this function
  void Effect_EffectSomethingPre(CEffect* effect, CEffect* other, bool & calloriginal);
  void Effect_EffectSomethingPost(CEffect* effect, CEffect* other, bool & calloriginal);
  void Effect_Something0Pre(CEffect* effect, u32 unk0, bool &calloriginal);
  void Effect_Effect_ParamCtorPre(CEffect* effect, u32 unk0, bool unk1, bool unk2, float unk3, float unk4, float unk5, bool unk6, bool& calloriginal);
  void Effect_CtorPre(CEffect* effect);
  void Effect_CtorPost(CEffect* effect);
  void Effect_Character_Unk0Pre(CEffect* effect, CCharacter* character, bool unk);
  void Effect_Character_Unk0Post(CEffect* effect, CCharacter* character, bool unk);

  // Pre Event for Monster AI and Idle
  void Monster_Idle(CMonster* monster, float dTime, bool & calloriginal);
  void Monster_ProcessAIPre(CMonster* monster, float dTime, u32 unk0, bool & calloriginal);
  void Monster_ProcessAI2Pre(CMonster* monster, float dTime, u32 unk0, u32 unk1, bool & calloriginal);
  void Monster_ProcessAI3Pre(CMonster* monster, u32 unk0, bool & calloriginal);
  void Monster_GetCharacterClosePost(CCharacter*, CMonster*, float, u32, bool&);

  // Pre Event for Character SetDestination
  void Character_Dtor(CCharacter*);
  void Character_SetAlignmentPre(CCharacter*, u32, bool&);
  void Character_SetAlignmentPost(CCharacter*, u32, bool&);
  void Character_SetActionPre(CCharacter*, u32, bool & calloriginal);
  void Character_SetDestination(CCharacter*, CLevel*, float, float);
  void Character_UseSkillPre(CCharacter*, u64, bool & calloriginal);
  void Character_UseSkillPost(CCharacter*, u64, bool & calloriginal);
  void Character_SetTarget(CCharacter*, CCharacter*, bool & calloriginal);
  void Character_AttackPre(CCharacter*, bool & calloriginal);
  void Character_AttackPost(CCharacter*, bool&);
  void Character_PickupEquipmentPre(CCharacter* character, CEquipment* equipment, CLevel* level, bool&);
  void Character_PickupEquipmentPost(CCharacter* character, CEquipment* equipment, CLevel* level, bool&);
  void Character_Character_UpdatePre(CCharacter*, PVOID, float*, float, bool&);
  void Character_Character_UpdatePost(CCharacter*, PVOID, float*, float, bool&);
  void Character_SetOrientationPre(CCharacter*, Vector3*, float, bool&);
  void Character_UpdateOrientationPre(CCharacter*, float, float, bool&);
  void Character_SetupSkillsPre(CCharacter*, CDataGroup*, u32, bool&);
  void Character_AddSkillPre(CCharacter*, wstring*, u32, bool&);
  void Character_AddSkillPost(CCharacter*, wstring*, u32, bool&);
  void Character_AddSkill2Pre(CCharacter*, wstring, bool&);
  void Character_UpdateHealthPre(CCharacter*, float, bool&);
  void Character_StrikePre(CCharacter*, CLevel*, CCharacter*, PVOID, u32, float, float, u32, bool&);
  void Character_ResurrectPre(CCharacter*, bool&);
  void Character_Update_LevelPre(CCharacter*, CLevel*, float, bool&);
  void Character_Update_CharacterPre(CCharacter*, CCharacter*, bool&);
  void Character_Player_KillMonsterExperiencePre(CCharacter*, CLevel*, CCharacter*, u32, u32, bool&);
  void Character_Player_KillMonsterExperiencePost(CCharacter*, CLevel*, CCharacter*, u32, u32, bool&);
  void Character_KilledPre(CCharacter*, CCharacter*, Ogre::Vector3*, float, u32, bool&);
  void Character_KilledPost(CCharacter*, CCharacter*, Ogre::Vector3*, float, u32, bool&);
  void Player_SwapWeaponsPre(CCharacter*, bool&);

  // Player
  void Player_LevelUpPre(CPlayer*, bool&);
  void Player_LevelUpPost(CPlayer*, bool&);

  // Item Gold
  void ItemGold_CtorPre(CItemGold*, PVOID, CResourceManager*, u32, bool&);
  void ItemGold_CtorPost(CItemGold*, PVOID, CResourceManager*, u32, bool&);

  // Trigger Unit
  void TriggerUnit_TriggeredPre(CTriggerUnit*, CPlayer*, bool&);
  void TriggerUnit_CtorPost(CTriggerUnit*, CLayout*, bool&);
  void TriggerUnit_Triggered2Pre(CTriggerUnit*, CCharacter*, bool&);

  // BaseUnit
  void BaseUnit_AddSkillPre(CBaseUnit*, wstring*, u32, bool&);
  void BaseUnit_AddSkillPost(CBaseUnit*, wstring*, u32, bool&);

  // Breakable
  void Breakable_TriggeredPre(CBreakable*, CPlayer*, bool&);

  // Skill Manager
  void SkillManager_AddSkillPre(CSkillManager*, CSkill*, bool, u32, bool&);
  void SkillManager_SetSkillLevelPre(CSkillManager*, CSkill*, u32, bool&);
  void SkillManager_SetSkillLevelPost(CSkillManager*, CSkill*, u32, bool&);

  // Quest Manager
  void QuestManager_SetQuestCompletedPre(CQuestManager*, CQuest*, CCharacter*, u32, bool&);
  void QuestManager_SetQuestCompletedPost(CQuestManager*, CQuest*, CCharacter*, u32, bool&);

  // Post Event for equipment initialization
  void Equipment_DtorPre(CEquipment*);
  void Equipment_DtorPost(CEquipment*);
  void CreateItemPre(CItem*, CResourceManager*, u64, u32, u32, u32, bool&);
  void CreateItemPost(CItem*, CResourceManager*, u64, u32, u32, u32, bool&);
  void EquipmentInitialize(CEquipment* equipment, CItemSaveState* itemSaveState, bool & calloriginal);
  void EquipmentAddStackCountPost(CEquipment*, u32);
  void EquipmentAddGemPre(CEquipment*, CEquipment*, bool&);
  void EquipmentIdentifyPre(CEquipment*, CPlayer*, CEquipment*, bool&);
  void Equipment_EnchantPre(u32 retval, CEquipment* equipment, u32 unk0, u32 unk1, u32 unk2, bool & calloriginal);

  void EquipmentRefDtorPre(CEquipmentRef*, u32);
  void EquipmentRefDtorPost(CEquipmentRef*, u32);

  // Pre event for seed
  void Global_SetSeedValue0Pre(u32, bool&);
  void Global_SetSeedValue0Post(u32, bool&);
  void Global_SetSeedValue2Pre(u32, bool&);
  void Global_SetSeedValue2Post(u32, bool&);

  // Pre event for enchant menu
  void EnchantMenuEnchantItemPre(CEnchantMenu*, bool&);
  
  // Pre Event for player initialization
  void Level_CharacterInitialize(CCharacter*, CLevel*, CCharacter*, Vector3*, u32, bool&);
  void Level_DropItemPre(CLevel* level, CItem* equipment, Vector3 & position, bool unk0, bool&);
  void Level_DropItemPost(CLevel* level, CItem* equipment, Vector3 & position, bool unk0, bool&);
  void Level_Dtor(CLevel* level, u32, bool&);
  void Level_Ctor(wstring name, CSettings*, CGameClient*, CResourceManager*, PVOID OctreeSM, CSoundManager*, u32, u32, bool&);
  void Level_UpdatePre(CLevel*, Vector3*, u32, float, bool&);
  void Level_UpdatePost(CLevel*, Vector3*, u32, float, bool&);
  void Level_CleanupPre(CLevel*, u32, u32, bool&);
  void Level_CleanupPost(CLevel*, u32, u32, bool&);
  void Level_Level_CharacterKilledCharacterPre(CLevel*, CCharacter*, CCharacter*, Vector3*, u32, bool&);
  void Level_Level_CharacterKilledCharacterPost(CLevel*, CCharacter*, CCharacter*, Vector3*, u32, bool&);
  void Level_Level_RemoveEquipmentPre(CLevel*, CEquipment*, bool&);
  void Level_Level_RemoveEquipmentPost(CLevel*, CEquipment*, bool&);

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
  void GameClient_LoadMapPre(CGameClient*, u32 unk0, u32 unk1, bool & calloriginal);
  void GameClient_LoadMapPost(CGameClient*, u32 unk0, u32 unk1, bool & calloriginal);
  void GameClientSaveGamePre(CGameClient *gameClient, u32 unk0, u32 unk1, bool & callOriginal);
  void GameClientSaveGamePost(CGameClient *gameClient, u32 unk0, u32 unk1, bool & callOriginal);
  void GameClientGamePausedPost(bool& retval, CGameClient *gameClient, bool & calloriginal);
  void GameClient_ChangeLevelPre(CGameClient*, wstring, s32, u32, u32, wstring, u32, bool&);
  void GameClient_ChangeLevelPost(CGameClient*, wstring, s32, u32, u32, wstring, u32, bool&);

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

  // Pre Event for InventoryMenuOpenClose
  void InventoryMenu_OpenClosePre(CInventoryMenu *menu, bool open, bool& calloriginal);
  void InventoryMenu_MouseEventPre(CInventoryMenu*, const CEGUI::MouseEventArgs*, bool&);

  // Helpers
  void SendInventoryAddEquipmentToServer(CCharacter* owner, CEquipment* equipment, u32 slot, u32 unk);
  
};
