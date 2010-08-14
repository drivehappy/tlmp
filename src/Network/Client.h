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

      inline void SetGameStarted(bool value)                { m_bGameStarted = value; if (m_bGameStarted) m_bWaitingForGame = true; }
      inline bool GetGameStarted()                          { return m_bGameStarted; }
      inline bool HasGameStarted()                          { return m_bWaitingForGame; }

      inline bool GetServerGameStarted()                    { return m_bServerGameStarted; }
      inline void SetServerGameStarted(bool value)          { m_bServerGameStarted = value; }

      inline bool GetSuppressed_SetDestination()            { return m_bSuppressNetwork_SetDestination; }
      inline void SetSuppressed_SetDestination(bool value)  { m_bSuppressNetwork_SetDestination = value; }

      inline bool GetSuppressed_CharacterCreation()            { return m_bSuppressNetwork_CharacterCreation; }
      inline void SetSuppressed_CharacterCreation(bool value)  { m_bSuppressNetwork_CharacterCreation = value; }

      inline bool GetSuppressed_EquipmentCreation()            { return m_bSuppressNetwork_EquipmentCreation; }
      inline void SetSuppressed_EquipmentCreation(bool value)  { m_bSuppressNetwork_EquipmentCreation = value; }

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
      
      /** Work on received packet data. */
      void WorkMessage(Message msg, RakNet::BitStream *bitStream);

      /** Functions for handling individual messages. */
      void HandleVersion(u32 version);
      void HandleHasGameStarted(bool gameStarted);
      void HandleGameStarted();
      void HandleGameEnded();
      void HandleRequestCharacterInfo();
      void HandleReplyCharacterId(u32 id);
      void HandleCharacterDestination(u32 commonId, Vector3 destination);
      void HandleCharacterCreation(Vector3 posCharacter, u64 guidCharacter, string characterName, u32 commonId);
      void HandleEquipmentCreation(u32 id, u64 guid);

      void PushEquipment();

      template<typename T>
      T* ParseMessage(RakNet::BitStream *bitStream);

      RakPeerInterface *m_pClient;
      RakNet::BitStream *m_pBitStream;

      OnConnected       m_pOnConnected;
      OnDisconnected    m_pOnDisconnected;
      OnConnectFailed   m_pOnConnectFailed;
      
      bool m_bWaitingForGame;
      bool m_bGameStarted;
      bool m_bServerGameStarted;

      bool m_bSuppressNetwork_SetDestination;
      bool m_bSuppressNetwork_CharacterCreation;
      bool m_bSuppressNetwork_EquipmentCreation;
    };

  };

};

// Template parameterization, needs to be defined after above declarations
#include "Client.cpp.h"
