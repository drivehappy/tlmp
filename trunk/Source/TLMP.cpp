#include "Version.h"

#include "TLMP.h"
#include "Hook.h"
#include "Offsets.h"
#include "UserInterface.h"

#include "Network.h"

#include "Map.h"
#include "Monster.h"
#include "Various.h"
#include "Player.h"
#include "Item.h"
#include "Object.h"


void TLMP::Initialize()
{
  printf("Blah1\n");
  log("Initialized");
  printf("Blah1\n");

  if (!ComputeVersion()) {
    ExitProcess(0);
  }

  PatchProcess();
  HookFunctions();

  log("Starting network thread...");
  Network::StartReceivingMessages();
}

void TLMP::Shutdown()
{
  log("Shutdown");
}

void TLMP::PatchProcess()
{
  // patch steam
	PatchJMP(0x5EF593,0x5EF62E);

	// always generate new uhm.. monsters? when zoning
	PatchJMP(0x416A8B,0x416ADB);

	//patch_jmp(0x4AB397,0x4AB4D2);
	PatchJMP(0x488D6D,0x488E6B);
}

void TLMP::HookFunctions()
{
  log("Hooking functions...");

  // Map
  Hook((void*)0x418940, _load_map_pre, _load_map_post, HOOK_THISCALL, 2);
  Hook((void*)0x40CF40, _load_area_pre, 0, HOOK_THISCALL, 18);
  Hook((void*)0x419800, _on_load_area_pre, _on_load_area_post, HOOK_THISCALL, 0);

  // Various
  Hook(SetDestination, _set_destination_pre, 0, HOOK_THISCALL, 3);
  Hook(OnStrike, _on_strike_pre, _on_strike_post, HOOK_THISCALL, 7);
	Hook((void*)0x5B6A10, 0, _random_post, HOOK_THISCALL, 0);
	Hook((void*)0x4F3030, _destroy_pre, 0, HOOK_THISCALL, 1);

  // Monster
  Hook(SpiderSomeCreate, _spider_some_create_pre, _spider_some_create_post, HOOK_THISCALL, 4);
  Hook(SpiderProcessAI, _spider_process_ai_pre, 0, HOOK_THISCALL, 2);
 	Hook((void*)0x4D2730, _spider_process_ai2_pre, 0, HOOK_THISCALL, 2);
  Hook((void*)0x4D2C20, _spider_idle_pre, 0, HOOK_THISCALL, 2);
 	Hook((void*)0x4D0CE0, _spider_on_hit_pre, 0, HOOK_THISCALL, 2);

  // Player
  Hook((void*)0x4D7D60, 0, _player_ctor_post, HOOK_THISCALL, 1);
  Hook((void*)0x4D3F80, _player_set_action_pre, 0, HOOK_THISCALL, 1);
  Hook(AddGoldToPlayer, _add_goldtoplayer, 0, HOOK_THISCALL, 1);
  Hook(PlayerInitialize, NULL, _initialize_player_post, HOOK_THISCALL, 2);
	Hook(PlayerDied, _player_died_pre, NULL, HOOK_THISCALL, 0);
	Hook(PlayerResurrect, _player_resurrect_pre, NULL, HOOK_THISCALL, 8);
  Hook(LevelUp, _levelup_pre, 0, HOOK_THISCALL, 0);
	Hook(LevelUpSilent, _levelup_silent_pre, NULL, HOOK_THISCALL, 0);

  // Item
  Hook((void*)ItemInitialize, _item_initialize_pre, 0, HOOK_THISCALL, 1);
  Hook(ItemCreate, _item_create_pre, _item_create_post, HOOK_THISCALL, 5);
	Hook(ItemDrop, _item_drop_pre, 0, HOOK_THISCALL, 3);
	Hook(ItemPickup, _item_pick_up_pre, _item_pick_up_post ,HOOK_THISCALL, 2);
	Hook(ItemEquip, _item_equip_pre, _item_equip_post, HOOK_THISCALL, 3);
	Hook(ItemUnequip, _item_unequip_pre, 0, HOOK_THISCALL, 1);

  // Object
  Hook(InteractWithObject, _interact_with_object, 0, HOOK_THISCALL, 1);
	Hook(ObjectCreate, _object_create_pre, _object_create_post, HOOK_THISCALL, 1);


  log("Hooking complete");
}
