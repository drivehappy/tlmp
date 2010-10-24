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
        SendMessage<LobbyMessages::Version>(*sysAddress, S_VERSION, &msgVersion);
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

      WorkMessage(packet->systemAddress, (Message)msg, m_pBitStream);

      break;
    }

    m_pServer->DeallocatePacket(packet);
    packet = m_pServer->Receive();
  }
}


void LobbyServer::WorkMessage(const SystemAddress address, Message msg, RakNet::BitStream *bitStream)
{
  printf("Lobby Server Received Message: %s from %s", MessageString[msg], address.ToString());

  switch (msg) {
  case C_VERSION:
    {
      LobbyMessages::Version *msgVersion = ParseMessage<LobbyMessages::Version>(m_pBitStream);
      unsigned int version = (unsigned int)(msgVersion->version());
      
      printf("Lobby Server Got Client Version: %i\n", version);
    }
    break;
  }
}
