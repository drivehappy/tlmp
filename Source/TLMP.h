#pragma once

#include "Common.h"

/*
#include "State.h"
#include "Client.h"
#include "Server.h"
*/

namespace TLMP {

  void Initialize();
  void Shutdown();

  void PatchProcess();
  void HookFunctions();

};
