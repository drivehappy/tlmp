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
TLFUNCPTR(SpiderSomeCreate,   PVOID,    __thiscall, (PVOID, u64, u32, bool),                           0x5FB460);
TLFUNCPTR(EntityInitialize,   PVOID,    __thiscall, (PVOID, PVOID, Vector3*, u32),                     0x4F2720);
TLFUNCPTR(CreateUnitByName,   PVOID,    __thiscall, (PVOID, const wchar_t*, const wchar_t*, u32, u32), 0x5FBEF0);
TLFUNCPTR(SetAlignment,       PVOID,    __thiscall, (PVOID, u32),                                      0x483690);
TLFUNCPTR(SetDestination,     PVOID,    __thiscall, (PVOID, PVOID, float, float),                      0x4926E0);
TLFUNCPTR(GetPosition,        PVOID,    __thiscall, (PVOID, Vector3, u32),                             0x50D920);
TLFUNCPTR(SetAction,          PVOID,    __thiscall, (PVOID, u32),                                      0x489A90);
TLFUNCPTR(UseSkill,           PVOID,    __thiscall, (PVOID, u64),                                      0x494A50);
TLFUNCPTR(SetPosition,        PVOID,    __thiscall, (PVOID, const Vector3),                            0x50D980);
TLFUNCPTR(AddMinion,          PVOID,    __thiscall, (PVOID, PVOID),                                    0x4A9610);
TLFUNCPTR(CreateSomething,    PVOID,    __thiscall, (PVOID, u64, u32, u32, u32),                       0x5FBA60);

TLFUNCPTR(SetAttack,          PVOID,    __thiscall, (PVOID, PVOID),                                    0x492580);


// !!! CHECK MY ARG COUNT, MAYBE +1/-1
TLFUNCPTR(OnStrike,           PVOID,    __thiscall, (PVOID, PVOID, PVOID, PVOID, u32, float, float, u32), 0x49FC80);

TLFUNCPTR(SpiderProcessAI,    PVOID,    __thiscall, (PVOID, PVOID, float),                             0x4D3040);
TLFUNCPTR(SetAnimation,       PVOID,    __thiscall, (PVOID, u32, bool, float, float, u32),             0x483EA0);

TLFUNCPTR(DoAttack,           PVOID,    __thiscall, (PVOID),                                           0x48F7F0);

TLFUNCPTR(ItemInitialize,     PVOID,    __thiscall, (PVOID, PVOID),                                    0x4BDD10);
TLFUNCPTR(ItemDrop,           PVOID,    __thiscall, (PVOID, PVOID, Vector3, bool),                     0x4F28A0);
TLFUNCPTR(ItemCreate,         PVOID,    __thiscall, (PVOID, u64, u32, u32, u32),                       0x5FAFC0);
TLFUNCPTR(ItemPickup,         PVOID,    __thiscall, (PVOID, PVOID, PVOID),                             0x4965B0);
TLFUNCPTR(ItemEquip,          PVOID,    __thiscall, (PVOID, PVOID, u32, u32),                          0x4E6510);
TLFUNCPTR(ItemUnequip,        PVOID,    __thiscall, (PVOID, PVOID),                                    0x4E6E40);
TLFUNCPTR(ItemHide,           PVOID,    __thiscall, (PVOID, PVOID, u32),                               0x4F40F0);

TLFUNCPTR(LoadArea,           PVOID,    __thiscall, (PVOID, wstring, u32, u32, u32, wstring, u32),     0x40CF20);

TLFUNCPTR(AddGoldToPlayer,    PVOID,    __thiscall, (PVOID, u32),                                      0x485D60);

TLFUNCPTR(LevelUp,            PVOID,    __thiscall, (PVOID),                                           0x4DB0C0);
TLFUNCPTR(LevelUpSilent,      PVOID,    __thiscall, (PVOID),                                           0x48E360);

TLFUNCPTR(PetawayTimer,       PVOID,    __thiscall, (PVOID, u32, u32),                                 0x4920F0);

TLFUNCPTR(InteractWithObject, PVOID,    __thiscall, (PVOID, PVOID),                                    0x4DDF40);

// !!! THIS COULD BE WRONG, THERE'S A BUNCH MATCHING -- I NEED TO DOUBLE CHECK THIS FUNC ANYWAYS - drivehappy
TLFUNCPTR(ObjectCreate,       PVOID,    __thiscall, (PVOID, u64),                                      0x446650);

TLFUNCPTR(BarrelDestroy,      PVOID,    __thiscall, (PVOID, PVOID),                                    0x4822C0);
TLFUNCPTR(BarrelKnockback,    PVOID,    __thiscall, (PVOID),                                           0x50A220);

TLFUNCPTR(CheckgamePaused,    void,     __thiscall, (PVOID),                                           0x40DD30);

TLFUNCPTR(PlayerInitialize,   void,     __thiscall, (PVOID, u32, u32),                                 0x5FB0A0);

TLFUNCPTR(WndProc,            LRESULT,  __thiscall, (HWND, UINT, WPARAM, LPARAM),                      0x401680);

TLFUNCPTR(GetPlayer,          PVOID,    __thiscall, (void),                                            0x5FAC20);
TLFUNCPTR(PlayerDied,         void,     __thiscall, (void),                                            0x547B10);
TLFUNCPTR(PlayerResurrect,    void,     __thiscall, (void),                                            0x56D810);

// -------------------------------------------------------------------------------- //
// In-place definitions

TLFUNCPTR(ProcessObjects,     void,     __thiscall, (PVOID, PVOID, PVOID, PVOID),                      0x41A6F0);

TLFUNCPTR(MonsterProcessAI2,  void,     __thiscall, (PVOID, PVOID),                                    0x4D3D80);
TLFUNCPTR(MonsterProcessAI3,  void,     __thiscall, (PVOID, u32),                                      0x498270);
TLFUNCPTR(MonsterIdle,        void,     __thiscall, (PVOID, PVOID),                                    0x4D4270);
TLFUNCPTR(MonsterOnHit,       void,     __thiscall, (PVOID, PVOID),                                    0x4D2330);
  
TLFUNCPTR(PlayerCtor,         void,     __thiscall, (PVOID),                                           0x4D99E0);

TLFUNCPTR(PlayerSetAction,    void,     __thiscall, (PVOID),                                           0x4D55E0);

TLFUNCPTR(TitleScreenProcess, void,     __thiscall, (PVOID, PVOID, PVOID, PVOID),                      0x40DF30);

TLFUNCPTR(LoadMap,            void,     __thiscall, (PVOID, PVOID),                                    0x418840);

TLFUNCPTR(Random,             void,     __thiscall, (),                                                0x5B9EF0);

TLFUNCPTR(Destroy,            void,     __thiscall, (PVOID),                                           0x4F5070);

TLFUNCPTR(EntityReadProp,     void,     __thiscall, (PVOID),                                           0x47E7A0);

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

  //SetupProcessBase();
  PatchProcess();
  HookFunctions();

  log("Starting network thread...");
  Network::StartReceivingMessages();
}

void TLMP::Shutdown()
{
  log("Shutdown");
}

void TLMP::SetupProcessBase()
{
  HMODULE hMod = GetModuleHandle(NULL);
  FARPROC addr = GetProcAddress(hMod, NULL);

//#define EXEBASEREAL #addr
}

void TLMP::PatchProcess()
{
  // patch steam
  //PatchJMP(0x5FA07D, 0x5FA5D3);

  // always generate new uhm.. monsters? when zoning
  PatchJMP(EXEOFFSET(0x416931), EXEOFFSET(0x416981));

  //patch_jmp(0x4AB397,0x4AB4D2);

  // What is this patch for?
  PatchJMP(EXEOFFSET(0x489BCD), EXEOFFSET(0x489CCB));
}

void TLMP::HookFunctions()
{
  log("Hooking functions...");

  // Map
  Hook(LoadMap, _load_map_pre, _load_map_post, HOOK_THISCALL, 2);
  Hook(LoadArea, _load_area_pre, _load_area_post, HOOK_THISCALL, 18);
  Hook((void*)EXEOFFSET(0x419740), _on_load_area_pre, _on_load_area_post, HOOK_THISCALL, 0);

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
  Hook(MonsterProcessAI2, _spider_process_ai2_pre, 0, HOOK_THISCALL, 2);
  Hook(MonsterProcessAI3, _spider_process_ai3_pre, _spider_process_ai3_post, HOOK_THISCALL, 1);
  Hook(MonsterIdle, _spider_idle_pre, 0, HOOK_THISCALL, 2);
  Hook(MonsterOnHit, _spider_on_hit_pre, 0, HOOK_THISCALL, 2);
  Hook(SetAlignment, _set_alignment_pre, 0, HOOK_THISCALL, 1);

  log("Monster Done");

  // Entity
  Hook(EntityInitialize, _entity_initialize_pre, 0, HOOK_THISCALL, 3);

  // Player
  Hook(PlayerCtor, 0, _player_ctor_post, HOOK_THISCALL, 1);
  Hook(PlayerSetAction, _player_set_action_pre, 0, HOOK_THISCALL, 1);
  Hook(AddGoldToPlayer, _add_goldtoplayer, 0, HOOK_THISCALL, 1);
  Hook(PlayerInitialize, NULL, _initialize_player_post, HOOK_THISCALL, 2);
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
  Hook((void*)EXEOFFSET(0x4F3190), test0_pre, test0_post, HOOK_THISCALL, 5);

  // Doesn't work, don't use
  //Hook(CreateUnitByName, test1_pre, test1_post, HOOK_THISCALL, 5);
}
