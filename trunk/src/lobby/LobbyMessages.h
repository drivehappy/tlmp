#pragma once

namespace TLMP {
  namespace Network {
    namespace Lobby {

      #define stringify(name) #name

      /** Version to signify any Messaging ID changes. */
      const int LobbyMessageVersion = 1;

      /** Network Control Messages */
      enum LobbyMessage {
        L_S_VERSION = 0,
        L_C_VERSION,
        
        L_C_PLAYERNAME,
        L_S_PLAYERNAME_BATCH,
        L_S_PLAYERNAME_JOIN,

        L_S_CHAT_MESSAGE,
        L_C_CHAT_MESSAGE,
      };

      /** Stringify of Control Messages */
      static const char* LobbyMessageString[] = {
        "L_S_VERSION",
        "L_C_VERSION",

        "L_C_PLAYERNAME",
        "L_S_PLAYERNAME_BATCH",
        "L_S_PLAYERNAME_JOIN",

        "L_S_CHAT_MESSAGE",
        "L_C_CHAT_MESSAGE",
      };

    };
  };
};
