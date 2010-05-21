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

#include "Test.h"

u32 exeBaseReal = (u32)GetModuleHandle("Torchlight.exe");

// Define the offset locations
TLFUNCPTR(SpiderSomeCreate,   PVOID,    __thiscall, (PVOID, u64, u32, bool),                           0x5FBB70);     // 1.15
TLFUNCPTR(EntityInitialize,   PVOID,    __thiscall, (PVOID, PVOID, Vector3*, u32),                     0x4F2EF0);     // 1.15
TLFUNCPTR(CreateUnitByName,   PVOID,    __thiscall, (PVOID, const wchar_t*, const wchar_t*, u32, u32), 0x5FC600);     // 1.15
TLFUNCPTR(SetAlignment,       PVOID,    __thiscall, (PVOID, u32),                                      0x4839E0);     // 1.15
TLFUNCPTR(SetDestination,     PVOID,    __thiscall, (PVOID, PVOID, float, float),                      0x492AD0);     // 1.15
TLFUNCPTR(GetPosition,        PVOID,    __thiscall, (PVOID, Vector3, u32),                             0x50E3F0);     // 1.15
TLFUNCPTR(SetAction,          PVOID,    __thiscall, (PVOID, u32),                                      0x489E50);     // 1.15
TLFUNCPTR(UseSkill,           PVOID,    __thiscall, (PVOID, u64),                                      0x494E50);     // 1.15
TLFUNCPTR(SetPosition,        PVOID,    __thiscall, (PVOID, const Vector3),                            0x50E450);     // 1.15
TLFUNCPTR(AddMinion,          PVOID,    __thiscall, (PVOID, PVOID),                                    0x4A9B20);     // 1.15
TLFUNCPTR(CreateSomething,    PVOID,    __thiscall, (PVOID, u64, u32, u32, u32),                       0x5FC170);     // 1.15

TLFUNCPTR(SetAttack,          PVOID,    __thiscall, (PVOID, PVOID),                                    0x492970);     // 1.15


// !!! CHECK MY ARG COUNT, MAYBE +1/-1
TLFUNCPTR(OnStrike,           PVOID,    __thiscall, (PVOID, PVOID, PVOID, PVOID, u32, float, float, u32), 0x4A0190);  // 1.15

TLFUNCPTR(SpiderProcessAI,    PVOID,    __thiscall, (PVOID, PVOID, float),                             0x4D36F0);     // 1.15
TLFUNCPTR(SetAnimation,       PVOID,    __thiscall, (PVOID, u32, bool, float, float, u32),             0x4841F0);     // 1.15

TLFUNCPTR(DoAttack,           PVOID,    __thiscall, (PVOID),                                           0x48FBD0);     // 1.15

TLFUNCPTR(ItemInitialize,     PVOID,    __thiscall, (PVOID, PVOID),                                    0x4BE250);     // 1.15
TLFUNCPTR(ItemDrop,           PVOID,    __thiscall, (PVOID, PVOID, Vector3 &, bool),                   0x4F3070);     // 1.15
TLFUNCPTR(ItemCreate,         PVOID,    __thiscall, (PVOID, u64, u32, u32, u32),                       0x5FB6D0);     // 1.15
TLFUNCPTR(ItemPickup,         PVOID,    __thiscall, (PVOID, PVOID, PVOID),                             0x4969B0);     // 1.15
TLFUNCPTR(ItemEquip,          PVOID,    __thiscall, (PVOID, PVOID, u32, u32),                          0x4E6CE0);     // 1.15
TLFUNCPTR(ItemUnequip,        PVOID,    __thiscall, (PVOID, PVOID),                                    0x4E7610);     // 1.15
TLFUNCPTR(ItemHide,           PVOID,    __thiscall, (PVOID, PVOID, u32),                               0x4F48C0);     // 1.15

TLFUNCPTR(ChangeLevel,        PVOID,    __thiscall, (PVOID, wstring, u32, u32, u32, wstring, u32),     0x40CF60);     // 1.15

TLFUNCPTR(AddGoldToPlayer,    PVOID,    __thiscall, (PVOID, u32),                                      0x4860B0);     // 1.15

TLFUNCPTR(LevelUp,            PVOID,    __thiscall, (PVOID),                                           0x4DB840);     // 1.15
TLFUNCPTR(LevelUpSilent,      PVOID,    __thiscall, (PVOID),                                           0x48E730);     // 1.15

TLFUNCPTR(PetawayTimer,       PVOID,    __thiscall, (PVOID, u32, u32),                                 0x4924E0);     // 1.15

TLFUNCPTR(InteractWithObject, PVOID,    __thiscall, (PVOID, PVOID),                                    0x4DE6C0);     // 1.15

// !!! THIS COULD BE WRONG, THERE'S A BUNCH MATCHING -- I NEED TO DOUBLE CHECK THIS FUNC ANYWAYS - drivehappy
TLFUNCPTR(ObjectCreate,       PVOID,    __thiscall, (PVOID, u64),                                      0x446390);     // 1.15

TLFUNCPTR(BarrelDestroy,      PVOID,    __thiscall, (PVOID, PVOID),                                    0x482600);     // 1.15
TLFUNCPTR(BarrelKnockback,    PVOID,    __thiscall, (PVOID),                                           0x50ACF0);     // 1.15

TLFUNCPTR(CheckgamePaused,    void,     __thiscall, (PVOID),                                           0x40DD70);     // 1.15

TLFUNCPTR(PlayerInitialize,   void,     __thiscall, (PVOID, u32, u32),                                 0x5FB5F0);     // 1.15  -- extra argument needed

TLFUNCPTR(WndProc,            LRESULT,  __thiscall, (HWND, UINT, WPARAM, LPARAM),                      0x4016B0);     // 1.15

TLFUNCPTR(GetPlayer,          PVOID,    __thiscall, (void),                                            0x5FB330);     // 1.15
TLFUNCPTR(PlayerDied,         void,     __thiscall, (void),                                            0x548270);     // 1.15
TLFUNCPTR(PlayerResurrect,    void,     __thiscall, (void),                                            0x56E000);     // 1.15

// -------------------------------------------------------------------------------- //
// In-place definitions

TLFUNCPTR(ProcessObjects,     void,     __thiscall, (PVOID, PVOID, PVOID, PVOID),                      0x41A790);     // 1.15

TLFUNCPTR(MonsterProcessAI2,  void,     __thiscall, (PVOID, PVOID),                                    0x4D4450);     // 1.15  -- extra argument needed
TLFUNCPTR(MonsterProcessAI3,  void,     __thiscall, (PVOID, u32),                                      0x498670);     // 1.15
TLFUNCPTR(MonsterIdle,        void,     __thiscall, (PVOID, PVOID),                                    0x4D4950);     // 1.15  -- extra argument needed
TLFUNCPTR(MonsterOnHit,       void,     __thiscall, (PVOID, PVOID),                                    0x4D29E0);     // 1.15
  
TLFUNCPTR(PlayerCtor,         void,     __thiscall, (PVOID),                                           0x4DA160);     // 1.15

TLFUNCPTR(PlayerSetAction,    void,     __thiscall, (PVOID),                                           0x4D5D00);     // 1.15

TLFUNCPTR(TitleScreenProcess, void,     __thiscall, (PVOID, PVOID, PVOID, PVOID),                      0x40DF70);     // 1.15

TLFUNCPTR(LoadMap,            void,     __thiscall, (PVOID, u32),                                      0x4188E0);     // 1.15

TLFUNCPTR(Random,             void,     __thiscall, (),                                                0x5BA660);     // 1.15

TLFUNCPTR(Destroy,            void,     __thiscall, (PVOID),                                           0x4F5AA0);     // 1.15

TLFUNCPTR(EntityReadProp,     void,     __thiscall, (PVOID),                                           0x47EAF0);     // 1.15

//TLFUNCPTR(LoadArea,           void,     __thiscall, (/* 18 */),                                        0x40CF20);
// ... and add more later


void TLMP::Initialize()
{
  log("Initialized");

  /*
  if (!ComputeVersion()) {
    log("Error: Unknown version, not patching in multiplayer.");
    return;
  }
  */

  PatchProcess();
  HookFunctions();
}

void TLMP::Shutdown()
{
  log("Shutdown");
}

void TLMP::PatchProcess()
{
  // patch steam
  //PatchJMP(0x5FA07D, 0x5FA5D3);

  // always generate new uhm.. monsters? when zoning
  PatchJMP(EXEOFFSET(0x4169D1), EXEOFFSET(0x416A21));   // v1.15

  //patch_jmp(0x4AB397,0x4AB4D2);

  // What is this patch for?
  PatchJMP(EXEOFFSET(0x489F8D), EXEOFFSET(0x48A08B));   // v1.15
}

void TLMP::HookFunctions()
{
  log("Hooking functions...");

  // Map
  Hook(LoadMap, _load_map_pre, _load_map_post, HOOK_THISCALL, 2);
  Hook(ChangeLevel, _change_level_pre, _change_level_post, HOOK_THISCALL, 18);
  Hook((void*)EXEOFFSET(0x4197E0), _on_load_area_pre, _on_load_area_post, HOOK_THISCALL, 0);    // v1.15

  log("Map Done");

  // Various
  Hook(SetDestination, _set_destination_pre, 0, HOOK_THISCALL, 3);
  Hook(OnStrike, _on_strike_pre, _on_strike_post, HOOK_THISCALL, 7);
  Hook(Random, 0, _random_post, HOOK_THISCALL, 0);
  Hook(Destroy, _destroy_pre, 0, HOOK_THISCALL, 1);
  Hook(ProcessObjects, _process_objects_pre, _process_objects_pre, HOOK_THISCALL, 4);
  Hook(WndProc, _wnd_proc_pre, NULL, HOOK_STDCALL, 5);

  log("Various Done");

  // Monster
  Hook(SpiderSomeCreate, _spider_some_create_pre, _spider_some_create_post, HOOK_THISCALL, 4);
  Hook(SpiderProcessAI, _spider_process_ai_pre, 0, HOOK_THISCALL, 2);
  
  // Broken in v1.15 -- Look into this
  // Updated arg count from 2 to 3... check what was added
  Hook(MonsterProcessAI2, _spider_process_ai2_pre, 0, HOOK_THISCALL, 3);
  
  Hook(MonsterProcessAI3, _spider_process_ai3_pre, _spider_process_ai3_post, HOOK_THISCALL, 1);
  
  // Broken in v1.15 -- Look into this
  // Updated arg count from 2 to 3... check what was added
  Hook(MonsterIdle, _spider_idle_pre, 0, HOOK_THISCALL, 3);

  Hook(MonsterOnHit, _spider_on_hit_pre, 0, HOOK_THISCALL, 2);
  Hook(SetAlignment, _set_alignment_pre, 0, HOOK_THISCALL, 1);

  log("Monster Done");

  // Entity
  Hook(EntityInitialize, _entity_initialize_pre, _entity_initialize_post, HOOK_THISCALL, 3);

  // Player
  Hook(PlayerCtor, 0, _player_ctor_post, HOOK_THISCALL, 1);
  Hook(PlayerSetAction, _player_set_action_pre, 0, HOOK_THISCALL, 1);
  Hook(AddGoldToPlayer, _add_goldtoplayer, 0, HOOK_THISCALL, 1);
  
  // Broken in v1.15 -- Look into this
  // Updated arg count from 2 to 3... check what was added
  Hook(PlayerInitialize, _initialize_player_pre, _initialize_player_post, HOOK_THISCALL, 3);
  
  Hook(PlayerDied, _player_died_pre, NULL, HOOK_THISCALL, 0);
  Hook(PlayerResurrect, _player_resurrect_pre, NULL, HOOK_THISCALL, 8);
  Hook(LevelUp, _levelup_pre, 0, HOOK_THISCALL, 0);
  Hook(LevelUpSilent, _levelup_silent_pre, NULL, HOOK_THISCALL, 0);
  Hook(AddMinion, _add_minion_pre, 0, HOOK_THISCALL, 1);

  log("Player Done");

  // Item
  Hook(ItemInitialize, _item_initialize_pre, 0, HOOK_THISCALL, 1);
  Hook(ItemCreate, _item_create_pre, _item_create_post, HOOK_THISCALL, 5);
  Hook(ItemDrop, _item_drop_pre, 0, HOOK_THISCALL, 3);
  Hook(ItemPickup, _item_pick_up_pre, _item_pick_up_post ,HOOK_THISCALL, 2);
  Hook(ItemEquip, _item_equip_pre, _item_equip_post, HOOK_THISCALL, 3);
  Hook(ItemUnequip, _item_unequip_pre, 0, HOOK_THISCALL, 1);

  log("Item Done");

  // Object
  Hook(InteractWithObject, _interact_with_object, 0, HOOK_THISCALL, 1);
  Hook(ObjectCreate, _object_create_pre, _object_create_post, HOOK_THISCALL, 1);
  log("Object Done");

  // Ogre
  Hook(GetProcAddress(GetModuleHandle("OgreMain.dll"), "?isActive@RenderWindow@Ogre@@UBE_NXZ"), _ogre_is_active, 0, HOOK_THISCALL, 0);
  log("OGRE Done");

  log("Hooking complete");

  // Unknowns, Testings
  // This one handles bypassing monster creation as well (I think this is some sort of initial
  // monster load from file, while the rest are generated at runtime).
  // This calls the spider_create, but doesn't handle when it returns null.
  Hook((PVOID)EXEOFFSET(0x4F3960), test0_pre, test0_post, HOOK_THISCALL, 5);    // v1.15

  // Doesn't work, don't use
  //Hook(CreateUnitByName, test1_pre, test1_post, HOOK_THISCALL, 5);

  log("Unknowns/Tests");

  // Handles item loading and positioning of objects (if we don't call org then:
  //   player appears to start at (0,0), no items are loaded, but it does load the level the player is at
  Hook((PVOID)EXEOFFSET(0x4AA580), test2_pre, test2_post, HOOK_THISCALL, 1);

  // True level load?
  Hook((PVOID)EXEOFFSET(0x4FC9F0), test3_pre, test3_post, HOOK_THISCALL, 11);

  // More level loading tests -- appears to be saving items on the previous level
  Hook((PVOID)EXEOFFSET(0x4E1210), test4_pre, test4_post, HOOK_THISCALL, 3);

  // dengus' unknown/weird function
  // I'm seeing this getting called 7 times per frame on the main menu,
  // although it doesn't do it's memmove processing
  Hook((PVOID)EXEOFFSET(0x5B1DA0), test5_pre, test5_post, HOOK_THISCALL, 5);

  // delete(void*)
  Hook((PVOID)EXEOFFSET(0x6059B8), test6_pre, test6_post, HOOK_CDECL, 1);

  // new(void*)
  Hook((PVOID)EXEOFFSET(0x605B54), test7_pre, test7_post, HOOK_CDECL, 1);
  //Hook((PVOID)EXEOFFSET(0x605954), test7_pre, test7_post, HOOK_CDECL, 1);
  
  // Player ctor?
  Hook((PVOID)EXEOFFSET(0x4DB820), test8_pre, test8_post, HOOK_THISCALL, 1);
  
  // CCharacterSaveSlot ctor
  Hook((PVOID)EXEOFFSET(0x4AE350), test9_pre, NULL, HOOK_THISCALL, 1);
  
  // CCharacter ctor
  Hook((PVOID)EXEOFFSET(0x4A8CE0), test10_pre, NULL, HOOK_THISCALL, 1);
  
  // CItemGold ctor
  Hook((PVOID)EXEOFFSET(0x4D0730), testItemGold_pre, NULL, HOOK_THISCALL, 1);
  
  // CItem ctor
  Hook((PVOID)EXEOFFSET(0x4CFC20), testItem_pre, NULL, HOOK_THISCALL, 1);
}
