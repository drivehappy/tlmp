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
      ParseMessage(packet->data, packet->length);
      break;
		}

		m_pClient->DeallocatePacket(packet);
		packet = m_pClient->Receive();
	}
}

void Client::SendMessage(TLMP::NetworkMessages::Player message)
{
  // Write message data to array
  u32 size = sizeof(u8) * message.ByteSize();
  u8 *dump = new u8[size];
  message.SerializeToArray(dump, size);

  // Write data and size to raknet
  m_pBitStream->Write((u8)(ID_USER_PACKET_ENUM+1));
  m_pBitStream->Write(size);
  m_pBitStream->Write((const char *)dump, size);

  m_pClient->Send(m_pBitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 1, UNASSIGNED_SYSTEM_ADDRESS, true);

  delete dump;
}

void Client::OnConnect(void *args)
{
  TLMP::NetworkMessages::Player player;

  // Init player
  player.set_id((u64)15);
  player.set_name("drivehappy");
  player.set_type(NetworkMessages::Player_ClassType_ALCHEMIST);

  SendMessage(player);
}

void Client::ParseMessage(u8 *packetData, u32 length)
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
