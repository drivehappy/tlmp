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


// Define the offset locations
TLFUNCPTR(SpiderSomeCreate,   PVOID,    __thiscall, (PVOID, u64, u32, bool),                           0x5F03F0);
TLFUNCPTR(EntityInitialize,   PVOID,    __thiscall, (PVOID, PVOID, Vector3, u32),                      0x4F04D0);
TLFUNCPTR(CreateUnitByName,   PVOID,    __thiscall, (PVOID, const wchar_t, const wchar_t, u32, u32),   0x5F0E80);
TLFUNCPTR(SetAlignment,       PVOID,    __thiscall, (PVOID, u32),                                      0x482930);
TLFUNCPTR(SetDestination,     PVOID,    __thiscall, (PVOID, PVOID, float, float),                      0x491840);
TLFUNCPTR(GetPosition,        PVOID,    __thiscall, (PVOID, Vector3, u32),                             0x50B770);
TLFUNCPTR(SetAction,          PVOID,    __thiscall, (PVOID, u32),                                      0x488C30);
TLFUNCPTR(UseSkill,           PVOID,    __thiscall, (PVOID, u64),                                      0x493BC0);
TLFUNCPTR(SetPosition,        PVOID,    __thiscall, (PVOID, const Vector3),                            0x50B7D0);
TLFUNCPTR(AddMinion,          PVOID,    __thiscall, (PVOID, PVOID),                                    0x4A86E0);
TLFUNCPTR(CreateSomething,    PVOID,    __thiscall, (PVOID, u64, u32, u32, u32),                       0x5F09F0);

TLFUNCPTR(SetAttack,          PVOID,    __thiscall, (PVOID, PVOID),                                    0x4916E0);

TLFUNCPTR(OnStrike,           PVOID,    __thiscall, (PVOID, PVOID, PVOID, PVOID, u32, float, float, u32), 0x49EDD0);

TLFUNCPTR(SpiderProcessAI,    PVOID,    __thiscall, (PVOID, PVOID, float),                             0x4D19F0);
TLFUNCPTR(SetAnimation,       PVOID,    __thiscall, (PVOID, u32, bool, float, float, u32),             0x483080);

TLFUNCPTR(DoAttack,           PVOID,    __thiscall, (PVOID),                                           0x48E930);

TLFUNCPTR(ItemInitialize,     PVOID,    __thiscall, (PVOID, PVOID),                                    0x4BCC50);
TLFUNCPTR(ItemDrop,           PVOID,    __thiscall, (PVOID, PVOID, Vector3, bool),                     0x4F0650);
TLFUNCPTR(ItemCreate,         PVOID,    __thiscall, (PVOID, u64, u32, u32, u32),                       0x5EFF50);
TLFUNCPTR(ItemPickup,         PVOID,    __thiscall, (PVOID, PVOID, PVOID),                             0x495720);
TLFUNCPTR(ItemEquip,          PVOID,    __thiscall, (PVOID, PVOID, u32, u32),                          0x4E4340);
TLFUNCPTR(ItemUnequip,        PVOID,    __thiscall, (PVOID, PVOID),                                    0x4E4BF0);
TLFUNCPTR(ItemHide,           PVOID,    __thiscall, (PVOID, PVOID, u32),                               0x4F1E80);

TLFUNCPTR(LoadArea,           PVOID,    __thiscall, (PVOID, wstring, u32, u32, u32, wstring, u32),     0x40CF40);

TLFUNCPTR(AddGoldToPlayer,    PVOID,    __thiscall, (PVOID, u32),                                      0x484F10);

TLFUNCPTR(LevelUp,            PVOID,    __thiscall, (PVOID),                                           0x4D9520);
TLFUNCPTR(LevelUpSilent,      PVOID,    __thiscall, (PVOID),                                           0x48D4A0);

TLFUNCPTR(PetawayTimer,       PVOID,    __thiscall, (PVOID, u32, u32),                                 0x491250);

TLFUNCPTR(InteractWithObject, PVOID,    __thiscall, (PVOID, PVOID),                                    0x4DC360);
TLFUNCPTR(ObjectCreate,       PVOID,    __thiscall, (PVOID, u64),                                      0x4448F0);

TLFUNCPTR(BarrelDestroy,      PVOID,    __thiscall, (PVOID, PVOID),                                    0x4815A0);
TLFUNCPTR(BarrelKnockback,    PVOID,    __thiscall, (PVOID),                                           0x508110);

TLFUNCPTR(CheckgamePaused,    void,     __thiscall, (PVOID),                                           0x40DD50);

TLFUNCPTR(PlayerInitialize,   void,     __thiscall, (PVOID, u32, u32),                                 0x5F0030);

TLFUNCPTR(WndProc,            LRESULT,  __thiscall, (HWND, UINT, WPARAM, LPARAM),                      0x401670);

TLFUNCPTR(GetPlayer,          PVOID,    __thiscall, (void),                                            0x5EFBD0);
TLFUNCPTR(PlayerDied,         void,     __thiscall, (void),                                            0x5458D0);
TLFUNCPTR(PlayerResurrect,    void,     __thiscall, (void),                                            0x56AE30);

// -------------------------------------------------------------------------------- //
// In-place definitions

TLFUNCPTR(ProcessObjects,     void,     __thiscall, (PVOID, PVOID, PVOID, PVOID),                      0x41A780);

TLFUNCPTR(MonsterProcessAI2,  void,     __thiscall, (PVOID, PVOID),                                    0x4D2730);
TLFUNCPTR(MonsterIdle,        void,     __thiscall, (PVOID, PVOID),                                    0x4D2C20);
TLFUNCPTR(MonsterOnHit,       void,     __thiscall, (PVOID, PVOID),                                    0x4D0CE0);
  
TLFUNCPTR(PlayerCtor,         void,     __thiscall, (PVOID),                                           0x4D7D60);

TLFUNCPTR(PlayerSetAction,    void,     __thiscall, (PVOID),                                           0x4D3F80);

TLFUNCPTR(TitleScreenProcess, void,     __thiscall, (PVOID, PVOID, PVOID, PVOID),                      0x40DF50);

TLFUNCPTR(LoadMap,            void,     __thiscall, (PVOID, PVOID),                                    0x418940);

TLFUNCPTR(Random,             void,     __thiscall, (),                                                0x5B6A10);

TLFUNCPTR(Destroy,            void,     __thiscall, (PVOID),                                           0x4F3030);

TLFUNCPTR(EntityReadProp,     void,     __thiscall, (PVOID),                                           0x47DB00);

//TLFUNCPTR(LoadArea,           void,     __thiscall, (/* 18 */),                                        0x40CF40);
// ... and add more later


void TLMP::Initialize()
{
  log("Initialized");

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
  Hook(SetAlignment, _set_alignment_pre, 0, HOOK_THISCALL, 1);

  // Player
  Hook((void*)0x4D7D60, 0, _player_ctor_post, HOOK_THISCALL, 1);
  Hook((void*)0x4D3F80, _player_set_action_pre, 0, HOOK_THISCALL, 1);
  Hook(AddGoldToPlayer, _add_goldtoplayer, 0, HOOK_THISCALL, 1);
  Hook(PlayerInitialize, NULL, _initialize_player_post, HOOK_THISCALL, 2);
	Hook(PlayerDied, _player_died_pre, NULL, HOOK_THISCALL, 0);
	Hook(PlayerResurrect, _player_resurrect_pre, NULL, HOOK_THISCALL, 8);
  Hook(LevelUp, _levelup_pre, 0, HOOK_THISCALL, 0);
	Hook(LevelUpSilent, _levelup_silent_pre, NULL, HOOK_THISCALL, 0);
  Hook(AddMinion, _add_minion_pre, 0, HOOK_THISCALL, 1);

  // Item
  Hook(ItemInitialize, _item_initialize_pre, 0, HOOK_THISCALL, 1);
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
