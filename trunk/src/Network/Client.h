#pragma once

#include <vector>
using namespace std;

#include "Common.h"
#include "UI.h"
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

      void Reset();

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

      inline bool Get_IsSendingEquipmentAddGem()             { return m_bIsEquipmentAddingGem; }
      inline void Set_IsSendingEquipmentAddGem(bool value)   { m_bIsEquipmentAddingGem = value; }      

      inline bool GetSuppressed_SendUseSkill()           { return m_bSuppressNetwork_SendUseSkill; }
      inline void SetSuppressed_SendUseSkill(bool value) { m_bSuppressNetwork_SendUseSkill = value; }

      inline bool GetSuppressed_SendEquipmentUnequip()           { return m_bSuppressNetwork_SendEquipmentUnequip; }
      inline void SetSuppressed_SendEquipmentUnequip(bool value) { m_bSuppressNetwork_SendEquipmentUnequip = value; }

      inline bool GetSuppressed_CharacterAction()           { return m_bSuppressNetwork_CharacterAction; }
      inline void SetSuppressed_CharacterAction(bool value) { m_bSuppressNetwork_CharacterAction = value; }

      inline bool GetSuppressed_CharacterAttack()           { return m_bSuppressNetwork_CharacterAttack; }
      inline void SetSuppressed_CharacterAttack(bool value) { m_bSuppressNetwork_CharacterAttack = value; }

      inline bool GetSuppressed_SendEquipmentStack()              { return m_bSuppressNetwork_SendEquipmentStack; }
      inline void SetSuppressed_SendEquipmentStack(bool value)    { m_bSuppressNetwork_SendEquipmentStack = value; }

      inline bool GetAllow_ChangeLevel()              { return m_bAllow_ChangeLevel; }
      inline void SetAllow_ChangeLevel(bool value)    { m_bAllow_ChangeLevel = value; }

      inline bool GetAllow_HealthUpdate()             { return m_bAllow_HealthUpdate; }
      inline void SetAllow_HealthUpdate(bool value)   { m_bAllow_HealthUpdate = value; }

      inline bool GetAllow_LevelItemDrop()             { return m_bAllow_LevelItemDrop; }
      inline void SetAllow_LevelItemDrop(bool value)   { m_bAllow_LevelItemDrop = value; }
      
      inline bool GetSuppressed_SendBreakableTriggered()             { return m_bSuppressNetwork_SendBreakableTriggered; }
      inline void SetSuppressed_SendBreakableTriggered(bool value)   { m_bSuppressNetwork_SendBreakableTriggered = value; }
            
      inline bool GetSuppressed_SendTriggerUnitTriggered()             { return m_bSuppressNetwork_SendTriggerUnitTriggered; }
      inline void SetSuppressed_SendTriggerUnitTriggered(bool value)   { m_bSuppressNetwork_SendTriggerUnitTriggered = value; }
            
      inline bool GetAllow_CharacterSetTarget()             { return m_bAllow_CharacterSetTarget; }
      inline void SetAllow_CharacterSetTarget(bool value)   { m_bAllow_CharacterSetTarget = value; }
                  
      inline bool GetAllow_EquipmentIdentify()              { return m_bAllow_EquipmentIdentify; }
      inline void SetAllow_EquipmentIdentify(bool value)    { m_bAllow_EquipmentIdentify = value; }
                        
      inline bool GetAllow_CharacterResurrect()             { return m_bAllow_CharacterResurrect; }
      inline void SetAllow_CharacterResurrect(bool value)   { m_bAllow_CharacterResurrect = value; }

      inline bool GetAllow_AddEffect()                      { return m_bAllow_AddEffect; }
      inline void SetAllow_AddEffect(bool value)            { m_bAllow_AddEffect = value; }

      inline bool GetAllow_AddExperience()                  { return m_bAllow_AddExperience; }
      inline void SetAllow_AddExperience(bool value)        { m_bAllow_AddExperience = value; }

      inline bool GetAllow_RandomSeed()                     { return m_bAllow_RandomSeed; }
      inline void SetAllow_RandomSeed(bool value)           { m_bAllow_RandomSeed = value; }

      inline bool GetAllow_UpdateOrientation()              { return m_bAllow_UpdateOrientation; }
      inline void SetAllow_UpdateOrientation(bool value)    { m_bAllow_UpdateOrientation = value; }

      inline bool GetAllow_WeaponSwap()                     { return m_bAllow_WeaponSwap; }
      inline void SetAllow_WeaponSwap(bool value)           { m_bAllow_WeaponSwap = value; }

      void ReceiveMessages();

      template<typename T>
      void SendMessage(Message msg, ::google::protobuf::Message *message);

      void Helper_ClientPushAllEquipment();
      void Helper_ClientPushEquipment(CEquipment *equipment);
      vector<CCharacter*>* Helper_ProduceClientSideCharacters(CCharacter *character);

      u32 GetSeed() const { return m_Seed; }

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
      void HandleCharacterDestination(u32 commonId, Vector3 current, Vector3 destination, u8 running, u8 attacking);
      void HandleCharacterCreation(NetworkMessages::Character *msgCharacter);
      void HandleEquipmentCreation(NetworkMessages::Equipment *equipment);
      void HandleInventoryAddEquipment(u32 ownerId, u32 equipmentId, u32 slot, u32 unk0);
      void HandleInventoryRemoveEquipment(u32 ownerId, u32 equipmentId);
      void HandleEquipmentDrop(u32 equipmentId, Vector3 position, bool unk0);
      void HandleEquipmentPickup(u32 characterId, u32 equipmentId);
      void HandleReplyEquipmentID(NetworkMessages::EquipmentSetID *msgEquipmentSetID);
      void HandleCharacterSetAction(NetworkMessages::CharacterAction*);
      void HandleCharacterAttack(NetworkMessages::CharacterAttack*);
      void HandleCharacterUseSkill(NetworkMessages::CharacterUseSkill*);
      void HandleEquipmentUpdateStack(NetworkMessages::EquipmentUpdateStackSize*);
      void HandleEquipmentAddGem(NetworkMessages::EquipmentAddGem*);
      void HandleEquipmentRemoveGems(NetworkMessages::EquipmentRemoveGems *);
      void HandleChatMessage(NetworkMessages::ChatPlayerText *);
      void HandleChangeLevel(NetworkMessages::ChangeLevel*);
      void HandleUpdateHealth(NetworkMessages::CharacterUpdateHealth*);
      void HandleCharacterDestroy(NetworkMessages::CharacterDestroy*);
      void HandleLevelCreateItem(NetworkMessages::LevelCreateItem*);
      void HandleLevelDropItem(NetworkMessages::LevelDropItem*);
      void HandleBreakableTriggered(NetworkMessages::BreakableTriggered*);
      void HandleTriggerUnitTriggered(NetworkMessages::TriggerUnitTriggered*);
      void HandleItemGoldAmount(NetworkMessages::ItemGoldCreate*);
      void HandleCharacterAlignment(NetworkMessages::CharacterAlignment*);
      void HandleCharacterSetTarget(NetworkMessages::CharacterSetTarget *);
      void HandleEquipmentIdentify(NetworkMessages::EquipmentIdentify *);
      void HandleCurrentLevel(NetworkMessages::CurrentLevel *);
      void HandleTriggerUnitSync(NetworkMessages::TriggerUnitSync *);
      void HandleCharacterResurrect(NetworkMessages::CharacterResurrect *);
      void HandleCharacterAddExperience(NetworkMessages::CharacterAddExperience *);
      void HandleCharacterKillCharacter(NetworkMessages::CharacterKilledCharacter *);
      void HandleCharacterAddSkill(NetworkMessages::BaseUnitAddSkill *);
      void HandleRandomSeed(NetworkMessages::RandomSeed *);
      void HandleCharacterOrientation(NetworkMessages::CharacterOrientation *);
      void HandlePlayerWeaponSwap(NetworkMessages::PlayerSwapWeapons *);

      void Helper_PopulateEquipmentMessage(NetworkMessages::Equipment* msgEquipment, CEquipment *equipment, NetworkEntity *netEquipment);

      void PushEquipment();

      template<typename T>
      T* ParseMessage(RakNet::BitStream *bitStream);

      RakPeerInterface *m_pClient;
      RakNet::BitStream *m_pBitStream;

      vector<OnConnected>       m_pOnConnected;
      vector<OnDisconnected>    m_pOnDisconnected;
      vector<OnConnectFailed>   m_pOnConnectFailed;
      
      bool m_bWaitingForGame;
      bool m_bGameStarted;
      bool m_bServerGameStarted;

      u32 m_Seed;

      bool m_bSuppressNetwork_SetDestination;
      bool m_bSuppressNetwork_CharacterCreation;
      bool m_bSuppressNetwork_EquipmentCreation;
      bool m_bSuppressNetwork_EquipmentDrop;
      bool m_bSuppressNetwork_EquipmentPickup;
      bool m_bSuppressNetwork_SendEquipmentUnequip;
      bool m_bSuppressNetwork_CharacterAction;
      bool m_bSuppressNetwork_CharacterAttack;
      bool m_bSuppressNetwork_SendUseSkill;
      bool m_bSuppressNetwork_SendEquipmentStack;
      bool m_bSuppressNetwork_SendBreakableTriggered;
      bool m_bSuppressNetwork_SendTriggerUnitTriggered;
      bool m_bAllow_CharacterResurrect;

      bool m_bAllow_ChangeLevel;
      bool m_bAllow_HealthUpdate;

      bool m_bIsSendingPickup;      // Required because EquipmentPickup function is called across multiple frames (MouseDown?)
      bool m_bIsSendingUseSkill;    // Required because Same as above
      bool m_bIsEquipmentAddingGem; // Used as a request mechanism to the server
      bool m_bAllow_LevelItemDrop;
      bool m_bAllow_CharacterSetTarget;
      bool m_bAllow_EquipmentIdentify;
      bool m_bAllow_AddEffect;
      bool m_bAllow_AddExperience;
      bool m_bAllow_RandomSeed;
      bool m_bAllow_UpdateOrientation;
      bool m_bAllow_WeaponSwap;
    };

  };

};

// Template parameterization, needs to be defined after above declarations
#include "Client.cpp.h"
