#pragma once

#include "Common.h"
using namespace TLMP;

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

      inline bool GetSuppressed_EquipmentDrop()            { return m_bSuppressNetwork_EquipmentDrop; }
      inline void SetSuppressed_EquipmentDrop(bool value)  { m_bSuppressNetwork_EquipmentDrop = value; }

      inline bool GetSuppressed_EquipmentPickup()            { return m_bSuppressNetwork_EquipmentPickup; }
      inline void SetSuppressed_EquipmentPickup(bool value)  { m_bSuppressNetwork_EquipmentPickup = value; }

      inline bool Get_IsSendingPickup()                      { return m_bIsSendingPickup; }
      inline void Set_IsSendingPickup(bool value)            { m_bIsSendingPickup = value; }

      inline bool Get_IsSendingUseSkill()                      { return m_bIsSendingUseSkill; }
      inline void Set_IsSendingUseSkill(bool value)            { m_bIsSendingUseSkill = value; }

      inline bool GetSuppressed_SendUseSkill()           { return m_bSuppressNetwork_SendUseSkill; }
      inline void SetSuppressed_SendUseSkill(bool value) { m_bSuppressNetwork_SendUseSkill = value; }

      inline bool GetSuppressed_SendEquipmentUnequip()           { return m_bSuppressNetwork_SendEquipmentUnequip; }
      inline void SetSuppressed_SendEquipmentUnequip(bool value) { m_bSuppressNetwork_SendEquipmentUnequip = value; }

      inline bool GetSuppressed_CharacterAction()           { return m_bSuppressNetwork_CharacterAction; }
      inline void SetSuppressed_CharacterAction(bool value) { m_bSuppressNetwork_CharacterAction = value; }

      inline bool GetSuppressed_CharacterAttack()           { return m_bSuppressNetwork_CharacterAttack; }
      inline void SetSuppressed_CharacterAttack(bool value) { m_bSuppressNetwork_CharacterAttack = value; }


      void ReceiveMessages();

      template<typename T>
      void SendMessage(Message msg, ::google::protobuf::Message *message);

      void Helper_ClientPushAllEquipment();
      void Helper_ClientPushEquipment(CEquipment *equipment);
      vector<CCharacter*>* Helper_ProduceClientSideCharacters(CCharacter *character);

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
      void HandleReplyCharacterId(NetworkMessages::ReplyCharacterId *msgReplyCharacterId);
      void HandleCharacterDestination(u32 commonId, Vector3 destination, u8 running, u8 attacking);
      void HandleCharacterCreation(Vector3 posCharacter, u64 guidCharacter, string characterName, u32 commonId, u32 health, u32 mana);
      void HandleEquipmentCreation(NetworkMessages::Equipment *equipment);
      void HandleInventoryAddEquipment(u32 ownerId, u32 equipmentId, u32 slot, u32 unk0);
      void HandleInventoryRemoveEquipment(u32 ownerId, u32 equipmentId);
      void HandleEquipmentDrop(u32 equipmentId, Vector3 position, bool unk0);
      void HandleEquipmentPickup(u32 characterId, u32 equipmentId);
      void HandleReplyEquipmentID(NetworkMessages::EquipmentSetID *msgEquipmentSetID);
      void HandleCharacterSetAction(NetworkMessages::CharacterAction*);
      void HandleCharacterAttack(NetworkMessages::CharacterAttack*);
      void HandleCharacterUseSkill(NetworkMessages::CharacterUseSkill*);
      

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
      bool m_bSuppressNetwork_EquipmentDrop;
      bool m_bSuppressNetwork_EquipmentPickup;
      bool m_bSuppressNetwork_SendEquipmentUnequip;
      bool m_bSuppressNetwork_CharacterAction;
      bool m_bSuppressNetwork_CharacterAttack;
      bool m_bSuppressNetwork_SendUseSkill;

      bool m_bIsSendingPickup;  // Required because EquipmentPickup function is called across multiple frames (MouseDown?)
      bool m_bIsSendingUseSkill;  // Required because Same as above
    };

  };

};

// Template parameterization, needs to be defined after above declarations
#include "Client.cpp.h"
