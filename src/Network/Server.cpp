#include "Server.h"

using namespace TLMP::Network;

Server& Server::getSingleton()
{
  static Server m_pSingleton;
  return m_pSingleton;
}

Server::Server()
{
  m_pServer = NULL;
  m_pBitStream = new RakNet::BitStream(1024);

  m_bSuppressNetwork_SetDestination = false;

  m_pOnListening = NULL;
  m_pOnShutdown = NULL;
  m_pOnClientConnect = NULL;
  m_pOnClientDisconnect = NULL;

  m_bWaitingForGame = true;
}

Server::~Server()
{
  delete m_pBitStream;
}

void Server::Listen(u16 port, u16 maxconnections)
{
  multiplayerLogger.WriteLine(Info, L"Starting server...");

  if (m_pServer) {
    multiplayerLogger.WriteLine(Info, L"Shutting down server");
    m_pServer->Shutdown(0);
  }

  m_pServer = RakNetworkFactory::GetRakPeerInterface();
  multiplayerLogger.WriteLine(Info, L"Got server: %p", m_pServer);

  SocketDescriptor socketDescriptor(port, 0);
  m_pServer->Startup(maxconnections, 30, &socketDescriptor, 1);
  m_pServer->SetMaximumIncomingConnections(maxconnections);

  multiplayerLogger.WriteLine(Info, L"Server listening on port %i (max connections = %i)", port, maxconnections);
  if (m_pOnListening) {
    m_pOnListening(NULL);
  }
}

void Server::Shutdown()
{
  if (m_pServer) {
    m_pServer->Shutdown(0);
    m_pServer = NULL;

    multiplayerLogger.WriteLine(Info, L"Server shutdown");

    if (m_pOnShutdown)
      m_pOnShutdown(NULL);
  }
}

void Server::SetCallback_OnListening(OnListening callback)
{
  m_pOnListening = callback;
}

void Server::SetCallback_OnShutdown(OnShutdown callback)
{
  m_pOnShutdown = callback;
}

void Server::SetCallback_OnClientConnect(OnClientConnect callback)
{
  m_pOnClientConnect = callback;
}

void Server::SetCallback_OnClientDisconnect(OnClientDisconnect callback)
{
  m_pOnClientDisconnect = callback;
}

void Server::ReceiveMessages()
{
  if (!m_pServer)
    return;

  // Round up any packets we've received
  Packet *packet = m_pServer->Receive();
  while (packet) {
    switch(packet->data[0]) {
    case ID_NEW_INCOMING_CONNECTION:
      multiplayerLogger.WriteLine(Info, L"Client connected");
      if (m_pOnClientConnect) {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
        m_pOnClientConnect((void*)sysAddress);
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      multiplayerLogger.WriteLine(Info, L"Client disconnected");
      if (m_pOnClientDisconnect) {
        m_pOnClientDisconnect(NULL);
      }
      break;
    case ID_CONNECTION_LOST:
      multiplayerLogger.WriteLine(Info, L"Client disconnected");
      if (m_pOnClientDisconnect) {
        m_pOnClientDisconnect(NULL);
      }
      break;
    case ID_USER_PACKET_ENUM+1:
      u32 msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<u32>(msg);

      WorkMessage(packet->systemAddress, (Message)msg, m_pBitStream);

      break;
    }

    m_pServer->DeallocatePacket(packet);
    packet = m_pServer->Receive();
  }
}

void Server::WorkMessage(const SystemAddress address, Message msg, RakNet::BitStream *bitStream)
{
  if (msg >= sizeof(MessageString) / sizeof(char*)) {
    log("Error: Message out of range.");
    multiplayerLogger.WriteLine(Info, L"Error: Message out of range.");
  }

  wstring msgString = convertAcsiiToWide(MessageString[msg]);

  multiplayerLogger.WriteLine(Info, L"Server Received Message: %s", msgString.c_str());
  log(L"Server Received Message: %s", msgString.c_str());

  switch (msg) {
  case C_VERSION:
    {
      NetworkMessages::Version *msgVersion = ParseMessage<NetworkMessages::Version>(m_pBitStream);
      u32 version = (u32)(msgVersion->version());

      HandleVersion(address, version);
    }
    break;

  case C_REQUEST_HASGAMESTARTED:
    {
      NetworkMessages::GameHasStarted *msgGameHasStarted = ParseMessage<NetworkMessages::GameHasStarted>(m_pBitStream);

      HandleHasGameStarted(address);
    }
    break;

  case C_PUSH_GAMEENTER:
    {
      NetworkMessages::GameEnter *msgGameEnter = ParseMessage<NetworkMessages::GameEnter>(m_pBitStream);

      HandleGameEnter(address);
    }
    break;

  case C_PUSH_GAMEEXITED:
    {
      NetworkMessages::GameExited *msgGameExited = ParseMessage<NetworkMessages::GameExited>(m_pBitStream);

      HandleGameExited(address);
    }
    break;

  case C_REPLY_CHARINFO:
    {
      NetworkMessages::ReplyCharacterInfo *msgReplyCharacterInfo = ParseMessage<NetworkMessages::ReplyCharacterInfo>(m_pBitStream);
      NetworkMessages::Character msgPlayer;
      NetworkMessages::Character msgPet;
      NetworkMessages::Position  msgPlayerPosition;

      msgPlayer = msgReplyCharacterInfo->player(0);
      msgPlayerPosition = msgPlayer.position(0);
      msgPet = msgPlayer.minion(0);

      Vector3 posPlayer;
      posPlayer.x = msgPlayerPosition.x();
      posPlayer.y = msgPlayerPosition.y();
      posPlayer.z = msgPlayerPosition.z();

      HandleReplyCharacterInfo(address, posPlayer, msgPlayer.guid(), msgPlayer.name(), msgPet.guid(), msgPet.name());
    }
    break;

  case C_PUSH_EQUIPMENT:
    break;

  case C_PUSH_EQUIPMENT_DROP:
    break;

  case C_PUSH_EQUIPMENT_PICKUP:
    break;

  case C_PUSH_EQUIPMENT_EQUIP:
    break;

  case C_PUSH_EQUIPMENT_UNEQUIP:
    break;

  case C_PUSH_EQUIPMENT_USE:
    break;

  case C_PUSH_EQUIPMENT_IDENTIFY:
    break;

  case C_PUSH_EQUIPMENT_ADDENCHANT:
    break;

  case C_PUSH_EQUIPMENT_ADDSOCKET:
    break;

  case C_PUSH_CHARACTER_SETDEST:
    {
      NetworkMessages::CharacterDestination *msgCharacterDestination = ParseMessage<NetworkMessages::CharacterDestination>(m_pBitStream);
      NetworkMessages::Position msgDestination = msgCharacterDestination->destination().Get(0);

      u32 commonId = msgCharacterDestination->id();

      Vector3 destination;
      destination.x = msgDestination.x();
      destination.z = msgDestination.z();

      HandleCharacterDestination(address, commonId, destination);
    }
    break;

  }
}

//
// Below are message handlers

// Server receives a version message from Client.
void Server::HandleVersion(const SystemAddress clientAddress, u32 version)
{
  multiplayerLogger.WriteLine(Info, L"Server received Client Version: %i", version);

  if (version != MessageVersion) {
    multiplayerLogger.WriteLine(Info, L"Disconnecting the Client: Version Mismatch: (Client = %i  Server = %i)",
      version, MessageVersion);

    // TODO Disconnect Client
  }
}

// Server receives game started request from Client upon initial login.
void Server::HandleHasGameStarted(const SystemAddress clientAddress)
{
  NetworkMessages::GameHasStarted msgGameHasStarted;
  msgGameHasStarted.set_started(m_bGameStarted);

  SendMessage<NetworkMessages::GameHasStarted>(clientAddress, S_REPLY_HASGAMESTARTED, &msgGameHasStarted);
}

// Server receives game entrance from Client
void Server::HandleGameEnter(const SystemAddress clientAddress)
{
  multiplayerLogger.WriteLine(Info, L"Client has Entered the Game");

  // Send a request for character info
  NetworkMessages::RequestCharacterInfo msgRequestCharacterInfo;

  SendMessage<NetworkMessages::RequestCharacterInfo>(clientAddress, S_REQUEST_CHARINFO, &msgRequestCharacterInfo);
}

// Server receives game exit from client
void Server::HandleGameExited(const SystemAddress clientAddress)
{
  multiplayerLogger.WriteLine(Info, L"Client has Exited the Game");
}

void Server::HandleReplyCharacterInfo(const SystemAddress clientAddress, Vector3 posCharacter, u64 guidCharacter, string nameCharacter, u64 guidPet, string namePet)
{
  multiplayerLogger.WriteLine(Info, L"Server Received Client Character Info:");
  multiplayerLogger.WriteLine(Info, L"  GUID: %016I64X", guidCharacter);
  multiplayerLogger.WriteLine(Info, L"  Name: %s", convertAcsiiToWide(nameCharacter).c_str());
  multiplayerLogger.WriteLine(Info, L"  Position: %f %f %f", posCharacter.x, posCharacter.y, posCharacter.z);
  multiplayerLogger.WriteLine(Info, L"  GUID: %016I64X", guidPet);
  multiplayerLogger.WriteLine(Info, L"  Name: %s", convertAcsiiToWide(namePet).c_str());

  //
  // Create a new network message for other clients to create this character
  NetworkMessages::Character msgNewCharacter;
  msgNewCharacter.set_guid(guidCharacter);
  msgNewCharacter.set_name(nameCharacter);

  NetworkMessages::Position  *msgPlayerPosition = msgNewCharacter.add_position();
  msgPlayerPosition->set_x(posCharacter.x);
  msgPlayerPosition->set_y(posCharacter.y);
  msgPlayerPosition->set_z(posCharacter.z);

  // This will broadcast to all clients except the one we received it from
  Server::getSingleton().BroadcastMessage<NetworkMessages::Character>(clientAddress, S_PUSH_NEWCHAR, &msgNewCharacter);

  // Create this character on this instance
  log("Creating character... gameClient = %p", gameClient);
  log("                      player = %p", gameClient->pCPlayer);
  log("                      resourceManager = %p", gameClient->pCPlayer->pCResourceManager);
  log("                      guid = %016I64X", guidCharacter);
  CResourceManager *resourceManager = (CResourceManager *)gameClient->pCPlayer->pCResourceManager;
  CMonster *clientCharacter = resourceManager->CreateMonster(guidCharacter, 1, false);

  log("Character created: %p", clientCharacter);

  if (clientCharacter) {
    clientCharacter->characterName.assign(convertAcsiiToWide(nameCharacter));
    clientCharacter->SetAlignment(0);
  } else {
    log("Error: clientCharacter is null!");
    multiplayerLogger.WriteLine(Info, L"Error: clientCharacter is null!");
  }

  log("HandleReplyCharacterInfo: gameClient = %p", gameClient);
  log("                          level = %p", gameClient->pCLevel);
  gameClient->pCLevel->CharacterInitialize(clientCharacter, &posCharacter, 0);

  // Create a network ID to identify this character later
  NetworkEntity *newEntity = new NetworkEntity(clientCharacter);
  multiplayerLogger.WriteLine(Info, L"Created NetworkEntity for Client's Character: internalID = %x, networkID = %x",
    newEntity->getInternalId(), newEntity->getCommonId());
  NetworkSharedCharacters->push_back(newEntity);

  // Send this ID to the Client that created the character
  NetworkMessages::ReplyCharacterId msgReplyCharacterId;
  msgReplyCharacterId.set_id(newEntity->getCommonId());

  Server::getSingleton().SendMessage<NetworkMessages::ReplyCharacterId>(clientAddress, S_REPLY_CHARID, &msgReplyCharacterId);
}


void Server::HandleCharacterDestination(const SystemAddress clientAddress, u32 commonId, Vector3 destination)
{
  multiplayerLogger.WriteLine(Info, L"Server received character setDestination (CommonID = %x), Position = %f, %f",
    commonId, destination.x, destination.z);

  // Search for our Character given the CommonID
  NetworkEntity *entity = searchCharacterByCommonID(commonId);

  if (!entity) {
    multiplayerLogger.WriteLine(Error, L"Error: Could not find internal object in shared characters from commonId: %x when receiving character destination.", commonId);
    return;
  }

  CCharacter *character = (CCharacter *)entity->getInternalObject();

  character->SetDestination(gameClient->pCLevel, destination.x, destination.z);
}
