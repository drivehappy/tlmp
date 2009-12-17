#pragma once

#include "DataTypes.h"
#include "Common.h"

#include "../ExternalLibs/raknet/include/RakPeerInterface.h"
#include "../ExternalLibs/raknet/include/RakNetworkFactory.h"
#include "../ExternalLibs/raknet/include/MessageIdentifiers.h"

namespace TLMP {

  namespace Network {

    class Client {
    public:
      static Client& getSingleton();

      void Connect(const char* address, u16 port);
      void Disconnect();

      void ReceiveMessages();

    protected:
      Client();
      Client(const Client&);
      Client& operator=(const Client&);
      ~Client();

    private:
      RakPeerInterface *m_pClient;
    };

  };

};
