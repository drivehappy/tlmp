#pragma once

#include "Common.h"
#include "Player.h"
#include "Network.h"
using namespace TLMP::Network;

namespace TLMP {

  void _set_destination_pre STDARG;

  void _on_strike_pre STDARG;
  void _on_strike_post STDARG;

  void _random_post STDARG;

  void _destroy_pre STDARG;

};
