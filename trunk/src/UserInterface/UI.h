#pragma once


#include "Events.h"
#include "Common.h"

#include "CPlayer.h"
#include "Globals.h"

#include "Logger.h"
#include "UserInterface.h"
using namespace TLAPI;

#include "Network.h"
using namespace TLMP::Network;

namespace TLMP
{
  extern Logger multiplayerLogger;
  extern bool   suppressInGameMouse;

  // Post Event for handling UI creation
  void GameClient_CreateUI(CGame* game);

  void SetupUI();
  void ResizeUI();

  // Helper for displaying the mainmenu UI
  void DisplayWaitForServerWindow();

  // Button Events
  bool ButtonEvent_OpenMultiplayerOptions(const CEGUI::EventArgs& args);
  bool ButtonEvent_CloseMultiplayerOptions(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Host(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Join(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Join_Cancel(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Host_Cancel(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Host_Host(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_Join_Join(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerOptions_WaitServer_OkButton(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerSplash_OkButton(const CEGUI::EventArgs& args);

  bool ButtonEvent_MultiplayerClientConnecting_OkButton(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerClientConnecting_CancelButton(const CEGUI::EventArgs& args);

  bool ButtonEvent_MultiplayerClientConnectedFailed_OkButton(const CEGUI::EventArgs& args);
  bool ButtonEvent_MultiplayerClientDisconnected_OkButton(const CEGUI::EventArgs& args);

  bool EditboxEvent_KeyDownChatEntry(const CEGUI::EventArgs& args);

  void OnClientConnected(void*);
  void OnClientDisconnected(void*);
  void OnClientConnectFailed(void*);

  CEGUI::Window* getChatHistoryWindow();
  CEGUI::Window* getChatEntryWindow();
  CEGUI::Window* getChatEntryBackgroundWindow();

  extern bool UISetup;

  // Internal "Events"
  void onNetworkStateChange();
};
