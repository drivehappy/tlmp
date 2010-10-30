#include "lobbyServer.h"
using namespace TLMP::Network::Lobby;


LobbyServer::LobbyServer()
{
  m_pServer = NULL;
  m_pBitStream = new RakNet::BitStream(1024);
}

LobbyServer::~LobbyServer()
{
  delete m_pBitStream;
}

void LobbyServer::Initialize(unsigned short port, unsigned short maxconnections)
{
  m_iPort = port;
  m_iMaxConnections = maxconnections;

  m_pServer = RakNetworkFactory::GetRakPeerInterface();

  m_pServer->SetTimeoutTime(10000, UNASSIGNED_SYSTEM_ADDRESS);

  SocketDescriptor socketDescriptor(m_iPort, 0);
  m_pServer->Startup(m_iMaxConnections, 30, &socketDescriptor, 1);
  m_pServer->SetMaximumIncomingConnections(m_iMaxConnections);
}

void LobbyServer::Process()
{
  if (!m_pServer)
    return;

  // Round up any packets we've received
  Packet *packet = m_pServer->Receive();
  while (packet) {
    switch(packet->data[0]) {
    case ID_NEW_INCOMING_CONNECTION:
      {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;

        printf("Client connected: %s\n", sysAddress->ToString());

        LobbyMessages::Version msgVersion;
        msgVersion.set_version(LobbyMessageVersion);
        SendMessage<LobbyMessages::Version>(*sysAddress, L_S_VERSION, &msgVersion);
      }
      break;

    case ID_DISCONNECTION_NOTIFICATION:
      {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
        
        printf("Client disconnected: %s\n", sysAddress->ToString());
        HandlePlayerDisconnect(*sysAddress);
      }
      break;

    case ID_CONNECTION_LOST:
      {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
        
        printf("Client connection lost: %s\n", sysAddress->ToString());
        HandlePlayerDisconnect(*sysAddress);
      }
      break;

    case ID_USER_PACKET_ENUM+1:
      unsigned int msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<unsigned int>(msg);

      WorkMessage(packet->systemAddress, (LobbyMessage)msg, m_pBitStream);

      break;
    }

    m_pServer->DeallocatePacket(packet);
    packet = m_pServer->Receive();
  }
}



void LobbyServer::WorkMessage(const SystemAddress address, LobbyMessage msg, RakNet::BitStream *bitStream)
{
  printf("Lobby Server Received Message: %s from %s\n", LobbyMessageString[msg], address.ToString());

  switch (msg) {
  case L_C_VERSION:
    {
      LobbyMessages::Version *msgVersion = ParseMessage<LobbyMessages::Version>(m_pBitStream);
      
      HandleVersion(address, msgVersion);
    }
    break;

  case L_C_PLAYERNAME:
    {
      LobbyMessages::ClientPlayerName *msgClientPlayerName = ParseMessage<LobbyMessages::ClientPlayerName>(m_pBitStream);
      
      HandlePlayerName(address, msgClientPlayerName);
    }
    break;

  case L_C_CHAT_MESSAGE:
    {
      LobbyMessages::ChatMessage *msgChat = ParseMessage<LobbyMessages::ChatMessage>(m_pBitStream);

      HandleChatMessage(msgChat);
    }
    break;

  case L_C_VIEW_GAMES:
    {
      //LobbyMessages::ViewGames *msgViewGames = ParseMessage<LobbyMessages::ViewGames>(m_pBitStream);

      HandleViewGames(address);
    }
    break;
    
  case L_C_HOSTING_NEW_GAME:
    {
      LobbyMessages::HostingNewGame *msgHostingNewGame = ParseMessage<LobbyMessages::HostingNewGame>(m_pBitStream);

      HandleHostingNewGame(address, msgHostingNewGame);
    }
    break;
    
  case L_C_REQUEST_JOINGAME:
    {
      LobbyMessages::GameID *msgGameID = ParseMessage<LobbyMessages::GameID>(m_pBitStream);

      HandleRequestGameInfo(address, msgGameID);
    }
    break;
  }
}

void LobbyServer::HandlePlayerDisconnect(const SystemAddress address)
{
  printf("Handling player disconnect: %s\n", address.ToString());
  string playerName;

  // Find the address in our player map
  map<SystemAddress, string>::iterator itr;
  for (itr = m_PlayerNames.begin(); itr != m_PlayerNames.end(); itr++) {
    if ((*itr).first.binaryAddress == address.binaryAddress) {
      playerName = (*itr).second;
      m_PlayerNames.erase(itr);
      break;
    }
  }

  LobbyMessages::PlayerLeft msgPlayerLeft;

  // Push the remaining player names out as well to rebuild the list
  for (itr = m_PlayerNames.begin(); itr != m_PlayerNames.end(); itr++) {
    msgPlayerLeft.add_playersremaining((*itr).second);
  }
  
  msgPlayerLeft.set_playername(playerName);
  BroadcastMessage<LobbyMessages::PlayerLeft>(address, L_S_PLAYERNAME_LEAVE, &msgPlayerLeft);
}

void LobbyServer::HandleVersion(const SystemAddress address, LobbyMessages::Version *msgVersion)
{
  unsigned int version = (unsigned int)(msgVersion->version());
      
  printf("Lobby Server Got Client Version: %i\n", version);

  if (version != LobbyMessageVersion) {
    printf("Lobby client version mismatch with server (%i != %i)\n", version, LobbyMessageVersion);
    m_pServer->CloseConnection(address, true);
  }
}

void LobbyServer::HandlePlayerName(const SystemAddress address, LobbyMessages::ClientPlayerName *msgClientPlayerName)
{
  std::string playerName = msgClientPlayerName->playername();

  printf("Lobby server received playername: %s\n", playerName.c_str());

  // Add the player name to our address map
  m_PlayerNames[address] = playerName;

  // Push the new name out to everyone except the new guy
  BroadcastMessage<LobbyMessages::ClientPlayerName>(address, L_S_PLAYERNAME_JOIN, msgClientPlayerName);

  // Push the batch of players out to the new guy
  LobbyMessages::BatchPlayerNames msgBatchPlayerNames;
  map<SystemAddress, string>::iterator itr;
  for (itr = m_PlayerNames.begin(); itr != m_PlayerNames.end(); itr++) {
    msgBatchPlayerNames.add_playernames((*itr).second);
  }
  SendMessage<LobbyMessages::ClientPlayerName>(address, L_S_PLAYERNAME_BATCH, &msgBatchPlayerNames);
}

void LobbyServer::HandleChatMessage(LobbyMessages::ChatMessage *msgChat)
{
  const std::string sender = msgChat->sender();
  const std::string msg = msgChat->message();

  printf("Lobby Server received chat message from sender: %s (%s)\n", sender.c_str(), msg.c_str());

  // Push it back out through a broadcast
  BroadcastMessage<LobbyMessages::Version>(L_S_CHAT_MESSAGE, msgChat);
}

void LobbyServer::HandleViewGames(const SystemAddress address)
{
  LobbyMessages::ViewGames *msgViewGames = new LobbyMessages::ViewGames();

  map<SystemAddress, Game*>::iterator itr;
  for (itr = m_AvailableGames.begin(); itr != m_AvailableGames.end(); itr++) {
    LobbyMessages::Game* game = msgViewGames->add_games();
    populateGameMessage(game, *((*itr).second));
  }

  SendMessage<LobbyMessages::ViewGames>(address, L_S_VIEW_GAMES, msgViewGames);
}

void LobbyServer::HandleHostingNewGame(const SystemAddress address, LobbyMessages::HostingNewGame *msgHostingNewGame)
{
  Game *newGame = new Game();

  LobbyMessages::Game *msgGame = msgHostingNewGame->mutable_game();
  populateGameFromMessage(msgGame, newGame);
  m_AvailableGames[address] = newGame;

  // Dump out the game info
  printf("New Game Created:\n");
  printf("  Name: %s\n", newGame->getName().c_str());
  printf("  Players: %i / %i\n", newGame->getCurrentPlayers(), newGame->getMaxPlayers());
  printf("  Description: %s\n", newGame->getDescription().c_str());
  printf("  Level: %s\n", newGame->getCurrentLevel().c_str());
  printf("  Port: %i\n", newGame->getPort());

  LobbyMessages::GameID gameID;
  gameID.set_id(newGame->getID());
  SendMessage<LobbyMessages::GameID>(address, L_S_HOSTING_REPLY, &gameID);
}

void LobbyServer::HandleRequestGameInfo(const SystemAddress address, LobbyMessages::GameID *msgGameInfo)
{
  int requestedID = msgGameInfo->id();

  map<SystemAddress, Game*>::iterator itr;
  for (itr = m_AvailableGames.begin(); itr != m_AvailableGames.end(); itr++) {
    if (requestedID == (*itr).second->getID()) {
      // Send the game information back to the client
      LobbyMessages::GameInfo msgGameInfo;
      msgGameInfo.set_host_name((*itr).first.ToString(false));
      msgGameInfo.set_port((*itr).second->getPort());
      SendMessage<LobbyMessages::GameInfo>(address, L_S_REPLAY_JOINGAME, &msgGameInfo);
      return;
    }
  }

  printf("Error: Could not find requested game of ID = %i\n", requestedID);
}

void LobbyServer::populateGameMessage(LobbyMessages::Game* msgGame, Game game)
{
  msgGame->set_current_level(game.getCurrentLevel());
  msgGame->set_current_players(game.getCurrentPlayers());
  msgGame->set_max_players(game.getMaxPlayers());
  msgGame->set_description(game.getDescription());
  msgGame->set_id(game.getID());
  msgGame->set_name(game.getName());
  msgGame->set_port(game.getPort());
}

void LobbyServer::populateGameFromMessage(LobbyMessages::Game* msgGame, Game* game)
{
  game->setID(getUniqueID());
  game->setCurrentLevel(msgGame->current_level());
  game->setCurrentPlayers(msgGame->current_players());
  game->setDescription(msgGame->description());
  game->setMaxPlayers(msgGame->max_players());
  game->setName(msgGame->name());
  game->setPort(msgGame->port());
}

int LobbyServer::getUniqueID()
{
  static int m_currentID = 0;

  return m_currentID++;
}
