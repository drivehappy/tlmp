#pragma once

#include "DataTypes.h"

namespace TLMP {

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

};
