#pragma once

#include "Common.h"
#include "Monster.h"

namespace TLMP {

  extern PVOID me;
  extern PVOID otherPlayer;
  extern PVOID otherPlayerPet;
  extern PVOID UnitManager;

  void _player_ctor_post STDARG;

  void _player_set_action_pre STDARG;

  void _add_goldtoplayer STDARG;

  void _initialize_player_post STDARG;

  void _player_died_pre STDARG;

  void _player_resurrect_pre STDARG;

  void _levelup_pre STDARG;

  void _levelup_silent_pre STDARG;

  void _add_minion_pre STDARG;

  PVOID SpawnPlayer(u64 guid, u32 level, Vector3 position);

};
