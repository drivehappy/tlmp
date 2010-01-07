#pragma once

#include "DataTypes.h"

namespace TLMP {

  TLFUNC(SpiderSomeCreate,   PVOID,    __thiscall, (PVOID, u64, u32, bool));
  TLFUNC(EntityInitialize,   PVOID,    __thiscall, (PVOID, PVOID, Vector3*, u32));
  TLFUNC(CreateUnitByName,   PVOID,    __thiscall, (PVOID, const wchar_t*, const wchar_t*, u32, u32));
  TLFUNC(SetAlignment,       PVOID,    __thiscall, (PVOID, u32));
  TLFUNC(SetDestination,     PVOID,    __thiscall, (PVOID, PVOID, float, float));
  TLFUNC(GetPosition,        PVOID,    __thiscall, (PVOID, Vector3, u32));
  TLFUNC(SetAction,          PVOID,    __thiscall, (PVOID, u32));
  TLFUNC(UseSkill,           PVOID,    __thiscall, (PVOID, u64));
  TLFUNC(SetPosition,        PVOID,    __thiscall, (PVOID, const Vector3));
  TLFUNC(AddMinion,          PVOID,    __thiscall, (PVOID, PVOID));
  TLFUNC(CreateSomething,    PVOID,    __thiscall, (PVOID, u64, u32, u32, u32));

  TLFUNC(SetAttack,          PVOID,    __thiscall, (PVOID, PVOID));

  TLFUNC(OnStrike,           PVOID,    __thiscall, (PVOID, PVOID, PVOID, PVOID, u32, float, float, u32));

  TLFUNC(SpiderProcessAI,    PVOID,    __thiscall, (PVOID, PVOID, float));
  TLFUNC(SetAnimation,       PVOID,    __thiscall, (PVOID, u32, bool, float, float, u32));

  TLFUNC(DoAttack,           PVOID,    __thiscall, (PVOID));

  TLFUNC(ItemInitialize,     PVOID,    __thiscall, (PVOID, PVOID));
  TLFUNC(ItemDrop,           PVOID,    __thiscall, (PVOID, PVOID, Vector3, bool));
  TLFUNC(ItemCreate,         PVOID,    __thiscall, (PVOID, u64, u32, u32, u32));
  TLFUNC(ItemPickup,         PVOID,    __thiscall, (PVOID, PVOID, PVOID));
  TLFUNC(ItemEquip,          PVOID,    __thiscall, (PVOID, PVOID, u32, u32));
  TLFUNC(ItemUnequip,        PVOID,    __thiscall, (PVOID, PVOID));
  TLFUNC(ItemHide,           PVOID,    __thiscall, (PVOID, PVOID, u32));

  TLFUNC(LoadArea,           PVOID,    __thiscall, (PVOID, wstring, u32, u32, u32, wstring, u32));

  TLFUNC(AddGoldToPlayer,    PVOID,    __thiscall, (PVOID, u32));

  TLFUNC(LevelUp,            PVOID,    __thiscall, (PVOID));
  TLFUNC(LevelUpSilent,      PVOID,    __thiscall, (PVOID));

  TLFUNC(PetawayTimer,       PVOID,    __thiscall, (PVOID, u32, u32));

  TLFUNC(InteractWithObject, PVOID,    __thiscall, (PVOID, PVOID));
  TLFUNC(ObjectCreate,       PVOID,    __thiscall, (PVOID, u64));

  TLFUNC(BarrelDestroy,      PVOID,    __thiscall, (PVOID, PVOID));
  TLFUNC(BarrelKnockback,    PVOID,    __thiscall, (PVOID));

  TLFUNC(CheckgamePaused,    void,     __thiscall, (PVOID));

  TLFUNC(PlayerInitialize,   void,     __thiscall, (PVOID, u32, u32));

  TLFUNC(WndProc,            LRESULT,  __thiscall, (HWND, UINT, WPARAM, LPARAM));

  TLFUNC(GetPlayer,          PVOID,    __thiscall, (void));
  TLFUNC(PlayerDied,         void,     __thiscall, (void));
  TLFUNC(PlayerResurrect,    void,     __thiscall, (void));

  // -------------------------------------------------------------------------------- //
  // In-place definitions

  TLFUNC(ProcessObjects,     void,     __thiscall, (PVOID, PVOID, PVOID, PVOID));

  TLFUNC(MonsterProcessAI2,  void,     __thiscall, (PVOID, PVOID));
  TLFUNC(MonsterIdle,        void,     __thiscall, (PVOID, PVOID));
  TLFUNC(MonsterOnHit,       void,     __thiscall, (PVOID, PVOID));
    
  TLFUNC(PlayerCtor,         void,     __thiscall, (PVOID));

  TLFUNC(PlayerSetAction,    void,     __thiscall, (PVOID));
  
  TLFUNC(TitleScreenProcess, void,     __thiscall, (PVOID, PVOID, PVOID, PVOID));

  TLFUNC(LoadMap,            void,     __thiscall, (PVOID, PVOID));

  TLFUNC(Random,             void,     __thiscall, ());

  TLFUNC(Destroy,            void,     __thiscall, (PVOID));

  TLFUNC(EntityReadProp,     void,     __thiscall, (PVOID));

  //TLFUNCPTR(LoadArea,           void,     __thiscall, (/* 18 */),                                        0x40CF40);

  // ... and add more later

};
