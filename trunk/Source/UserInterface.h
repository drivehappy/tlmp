#pragma once

#include "Common.h"
#include "Timer.h"
#include "../ExternalLibs/cegui/include/CEGUI.h"

/*
	void hostGame();
	void connectGame(unsigned long ip = 0);
	void sendMessage(const char* message, int length);
*/

namespace TLMP {

  namespace UI {

	  static CEGUI::WindowManager *g_pWm = NULL;

	  void init();

	  CEGUI::Window* getWindowFromName(const char* name);
	  CEGUI::Window* createWindow(const char* name);

	  void updateLatency(const double latency);
	  void updateChatHistory(const char* message);
	  bool isChatFocused();

	  void createWindow();

	  void showChatWindow(bool visible);

	  bool mouseEventMultiplayer(const CEGUI::EventArgs& args);
	  bool mouseEventCancel(const CEGUI::EventArgs& args);
	  bool mouseEventConnect(const CEGUI::EventArgs& args);
	  bool mouseEventHost(const CEGUI::EventArgs& args);

	  bool mouseEventChat(const CEGUI::EventArgs& args);
	  bool keyboardEventChat(const CEGUI::EventArgs& args);

	  void dumpToFile(const char* filename);
	  void dumpToLog(CEGUI::Window* window, int level=0);

  };
	
};
