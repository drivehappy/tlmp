#include "LobbyClient.h"
using namespace TLMP::Network::Lobby;


LobbyClient& LobbyClient::getSingleton()
{
  static LobbyClient m_Instance;

  return m_Instance;
}

LobbyClient::LobbyClient()
{
  m_pClient = NULL;
  m_pBitStream = new RakNet::BitStream(1024);
}

LobbyClient::~LobbyClient()
{
  if (m_pClient) {
    delete m_pClient;
  }
}


void LobbyClient::Connect(const char* address, unsigned short port)
{
  printf("Lobby client connecting to %s (%i)\n", address, port);

  if (m_pClient) {
    m_pClient->Shutdown(0);
  }

  m_pClient = RakNetworkFactory::GetRakPeerInterface();

  m_pClient->SetTimeoutTime(10000, UNASSIGNED_SYSTEM_ADDRESS);

  SocketDescriptor socketDescriptor(0, 0);
  m_pClient->Startup(1, 30, &socketDescriptor, 1);
  m_pClient->Connect(address, port, 0, 0);
}

void LobbyClient::Disconnect()
{
  printf("Lobby client disconnecting...\n");

  if (m_pClient) {
    m_pClient->Shutdown(0);
    m_pClient = NULL;
  }
}

void LobbyClient::ReceiveMessages()
{
  if (!m_pClient)
    return;

  Packet *packet = m_pClient->Receive();
  while (packet)
  {
    switch(packet->data[0])
    {
    case ID_CONNECTION_REQUEST_ACCEPTED:
      {
        printf("Client connected to server.");

        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
      }

      break;
    case ID_NO_FREE_INCOMING_CONNECTIONS:
      {
        printf("Could not connect to server: No room.");
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      {
        printf("Client disconnected.");
      }
      break;
    case ID_CONNECTION_LOST:
      {
        printf("Client connection lost.");
      }
      break;
    case ID_CONNECTION_ATTEMPT_FAILED:
      {
        printf("Client connection failed.");
      }
      break;
    case ID_USER_PACKET_ENUM+1:
      unsigned int msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<unsigned int>(msg);

      WorkMessage((LobbyMessage)msg, m_pBitStream);

      break;
    }

    m_pClient->DeallocatePacket(packet);
    packet = m_pClient->Receive();
  }
}

void LobbyClient::WorkMessage(LobbyMessage msg, RakNet::BitStream *bitStream)
{
  wstring msgString = convertAsciiToWide(MessageString[msg]);

  multiplayerLogger.WriteLine(Info, L"Lobby Client Received Message: %s", msgString.c_str());
  logColor(B_GREEN, L"Lobby Client Received Message: %s", msgString.c_str());

  switch (msg) {
  case L_S_VERSION:
    {
      NetworkMessages::Version *msgVersion = ParseMessage<NetworkMessages::Version>(m_pBitStream);
      unsigned int version = (unsigned int)(msgVersion->version());

      log("Lobby Client Got Server Version: %i\n", version);
    }
    break;
  }
}

