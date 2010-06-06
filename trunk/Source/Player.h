#pragma once

#include "Common.h"
#include "Monster.h"
#include "MemoryManager.h"


namespace TLMP {

  extern CPlayer* me;
  extern CCharacter* otherPlayer;
  extern CCharacter* otherPlayerPet;
  extern PVOID UnitManager;

  enum ResurrectOptions {
    RESURRECT_AT_BODY = 0xE,
    RESURRECT_AT_LEVEL = 0xF,
    RESURRECT_AT_TOWN = 0x10,
  };

  void _player_ctor_post STDARG;

  void _player_set_action_pre STDARG;

  void _add_goldtoplayer STDARG;

  void _initialize_player_pre STDARG;
  void _initialize_player_post STDARG;

  void _player_died_pre STDARG;

  void _player_resurrect_pre STDARG;

  void _levelup_pre STDARG;

  void _levelup_silent_pre STDARG;

  void _add_minion_pre STDARG;

  CCharacter* SpawnPlayer(u64 guid, u32 level, Vector3 position);

};
