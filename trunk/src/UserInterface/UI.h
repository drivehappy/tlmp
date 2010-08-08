#pragma once


#include "Events.h"
#include "Common.h"

#include "CPlayer.h"
#include "Globals.h"

#include "Logger.h"
#include "UserInterface.h"
using namespace TLAPI;

namespace TLMP
{
  extern Logger multiplayerLogger;
  extern bool   suppressInGameMouse;

  // Post Event for handling UI creation
  void GameClient_CreateUI(CGame* game);

  // Button Events
  bool ButtonEvent_OpenMultiplayerOptions(const CEGUI::EventArgs& args);
  bool ButtonEvent_CloseMultiplayerOptions(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Host(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Join(const CEGUI::EventArgs& args);

};
