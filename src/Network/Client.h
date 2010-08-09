#pragma once

#include "Common.h"

namespace TLMP {

  namespace Network {

    class Client {
    public:
      typedef void (*OnConnected)(void *);
      typedef void (*OnDisconnected)(void *);
      typedef void (*OnConnectFailed)(void *);

      static Client& getSingleton();

      void Connect(const char* address, u16 port);
      void Disconnect();

      void SetCallback_OnConnected(OnConnected callback);
      void SetCallback_OnDisconnected(OnDisconnected callback);
      void SetCallback_OnConnectFailed(OnConnectFailed callback);

      inline void SetGameStarted(bool value)  { m_bGameStarted = value; if (m_bGameStarted) m_bWaitingForGame = true; }
      inline bool GetGameStarted()            { return m_bGameStarted; }
      inline bool HasGameStarted()            { return m_bWaitingForGame; }

      void ReceiveMessages();

      template<typename T>
      void SendMessage(Message msg, ::google::protobuf::Message *message);

    protected:
      Client();
      Client(const Client&);
      Client& operator=(const Client&);
      ~Client();

    private:
      void OnConnect(void *);
      
      void WorkMessage(Message msg, RakNet::BitStream *bitStream);

      template<typename T>
      T* ParseMessage(RakNet::BitStream *bitStream);

      RakPeerInterface *m_pClient;
      RakNet::BitStream *m_pBitStream;

      OnConnected       m_pOnConnected;
      OnDisconnected    m_pOnDisconnected;
      OnConnectFailed   m_pOnConnectFailed;
      
      bool m_bWaitingForGame;
      bool m_bGameStarted;
    };

  };

};

// Template parameterization, needs to be defined after above declarations
#include "Client.cpp.h"
