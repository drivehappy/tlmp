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

  m_bWaitingForGame = false;
  m_bGameStarted = false;
  m_bServerGameStarted = false;
  m_bSuppressNetwork_SetDestination = false;
  m_bSuppressNetwork_CharacterCreation = true;
  m_bSuppressNetwork_EquipmentCreation = true;

  m_pOnConnected = NULL;
  m_pOnDisconnected = NULL;
  m_pOnConnectFailed = NULL;
}

Client::~Client()
{
  delete m_pBitStream;
}

void Client::Connect(const char* address, u16 port)
{
  log("Client connecting to %s (%i)", address, port);

  if (m_pClient) {
    m_pClient->Shutdown(0);
  }

  m_pClient = RakNetworkFactory::GetRakPeerInterface();

  SocketDescriptor socketDescriptor(0, 0);
  m_pClient->Startup(1, 30, &socketDescriptor, 1);
  m_pClient->Connect(address, port, 0, 0);
}

void Client::Disconnect()
{
  log("Client disconnecting");

  if (m_pClient) {
    m_pClient->Shutdown(0);
    m_pClient = NULL;
  }
}

void Client::SetCallback_OnConnected(OnConnected callback)
{
  m_pOnConnected = callback;
}

void Client::SetCallback_OnDisconnected(OnDisconnected callback)
{
  m_pOnDisconnected = callback;
}

void Client::SetCallback_OnConnectFailed(OnConnectFailed callback)
{
  m_pOnConnectFailed = callback;
}

void Client::ReceiveMessages()
{
  if (!m_pClient)
    return;

  Packet *packet = m_pClient->Receive();
  while (packet)
  {
    switch(packet->data[0])
    {
    case ID_CONNECTION_REQUEST_ACCEPTED:
      if (m_pOnConnected) {
        m_pOnConnected(NULL);
      }
      OnConnect(NULL);
      break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
      if (m_pOnConnectFailed) {
        m_pOnConnectFailed(NULL);
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      if (m_pOnDisconnected) {
        m_pOnDisconnected(NULL);
      }
      break;
    case ID_CONNECTION_LOST:
      if (m_pOnDisconnected) {
        m_pOnDisconnected(NULL);
      }
      break;
    case ID_CONNECTION_ATTEMPT_FAILED:
      if (m_pOnConnectFailed) {
        m_pOnConnectFailed(NULL);
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

  log("[CLIENT] Connected.");
}

void Client::WorkMessage(Message msg, RakNet::BitStream *bitStream)
{
  wstring msgString = convertAcsiiToWide(MessageString[msg]);

  multiplayerLogger.WriteLine(Info, L"Client Received Message: %s", msgString.c_str());
  log(L"Client Received Message: %s", msgString.c_str());

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
    }
    break;

  case S_REQUEST_CHARINFO:
    {
      NetworkMessages::RequestCharacterInfo *msgRequestCharacterInfo = ParseMessage<NetworkMessages::RequestCharacterInfo>(m_pBitStream);

      HandleRequestCharacterInfo();
    }
    break;

  case S_REPLY_CHARID:
    {
      NetworkMessages::ReplyCharacterId *msgReplyCharacterId = ParseMessage<NetworkMessages::ReplyCharacterId>(m_pBitStream);
      u32 id = msgReplyCharacterId->id();

      HandleReplyCharacterId(id);
    }
    break;

  case S_PUSH_NEWCHAR:
    {
      // Ignore server character creation if we're not in the game
      if (!gameClient->inGame) {
        break;
      }

      NetworkMessages::Character *msgCharacter = ParseMessage<NetworkMessages::Character>(m_pBitStream);

      NetworkMessages::Position msgCharacterPosition = msgCharacter->position(0);

      Vector3 posCharacter;
      posCharacter.x = msgCharacterPosition.x();
      posCharacter.y = msgCharacterPosition.y();
      posCharacter.z = msgCharacterPosition.z();

      u32 commonId = msgCharacter->id();

      HandleCharacterCreation(posCharacter, msgCharacter->guid(), msgCharacter->name(), commonId);
    }
    break;

  case S_PUSH_NEWEQUIPMENT:
    {
      // Ignore server character creation if we're not in the game
      if (!gameClient->inGame) {
        break;
      }

      NetworkMessages::Equipment *msgEquipment = ParseMessage<NetworkMessages::Equipment>(m_pBitStream);
      u32 id = msgEquipment->id();
      u64 guid = msgEquipment->guid();

      HandleEquipmentCreation(id, guid);
    }
    break;

  case S_PUSH_ADDCHARMINION:
    break;

  case S_PUSH_SETCHARALIGNMENT:
    break;

  case S_PUSH_EQUIPMENT_DROP:
    break;

  case S_PUSH_EQUIPMENT_PICKUP:
    break;

  case S_PUSH_EQUIPMENT_EQUIP:
    {
      NetworkMessages::InventoryAddEquipment *msgInventoryAddEquipment = ParseMessage<NetworkMessages::InventoryAddEquipment>(m_pBitStream);
      u32 ownerId = msgInventoryAddEquipment->ownerid();
      u32 equipmentId = msgInventoryAddEquipment->equipmentid();
      u32 slot = msgInventoryAddEquipment->slot();

      HandleInventoryAddEquipment(ownerId, equipmentId, slot);
    }
    break;

  case S_PUSH_EQUIPMENT_UNEQUIP:
    break;

  case S_PUSH_EQUIPMENT_USE:
    break;

  case S_PUSH_EQUIPMENT_IDENTIFY:
    break;

  case S_PUSH_EQUIPMENT_ADDENCHANT:
    break;

  case S_PUSH_EQUIPMENT_ADDSOCKET:
    break;

  case S_PUSH_CHARACTER_SETDEST:
    {
      NetworkMessages::CharacterDestination *msgCharacterDestination = ParseMessage<NetworkMessages::CharacterDestination>(m_pBitStream);
      NetworkMessages::Position msgDestination = msgCharacterDestination->destination().Get(0);

      u32 id = msgCharacterDestination->id();

      Vector3 destination;
      destination.x = msgDestination.x();
      destination.z = msgDestination.z();

      HandleCharacterDestination(id, destination);
    }
    break;
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
  Client::getSingleton().SetServerGameStarted(false);
  
  // TODO Exit Player to MainMenu
}

// Receives a request from the Server for Character Info
void Client::HandleRequestCharacterInfo()
{
  NetworkMessages::ReplyCharacterInfo msgReplyCharacterInfo;

  NetworkMessages::Character *msgPlayer         = msgReplyCharacterInfo.add_player();
  NetworkMessages::Character *msgPet            = msgPlayer->add_minion();
  NetworkMessages::Position  *msgPlayerPosition = msgPlayer->add_position();

  // Add the player information
  CPlayer *player = gameClient->pCPlayer;
  string playerName(player->characterName.begin(), player->characterName.end());
  playerName.assign(player->characterName.begin(), player->characterName.end());

  msgPet->set_guid(player->GUID);
  msgPet->set_name(playerName);

  msgPlayer->set_guid(player->GUID);
  msgPlayer->set_name(playerName);
  
  msgPlayerPosition->set_x(player->position.x);
  msgPlayerPosition->set_y(player->position.y);
  msgPlayerPosition->set_z(player->position.z);

  Client::getSingleton().SendMessage<NetworkMessages::ReplyCharacterInfo>(C_REPLY_CHARINFO, &msgReplyCharacterInfo);
}

// Receives a Character Network ID from the server to apply to our character
// This is really a special message since the Client creates the player without the server
// telling it to
void Client::HandleReplyCharacterId(u32 id)
{
  CPlayer *player = gameClient->pCPlayer;

  NetworkEntity *newEntity = new NetworkEntity(player, id);
  NetworkSharedCharacters->push_back(newEntity);
}

// Pushes player inventory equipment list to the server to create
void Client::PushEquipment()
{
}

// Handles Character Set Destination
void Client::HandleCharacterDestination(u32 commonId, Vector3 destination)
{
  multiplayerLogger.WriteLine(Info, L"Client received character setDestination (CommonID = %x), Position = %f, %f",
    commonId, destination.x, destination.z);

  // Search for our Character given the CommonID
  NetworkEntity *entity = searchCharacterByCommonID(commonId);

  if (!entity) {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find internal object in shared characters from commonId: %x when receiving character destination.", commonId);
    log(L"Error: Could not find internal object in shared characters from commonId: %x when receiving character destination.", commonId);
    return;
  }

  CCharacter *character = (CCharacter *)entity->getInternalObject();

  if (character) {
    // Lock the Client from sending a network message back out when setting the Character Destination
    Client::getSingleton().SetSuppressed_SetDestination(true);
    character->SetDestination(gameClient->pCLevel, destination.x, destination.z);
    Client::getSingleton().SetSuppressed_SetDestination(false);
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Character is NULL");
    log(L"Error: Character is NULL");
  }
}

// Handles Character Creation
void Client::HandleCharacterCreation(Vector3 posCharacter, u64 guidCharacter, string characterName, u32 commonId)
{
  multiplayerLogger.WriteLine(Info, L"Client received character creation: (CommonID = %x) (GUID = %016I64X, name = %s)",
    commonId, guidCharacter, TLMP::convertAcsiiToWide(characterName).c_str());

  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  CLevel *level = gameClient->pCLevel;

  Client::getSingleton().SetSuppressed_CharacterCreation(false);
  CMonster* monster = resourceManager->CreateMonster(guidCharacter, 1, true);
  level->CharacterInitialize(monster, &posCharacter, 0);
  Client::getSingleton().SetSuppressed_CharacterCreation(true);

  // Create a network ID to identify this monster later
  NetworkEntity *newEntity = addCharacter(monster, commonId);
}

void Client::HandleEquipmentCreation(u32 id, u64 guid)
{
  multiplayerLogger.WriteLine(Info, L"Client received equipment creation: (CommonID = %x) (GUID = %016I64X)",
    id, guid);
  //log(L"Client received equipment creation: (CommonID = %x) (GUID = %016I64X)",
  //  id, guid);

  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;

  if (resourceManager) {
    //Client::getSingleton().SetSuppressed_EquipmentCreation(false);
    CEquipment *equipment = resourceManager->CreateEquipment(guid, 1, 1, 0);
    //Client::getSingleton().SetSuppressed_EquipmentCreation(true);

    log(L"Client: Adding equipment to shared network equipment...");
    NetworkEntity *newEntity = addEquipment(equipment, id);
  }
}

void Client::HandleInventoryAddEquipment(u32 ownerId, u32 equipmentId, u32 slot)
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
      Client::getSingleton().SetSuppressed_EquipmentCreation(false);
      inventory->AddEquipment(equipmentReal, slot);
      Client::getSingleton().SetSuppressed_EquipmentCreation(true);
    } else {
      multiplayerLogger.WriteLine(Error, L"Error: Could not find Equipment with ID = %x", equipmentId);
      log(L"Error: Could not find Equipment with ID = %x", equipmentId);
    }
  } else {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find Character with ID = %x", ownerId);
    log(L"Error: Could not find Character with ID = %x", ownerId);
  }
}
