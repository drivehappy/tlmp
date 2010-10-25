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
        L_S_PLAYERNAME_LEAVE,

        L_S_CHAT_MESSAGE,
        L_C_CHAT_MESSAGE,

        L_C_VIEW_GAMES,           // Client requests a list of current games
        L_S_VIEW_GAMES,           // Server pushes a list of current games

        L_C_HOSTING_NEW_GAME,     // Client informs us they're hosting a new game
        L_S_HOSTING_REPLY,        // Ack

        L_C_REQUEST_JOINGAME,     // Requests lobby server to connect to a hosted game
        L_S_REPLAY_JOINGAME,      // Returns hosted game IP and Port
      };

      /** Stringify of Control Messages */
      static const char* LobbyMessageString[] = {
        "L_S_VERSION",
        "L_C_VERSION",

        "L_C_PLAYERNAME",
        "L_S_PLAYERNAME_BATCH",
        "L_S_PLAYERNAME_JOIN",
        "L_S_PLAYERNAME_LEAVE",

        "L_S_CHAT_MESSAGE",
        "L_C_CHAT_MESSAGE",
      };

    };
  };
};
