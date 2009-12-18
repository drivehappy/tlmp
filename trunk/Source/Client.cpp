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
}

Client::~Client()
{
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
			log("ID_CONNECTION_REQUEST_ACCEPTED");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			log("ID_NO_FREE_INCOMING_CONNECTIONS");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			log("ID_DISCONNECTION_NOTIFICATION");
			break;
		case ID_CONNECTION_LOST:
			log("ID_CONNECTION_LOST");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			log("Connection attempt failed");
			break;
		}

		m_pClient->DeallocatePacket(packet);
		packet = m_pClient->Receive();
	}
}
