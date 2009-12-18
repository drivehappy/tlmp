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
      NetworkMessages::Player *player = ParseMessage<NetworkMessages::Player>(packet->data, packet->length);
      log("Player Info Received:");
      log("  id = %016I64X", player->id());
      log("  name = %s", player->name().c_str());
      log("  type = %i", player->type());
      break;
    }

		m_pServer->DeallocatePacket(packet);
		packet = m_pServer->Receive();
	}
}
