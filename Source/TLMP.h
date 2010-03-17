#pragma once

#include "Common.h"
#include "ServerState.h"

/*
#include "State.h"
#include "Client.h"
#include "Server.h"
*/

namespace TLMP {

  void Initialize();
  void Shutdown();

  void SetupProcessBase();
  void PatchProcess();
  void HookFunctions();

};
