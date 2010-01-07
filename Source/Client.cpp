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
  TLMP::NetworkMessages::Player player;

  // Init player
  player.set_id((u64)15);
  player.set_name("drivehappy");
  player.set_type(NetworkMessages::Player_ClassType_ALCHEMIST);

  SendMessage<NetworkMessages::Player>(C_PLAYER_INFO, &player);

  log("[CLIENT] Connected.");
}

void Client::WorkMessage(Message msg, RakNet::BitStream *bitStream)
{
  //log("Message Received: %x", msg);

  switch (msg) {
  case S_PLAYER_INFO:
    {
      NetworkMessages::Player *player = ParseMessage<NetworkMessages::Player>(m_pBitStream);

      log("Player Info Received:");
      log("  id = %016I64X", player->id());
      log("  name = %s", player->name().c_str());
      log("  type = %i", player->type());
    }
    break;

  case S_SPAWN_MONSTER:
    {
      NetworkMessages::Entity *entity = ParseMessage<NetworkMessages::Entity>(m_pBitStream);

      log("[CLIENT] Received Monster Spawn");
      log("         Level = %i", entity->level());
      log("         GUID  = %016I64X", entity->guid());
      log("         NoItems = %i", entity->noitems());

      ClientAllowSpawn = true;
      //SpiderSomeCreate(EntityManager, entity->guid(), entity->level(), entity->noitems());
      ClientAllowSpawn = false;
    }
    break;

  case S_ENTITY_SETDEST:
    {
      NetworkMessages::Position *destination = ParseMessage<NetworkMessages::Position>(m_pBitStream);

      /*
      log("Entity Destination Received:");
      log("  x = %f", destination->x());
      log("  y = %f", destination->y());
      log("  z = %f", destination->z());
      */
    }
    break;
  }     
}
