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
      }
      break;

    case ID_CONNECTION_LOST:
      {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
        
        printf("Client connection lost: %s\n", sysAddress->ToString());
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
  }
}


void LobbyServer::HandleVersion(const SystemAddress address, Version *msgVersion)
{
  unsigned int version = (unsigned int)(msgVersion->version());
      
  printf("Lobby Server Got Client Version: %i\n", version);

  if (version != LobbyMessageVersion) {
    printf("Lobby client version mismatch with server (%i != %i)\n", version, LobbyMessageVersion);
    m_pServer->CloseConnection(address, true);
  }
}

void LobbyServer::HandlePlayerName(const SystemAddress address, ClientPlayerName *msgClientPlayerName)
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

void LobbyServer::HandleChatMessage(ChatMessage *msgChat)
{
  const std::string sender = msgChat->sender();
  const std::string msg = msgChat->message();

  printf("Lobby Server received chat message from sender: %s (%s)\n", sender.c_str(), msg.c_str());

  // Push it back out through a broadcast
  BroadcastMessage<LobbyMessages::Version>(L_S_CHAT_MESSAGE, msgChat);
}