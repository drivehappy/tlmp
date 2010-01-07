#pragma once

#include "Common.h"
#include "UserInterface.h"
#include "Entity.h"
#include "Player.h"
#include "Network.h"
using namespace TLMP::Network;

namespace TLMP {

  static PVOID load_area_this = 0;

  void _load_map_pre STDARG;
  void _load_map_post STDARG;

  void _load_area_pre STDARG;
  void _load_area_post STDARG;

  void _on_load_area_pre STDARG;
  void _on_load_area_post STDARG;

};