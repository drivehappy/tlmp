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

      inline bool GetSuppressed_SetDestination()            { return m_bSuppressNetwork_SetDestination; }
      inline void SetSuppressed_SetDestination(bool value)  { m_bSuppressNetwork_SetDestination = value; }


      void ReceiveMessages();

      template<typename T>
      void BroadcastMessage(Message msg, ::google::protobuf::Message *message);
      template<typename T>
      void BroadcastMessage(const AddressOrGUID systemIdentifier, Message msg, ::google::protobuf::Message *message);
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
      void HandleReplyCharacterInfo(const SystemAddress clientAddress, Vector3 posCharacter, u64 guidCharacter, string nameCharacter, u64 guidPet, string namePet);

      void HandleCharacterDestination(const SystemAddress clientAddress, u32 commonId, Vector3 destination);


      void SendClientEntities();

      RakPeerInterface *m_pServer;
      RakNet::BitStream *m_pBitStream;
      
      OnListening         m_pOnListening;
      OnShutdown          m_pOnShutdown;
      OnClientConnect     m_pOnClientConnect;
      OnClientDisconnect  m_pOnClientDisconnect;

      bool m_bWaitingForGame;
      bool m_bGameStarted;

      bool m_bSuppressNetwork_SetDestination;
    };

  };

};

// Template parameterization, needs to be defined after above declarations
#include "Server.cpp.h"
