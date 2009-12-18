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
}

Server::~Server()
{
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
}

void Server::Shutdown()
{
  if (m_pServer) {
    m_pServer->Shutdown(0);
    m_pServer = NULL;

    log("Server shutdown");
  }
}

void Server::ReceiveMessages()
{
  if (!m_pServer)
    return;

  Packet *packet = m_pServer->Receive();
	while (packet) {
		switch(packet->data[0]) {
		case ID_NEW_INCOMING_CONNECTION:
			log("ID_NEW_INCOMING_CONNECTION");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			log("ID_DISCONNECTION_NOTIFICATION");
			break;
		case ID_CONNECTION_LOST:
			log("ID_CONNECTION_LOST");
			break;
		case ID_USER_PACKET_ENUM+1:
      log("ID_USER_PACKET_ENUM+1");
      break;
    }

		m_pServer->DeallocatePacket(packet);
		packet = m_pServer->Receive();
	}
}
