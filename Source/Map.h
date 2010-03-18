#pragma once

#include "Common.h"
#include "UserInterface.h"
#include "Entity.h"
#include "Player.h"
#include "Network.h"
using namespace TLMP::Network;

namespace TLMP {

  extern PVOID load_map_this;
  extern PVOID load_area_this;

  void _load_map_pre STDARG;
  void _load_map_post STDARG;

  void _change_level_pre STDARG;
  void _change_level_post STDARG;

  void _on_load_area_pre STDARG;
  void _on_load_area_post STDARG;

  void ChangeGameStateToMainMenu();
  void ChangeGameStateToInGame();

  void LoadAreaTown();

};