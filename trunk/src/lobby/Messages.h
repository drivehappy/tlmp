#pragma once

namespace TLMP {
  namespace Network {
    namespace Lobby {

      #define stringify(name) #name

      /** Version to signify any Messaging ID changes. */
      const int LobbyMessageVersion = 1;

      /** Network Control Messages */
      enum Message {
        S_VERSION = 0,
        C_VERSION,
      };

      /** Stringify of Control Messages */
      static const char* MessageString[] = {
        "S_VERSION",
        "C_VERSION",
      };

    };
  };
};
