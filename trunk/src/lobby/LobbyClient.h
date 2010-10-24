#pragma once

#include "raknet/include/RakPeerInterface.h"
#include "raknet/include/RakNetworkFactory.h"
#include "raknet/include/MessageIdentifiers.h"
#include "raknet/include/BitStream.h"

#include "lobby/LobbyMessages.h"
using namespace TLMP::Network::Lobby;

#include "UI.h"


namespace TLMP {
  namespace Network {
    namespace Lobby {

      class LobbyClient
      {
      public:
        static LobbyClient& getSingleton();

        void Connect(const char* address, unsigned short port);
        void Disconnect();

        void ReceiveMessages();

        template<typename T>
        void SendMessage(LobbyMessage msg, ::google::protobuf::Message *message);

      protected:
        LobbyClient();
        LobbyClient(const LobbyClient&);
        LobbyClient& operator=(const LobbyClient&);
        ~LobbyClient();

      private:
        /** Work on received packet data. */
        void WorkMessage(LobbyMessage msg, RakNet::BitStream *bitStream);
        
        template<typename T>
        T* ParseMessage(RakNet::BitStream *bitStream);

        RakPeerInterface *m_pClient;
        RakNet::BitStream *m_pBitStream;
      };

    };
  };
};

// Template parameterization, needs to be defined after above declarations
#include "LobbyClient.cpp.h"
