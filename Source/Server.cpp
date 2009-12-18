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

  m_pOnListening = NULL;
  m_pOnShutdown = NULL;
  m_pOnClientConnect = NULL;
  m_pOnClientDisconnect = NULL;
}

Server::~Server()
{
  delete m_pBitStream;
}

void Server::Listen(u16 port, u16 maxconnections)
{
  log("Starting server...");

  if (m_pServer) {
    log("Shutting down server");
    m_pServer->Shutdown(0);
  }

  m_pServer = RakNetworkFactory::GetRakPeerInterface();
  log("Got server: %p", m_pServer);

  SocketDescriptor socketDescriptor(port, 0);
  m_pServer->Startup(maxconnections, 30, &socketDescriptor, 1);
  m_pServer->SetMaximumIncomingConnections(maxconnections);

  log("Server listening on port %i (max connections = %i)", port, maxconnections);
  if (m_pOnListening) {
    m_pOnListening(NULL);
  }
}

void Server::Shutdown()
{
  if (m_pServer) {
    m_pServer->Shutdown(0);
    m_pServer = NULL;

    log("Server shutdown");
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

  Packet *packet = m_pServer->Receive();
	while (packet) {
		switch(packet->data[0]) {
		case ID_NEW_INCOMING_CONNECTION:
      if (m_pOnClientConnect) {
			  m_pOnClientConnect(NULL);
      }
			break;
		case ID_DISCONNECTION_NOTIFICATION:
      if (m_pOnClientDisconnect) {
			  m_pOnClientDisconnect(NULL);
      }
			break;
		case ID_CONNECTION_LOST:
      if (m_pOnClientDisconnect) {
			  m_pOnClientDisconnect(NULL);
      }
			break;
		case ID_USER_PACKET_ENUM+1:
      ParseMessage(packet->data, packet->length);
      break;
    }

		m_pServer->DeallocatePacket(packet);
		packet = m_pServer->Receive();
	}
}

void Server::SendMessage(TLMP::NetworkMessages::Player message)
{
  // Write message data to array
  u32 size = sizeof(u8) * message.ByteSize();
  u8 *dump = new u8[size];
  message.SerializeToArray(dump, size);

  // Write data and size to raknet
  m_pBitStream->Write((u8)(ID_USER_PACKET_ENUM+1));
  m_pBitStream->Write(size);
  m_pBitStream->Write((const char *)dump, size);

  m_pServer->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, UNASSIGNED_SYSTEM_ADDRESS, true);

  delete dump;
}

void Server::ParseMessage(u8 *packetData, u32 length)
{
  TLMP::NetworkMessages::Player player;

  m_pBitStream->Reset();
  m_pBitStream->Write((const char *)packetData, length);
	m_pBitStream->IgnoreBits(8); // Ignore ID_USER_PACKET_ENUM+1

  u32 size = 0;
  u8 *data;
  m_pBitStream->ReadPtr<u32>(&size);
  data = new u8[size];
  m_pBitStream->ReadAlignedBytes(data, size);
  log("Received serialized of size: %i", size);

  player.ParseFromArray(data, size);

  log("Player information received:");
  log("  id = %i", player.id());
  log("  name = %s", player.name().c_str());
  log("  type = %i", player.type());

  delete data;
}


