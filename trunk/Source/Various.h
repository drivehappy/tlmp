#pragma once

#include "Common.h"
#include "Player.h"
#include "Network.h"
#include "Entity.h"
using namespace TLMP::Network;

#include "MemoryManager.h"


namespace TLMP {

  void _set_destination_pre STDARG;

  void _on_strike_pre STDARG;
  void _on_strike_post STDARG;

  void _random_post STDARG;

  void _destroy_pre STDARG;

  void _ogre_is_active STDARG;

  void _process_objects_pre STDARG;
  void _process_objects_post STDARG;
  
  void _wnd_proc_pre STDARG;

};
