#pragma once

#include "Common.h"

namespace TLMP {

  static PVOID me = 0;

  void _player_ctor_post STDARG;

  void _player_set_action_pre STDARG;

  void _add_goldtoplayer STDARG;

  void _initialize_player_post STDARG;

  void _player_died_pre STDARG;

  void _player_resurrect_pre STDARG;

  void _levelup_pre STDARG;

  void _levelup_silent_pre STDARG;


};
