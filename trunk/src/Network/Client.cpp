#include "Client.h"

using namespace TLMP::Network;

Client& Client::getSingleton()
{
  static Client m_pSingleton;
  return m_pSingleton;
}

Client::Client()
{
  m_pClient = NULL;
  m_pBitStream = new RakNet::BitStream(1024);

  Reset();
}

Client::~Client()
{
  delete m_pBitStream;
}

void Client::Reset()
{
  m_bWaitingForGame = false;
  m_bGameStarted = false;
  m_bServerGameStarted = false;
  m_bSuppressNetwork_SetDestination = false;
  m_bSuppressNetwork_CharacterCreation = true;
  m_bSuppressNetwork_EquipmentCreation = true;
  m_bSuppressNetwork_EquipmentDrop = false;
  m_bSuppressNetwork_EquipmentPickup = true;
  m_bSuppressNetwork_SendEquipmentUnequip = false;
  m_bSuppressNetwork_CharacterAction = true;
  m_bSuppressNetwork_CharacterAttack = true;
  m_bSuppressNetwork_SendUseSkill = false;
  m_bSuppressNetwork_SendEquipmentStack = false;
  m_bSuppressNetwork_SendBreakableTriggered = false;
  m_bSuppressNetwork_SendTriggerUnitTriggered = false;

  m_bIsSendingPickup = false;
  m_bIsSendingUseSkill = false;
  m_bIsEquipmentAddingGem = false;
  m_bAllow_ChangeLevel = false;
  m_bAllow_HealthUpdate = false;
  m_bAllow_LevelItemDrop = false;
  m_bAllow_CharacterSetTarget = false;
  m_bAllow_EquipmentIdentify = false;
  m_bAllow_CharacterResurrect = false;
  m_bAllow_AddEffect = false;
  m_bAllow_AddExperience = false;
  m_bAllow_RandomSeed = false;
  m_bAllow_UpdateOrientation = false;
  m_bAllow_WeaponSwap = false;
  m_bAllow_SetSkillPoints = false;
  m_bAllow_AutoEquip = false;
}

void Client::Connect(const char* address, u16 port)
{
  log("Client connecting to %s (%i)", address, port);

  if (m_pClient) {
    m_pClient->Shutdown(0);
    RakNetworkFactory::DestroyRakPeerInterface(m_pClient);
    m_pClient = NULL;
  }

  Reset();
  m_pClient = RakNetworkFactory::GetRakPeerInterface();

  // Debugging - allow 2 minutes until disconnect
  m_pClient->SetTimeoutTime(30000, UNASSIGNED_SYSTEM_ADDRESS);
  //--

  SocketDescriptor socketDescriptor(0, 0);
  m_pClient->Startup(1, 30, &socketDescriptor, 1);
  m_pClient->Connect(address, port, 0, 0);
}

void Client::Disconnect()
{
  multiplayerLogger.WriteLine(Info, L"Client disconnecting...");
  log(L"Client disconnecting... %p", m_pClient);

  if (m_pClient) {
    Reset();

    m_pClient->Shutdown(0);
    RakNetworkFactory::DestroyRakPeerInterface(m_pClient);
    m_pClient = NULL;
  }
}

void Client::SetCallback_OnConnected(OnConnected callback)
{
  m_pOnConnected.push_back(callback);
}

void Client::SetCallback_OnDisconnected(OnDisconnected callback)
{
  m_pOnDisconnected.push_back(callback);
}

void Client::SetCallback_OnConnectFailed(OnConnectFailed callback)
{
  m_pOnConnectFailed.push_back(callback);
}

void Client::ReceiveMessages()
{
  if (!m_pClient) {
    return;
  }

  Packet *packet = m_pClient->Receive();
  while (packet)
  {
    switch(packet->data[0])
    {
    case ID_CONNECTION_REQUEST_ACCEPTED:
      {
        multiplayerLogger.WriteLine(Info, L"Client connected to server.");
        logColor(B_GREEN, L"Client connected to server.");

        vector<OnConnected>::iterator itr;
        for (itr = m_pOnConnected.begin(); itr != m_pOnConnected.end(); itr++) {
          SystemAddress *sysAddress = new SystemAddress();
          *sysAddress = packet->systemAddress;
          (*itr)((void*)sysAddress);
        }
      }

      break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
      {
        multiplayerLogger.WriteLine(Info, L"Could not connect to server: No Room.");
        logColor(B_GREEN, L"Could not connect to server: No Room.");

        vector<OnDisconnected>::iterator itr;
        for (itr = m_pOnConnectFailed.begin(); itr != m_pOnConnectFailed.end(); itr++) {
          (*itr)(NULL);
        }
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      {
        multiplayerLogger.WriteLine(Info, L"Client disconnected.");
        logColor(B_GREEN, L"Client disconnected.");

        vector<OnDisconnected>::iterator itr;
        for (itr = m_pOnDisconnected.begin(); itr != m_pOnDisconnected.end(); itr++) {
          (*itr)(NULL);
        }
      }
      break;
    case ID_CONNECTION_LOST:
      {
        multiplayerLogger.WriteLine(Info, L"Client connection lost.");
        logColor(B_GREEN, L"Client connection lost.");

        vector<OnDisconnected>::iterator itr;
        for (itr = m_pOnDisconnected.begin(); itr != m_pOnDisconnected.end(); itr++) {
          (*itr)(NULL);
        }
      }
      break;
    case ID_CONNECTION_ATTEMPT_FAILED:
      {
        multiplayerLogger.WriteLine(Info, L"Client connection failed.");
        logColor(B_GREEN, L"Client connection failed.");

        vector<OnDisconnected>::iterator itr;
        for (itr = m_pOnConnectFailed.begin(); itr != m_pOnConnectFailed.end(); itr++) {
          (*itr)(NULL);
        }
      }
      break;
    case ID_USER_PACKET_ENUM+1:
      u32 msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<u32>(msg);

      WorkMessage((Message)msg, m_pBitStream);

      break;
    }

    // Debug
    //logColor(B_RED, L"m_pClient = %p", m_pClient);
    // --

    // Double check to make sure our client wasn't removed during a packet event (eg. S_PUSH_GAMEEXITED).
    if (!m_pClient) {
      break;
    }

    m_pClient->DeallocatePacket(packet);
    packet = m_pClient->Receive();
  }
}

void Client::OnConnect(void *args)
{
  /*
  TLMP::NetworkMessages::Player player;

  // Init player
  player.set_id((u64)15);
  player.set_name("drivehappy");
  player.set_type(NetworkMessages::Player_ClassType_ALCHEMIST);

  SendMessage<NetworkMessages::Player>(C_PLAYER_INFO, &player);
  */

  multiplayerLogger.WriteLine(Info, L"Client Connected.");
  log("Client Connected.");
}

void Client::WorkMessage(Network::Message msg, RakNet::BitStream *bitStream)
{
  wstring msgString = convertAsciiToWide(MessageString[msg]);

  // For sake of less spam
  if (msg != S_PUSH_CHARACTER_SETDEST &&
      msg != S_PUSH_CHAR_UPDATE_HEALTH &&
      msg != S_PUSH_CHARACTER_SETTARGET)
  {
    multiplayerLogger.WriteLine(Info, L"Client Received Message: %s", msgString.c_str());
    logColor(B_GREEN, L"Client Received Message: %s", msgString.c_str());
  }

  // Special - handle the version and game started so we can handle checking if it has for all other messages
  switch (msg) {
    case S_VERSION:
      {
        NetworkMessages::Version *msgVersion = ParseMessage<NetworkMessages::Version>(m_pBitStream);
        u32 version = (u32)(msgVersion->version());

        HandleVersion(version);
      }
      break;

    case S_REPLY_HASGAMESTARTED:
      {
        NetworkMessages::GameHasStarted *msgGameHasStarted = ParseMessage<NetworkMessages::GameHasStarted>(m_pBitStream);
        bool gameStarted = msgGameHasStarted->started();

        HandleHasGameStarted(gameStarted);
      }
      break;

    case S_PUSH_GAMESTARTED:
      {
        NetworkMessages::GameStarted *msgGameStarted = ParseMessage<NetworkMessages::GameStarted>(m_pBitStream);

        HandleGameStarted();
      }
      break;

    case S_PUSH_GAMEENDED:
      {
        NetworkMessages::GameEnded *msgGameEnded = ParseMessage<NetworkMessages::GameEnded>(m_pBitStream);

        HandleGameEnded();

        log("debug /HandleGameEnded");
      }
      break;

    case S_PUSH_CURRENT_LEVEL:
      {
        NetworkMessages::CurrentLevel *msgCurrentLevel = ParseMessage<NetworkMessages::CurrentLevel>(m_pBitStream);

        HandleCurrentLevel(msgCurrentLevel);
      }
      break;
  }

  // All other messages
  if (Client::getSingleton().GetServerGameStarted()) {
    switch (msg) {
    case S_REQUEST_CHARINFO:
      {
        NetworkMessages::RequestCharacterInfo *msgRequestCharacterInfo = ParseMessage<NetworkMessages::RequestCharacterInfo>(m_pBitStream);

        HandleRequestCharacterInfo();
      }
      break;

    case S_REPLY_CHARID:
      {
        NetworkMessages::ReplyCharacterId *msgReplyCharacterId = ParseMessage<NetworkMessages::ReplyCharacterId>(m_pBitStream);

        HandleReplyCharacterId(msgReplyCharacterId);
      }
      break;

    case S_REPLY_EQUIPMENT_ID:
      {
        NetworkMessages::EquipmentSetID *msgEquipmentSetID = ParseMessage<NetworkMessages::EquipmentSetID>(m_pBitStream);

        HandleReplyEquipmentID(msgEquipmentSetID);
      }
      break;

    case S_PUSH_NEWCHAR:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::Character *msgCharacter = ParseMessage<NetworkMessages::Character>(m_pBitStream);

        HandleCharacterCreation(msgCharacter);
      }
      break;

    case S_PUSH_NEWEQUIPMENT:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::Equipment *msgEquipment = ParseMessage<NetworkMessages::Equipment>(m_pBitStream);
        
        HandleEquipmentCreation(msgEquipment); 
      }
      break;

    case S_PUSH_ADDCHARMINION:
      break;

    case S_PUSH_EQUIPMENT_DROP:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::EquipmentDrop *msgEquipmentDrop = ParseMessage<NetworkMessages::EquipmentDrop>(m_pBitStream);
        NetworkMessages::Position msgPosition = msgEquipmentDrop->position().Get(0);

        Vector3 position;
        position.x = msgPosition.x();
        position.y = msgPosition.y();
        position.z = msgPosition.z();

        u32 equipmentId = msgEquipmentDrop->equipmentid();
        bool unk0 = msgEquipmentDrop->unk0();

        HandleEquipmentDrop(equipmentId, position, unk0);
      }
      break;

    case S_PUSH_EQUIPMENT_PICKUP:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::EquipmentPickup *msgEquipmentPickup = ParseMessage<NetworkMessages::EquipmentPickup>(m_pBitStream);

        u32 equipmentId = msgEquipmentPickup->equipmentid();
        u32 characterId = msgEquipmentPickup->characterid();

        NetworkMessages::Position msgPosition = msgEquipmentPickup->position();
        Vector3 position(msgPosition.x(), msgPosition.y(), msgPosition.z());

        HandleEquipmentPickup(characterId, equipmentId, position);
      }
      break;

    case S_PUSH_EQUIPMENT_EQUIP:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::InventoryAddEquipment *msgInventoryAddEquipment = ParseMessage<NetworkMessages::InventoryAddEquipment>(m_pBitStream);
        u32 ownerId = msgInventoryAddEquipment->ownerid();
        u32 equipmentId = msgInventoryAddEquipment->equipmentid();
        u32 slot = msgInventoryAddEquipment->slot();
        u32 unk0 = msgInventoryAddEquipment->unk0();

        HandleInventoryAddEquipment(ownerId, equipmentId, slot, unk0);
      }
      break;

    case S_PUSH_EQUIPMENT_UNEQUIP:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::InventoryRemoveEquipment *msgInventoryRemoveEquipment = ParseMessage<NetworkMessages::InventoryRemoveEquipment>(m_pBitStream);
        u32 ownerId = msgInventoryRemoveEquipment->ownerid();
        u32 equipmentId = msgInventoryRemoveEquipment->equipmentid();

        HandleInventoryRemoveEquipment(ownerId, equipmentId);
      }
      break;

    case S_PUSH_EQUIPMENT_USE:
      break;

    case S_PUSH_EQUIPMENT_IDENTIFY:
      {
        NetworkMessages::EquipmentIdentify *msgEquipmentIdentify = ParseMessage<NetworkMessages::EquipmentIdentify>(m_pBitStream);

        HandleEquipmentIdentify(msgEquipmentIdentify);
      }
      break;

    case S_PUSH_EQUIPMENT_ADDENCHANT:
      break;

    case S_PUSH_EQUIPMENT_ADDSOCKET:
      break;

    case S_PUSH_CHARACTER_SETDEST:
      {
        // Ignore server character creation if we're not in the game
        if (!gameClient->inGame) {
          break;
        }

        NetworkMessages::CharacterDestination *msgCharacterDestination = ParseMessage<NetworkMessages::CharacterDestination>(m_pBitStream);
        NetworkMessages::Position msgDestination = msgCharacterDestination->destination();
        NetworkMessages::Position msgCurrent = msgCharacterDestination->current();

        u32 id = msgCharacterDestination->id();

        Vector3 destination;
        destination.x = msgDestination.x();
        destination.y = msgDestination.z();
        destination.z = msgDestination.z();

        Vector3 currentPosition;
        currentPosition.x = msgCurrent.x();
        currentPosition.y = msgCurrent.y();
        currentPosition.z = msgCurrent.z();

        u8 running = msgCharacterDestination->running();
        u8 attacking = msgCharacterDestination->attacking();

        HandleCharacterDestination(id, currentPosition, destination, running, attacking);
      }
      break;

    case S_PUSH_CHARACTER_ACTION:
      {
        NetworkMessages::CharacterAction *msgCharacterAction = ParseMessage<NetworkMessages::CharacterAction>(m_pBitStream);

        HandleCharacterSetAction(msgCharacterAction);
      }
      break;

      
    case S_PUSH_CHARACTER_ATTACK:
      {
        //* Hold off on this for now
        NetworkMessages::CharacterAttack *msgCharacterAttack = ParseMessage<NetworkMessages::CharacterAttack>(m_pBitStream);

        HandleCharacterAttack(msgCharacterAttack);
        //*/
      }
      break;

    case S_PUSH_CHARACTER_USESKILL:
      {
        NetworkMessages::CharacterUseSkill *msgCharacterUseSkill = ParseMessage<NetworkMessages::CharacterUseSkill>(m_pBitStream);

        HandleCharacterUseSkill(msgCharacterUseSkill);
      }
      break;

    case S_PUSH_EQUIPMENT_STACKUPDATE:
      {
        NetworkMessages::EquipmentUpdateStackSize *msgEquipmentUpdateStackSize = ParseMessage<NetworkMessages::EquipmentUpdateStackSize>(m_pBitStream);

        HandleEquipmentUpdateStack(msgEquipmentUpdateStackSize);
      }
      break;

    case S_PUSH_EQUIPMENT_ADD_GEM:
      {
        NetworkMessages::EquipmentAddGem *msgEquipmentAddGem = ParseMessage<NetworkMessages::EquipmentAddGem>(m_pBitStream);

        HandleEquipmentAddGem(msgEquipmentAddGem);
      }
      break;

    case S_PUSH_EQUIPMENT_REMOVE_GEMS:
      {
        NetworkMessages::EquipmentRemoveGems *msgEquipmentRemoveGems = ParseMessage<NetworkMessages::EquipmentRemoveGems>(m_pBitStream);

        HandleEquipmentRemoveGems(msgEquipmentRemoveGems);
      }
      break;
      
    case S_PUSH_CHAT_PLAYER:
      {
        NetworkMessages::ChatPlayerText *msgChatPlayerText = ParseMessage<NetworkMessages::ChatPlayerText>(m_pBitStream);

        HandleChatMessage(msgChatPlayerText);
      }
      break;

    case S_PUSH_CHANGE_LEVEL:
      {
        NetworkMessages::ChangeLevel *msgChangeLevel = ParseMessage<NetworkMessages::ChangeLevel>(m_pBitStream);

        HandleChangeLevel(msgChangeLevel);
      }
      break;

    case S_PUSH_CHAR_UPDATE_HEALTH:
      {
        NetworkMessages::CharacterUpdateHealth *msgCharacterUpdateHealth = ParseMessage<NetworkMessages::CharacterUpdateHealth>(m_pBitStream);

        HandleUpdateHealth(msgCharacterUpdateHealth);
      }
      break;

    case S_PUSH_CHAR_DESTROY:
      {
        NetworkMessages::CharacterDestroy *msgCharacterDestroy = ParseMessage<NetworkMessages::CharacterDestroy>(m_pBitStream);

        HandleCharacterDestroy(msgCharacterDestroy);
      }
      break;

    case S_PUSH_LEVEL_CREATE_ITEM:
      {
        NetworkMessages::LevelCreateItem *msgLevelCreateItem = ParseMessage<NetworkMessages::LevelCreateItem>(m_pBitStream);

        HandleLevelCreateItem(msgLevelCreateItem);
      }
      break;

    case S_PUSH_LEVEL_ITEM_DROP:
      {
        NetworkMessages::LevelDropItem *msgLevelDropItem = ParseMessage<NetworkMessages::LevelDropItem>(m_pBitStream);

        HandleLevelDropItem(msgLevelDropItem);
      }
      break;
      
    case S_PUSH_BREAKABLE_TRIGGERED:
      {
        NetworkMessages::BreakableTriggered *msgBreakableTriggered = ParseMessage<NetworkMessages::BreakableTriggered>(m_pBitStream);

        HandleBreakableTriggered(msgBreakableTriggered);
      }
      break;

    case S_PUSH_TRIGGER_TRIGGERED:
      {
        NetworkMessages::TriggerUnitTriggered *msgTriggerUnitTriggered = ParseMessage<NetworkMessages::TriggerUnitTriggered>(m_pBitStream);

        HandleTriggerUnitTriggered(msgTriggerUnitTriggered);
      }
      break;

    case S_PUSH_ITEM_GOLD:
      {
        NetworkMessages::ItemGoldCreate *msgItemGoldCreate = ParseMessage<NetworkMessages::ItemGoldCreate>(m_pBitStream);

        HandleItemGoldAmount(msgItemGoldCreate);
      }
      break;

    case S_PUSH_CHARACTER_ALIGNMENT:
      {
        NetworkMessages::CharacterAlignment *msgCharacterAlignment = ParseMessage<NetworkMessages::CharacterAlignment>(m_pBitStream);

        HandleCharacterAlignment(msgCharacterAlignment);
      }
      break;

    case S_PUSH_CHARACTER_SETTARGET:
      {
        NetworkMessages::CharacterSetTarget *msgCharacterSetTarget = ParseMessage<NetworkMessages::CharacterSetTarget>(m_pBitStream);

        HandleCharacterSetTarget(msgCharacterSetTarget);
      }
      break;

    case S_PUSH_TRIGGERUNIT_SYNC:
      {
        NetworkMessages::TriggerUnitSync *msgTriggerUnitSync = ParseMessage<NetworkMessages::TriggerUnitSync>(m_pBitStream);

        HandleTriggerUnitSync(msgTriggerUnitSync);
      }
      break;

    case S_PUSH_CHARACTER_RESURRECT:
      {
        NetworkMessages::CharacterResurrect *msgCharacterResurrect = ParseMessage<NetworkMessages::CharacterResurrect>(m_pBitStream);

        HandleCharacterResurrect(msgCharacterResurrect);
      }
      break;

    case S_PUSH_ADDEXPERIENCE:
      {
        NetworkMessages::CharacterAddExperience *msgCharacterAddExperience = ParseMessage<NetworkMessages::CharacterAddExperience>(m_pBitStream);

        HandleCharacterAddExperience(msgCharacterAddExperience);
      }
      break;

    case S_PUSH_CHARACTERKILLED:
      {
        NetworkMessages::CharacterKilledCharacter *msgCharacterKilledCharacter = ParseMessage<NetworkMessages::CharacterKilledCharacter>(m_pBitStream);

        HandleCharacterKillCharacter(msgCharacterKilledCharacter);
      }
      break;

    case S_PUSH_CHARACTER_ADD_SKILL:
      {
        NetworkMessages::BaseUnitAddSkill *msgCharacterAddSkill = ParseMessage<NetworkMessages::BaseUnitAddSkill>(m_pBitStream);

        HandleCharacterAddSkill(msgCharacterAddSkill);
      }
      break;

    case S_PUSH_RANDOM_SEED:
      {
        NetworkMessages::RandomSeed *msgRandomSeed = ParseMessage<NetworkMessages::RandomSeed>(m_pBitStream);

        HandleRandomSeed(msgRandomSeed);
      }
      break;

    case S_PUSH_ORIENTATION:
      {
        NetworkMessages::CharacterOrientation *msgCharacterOrientation = ParseMessage<NetworkMessages::CharacterOrientation>(m_pBitStream);

        HandleCharacterOrientation(msgCharacterOrientation);
      }
      break;

    case S_PUSH_WEAPONSWAP:
      {
        NetworkMessages::PlayerSwapWeapons *msgPlayerSwapWeapons = ParseMessage<NetworkMessages::PlayerSwapWeapons>(m_pBitStream);

        HandlePlayerWeaponSwap(msgPlayerSwapWeapons);
      }
      break;

    case S_PUSH_SKILLLEVEL:
      {
        NetworkMessages::CharacterSetSkillPoints *msgCharacterSetSkillPoints = ParseMessage<NetworkMessages::CharacterSetSkillPoints>(m_pBitStream);

        HandleCharacterSetSkillPoints(msgCharacterSetSkillPoints);
      }
      break;

    case S_PUSH_CHARACTER_VISIBILITY:
      {
        NetworkMessages::Visibility *msgVisibility = ParseMessage<NetworkMessages::Visibility>(m_pBitStream);

        HandleCharacterVisibility(msgVisibility);
      }
      break;

    case S_PUSH_AUTOEQUIP:
      {
        NetworkMessages::EquipmentAutoEquip *msgEquipmentAutoEquip = ParseMessage<NetworkMessages::EquipmentAutoEquip>(m_pBitStream);

        HandleEquipmentAutoEquip(msgEquipmentAutoEquip);
      }
      break;

    }
  }
}

//
// Below are the message handlers

// Receives a version from the server, replies with it's own version
void Client::HandleVersion(u32 version)
{
  multiplayerLogger.WriteLine(Info, L"Client received Server Version: %i", version);

  NetworkMessages::Version msgVersion;
  msgVersion.set_version(MessageVersion);

  SendMessage<NetworkMessages::Version>(C_VERSION, &msgVersion);

  // After we send, check if they're the same
  if (version != MessageVersion) {
    multiplayerLogger.WriteLine(Info, L"Disconnecting from the Server: Version Mismatch: (Client = %i  Server = %i)",
      MessageVersion, version);

    Disconnect();
  } else {
    // If they're Ok, Request whether the Game has started yet
    NetworkMessages::GameHasStarted msgGameHasStarted;

    SendMessage<NetworkMessages::GameHasStarted>(C_REQUEST_HASGAMESTARTED, &msgGameHasStarted);
  }
}

// Receives from server whether the game has started
// If it has, it's safe to enter the game, else block player from entering.
void Client::HandleHasGameStarted(bool gameStarted)
{
  log(L"Server game started: %s", gameStarted ? L"true" : L"false");

  Client::getSingleton().SetServerGameStarted(gameStarted);
}

// Receives a push from the server when it finally starts the game.
void Client::HandleGameStarted()
{
  Client::getSingleton().SetServerGameStarted(true);

  // TODO Show Window Dialog informing user the server has started and they can enter.
}

// Receives a push from server when the game has been stopped (Exited to MainMenu)
void Client::HandleGameEnded()
{
  // Debugging, this func @: 5E1EE480

  logColor(B_RED, L"Server has Exited the Game");

  /*
  log("Removing server created characters...");
  {
    LinkedListNode* itr = *gameClient->pCLevel->ppCCharacters2;
    while (itr != NULL) {
      CCharacter* character = (CCharacter*)itr->pCBaseUnit;
      logColor(B_GREEN, L"  Level Character2: (itr = %p) %p %s", itr, character, character->characterName.c_str());
      //multiplayerLogger.WriteLine(Info, L"  Level Item: (itr = %p) %p %s", itr, character, character->characterName.c_str());
      itr = itr->pNext;
    }
  }
  */

  // Load to title menu
  log(L"LoadMap...");
  //GameClient_LoadMap(gameClient, 0);
  gameClient->LoadGameMap(0, 0);
  log(L"LoadMap...Done");

  // Kill the connection
  //Disconnect();

  //Client::getSingleton().SetServerGameStarted(false);
  log(L"debug0 point");
}

// Receives a request from the Server for Character Info
void Client::HandleRequestCharacterInfo()
{
  NetworkMessages::ReplyCharacterInfo msgReplyCharacterInfo;

  NetworkMessages::Character *msgPlayer         = msgReplyCharacterInfo.mutable_player();
  NetworkMessages::Position  *msgPlayerPosition = msgPlayer->mutable_position();

  // Add the player information
  CPlayer *player = gameClient->pCPlayer;
  string playerName(player->characterName.begin(), player->characterName.end());
  playerName.assign(player->characterName.begin(), player->characterName.end());

  msgPlayer->set_guid(player->GUID);
  msgPlayer->set_name(playerName);
  msgPlayer->set_level(player->level);

  // This allows a workaround for the modified attributes needed for equipment
  /*
  msgPlayer->set_strength(player->baseStrength);
  msgPlayer->set_dexterity(player->baseDexterity);
  msgPlayer->set_defense(player->baseDefense);
  msgPlayer->set_magic(player->baseMagic);
  */
  /*
  msgPlayer->set_strength(10000);
  msgPlayer->set_dexterity(10000);
  msgPlayer->set_defense(10000);
  msgPlayer->set_magic(10000);
  */
  msgPlayer->set_strength(player->baseStrength + 10);
  msgPlayer->set_dexterity(player->baseDexterity + 10);
  msgPlayer->set_defense(player->baseDefense + 10);
  msgPlayer->set_magic(player->baseMagic + 10);

  msgPlayer->set_health(player->healthMax);
  msgPlayer->set_mana(player->manaMax);
  msgPlayer->set_weaponsetactive(player->weaponSetToggle);

  // Inventory tab sizes
  Helper_BuildInventoryTabIndexSize(*msgPlayer, player);

  msgPlayerPosition->set_x(player->GetPosition().x);
  msgPlayerPosition->set_y(player->GetPosition().y);
  msgPlayerPosition->set_z(player->GetPosition().z);

  // Add the Character's Skills from both lists
  CSkillManager *skillManager = player->pCSkillManager;  
  for (u32 i = 0; i < skillManager->listSkills0.size; i++) {
    CSkill *skill = skillManager->listSkills0[i];
    string *skillName = msgPlayer->add_skills();

    skillName->assign(skill->pCSkillProperty0->skillName.begin(), skill->pCSkillProperty0->skillName.end());
  }
  for (u32 i = 0; i < skillManager->listSkills1.size; i++) {
    CSkill *skill = skillManager->listSkills1[i];
    string *skillName = msgPlayer->add_skills();

    skillName->assign(skill->pCSkillProperty0->skillName.begin(), skill->pCSkillProperty0->skillName.end());
  }
  
  // Add Minions
  vector<CCharacter*> *minionList = gameClient->pCPlayer->GetMinions();
  vector<CCharacter*>::iterator itr;
  for (itr = minionList->begin(); itr != minionList->end(); itr++) {
    string petName((*itr)->characterName.begin(), (*itr)->characterName.end());
    petName.assign((*itr)->characterName.begin(), (*itr)->characterName.end());

    NetworkMessages::Character *msgPet = msgPlayer->add_minion();
    msgPet->set_guid((*itr)->GUID);
    msgPet->set_name(petName);
    msgPet->set_level((*itr)->level);

    Helper_BuildInventoryTabIndexSize(*msgPet, (*itr));

    multiplayerLogger.WriteLine(Info, L"Client::Pet tab sizes count: %i %i", (*itr)->pCInventory->tabIndices.size(), (*itr)->pCInventory->tabSizes.size());
    for (u32 i = 0; i < (*itr)->pCInventory->tabIndices.size(); ++i) {
      multiplayerLogger.WriteLine(Info, L"  Tab [%i] size: %i", (*itr)->pCInventory->tabIndices[i], (*itr)->pCInventory->tabSizes[i]);
    }
  }

  Client::getSingleton().SendMessage<NetworkMessages::ReplyCharacterInfo>(C_REPLY_CHARINFO, &msgReplyCharacterInfo);

  Helper_ClientPushAllEquipment();
}

// Receives a Character Network ID from the server to apply to our character
// This is really a special message since the Client creates the player without the server
// telling it to
void Client::HandleReplyCharacterId(TLMP::NetworkMessages::ReplyCharacterId *msgReplyCharacterId)
{
  CPlayer *player = gameClient->pCPlayer;

  // Update our player or minion with the Network ID
  if (player->GUID == msgReplyCharacterId->guid()) {
    NetworkEntity *newEntity = new NetworkEntity(player, msgReplyCharacterId->id());
    NetworkSharedCharacters->push_back(newEntity);
  } else {
    vector<CCharacter*> *minions = player->GetMinions();
    vector<CCharacter*>::iterator itr;
    for (itr = minions->begin(); itr != minions->end(); itr++) {
      if ((*itr)->GUID == msgReplyCharacterId->guid()) {
        NetworkEntity *newEntity = new NetworkEntity((*itr), msgReplyCharacterId->id());
        NetworkSharedCharacters->push_back(newEntity);

        break;
      }
    }
  }  
}

// Pushes player inventory equipment list to the server to create
void Client::PushEquipment()
{
}

// Handles Character Set Destination
void Client::HandleCharacterDestination(u32 commonId, Vector3 current, Vector3 destination, u8 running, u8 attacking)
{
  //multiplayerLogger.WriteLine(Info, L"Client received character setDestination (CommonID = %x), Position = %f, %f",
  //  commonId, destination.x, destination.z);
  //log(L"Client received character setDestination (CommonID = %x), Position = %f, %f",
  //  commonId, destination.x, destination.z);

  // Search for our Character given the CommonID
  NetworkEntity *entity = searchCharacterByCommonID(commonId);

  if (!entity) {
    //multiplayerLogger.WriteLine(Error, L"Error: Could not find internal object in shared characters from commonId: %x when receiving character destination.", commonId);
    //log(L"Error: Could not find internal object in shared characters from commonId: %x when receiving character destination.", commonId);
    return;
  }

  CCharacter *character = (CCharacter *)entity->getInternalObject();

  if (character) {
    HelperCharacterPositioning(character, current);

    // Lock the Client from sending a network message back out when setting the Character Destination
    Client::getSingleton().SetSuppressed_SetDestination(true);
    character->SetDestination(gameClient->pCLevel, destination.x, destination.z);
    Client::getSingleton().SetSuppressed_SetDestination(false);

    character->running = running;
    character->attacking = attacking;
  } else {
    //multiplayerLogger.WriteLine(Error, L"Error: Character is NULL");
    //log(L"Error: Character is NULL (NetworkID: %x) (NetworkCharacter List Size: %i)", 
    //  commonId, NetworkSharedCharacters->size());
  }
}

// Handles Character Creation
void Client::HandleCharacterCreation(NetworkMessages::Character *msgCharacter)
{
  NetworkMessages::Position msgCharacterPosition = msgCharacter->position();
  NetworkMessages::Visibility msgCharacterVisibility = msgCharacter->visibility();

  Vector3 posCharacter;
  posCharacter.x = msgCharacterPosition.x();
  posCharacter.y = msgCharacterPosition.y();
  posCharacter.z = msgCharacterPosition.z();

  u32 commonId = msgCharacter->id();
  u32 health = msgCharacter->health();
  u32 mana = msgCharacter->mana();
  u32 levelCharacter = msgCharacter->level();
  u32 strength = msgCharacter->strength();
  u32 dexterity = msgCharacter->dexterity();
  u32 defense = msgCharacter->defense();
  u32 magic = msgCharacter->magic();
  u64 guidCharacter = msgCharacter->guid();
  string characterName = msgCharacter->name();
  u32 alignment = msgCharacter->alignment();
  u32 weaponSetToggle = msgCharacter->weaponsetactive();

  multiplayerLogger.WriteLine(Info, L"Client received character creation: (CommonID = %x) (GUID = %016I64X, name = %s)",
    commonId, guidCharacter, TLMP::convertAsciiToWide(characterName).c_str());
  log(L"Client received character creation: (CommonID = %x) (GUID = %016I64X, name = %s)",
    commonId, guidCharacter, TLMP::convertAsciiToWide(characterName).c_str());

  // Do not create the stash or shared stash from the server
  if (guidCharacter == STASH || guidCharacter == SHAREDSTASH) {
    log(L"Suppressing stash/shared stash creation from server.");
    return;
  }

  NetworkEntity *existingEntity = searchCharacterByCommonID(commonId);

  if (!existingEntity) {
    CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
    CLevel *level = gameClient->pCLevel;

    // Create the monster, setup name and alignment
    Client::getSingleton().SetSuppressed_CharacterCreation(false);
    CMonster* monster = resourceManager->CreateMonster(guidCharacter, levelCharacter, true);
    
    if (monster) {
      monster->characterName.assign(convertAsciiToWide(characterName));
      monster->SetAlignment(alignment);
      level->CharacterInitialize(monster, &posCharacter, 0);
      monster->healthMax = health;
      monster->healthMax4 = health;
      monster->manaMax = mana;
      monster->baseStrength = strength;
      monster->baseDexterity = dexterity;
      monster->baseDefense = defense;
      monster->baseMagic = magic;
      monster->weaponSetToggle = weaponSetToggle;

      // Setup visibility
      monster->flagFadingIn = msgCharacterVisibility.fading_in();
      monster->flagHasBeenDisplayed = msgCharacterVisibility.has_been_displayed();
      monster->flagFadingOut = msgCharacterVisibility.fading_out();
      monster->unkTransparency1 = msgCharacterVisibility.transparency();

      // Handle inventory tab sizes
      Helper_ExtractInventoryTabIndexSize(msgCharacter, monster);
    } else {
      Client::getSingleton().SetSuppressed_CharacterCreation(true);
      multiplayerLogger.WriteLine(Error, L"Error: Character created was null!");
      //logColor(B_RED, L"Error: Character created was null!");
      return;
    }

    Client::getSingleton().SetSuppressed_CharacterCreation(true);

    // Create a network ID to identify this monster later
    NetworkEntity *newEntity = addCharacter(monster, commonId);

    // Add it to the BaseUnit list
    OutsideBaseUnits->push_back(monster);
  } else {
    CCharacter *character = (CCharacter*)existingEntity->getInternalObject();
    if (character) {
      //log(L"Client: Ignoring character creation for CommonID: %x (%s), already have character assigned: %s",
      //  commonId, convertAsciiToWide(characterName).c_str(), character->characterName.c_str());
    }
  }
}

void Client::HandleEquipmentCreation(TLMP::NetworkMessages::Equipment *msgEquipment)
{
  u32 id = msgEquipment->id();
  u64 guid = msgEquipment->guid();
  u32 stacksize = msgEquipment->stacksize();
  u32 stacksizemax = msgEquipment->stacksizemax();
  u32 socketcount = msgEquipment->socketcount();
  u32 gemCount = msgEquipment->gems_size();
  u32 enchantCount = msgEquipment->enchants_size();

  u8 identified = msgEquipment->identified();

  u32 minPhysicalDamage = msgEquipment->physical_damage_min();
  u32 maxPhysicalDamage = msgEquipment->physical_damage_max();

  wstring nameUnidentified = convertAsciiToWide(msgEquipment->name_unidentified());
  wstring namePrefix = convertAsciiToWide(msgEquipment->name_prefix());
  wstring nameSuffix = convertAsciiToWide(msgEquipment->name_suffix());

  u32 reqLevel = msgEquipment->req_level();
  u32 reqStrength = msgEquipment->req_strength();
  u32 reqDexterity = msgEquipment->req_dexterity();
  u32 reqMagic = msgEquipment->req_magic();
  u32 reqDefense = msgEquipment->req_defense();


  multiplayerLogger.WriteLine(Info, L"Client received equipment creation: (CommonID = %x) (GUID = %016I64X) (Stack: %i/%i) (SocketCount: %i)",
    id, guid, stacksize, stacksizemax, socketcount);
  log(L"Client received equipment creation: (CommonID = %x) (GUID = %016I64X) (Stack: %i/%i) (SocketCount: %i)",
    id, guid, stacksize, stacksizemax, socketcount);

  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;

  if (resourceManager) {
    log(L"ResourceManager::CreateEquipment Pre (%p, %016I64X)", resourceManager, guid);

    Client::getSingleton().SetSuppressed_EquipmentCreation(false);
    CEquipment *equipment = resourceManager->CreateEquipment(guid, 1, 1, 0);
    Client::getSingleton().SetSuppressed_EquipmentCreation(true);
    
    log(L"ResourceManager::CreateEquipment Post");

    equipment->socketCount = socketcount;
    equipment->stackSize = stacksize;
    equipment->stackSizeMax = stacksizemax;

    equipment->identified = identified;

    equipment->minimumPhysicalDamage = minPhysicalDamage;
    equipment->maximumPhysicalDamage = maxPhysicalDamage;
    equipment->changeMeForDamageUpdate = maxPhysicalDamage;

    equipment->nameUnidentified = nameUnidentified;
    equipment->namePrefix = namePrefix;
    equipment->nameSuffix = nameSuffix;
    equipment->requirements[0] = reqLevel;
    equipment->requirements[1] = reqStrength;
    equipment->requirements[2] = reqDexterity;
    equipment->requirements[3] = reqMagic;
    equipment->requirements[4] = reqDefense;
       
    // Add to our BaseUnits list
    multiplayerLogger.WriteLine(Info, L"Client: Adding gems to new equipment...");
    log(L"Client: Adding gems to new equipment...");

    // Add the gems
    for (u32 i = 0; i < gemCount; i++) {
      NetworkMessages::Equipment msgGem = msgEquipment->gems().Get(i);
      CEquipment *gem = resourceManager->CreateEquipment(msgGem.guid(), 1, 1, 0);
      
      multiplayerLogger.WriteLine(Info, L"Client: Adding gem to shared network equipment: %i", msgGem.id());
      log(L"Client: Adding gem to shared network equipment: %i", msgGem.id());
      NetworkEntity *newEntity = addEquipment(gem, msgGem.id());

      for (int j = 0; j < msgGem.enchants_size(); j++) {
        NetworkMessages::EnchantType msgGemEnchantType = msgGem.enchants().Get(j);

        SetAllow_AddEffect(true);
        gem->AddEnchant((EffectType)msgGemEnchantType.type(), (EnchantType)msgGemEnchantType.subtype(), msgGemEnchantType.value_min(), msgGemEnchantType.value_max());
        SetAllow_AddEffect(false);
      }

      equipment->gemList.push(gem);
    }

    multiplayerLogger.WriteLine(Info, L"Client: Adding enchants to new equipment... count = %i", enchantCount);
    log(L"Client: Adding enchants to new equipment... count = %i", enchantCount);

    // Add the enchants
    for (u32 i = 0; i < enchantCount; i++) {
      NetworkMessages::EnchantType msgEnchantType = msgEquipment->enchants().Get(i);

      multiplayerLogger.WriteLine(Info, L"Client: Enchant %i: Type: %x, SubType: %x, Value: %f / %f", i, msgEnchantType.type(), msgEnchantType.subtype(), msgEnchantType.value_min(), msgEnchantType.value_max());
      log(L"Client: Enchant %i: Type: %x, SubType: %x, Value: %f / %f", i, msgEnchantType.type(), msgEnchantType.subtype(), msgEnchantType.value_min(), msgEnchantType.value_max());

      SetAllow_AddEffect(true);
      equipment->AddEnchant((EffectType)msgEnchantType.type(), (EnchantType)msgEnchantType.subtype(), msgEnchantType.value_min(), msgEnchantType.value_max());
      SetAllow_AddEffect(false);
    }

    multiplayerLogger.WriteLine(Info, L"Client: Adding equipment to shared network equipment");
    log(L"Client: Adding equipment to shared network equipment");

    NetworkEntity *newEntity = addEquipment(equipment, id);
    
    multiplayerLogger.WriteLine(Info, L"Client: Adding equipment to shared network equipment.. Done");
    log(L"Client: Adding equipment to shared network equipment.. Done");
  }
}

void Client::HandleInventoryAddEquipment(u32 ownerId, u32 equipmentId, u32 slot, u32 unk0)
{
  multiplayerLogger.WriteLine(Info, L"Client received inventory add equipment: (CharacterID = %x, EquipmentID = %x, slot = %x)",
    ownerId, equipmentId, slot);
  log(L"Client received inventory add equipment: (CharacterID = %x, EquipmentID = %x, slot = %x)",
    ownerId, equipmentId, slot);

  NetworkEntity* owner = searchCharacterByCommonID(ownerId);
  NetworkEntity* equipment = searchEquipmentByCommonID(equipmentId);

  if (owner) {
    if (equipment) {
      CCharacter *characterOwner = (CCharacter*)owner->getInternalObject();
      CEquipment *equipmentReal = (CEquipment*)equipment->getInternalObject();
      CInventory *inventory = characterOwner->pCInventory;

      /*
      log(L"Debug: Owner: %p Equipment: %p Inventory: %p Slot: %x Unk0: %x",
        characterOwner, equipmentReal, inventory, slot, unk0);
      log(L"  Character: %s", characterOwner->characterName.c_str());
      log(L"  Equipment: %s", equipmentReal->nameReal.c_str());
      log(L"  Inventory EquipmentList Size: %x", inventory->equipmentList.size);
      */

      Client::getSingleton().SetSuppressed_EquipmentCreation(false);
      inventory->AddEquipment(equipmentReal, slot, unk0);
      Client::getSingleton().SetSuppressed_EquipmentCreation(true);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment with ID = %x", equipmentId);
      //log(L"Error: Could not find Equipment with ID = %x", equipmentId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    //log(L"Error: Could not find Character with ID = %x", ownerId);
  }

  //log(L"HandleInventoryAddEquipment Done.");
}

void Client::HandleInventoryRemoveEquipment(u32 ownerId, u32 equipmentId)
{
  multiplayerLogger.WriteLine(Info, L"Client received inventory remove equipment: (CharacterID = %x, EquipmentID = %x)",
    ownerId, equipmentId);
  //log(L"Client received inventory remove equipment: (CharacterID = %x, EquipmentID = %x)",
  //  ownerId, equipmentId);

  NetworkEntity* owner = searchCharacterByCommonID(ownerId);
  NetworkEntity* equipment = searchEquipmentByCommonID(equipmentId);

  if (owner) {
    if (equipment) {
      CCharacter *characterOwner = (CCharacter*)owner->getInternalObject();
      CEquipment *equipmentReal = (CEquipment*)equipment->getInternalObject();

      CInventory *inventory = characterOwner->pCInventory;

      if (inventory) {
        // Suppress from sending this out again
        SetSuppressed_SendEquipmentUnequip(true);
        inventory->RemoveEquipment(equipmentReal);
        SetSuppressed_SendEquipmentUnequip(false);
      } else {
        multiplayerLogger.WriteLine(Error, L"Error: Inventory was NULL.");
        //log(L"Error: Inventory was NULL.");
      }
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment with ID = %x", equipmentId);
      //log(L"Error: Could not find Equipment with ID = %x", equipmentId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    //log(L"Error: Could not find Character with ID = %x", ownerId);
  }
}

void Client::HandleEquipmentDrop(u32 equipmentId, Vector3 position, bool unk0)
{
  multiplayerLogger.WriteLine(Info, L"Client received Equipment Drop: (EquipmentId = %x, Position: %f, %f, %f, Unk0: %i",
    equipmentId, position.x, position.y, position.z, unk0);
  //log(L"Client received Equipment Drop: (EquipmentId = %x, Position: %f, %f, %f, Unk0: %i",
  //  equipmentId, position.x, position.y, position.z, unk0);

  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);

  if (netEquipment) {
    CEquipment *equipment = (CEquipment *)netEquipment->getInternalObject();
    CLevel *level = gameClient->pCLevel;

    SetSuppressed_EquipmentDrop(true);
    SetAllow_LevelItemDrop(true);
    level->EquipmentDrop(equipment, position, unk0);
    SetAllow_LevelItemDrop(false);

    /*
    // BUG Fix: Move through all character inventories and remove any EquipmentRef's to this equipment
    //  stops client from creating duplicate equipmentrefs when inventories are full - which shouldn't occur to begin with
    //  inventory::settabsize should stop this client-side
    {
      CLevel *level = gameClient->pCLevel;
      if (level) {
        vector<CCharacter*> &characters = level->GetCharacters();
        vector<CCharacter*>::iterator itr;
        for (itr = characters.begin(); itr != characters.end(); ++itr) {
          CCharacter* character = (*itr);
          if (character) {
            CInventory* inv = character->pCInventory;
            if (inv) {
              for (u32 i = 0; i < inv->equipmentList.size; ++i) {
                CEquipmentRef* ref = inv->equipmentList[i];
                if (ref->pCEquipment == equipment) {
                  inv->RemoveEquipment(equipment);
                  break;
                }
              }
            }
          }
        }
      }
    }
    */
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment from CommonId: %x",
      equipmentId);
    //log(L"Error: Could not find Equipment from CommonId: %x",
    //  equipmentId);
  }
}

void Client::HandleEquipmentPickup(u32 characterId, u32 equipmentId, const Vector3& position)
{
  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  //log(L"Client: Pickup Equipment: %x (Character: %x)", equipmentId, characterId);
  

  // Testing it out currently...
  // Turn this off for now - it causes a bug in KeyManager::0x4E4636
  //   Bad Param passed in?
  if (!netEquipment) {
    netEquipment = searchItemByCommonID(equipmentId);
  }


  if (netEquipment) {
    if (netCharacter) {
      CEquipment *equipment = (CEquipment *)netEquipment->getInternalObject();
      CCharacter *character = (CCharacter *)netCharacter->getInternalObject();

      SetSuppressed_EquipmentPickup(false);
      character->PickupEquipment(equipment, gameClient->pCLevel);
      SetSuppressed_EquipmentPickup(true);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Character from CommonId: %x", characterId);
      //log(L"Error: Could not find Character from CommonId: %x", characterId);
    }
  } else {
    /*
    // Search our level's items for equipment position
    if (netCharacter) {
      CCharacter *character = (CCharacter *)netCharacter->getInternalObject();

      LinkedListNode* itrEquipment = *gameClient->pCLevel->itemsAll;
      while (itrEquipment) {
        CEquipment *equipment = (CEquipment*)itrEquipment->pCBaseUnit;

        if (equipment->GetPosition().squaredDistance(position) < EPSILON) {
          NetworkEntity *entity = searchEquipmentByInternalObject(equipment);

          if (character) {
            character->PickupEquipment(equipment, gameClient->pCLevel);
          }
          break;
        }

        itrEquipment = itrEquipment->pNext;
      }
    }
    */

    //multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment from CommonId: %x", equipmentId);
    //log(L"Error: Could not find Equipment from CommonId: %x", equipmentId);
  }
}

void Client::HandleReplyEquipmentID(NetworkMessages::EquipmentSetID *msgEquipmentSetID)
{
  u64 guid = msgEquipmentSetID->guid();
  u32 client_id = msgEquipmentSetID->client_id();
  u32 id = msgEquipmentSetID->id();

  multiplayerLogger.WriteLine(Info, L"  Client: Received a commonID from server: %016I64X of Client Temp ID: %x  ID = %x",
    guid, client_id, id);
  //log(L"  Client: Received a commonID from server: %016I64X in Client Temp ID: %x  ID = %x",
  //  guid, client_id, id);

  // Search for the matching Client ID
  NetworkEntity *clientEquipment = searchEquipmentByClientID(client_id);

  if (clientEquipment) {
    // Remove the Client temp equipment and create the actual network ID with the appropriate common ID
    CEquipment *equipment = (CEquipment*)clientEquipment->getInternalObject();
    removeEquipmentByClientID(client_id);
    addEquipment(equipment, id);

    multiplayerLogger.WriteLine(Info, L"   Equipment name: %s", equipment->nameReal.c_str());

    // Find the character with the equipment equipped
    vector<CCharacter*>* clientCharacters = Helper_ProduceClientSideCharacters(gameClient->pCPlayer);
    vector<CCharacter*>::iterator itr;

    for (itr = clientCharacters->begin(); itr != clientCharacters->end(); ++itr) {
      CCharacter* checkChar = (*itr);

      NetworkEntity *netCharacter = searchCharacterByInternalObject(checkChar);
      if (netCharacter) {
        CInventory *inv = checkChar->pCInventory;
        if (inv) {
          s32 slot = inv->GetEquipmentSlot(equipment);
          //if (slot <= 0x0C) {
          if (slot >= 0) {
            NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
            msgInventoryAddEquipment.set_equipmentid(id);
            msgInventoryAddEquipment.set_guid(equipment->GUID);
            msgInventoryAddEquipment.set_ownerid(netCharacter->getCommonId());
            msgInventoryAddEquipment.set_slot(slot);
            msgInventoryAddEquipment.set_unk0(1);

            multiplayerLogger.WriteLine(Info, L"    Replying server with equipment Slot: %x (OwnerID: %x)", slot, netCharacter->getCommonId());

            Client::getSingleton().SendMessage<NetworkMessages::InventoryAddEquipment>(C_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);

            break;
          }
        } else {
          //log(L"Error: Character has no Inventory!");
          multiplayerLogger.WriteLine(Error, L"Error: Character has no Inventory!");
        }
      }
    }

    /*
    // If the equipment is equipped on the body - tell the server to add it
    NetworkEntity *netCharacter = searchCharacterByInternalObject(gameClient->pCPlayer);
    if (netCharacter) {
      CInventory *inv = gameClient->pCPlayer->pCInventory;
      if (inv) {
        s32 slot = inv->GetEquipmentSlot(equipment);
        //if (slot <= 0x0C) {
        if (slot >= 0) {
          NetworkMessages::InventoryAddEquipment msgInventoryAddEquipment;
          msgInventoryAddEquipment.set_equipmentid(id);
          msgInventoryAddEquipment.set_guid(equipment->GUID);
          msgInventoryAddEquipment.set_ownerid(netCharacter->getCommonId());
          msgInventoryAddEquipment.set_slot(slot);
          msgInventoryAddEquipment.set_unk0(1);

          multiplayerLogger.WriteLine(Info, L"    Replying server with equipment Slot: %x (OwnerID: %x)", slot, netCharacter->getCommonId());

          Client::getSingleton().SendMessage<NetworkMessages::InventoryAddEquipment>(C_PUSH_EQUIPMENT_EQUIP, &msgInventoryAddEquipment);
        }
      } else {
        //log(L"Error: Character has no Inventory!");
        multiplayerLogger.WriteLine(Error, L"Error: Character has no Inventory!");
      }
    }
    */

  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Expected to set common ID for client Equipment, but could not find the Equipment with Client ID = %x",
      client_id);
    //log(L"Error: Expected to set common ID for client Equipment, but could not find the Equipment with Client ID = %x",
    //  client_id);
  }

  // Set the common ID for the Equipment
}

void Client::Helper_ClientPushAllEquipment()
{
  // Search Player and Minion inventories for unknown Common IDs for the Network
  // Any not found will be pushed to the server so we can get them

  CPlayer *player = gameClient->pCPlayer;
  vector<CCharacter*>* clientCharacters = Helper_ProduceClientSideCharacters(player);

  multiplayerLogger.WriteLine(Info, L"Pushing all equipment to server, Client Character Count: %i",
    clientCharacters->size());
  //logColor(B_RED, L"Pushing all equipment to server, Client Character Count: %i",
  //  clientCharacters->size());

  vector<CCharacter*>::iterator itr;
  for (itr = clientCharacters->begin(); itr != clientCharacters->end(); ++itr) {
    CInventory *characterInventory = (*itr)->pCInventory;

    multiplayerLogger.WriteLine(Info, L"  Moving through inventory of size: %i",
      characterInventory->equipmentList.size);
    //log(L"  Moving through inventory of size: %i",
    //  characterInventory->equipmentList.size);

    for (u32 i = 0; i < characterInventory->equipmentList.size; i++) {
      CEquipmentRef* equipmentRef = characterInventory->equipmentList[i];
      CEquipment* equipment = equipmentRef->pCEquipment;

      // Check if the Equipment has a valid common ID, if not add to temp vec and push to server
      NetworkEntity *existingEquipment = searchEquipmentByInternalObject(equipment);

      if (!existingEquipment) {
        multiplayerLogger.WriteLine(Info, L"    Equipment doesn't have a common ID, creating temp and pushing to server.");
        //log(L"    Equipment doesn't have a common ID, creating temp and pushing to server.");

        Helper_ClientPushEquipment(equipment);
      } else {
        multiplayerLogger.WriteLine(Info, L"    Equipment already has a Common ID of: %x",
          existingEquipment->getCommonId());
        //log(L"    Equipment already has a Common ID of: %x",
        //  existingEquipment->getCommonId());
      }
    }
  }

  //log(L"Push All Equipment Completed.");
}

void Client::Helper_ClientPushEquipment(CEquipment *equipment)
{
  // Add this equipment to our temporary list
  NetworkEntity *clientEquipment = addEquipmentClientTemporary(equipment);

  multiplayerLogger.WriteLine(Info, L"Pushing Equipment to Server (Client Temp ID = %x)",
    clientEquipment->getCommonId());
  //log(L"Pushing Equipment to Server (Client Temp ID = %x)",
  //  clientEquipment->getCommonId());

  NetworkMessages::Equipment msgEquipment;
  Helper_PopulateEquipmentMessage(&msgEquipment, equipment, clientEquipment);

  //log(L"  DEBUG: Equipment gemList: %p (Offset = %x)",
  //  &equipment->gemList, ((u32)&equipment->gemList - (u32)equipment));
  multiplayerLogger.WriteLine(Info, L"  Equipment contains %i Gems", equipment->gemList.size);
  //log(L"  Equipment contains %i Gems", equipment->gemList.size);


  // Add the Gem Equipment
  for (u32 i = 0; i < equipment->gemList.size; i++) {
    multiplayerLogger.WriteLine(Info, L"Client: Adding gem...");
    //log(L"Client: Adding gem...");

    NetworkMessages::Equipment *msgGem = msgEquipment.add_gems();
    CEquipment *gem = equipment->gemList[i];

    NetworkEntity *netGem = addEquipmentClientTemporary(gem);
    Helper_PopulateEquipmentMessage(msgGem, gem, netGem);
  }

  Client::getSingleton().SendMessage<NetworkMessages::Equipment>(C_PUSH_EQUIPMENT, &msgEquipment);
}

vector<CCharacter*>* Client::Helper_ProduceClientSideCharacters(CCharacter *character)
{
  vector<CCharacter*>* retval = new vector<CCharacter*>();
  vector<CCharacter*>* minions = character->GetMinions();
  vector<CCharacter*>* minionList;

  retval->push_back(character);

  vector<CCharacter*>::iterator itr, itrMinion;
  for (itr = minions->begin(); itr != minions->end(); ++itr) {
    minionList = Helper_ProduceClientSideCharacters((*itr));
    for (itrMinion = minionList->begin(); itrMinion != minionList->end(); ++itrMinion) {
      retval->push_back((*itrMinion));
    }
    delete minionList;
  }

  return retval;
}

void Client::HandleCharacterSetAction(NetworkMessages::CharacterAction* msgCharacterAction)
{
  u32 id = msgCharacterAction->characterid();
  u32 action = msgCharacterAction->action();

  NetworkEntity *networkCharacter = searchCharacterByCommonID(id);

  if (networkCharacter) {
    CCharacter* character = (CCharacter*)networkCharacter->getInternalObject();

    // Stop the suppress and network request
    SetSuppressed_CharacterAction(false);
    character->SetAction(action);
    SetSuppressed_CharacterAction(true);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find character with common id = %x", id);
    //log(L"Error: Could not find character with common id = %x", id);
  }
}

void Client::HandleCharacterAttack(NetworkMessages::CharacterAttack* msgCharacterAttack)
{
  u32 id = msgCharacterAttack->characterid();

  NetworkEntity *networkCharacter = searchCharacterByCommonID(id);

  if (networkCharacter) {
    CCharacter* character = (CCharacter*)networkCharacter->getInternalObject();

    // Stop the suppress and network request
    Client::getSingleton().SetSuppressed_CharacterAttack(false);
    character->Attack();
    Client::getSingleton().SetSuppressed_CharacterAttack(true);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find character with common id = %x", id);
    //log(L"Error: Could not find character with common id = %x", id);
  }
}

void Client::HandleCharacterUseSkill(NetworkMessages::CharacterUseSkill* msgCharacterUseSkill)
{
  u32 id = msgCharacterUseSkill->characterid();
  u64 skill = msgCharacterUseSkill->skill();
  Vector3 direction;

  NetworkMessages::Position msgPosition = msgCharacterUseSkill->direction();
  direction.x = msgPosition.x();
  direction.y = msgPosition.y();
  direction.z = msgPosition.z();

  //log(L"Client HandleCharacterUseSkill: %f %f %f", 
  //  direction.x, direction.y, direction.z);

  NetworkEntity *networkCharacter = searchCharacterByCommonID(id);
  if (networkCharacter) {
    CCharacter* character = (CCharacter*)networkCharacter->getInternalObject();

    // Stop the suppress and network request
    Client::getSingleton().SetSuppressed_SendUseSkill(true);
    character->UseSkill(skill);
    Client::getSingleton().SetSuppressed_SendUseSkill(false);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find character with common id = %x", id);
    //log(L"Error: Could not find character with common id = %x", id);
  }
}

void Client::HandleEquipmentUpdateStack(NetworkMessages::EquipmentUpdateStackSize *msgEquipmentUpdateStackSize)
{
  u32 id = msgEquipmentUpdateStackSize->id();
  u32 amount = msgEquipmentUpdateStackSize->change_amount();

  NetworkEntity *netEquipment = searchEquipmentByCommonID(id);

  if (netEquipment) {
    CEquipment *equipment = (CEquipment*)netEquipment->getInternalObject();

    // Suppress the client from re-sending it
    SetSuppressed_SendEquipmentStack(true);
    EquipmentAddStackCount(equipment, amount);
    SetSuppressed_SendEquipmentStack(false);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find equipment with common id = %x", id);
    //log(L"Error: Could not find equipment with common id = %x", id);
  }
}

void Client::HandleEquipmentAddGem(NetworkMessages::EquipmentAddGem* msgEquipmentAddGem)
{
  u32 equipmentId = msgEquipmentAddGem->equipmentid();
  u32 gemId = msgEquipmentAddGem->gemid();

  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);
  NetworkEntity *netGem = searchEquipmentByCommonID(gemId);

  if (netEquipment && netGem) {
    CEquipment *equipment = (CEquipment*)netEquipment->getInternalObject();
    CEquipment *gem = (CEquipment*)netGem->getInternalObject();

    Set_IsSendingEquipmentAddGem(true);
    equipment->AddGem(gem);
    Set_IsSendingEquipmentAddGem(false);
  } else {
    //log(L"Equipment or Gem commonId not found: %x %x  ptrs: %p %p",
    //  equipmentId, gemId, netEquipment, netGem);
    multiplayerLogger.WriteLine(Error, L"Equipment or Gem commonId not found: %x %x  ptrs: %p %p",
      equipmentId, gemId, netEquipment, netGem);
  }
}

void Client::HandleChatMessage(NetworkMessages::ChatPlayerText *msgChatPlayerText)
{
  //log(L"Client: Received player chat text.");

  CEGUI::MultiLineEditbox *pWindow = (CEGUI::MultiLineEditbox *)getChatHistoryWindow();
  size_t selectionEnd;

  if (pWindow) {
    CEGUI::String message = msgChatPlayerText->text().c_str();
    u32 characterId = msgChatPlayerText->characterid();

    NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

    if (netCharacter) {
      CCharacter *character = (CCharacter*)netCharacter->getInternalObject();
      string characterName(character->characterName.begin(), character->characterName.end());
      characterName.assign(character->characterName.begin(), character->characterName.end());

      //selectionStart = pWindow->getText().length();
      pWindow->appendText(CEGUI::String(characterName) + CEGUI::String(": ") + message + CEGUI::String("\n"));
      selectionEnd = pWindow->getText().length();
      pWindow->setCaratIndex(selectionEnd);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error could not find Network entity of id: %x", characterId);
    }
  }
}

//
// Populates the msgEquipment with the given Equipment vars
void Client::Helper_PopulateEquipmentMessage(NetworkMessages::Equipment* msgEquipment, CEquipment *equipment, NetworkEntity *netEquipment)
{
  multiplayerLogger.WriteLine(Info, L"  Setting up regular variables for Equipment: %s", equipment->nameReal.c_str());
  //log(L"  Setting up regular variables for Equipment: %s", equipment->nameReal.c_str());

  // Fill out the easy stuff
  msgEquipment->set_guid(equipment->GUID);
  msgEquipment->set_id(netEquipment->getCommonId());
  msgEquipment->set_stacksize(equipment->stackSize);
  msgEquipment->set_stacksizemax(equipment->stackSizeMax);
  msgEquipment->set_socketcount(equipment->socketCount);
  msgEquipment->set_client_id(netEquipment->getCommonId());     // Create a temporary client ID to identify this when we receive the common ID

  msgEquipment->set_identified(equipment->identified);

  msgEquipment->set_physical_damage_min(equipment->minimumPhysicalDamage);
  msgEquipment->set_physical_damage_max(equipment->maximumPhysicalDamage);

  // Check if we're a:
  //  Breakable    - 29
  //  Interactable - 32
  //  ItemGold     - 34
  //  Openable     - 40
  // If so skip this
  if (equipment->type__ != 0x1D &&
      equipment->type__ != 0x22 &&
      equipment->type__ != 0x20 &&
      equipment->type__ != 0x28)  
  {
    string nameUnidentified(equipment->nameUnidentified.begin(), equipment->nameUnidentified.end());
    nameUnidentified.assign(equipment->nameUnidentified.begin(), equipment->nameUnidentified.end());
    msgEquipment->set_name_unidentified(nameUnidentified);

    string namePrefix(equipment->namePrefix.begin(), equipment->namePrefix.end());
    namePrefix.assign(equipment->namePrefix.begin(), equipment->namePrefix.end());
    msgEquipment->set_name_prefix(namePrefix);
    
    string nameSuffix(equipment->nameSuffix.begin(), equipment->nameSuffix.end());
    nameSuffix.assign(equipment->nameSuffix.begin(), equipment->nameSuffix.end());
    msgEquipment->set_name_suffix(nameSuffix);

    msgEquipment->set_req_level(equipment->requirements[0]);
    msgEquipment->set_req_strength(equipment->requirements[1]);
    msgEquipment->set_req_dexterity(equipment->requirements[2]);
    msgEquipment->set_req_magic(equipment->requirements[3]);
    msgEquipment->set_req_defense(equipment->requirements[4]);

    multiplayerLogger.WriteLine(Info, L"  Setting up Regular Enchants... size = %x",
      equipment->enchantList.size());
    //log(L"  Setting up Regular Enchants... size = %x",
    //  equipment->enchantList.size());

    // New
    // Work on Regular Enchants
    for (size_t index = 0; index < equipment->enchantList.size(); index++) {
      //log(L"  Adding Equipment Enchant: REGULAR");
      //log(L"    (%i: %i)", equipment->enchantTypeList[index], equipment->enchantList[index]);
      multiplayerLogger.WriteLine(Info, L"  Adding Equipment Enchant: REGULAR");
      multiplayerLogger.WriteLine(Info, L"    (%i: %i)", equipment->enchantTypeList[index], equipment->enchantList[index]);

      NetworkMessages::EnchantType *msgEnchantType = msgEquipment->add_enchants();
      msgEnchantType->set_type(KEFFECT_TYPE_DAMAGE_BONUS);
      msgEnchantType->set_subtype(equipment->enchantTypeList[index]);
      msgEnchantType->set_value_min((float)equipment->enchantList[index]);
      msgEnchantType->set_value_max((float)equipment->enchantList[index]);
    }


    // Add the Effect Enchants
    CEffectManager *effectManager = equipment->pCEffectManager;
    if (effectManager) {
      multiplayerLogger.WriteLine(Info, L"  Setting up Effect Enchants...");
      //log(L"  Setting up Effect Enchants...");

      // Work on Effect Enchants
      for (u32 i = 0; i < effectManager->effectList.size; i++) {
        CEffect *effect = effectManager->effectList[i];

        multiplayerLogger.WriteLine(Info, L"  Adding Equipment Effect Enchant: (%x: %f / %f)", effect->effectType, effect->effectValueMin, effect->effectValueMax);
        //log(L"  Adding Equipment Effect Enchant: (%x: %f)", effect->effectType, effect->effectValue);

        NetworkMessages::EnchantType *msgEnchantType = msgEquipment->add_enchants();
        msgEnchantType->set_type(effect->effectType);
        msgEnchantType->set_subtype(-1);  // Not used in this instance
        msgEnchantType->set_value_min(effect->effectValueMin);
        msgEnchantType->set_value_max(effect->effectValueMax);
      }
    }
  }
}

void Client::HandleEquipmentRemoveGems(NetworkMessages::EquipmentRemoveGems *msgEquipmentRemoveGems)
{
  u32 equipmentId = msgEquipmentRemoveGems->equipmentid();

  NetworkEntity *netEntity = searchEquipmentByCommonID(equipmentId);

  if (netEntity) {
    CEquipment* equipment = (CEquipment*)netEntity->getInternalObject();

    //log(L"Client: Removing gems from equipment: %s", equipment->nameReal.c_str());
    multiplayerLogger.WriteLine(Info, L"Client: Removing gems from equipment: %s", equipment->nameReal.c_str());

    equipment->gemList.size = 0;
  } else {
    //log(L"Error: Could not find NetworkEntity for equipment of id: %x", equipmentId);
    multiplayerLogger.WriteLine(Error, L"Error: Could not find NetworkEntity for equipment of id: %x", equipmentId);
  }
}

void Client::HandleChangeLevel(NetworkMessages::ChangeLevel *msgChangeLevel)
{
  wstring dungeonName = convertAsciiToWide(msgChangeLevel->dungeon());
  wstring unkString = convertAsciiToWide(msgChangeLevel->unkstring());
  s32 level = msgChangeLevel->level();
  s32 unk0 = msgChangeLevel->unk0();
  s32 unk1 = msgChangeLevel->unk1();
  s32 unk2 = msgChangeLevel->unk2();

  log("Client::HandleChangeLevel: %s %s", msgChangeLevel->dungeon().c_str(), msgChangeLevel->unkstring().c_str());
  log(L"Client::HandleChangeLevel: %s %p %x %x %s %x", dungeonName.c_str(), level, unk0, unk1, unkString.c_str(), unk2);

  Client::getSingleton().SetAllow_ChangeLevel(true);
  gameClient->ChangeLevel(dungeonName, level, unk0, unk1, unkString, unk2);
  Client::getSingleton().SetAllow_ChangeLevel(false);
}

void Client::HandleUpdateHealth(NetworkMessages::CharacterUpdateHealth* msgCharacterUpdateHealth)
{
  u32 characterId = msgCharacterUpdateHealth->characterid();
  float amount = msgCharacterUpdateHealth->amount();
  u32 maxHealth = msgCharacterUpdateHealth->max_health();
  float currentHealth = msgCharacterUpdateHealth->current_health();

  NetworkEntity *entity = searchCharacterByCommonID(characterId);
  
  NetworkMessages::Position *msgCurrent = msgCharacterUpdateHealth->mutable_position();
  Vector3 currentPosition;
  currentPosition.x = msgCurrent->x();
  currentPosition.y = msgCurrent->y();
  currentPosition.z = msgCurrent->z();

  if (entity) {
    CCharacter* character = (CCharacter*)entity->getInternalObject();

    if (character) {
      HelperCharacterPositioning(character, currentPosition);

      character->healthMax = maxHealth;
      character->healthCurrent = currentHealth;

      SetAllow_HealthUpdate(true);
      character->UpdateHealth(amount);
      SetAllow_HealthUpdate(false);
    }
  } else {
    //log(L"Error: Could not find entity with network ID %x to Update Health", characterId);
  }
}

void Client::HandleCharacterDestroy(NetworkMessages::CharacterDestroy *msgCharacterDestroy)
{
  u32 characterId = msgCharacterDestroy->characterid();

  NetworkEntity *entity = searchCharacterByCommonID(characterId);
  if (entity) {
    CCharacter* character = (CCharacter*)entity->getInternalObject();
    if (character) {
      // Suppress any server messages about deleting our own character or pet
      if (character != gameClient->pCPlayer && character != gameClient->pPet) {
        logColor(B_RED, L"Destroying character (id: %x, addr: %p)", characterId, character);

        character->destroy = true;
      }
    }
  } else {
    //log(L"Error: Could not find Network ID character: %x", characterId);
  }
}

void Client::HandleLevelCreateItem(NetworkMessages::LevelCreateItem *msgLevelCreateItem)
{
  u32 itemId = msgLevelCreateItem->itemid();
  u64 guid = msgLevelCreateItem->guid();
  u32 level = msgLevelCreateItem->level();
  u32 unk0 = msgLevelCreateItem->unk0();
  u32 unk1 = msgLevelCreateItem->unk1();

  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  if (resourceManager) {
    CItem *item = resourceManager->CreateItem(guid, level, unk0, unk1);

    // Attempt to clean up extra chests
    if (item->type__ == OPENABLE) {
      item->destroy = true;
      //item->position.x = 1000;
      //item->position.z = 1000;
      item->SetPosition(&Vector3(10000, 0, 10000));
    } else {
      addItem(item, itemId);
    }
  }
}

void Client::HandleLevelDropItem(NetworkMessages::LevelDropItem *msgLevelDropItem)
{
  NetworkMessages::Position msgPosition = msgLevelDropItem->position();
  u32 itemId = msgLevelDropItem->itemid();
  u32 unk0 = msgLevelDropItem->unk0();
  unk0 = 1;

  Vector3 position;
  position.x = msgPosition.x();
  position.y = msgPosition.y();
  position.z = msgPosition.z();

  NetworkEntity *entity = searchItemByCommonID(itemId);
  if (entity) {
    CItem *item = (CItem*)entity->getInternalObject();

    CLevel *level = gameClient->pCLevel;
    if (level) {
      SetAllow_LevelItemDrop(true);
      level->ItemDrop(item, position, unk0);
      SetAllow_LevelItemDrop(false);
    }
  } else {
    //log(L"Error: Could not find Item of Common ID: %x", itemId);
  }
}

void Client::HandleBreakableTriggered(NetworkMessages::BreakableTriggered* msgBreakableTriggered)
{
  u32 itemId = msgBreakableTriggered->itemid();
  u32 characterId = msgBreakableTriggered->characterid();

  NetworkMessages::Position msgPosition = msgBreakableTriggered->position();
  Vector3 position;
  position.x = msgPosition.x();
  position.y = msgPosition.y();
  position.z = msgPosition.z();

  NetworkEntity *entity = searchItemByCommonID(itemId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  log("  Handling breakable 1: %p %p (%x %x)", entity, netCharacter, itemId, characterId);

  if (entity) {
    /* Use strict positioning technique instead
    CBreakable *breakable = (CBreakable*)entity->getInternalObject();
    CPlayer *character = NULL;

    // Character ID can be -1 if there was a skill used to kill it
    if (characterId != -1 && netCharacter) {
      character = (CPlayer*)netCharacter->getInternalObject();
    }

    if (breakable) {
      log("  Handling breakable 2: %p %p", breakable, character);
      SetSuppressed_SendBreakableTriggered(true);
      breakable->Break(character);
      SetSuppressed_SendBreakableTriggered(false);
    }
    */

    // Assume the server's trigger units are sync'd with ours
    CLevel *level = gameClient->pCLevel;
    level->DumpTriggerUnits();

    // List our trigger Units
    LinkedListNode* itr = *level->itemsProximity;
    while (itr != NULL) {
      if (itr->pCBaseUnit->type__ == BREAKABLE) {
        CBreakable* breakable = (CBreakable*)itr->pCBaseUnit;
        CPlayer *character = NULL;
        
        if (netCharacter) {
          character = (CPlayer*)netCharacter->getInternalObject();
        }

        if (breakable) {
          if (breakable->GetPosition().squaredDistance(position) < EPSILON) {
            SetSuppressed_SendBreakableTriggered(true);
            breakable->Break(character);
            SetSuppressed_SendBreakableTriggered(false);
            break;
          }
        }
      }

      itr = itr->pNext;
    }
  } else {
    //log(L"Client: Error could not find entity with common ID = %x OR character with common ID = %x",
    //  itemId, characterId);
  }

  log(L"Done handling Breakable Triggered");
}

void Client::HandleTriggerUnitTriggered(NetworkMessages::TriggerUnitTriggered *msgTriggerUnitTrigger)
{
  u32 itemId = msgTriggerUnitTrigger->itemid();
  u32 characterId = msgTriggerUnitTrigger->characterid();

  NetworkMessages::Position msgPosition = msgTriggerUnitTrigger->position();
  Vector3 position;
  position.x = msgPosition.x();
  position.y = msgPosition.y();
  position.z = msgPosition.z();

  NetworkEntity *entity = searchItemByCommonID(itemId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  // Assume the server's trigger units are sync'd with ours
  CLevel *level = gameClient->pCLevel;

  log(L" Position: %f %f %f", position.x, position.y, position.z);
  level->DumpTriggerUnits();

  // Check items in proximity first
  LinkedListNode* itr = *level->itemsProximity;
  while (itr != NULL) {
    CTriggerUnit* triggerUnit = (CTriggerUnit*)itr->pCBaseUnit;
    CPlayer *character = NULL;
    
    if (triggerUnit->type__ == OPENABLE || triggerUnit->type__ == INTERACTABLE) {
      if (netCharacter) {
        character = (CPlayer*)netCharacter->getInternalObject();
      }

      if (triggerUnit->GetPosition().squaredDistance(position) < EPSILON) {
        log("  Found in trigger units: %p", triggerUnit);

        SetSuppressed_SendTriggerUnitTriggered(true);
        triggerUnit->Trigger(character);
        SetSuppressed_SendTriggerUnitTriggered(false);
        return;
      }
    }

    itr = itr->pNext;
  }

  // Check all items
  itr = *level->itemsAll;
  while (itr != NULL) {
    CTriggerUnit* triggerUnit = (CTriggerUnit*)itr->pCBaseUnit;
    CPlayer *character = NULL;
    
    if (triggerUnit->type__ == OPENABLE || triggerUnit->type__ == INTERACTABLE) {
      if (netCharacter) {
        character = (CPlayer*)netCharacter->getInternalObject();
      }

      if (triggerUnit->GetPosition().squaredDistance(position) < EPSILON) {
        log("  Found in items: %p", triggerUnit);

        SetSuppressed_SendTriggerUnitTriggered(true);
        triggerUnit->Trigger(character);
        SetSuppressed_SendTriggerUnitTriggered(false);
        return;
      }
    }

    itr = itr->pNext;
  }
}

void Client::HandleItemGoldAmount(NetworkMessages::ItemGoldCreate *msgItemGoldCreate)
{
  u32 itemId = msgItemGoldCreate->itemid();
  u32 amount = msgItemGoldCreate->amount();

  NetworkEntity *entity = searchItemByCommonID(itemId);
  if (entity) {
    //log(L"CommonID Already exists for Gold Item... replacing old with new");
  }

  // Create the gold
  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  //log(L"Adding Gold Item: CommonID = %x, Amount = %i", itemId, amount);
  //log(L"  ResourceManager = %p", resourceManager);

  CItemGold *gold = new CItemGold();
  gold->ItemCtor(resourceManager, amount);

  // Should hopefully be a ducttape fix until I can figure out the real cause
  //  For bug when an CItemGold is created the Client already has the same ID of it listed
  if (entity) {
    entity->SetNewInternalObject(gold);
  } else {
    addItem(gold, itemId);
  }
}

void Client::HandleCharacterAlignment(NetworkMessages::CharacterAlignment *msgCharacterAlignment)
{
  u32 characterId = msgCharacterAlignment->characterid();
  u32 alignment = msgCharacterAlignment->alignment();

  //log(L"Client setting character alignment... (Common ID: %x, Alignment = %x)", characterId, alignment);

  NetworkEntity *entity = searchCharacterByCommonID(characterId);

  if (entity) {
    CCharacter *character = (CCharacter*)entity->getInternalObject();
    if (character) {
      character->SetAlignment(alignment);
    }
  } else {
    //log(L"Error: Could not find entity with common ID = %x", characterId);
  }
}

void Client::HandleCharacterSetTarget(NetworkMessages::CharacterSetTarget *msgCharacterSetTarget)
{
  u32 characterId = msgCharacterSetTarget->characterid();
  u32 targetId = msgCharacterSetTarget->targetid();

  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);
  CCharacter *target = NULL;

  if (targetId != -1) {
    NetworkEntity *netTarget = searchCharacterByCommonID(targetId);
    if (netTarget) {
      target = (CCharacter*)netTarget->getInternalObject();
    } else {
      //log(L"Error: Could not find Target character of ID: %x", targetId);
      return;
    }
  }

  if (netCharacter) {
    CCharacter *character = (CCharacter*)netCharacter->getInternalObject();

    if (character) {
      SetAllow_CharacterSetTarget(true);
      character->SetTarget(target);
      SetAllow_CharacterSetTarget(false);
    }
  } else {
    //log(L"Error: Could not find Character of ID: %x", characterId);
  }
}

void Client::HandleEquipmentIdentify(NetworkMessages::EquipmentIdentify *msgEquipmentIdentify)
{
  /*
  u32 equipmentId = msgEquipmentIdentify->equipmentid();

  NetworkEntity *entity = searchEquipmentByCommonID(equipmentId);

  if (entity) {
    CEquipment* equipment = (CEquipment*)entity->getInternalObject();

    //log(L"  Equipment Identify = %p", equipment);
    //log(L"  Equipment Identify = %s", equipment->nameReal.c_str());

    if (equipment) {
      equipment->identified = 1;
      equipment->UpdateTooltip();
    }
  } else {
    //log(L"Error: Could not find equipment of ID: %x", equipmentId);
  }
  */

  u32 equipmentId = msgEquipmentIdentify->equipmentid();
  u32 characterId = msgEquipmentIdentify->characterid();
  bool characterType = msgEquipmentIdentify->target_player_type();
  u32 targetId = msgEquipmentIdentify->targetid();

  NetworkEntity *entity = searchEquipmentByCommonID(equipmentId);
  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);
  NetworkEntity *netTarget = NULL;

  if (characterType) {
    netTarget = searchCharacterByCommonID(targetId);
  } else {
    netTarget = searchEquipmentByCommonID(targetId);
  }
  if (!netTarget) {
    log(L"Error: Could not find Target from ID: %x", targetId);
    return;
  }

  //
  if (entity) {
    if (netCharacter) {
      CEquipment* equipment = (CEquipment*)entity->getInternalObject();
      CCharacter* character = (CCharacter*)netCharacter->getInternalObject();
      CBaseUnit* target = (CBaseUnit*)netTarget->getInternalObject();

      if (equipment) {
        //equipment->identified = 1;
        //equipment->UpdateTooltip();

        Client::getSingleton().SetAllow_EquipmentIdentify(true);
        equipment->Use(character, target);
        Client::getSingleton().SetAllow_EquipmentIdentify(false);
      }
    } else {
      log(L"Error: Could not find character of ID: %x", equipmentId);
    }
  } else {
    log(L"Error: Could not find equipment of ID: %x", equipmentId);
  }
}

void Client::HandleCurrentLevel(NetworkMessages::CurrentLevel *msgCurrentLevel)
{
  wstring dungeonSection = convertAsciiToWide(msgCurrentLevel->dungeonsection());
  s32 relativeLevel = msgCurrentLevel->relativelevel();
  s32 absoluteLevel = msgCurrentLevel->absolutelevel();

  SetAllow_ChangeLevel(true);
  //gameClient->ChangeLevel(dungeonSection, relativeLevel, absoluteLevel, 0, L"", 0);
  // TODO Show Client Error
  SetAllow_ChangeLevel(false);
}

void Client::HandleTriggerUnitSync(NetworkMessages::TriggerUnitSync *msgTriggerUnitSync)
{
  //log(L"~~~~ Client Handling Trigger Unit Sync.");

  // Assume the server's trigger units are sync'd with ours
  CLevel *level = gameClient->pCLevel;
  log(L"Level: %p", level);
  level->DumpTriggerUnits();

  u32 triggerSize = (u32)msgTriggerUnitSync->triggerunits().size();
  log(L"  TriggerUnitList Size: %i", triggerSize);
  for (u32 i = 0; i < triggerSize; i++) {
    NetworkMessages::TriggerUnit msgTriggerUnit = msgTriggerUnitSync->triggerunits().Get(i);
    NetworkMessages::Position *msgPosition = msgTriggerUnit.mutable_triggerposition();

    Vector3 serverTriggerPosition;
    serverTriggerPosition.x = msgPosition->x();
    serverTriggerPosition.y = msgPosition->y();
    serverTriggerPosition.z = msgPosition->z();

    u32 triggerId = msgTriggerUnit.triggerid();
    log(L"  TriggerID: %x", triggerId);
    log(L"  TriggerPosition: %f %f %f",
      serverTriggerPosition.x,
      serverTriggerPosition.y,
      serverTriggerPosition.z);
  }
  log(L"Done dumping");
  
  // List our trigger Units
  LinkedListNode* itr = *level->itemsProximity;
  while (itr != NULL) {
    CTriggerUnit* triggerUnit = (CTriggerUnit*)itr->pCBaseUnit;

    logColor(B_GREEN, L"  TriggerUnit: %p %s (%f, %f, %f)",
              triggerUnit,
              triggerUnit->nameReal.c_str(),
              triggerUnit->GetPosition().x,
              triggerUnit->GetPosition().y,
              triggerUnit->GetPosition().z);

    itr = itr->pNext;
  }


  for (u32 i = 0; i < triggerSize; i++) {
    NetworkMessages::TriggerUnit msgTriggerUnit = msgTriggerUnitSync->triggerunits().Get(i);
    NetworkMessages::Position *msgPosition = msgTriggerUnit.mutable_triggerposition();

    Vector3 serverTriggerPosition;
    serverTriggerPosition.x = msgPosition->x();
    serverTriggerPosition.y = msgPosition->y();
    serverTriggerPosition.z = msgPosition->z();

    u32 triggerId = msgTriggerUnit.triggerid();
    //log(L"  TriggerID: %x", triggerId);
    //log(L"  TriggerPosition: %f %f %f",
    //  serverTriggerPosition.x,
    //  serverTriggerPosition.y,
    //  serverTriggerPosition.z);

    bool found = false;
    LinkedListNode* itr = *level->itemsProximity;
    while (itr != NULL) {
      CTriggerUnit* triggerUnit = (CTriggerUnit*)itr->pCBaseUnit;  

      if (triggerUnit) {
        bool found = false;
        Ogre::Real dist = triggerUnit->GetPosition().squaredDistance(serverTriggerPosition);

        if (dist < 0.1f) {
          found = true;
        } else {
          // Check the world scenenode (foreman table, stairs use this positioning)
          if (triggerUnit->pOctreeNode_World) {
            dist = triggerUnit->pOctreeNode_World->getPosition().squaredDistance(serverTriggerPosition);
          }
          if (dist < 0.1f) {
            log("Found node through World OctreeNode");
            found = true;
          }
        }

        if (found) {
          logColor(B_GREEN, L"Syncing triggerUnit of ID %x to: %s", triggerId, triggerUnit->nameReal.c_str());
          log(L"  Distance: %f", dist);
          logColor(B_GREEN, L"  My TriggerUnit: %p %s (%f, %f, %f)  with Server: (%f, %f, %f)",
              triggerUnit,
              triggerUnit->nameReal.c_str(),
              triggerUnit->GetPosition().x,
              triggerUnit->GetPosition().y,
              triggerUnit->GetPosition().z,
              serverTriggerPosition.x,
              serverTriggerPosition.y,
              serverTriggerPosition.z);

          addItem(triggerUnit, triggerId);

          logColor(B_GREEN, L"Done syncing triggerUnit %p to %i", triggerUnit, triggerId);
          found = true;

          break;
        }
      }
      itr = itr->pNext;
    }

    // Report unknown trigger unit
    if (!found) {
      log(L"Error: Could not sync trigger unit ID: %x (%f, %f, %f)",
        triggerId, serverTriggerPosition.x, serverTriggerPosition.y, serverTriggerPosition.z);
    }
  }
}

void Client::HandleCharacterResurrect(NetworkMessages::CharacterResurrect *msgCharacterResurrect)
{
  //log(L"Client received character resurrection.");

  NetworkEntity *entity = searchCharacterByCommonID(msgCharacterResurrect->characterid());

  if (entity) {
    CCharacter *character = (CCharacter*)entity->getInternalObject();

    SetAllow_CharacterResurrect(true);
    character->Resurrect();
    SetAllow_CharacterResurrect(false);
  }
}

void Client::HandleCharacterAddExperience(NetworkMessages::CharacterAddExperience *msgCharacterAddExperience)
{
  log(L"Client received character add experience.");

  // Ignore the character sent, just update our experience and our pet's
  //NetworkEntity *entity = searchCharacterByCommonID(msgCharacterAddExperience->characterid());
  u32 experience = msgCharacterAddExperience->experience();
  u32 unk0 = msgCharacterAddExperience->unk0();

  //if (entity) {
  //CCharacter *character = (CCharacter*)entity->getInternalObject();

  SetAllow_AddExperience(true);
  
  //character->KillMonsterExperience(gameClient->pCLevel, NULL, experience, unk0);
  
  gameClient->pCPlayer->KillMonsterExperience(gameClient->pCLevel, NULL, experience, unk0);
  /*
  // Update minion experience
  vector<CCharacter*> *minions = gameClient->pCPlayer->GetMinions();
  vector<CCharacter*>::iterator itr;
  for (itr = minions->begin(); itr != minions->end(); itr++) {
    (*itr)->KillMonsterExperience(gameClient->pCLevel, NULL, experience, unk0);
  }
  */
  
  SetAllow_AddExperience(false);
}

void Client::HandleCharacterKillCharacter(NetworkMessages::CharacterKilledCharacter *msgCharacterKilledCharacter)
{
  u32 unk0 = msgCharacterKilledCharacter->unk0();
  u32 characterId = msgCharacterKilledCharacter->characterid();
  u32 otherId = msgCharacterKilledCharacter->otherid();

  NetworkMessages::Position msgPosition = msgCharacterKilledCharacter->position();

  NetworkEntity *entityCharacter = searchCharacterByCommonID(characterId);
  NetworkEntity *entityOther = searchCharacterByCommonID(otherId);

  if (!entityCharacter) {
    log(L"Could not find character with common id: %x", characterId);
    return;
  }

  if (!entityOther) {
    log(L"Could not find other character with common id: %x", otherId);
    return;
  }

  CCharacter *character = (CCharacter*)entityCharacter->getInternalObject();
  CCharacter *other = (CCharacter*)entityOther->getInternalObject();
  CLevel *level = gameClient->pCLevel;
  Vector3 position;
  position.x = msgPosition.x();
  position.y = msgPosition.y();
  position.z = msgPosition.z();

  if (level) {
    level->CharacterKill(character, other, &position, unk0);
  }

}

void Client::HandleCharacterAddSkill(NetworkMessages::BaseUnitAddSkill *msgCharacterAddSkill)
{
  u32 characterId = msgCharacterAddSkill->characterid();
  string skillName = msgCharacterAddSkill->skillname();

  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);

  if (netCharacter) {
    CBaseUnit *character = (CBaseUnit*)netCharacter->getInternalObject();
    wstring skillNameWide = convertAsciiToWide(skillName);

    log(L"Client: Adding skillName: %s to character: %s", skillNameWide.c_str(), ((CCharacter*)character)->characterName.c_str());
    character->addSkill(&skillNameWide, 0);
  } else {
    log(L"Error: Could could not find character with network ID = %x", characterId);
  }
}

void Client::HandleRandomSeed(NetworkMessages::RandomSeed *msgRandomSeed)
{
  m_Seed = msgRandomSeed->seed();

  log(L"Client received seed from server: %x", m_Seed);

  Seed1 = (u32*)EXEOFFSET(SeedOffset1);
  Seed2 = (u32*)EXEOFFSET(SeedOffset2);
  Seed3 = (u32*)EXEOFFSET(SeedOffset3);
  Seed4 = (u32*)EXEOFFSET(SeedOffset4);

  /*
  *Seed1 = m_Seed;
  *Seed2 = m_Seed;
  *Seed3 = m_Seed;
  *Seed4 = m_Seed;
  */

  /*
  SetAllow_RandomSeed(true);
  _GLOBAL::SetSeed0(seed);
  SetAllow_RandomSeed(false);
  */

  log(L"Client done setting seed.");
}

void Client::HandleCharacterOrientation(NetworkMessages::CharacterOrientation *msgCharacterOrientation)
{
  u32 characterId = msgCharacterOrientation->characterid();
  NetworkEntity *entity = searchCharacterByCommonID(characterId);
  NetworkMessages::Position msgOrientation = msgCharacterOrientation->orientation();

  if (!entity) {
    log("Error: Could not find character for network ID: %x", characterId);
    return;
  }

  CCharacter *character = (CCharacter*)entity->getInternalObject();
  SetAllow_UpdateOrientation(true);
  character->UpdateOrientation(msgOrientation.x(), msgOrientation.z());
  SetAllow_UpdateOrientation(false);
}

void Client::HandlePlayerWeaponSwap(NetworkMessages::PlayerSwapWeapons *msgPlayerSwapWeapons)
{
  u32 characterId = msgPlayerSwapWeapons->characterid();
  NetworkEntity *entity = searchCharacterByCommonID(characterId);

  if (!entity) {
    log("Error: Could not find character for network ID: %x", characterId);
    return;
  }

  CCharacter *character = (CCharacter*)entity->getInternalObject();
  SetAllow_WeaponSwap(true);
  character->WeaponSwap();
  SetAllow_WeaponSwap(false);
}

void Client::HandleCharacterSetSkillPoints(NetworkMessages::CharacterSetSkillPoints *msgCharacterSetSkillPoints)
{
  u32 characterId = msgCharacterSetSkillPoints->characterid();
  u64 skillGUID = msgCharacterSetSkillPoints->skillguid();
  u32 skillLevel = msgCharacterSetSkillPoints->skilllevel();

  NetworkEntity *entity = searchCharacterByCommonID(characterId);
  if (!entity) {
    log("Error: Could not find character for network ID: %x", characterId);
    return;
  }

  CCharacter *character = (CCharacter*)entity->getInternalObject();
  CSkillManager *skillManager = character->pCSkillManager;
  if (!skillManager) {
    log("Error: SkillManager for character is null");
    return;
  }

  CSkill *skill = skillManager->getSkillFromGUID(skillGUID);
  if (!skill) {
    log("Error: Skill with GUID: %016I64X could not be found", skillGUID);
    return;
  }

  SetAllow_SetSkillPoints(true);
  character->pCSkillManager->setSkillLevel(skill, skillLevel);
  SetAllow_SetSkillPoints(false);
}

void Client::HandleCharacterVisibility(NetworkMessages::Visibility *msgVisibility)
{
  u32 id = msgVisibility->id();
  
  NetworkEntity *netCharacter = searchCharacterByCommonID(id);

  if (netCharacter) {
    CCharacter* character = (CCharacter*)netCharacter->getInternalObject();

    if (character) {
      // Don't change visibility of our character
      vector<CCharacter*>* characters = Helper_ProduceClientSideCharacters(gameClient->pCPlayer);
      vector<CCharacter*>::iterator itr;
      for (itr = characters->begin(); itr != characters->end(); ++itr) {
        if ((*itr) == character) {
          return;
        }
      }

      character->flagFadingIn = msgVisibility->fading_in();
      character->flagHasBeenDisplayed = msgVisibility->has_been_displayed();
      character->flagFadingOut = msgVisibility->fading_out();
      character->unkTransparency1 = msgVisibility->transparency();
    }
  }
}

void Client::HandleEquipmentAutoEquip(NetworkMessages::EquipmentAutoEquip *msgEquipmentAutoEquip)
{
  u32 characterId = msgEquipmentAutoEquip->characterid();
  u32 equipmentId = msgEquipmentAutoEquip->equipmentid();

  NetworkEntity *netCharacter = searchCharacterByCommonID(characterId);
  NetworkEntity *netEquipment = searchEquipmentByCommonID(equipmentId);

  CCharacter *character = NULL;
  CInventory *inventory = NULL;
  CEquipment *equipment = NULL;

  if (netCharacter) {
    character = (CCharacter*)netCharacter->getInternalObject();
    if (character) {
      inventory = character->pCInventory;
    }
  }
  if (netEquipment) {
    equipment = (CEquipment*)netEquipment->getInternalObject();
  }

  //
  if (equipment && inventory) {
    SetAllow_AutoEquip(true);
    inventory->AutoEquipEquipment(equipment);
    SetAllow_AutoEquip(false);
  } else {
    log(L"Error: Could not auto equip: %p %p", equipment, inventory);
    multiplayerLogger.WriteLine(Error, L"Error: Could not auto equip: %p %p", equipment, inventory);
  }
}
        
