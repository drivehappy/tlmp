#pragma once

#include "DataTypes.h"
#include "Common.h"

#include "../ExternalLibs/raknet/include/RakPeerInterface.h"
#include "../ExternalLibs/raknet/include/RakNetworkFactory.h"
#include "../ExternalLibs/raknet/include/MessageIdentifiers.h"


namespace TLMP {

  namespace Network {

    class Server {
    public:
      static Server& getSingleton();

      void Listen(u16 port, u16 maxconnections);
      void Shutdown();

      void ReceiveMessages();

    protected:
      Server();
      Server(const Server&);
      Server& operator=(const Server&);
      ~Server();

    private:
      RakPeerInterface *m_pServer;
    };

  };

};