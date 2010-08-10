#pragma once

#include "Common.h"

namespace TLMP {

  namespace Network {

    class Server {
    public:
      typedef void (*OnListening)(void *);
      typedef void (*OnShutdown)(void *);
      typedef void (*OnClientConnect)(void *);
      typedef void (*OnClientDisconnect)(void *);
      

      static Server& getSingleton();

      void Listen(u16 port, u16 maxconnections);
      void Shutdown();

      void SetCallback_OnListening(OnListening callback);
      void SetCallback_OnShutdown(OnShutdown callback);
      void SetCallback_OnClientConnect(OnClientConnect callback);
      void SetCallback_OnClientDisconnect(OnClientDisconnect callback);

      inline void SetGameStarted(bool value)  { m_bGameStarted = value; if (m_bGameStarted) m_bWaitingForGame = true; }
      inline bool GetGameStarted()            { return m_bGameStarted; }
      inline bool HasGameStarted()            { return m_bWaitingForGame; }

      void ReceiveMessages();

      template<typename T>
      void BroadcastMessage(Message msg, ::google::protobuf::Message *message);
      template<typename T>
      void SendMessage(const AddressOrGUID systemIdentifier, Message msg, ::google::protobuf::Message *message);

    protected:
      Server();
      Server(const Server&);
      Server& operator=(const Server&);
      ~Server();

    private:
      template<typename T>
      T* ParseMessage(RakNet::BitStream *bitStream);

      /** Work on received packet data. */
      void WorkMessage(const SystemAddress clientAddress, Message msg, RakNet::BitStream *bitStream);

      /** Functions for handling individual messages. */
      void HandleVersion(const SystemAddress clientAddress, u32 version);
      void HandleHasGameStarted(const SystemAddress clientAddress);
      void HandleGameEnter(const SystemAddress clientAddress);
      void HandleGameExited(const SystemAddress clientAddress);

      void SendClientEntities();

      RakPeerInterface *m_pServer;
      RakNet::BitStream *m_pBitStream;
      
      OnListening         m_pOnListening;
      OnShutdown          m_pOnShutdown;
      OnClientConnect     m_pOnClientConnect;
      OnClientDisconnect  m_pOnClientDisconnect;

      bool m_bWaitingForGame;
      bool m_bGameStarted;
    };

  };

};

// Template parameterization, needs to be defined after above declarations
#include "Server.cpp.h"
