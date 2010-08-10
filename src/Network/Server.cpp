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

  m_bWaitingForGame = true;
}

Server::~Server()
{
  delete m_pBitStream;
}

void Server::Listen(u16 port, u16 maxconnections)
{
  multiplayerLogger.WriteLine(Info, L"Starting server...");

  if (m_pServer) {
    multiplayerLogger.WriteLine(Info, L"Shutting down server");
    m_pServer->Shutdown(0);
  }

  m_pServer = RakNetworkFactory::GetRakPeerInterface();
  multiplayerLogger.WriteLine(Info, L"Got server: %p", m_pServer);

  SocketDescriptor socketDescriptor(port, 0);
  m_pServer->Startup(maxconnections, 30, &socketDescriptor, 1);
  m_pServer->SetMaximumIncomingConnections(maxconnections);

  multiplayerLogger.WriteLine(Info, L"Server listening on port %i (max connections = %i)", port, maxconnections);
  if (m_pOnListening) {
    m_pOnListening(NULL);
  }
}

void Server::Shutdown()
{
  if (m_pServer) {
    m_pServer->Shutdown(0);
    m_pServer = NULL;

    multiplayerLogger.WriteLine(Info, L"Server shutdown");

    if (m_pOnShutdown)
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

  // Round up any packets we've received
  Packet *packet = m_pServer->Receive();
  while (packet) {
    switch(packet->data[0]) {
    case ID_NEW_INCOMING_CONNECTION:
      multiplayerLogger.WriteLine(Info, L"Client connected");
      if (m_pOnClientConnect) {
        SystemAddress *sysAddress = new SystemAddress();
        *sysAddress = packet->systemAddress;
        m_pOnClientConnect((void*)sysAddress);
      }
      break;
    case ID_DISCONNECTION_NOTIFICATION:
      multiplayerLogger.WriteLine(Info, L"Client disconnected");
      if (m_pOnClientDisconnect) {
        m_pOnClientDisconnect(NULL);
      }
      break;
    case ID_CONNECTION_LOST:
      multiplayerLogger.WriteLine(Info, L"Client disconnected");
      if (m_pOnClientDisconnect) {
        m_pOnClientDisconnect(NULL);
      }
      break;
    case ID_USER_PACKET_ENUM+1:
      u32 msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<u32>(msg);

      WorkMessage(packet->systemAddress, (Message)msg, m_pBitStream);

      break;
    }

    m_pServer->DeallocatePacket(packet);
    packet = m_pServer->Receive();
  }
}

void Server::WorkMessage(const SystemAddress address, Message msg, RakNet::BitStream *bitStream)
{
  wstring msgString = convertAcsiiToWide(MessageString[msg]);

  multiplayerLogger.WriteLine(Info, L"Server Received Message: %s", msgString.c_str());

  switch (msg) {
  case C_VERSION:
    {
      NetworkMessages::Version *msgVersion = ParseMessage<NetworkMessages::Version>(m_pBitStream);
      u32 version = (u32)(msgVersion->version());

      HandleVersion(address, version);
    }
    break;

  case C_REQUEST_HASGAMESTARTED:
    {
      NetworkMessages::GameHasStarted *msgGameHasStarted = ParseMessage<NetworkMessages::GameHasStarted>(m_pBitStream);

      HandleHasGameStarted(address);
    }
    break;

  case C_PUSH_GAMEENTER:
    {
      NetworkMessages::GameEnter *msgGameEnter = ParseMessage<NetworkMessages::GameEnter>(m_pBitStream);

      HandleGameEnter(address);
    }
    break;

  case C_PUSH_GAMEEXITED:
    {
      NetworkMessages::GameExited *msgGameExited = ParseMessage<NetworkMessages::GameExited>(m_pBitStream);

      HandleGameExited(address);
    }
    break;

  case C_REPLY_CHARINFO:
    break;

  case C_PUSH_EQUIPMENT_DROP:
    break;

  case C_PUSH_EQUIPMENT_PICKUP:
    break;

  case C_PUSH_EQUIPMENT_EQUIP:
    break;

  case C_PUSH_EQUIPMENT_UNEQUIP:
    break;

  case C_PUSH_EQUIPMENT_USE:
    break;

  case C_PUSH_EQUIPMENT_IDENTIFY:
    break;

  case C_PUSH_EQUIPMENT_ADDENCHANT:
    break;

  case C_PUSH_EQUIPMENT_ADDSOCKET:
    break;

  case C_PUSH_CHARACTER_SETDEST:
    break;

  }
}

//
// Below are message handlers

// Server receives a version message from Client.
void Server::HandleVersion(const SystemAddress clientAddress, u32 version)
{
  multiplayerLogger.WriteLine(Info, L"Server received Client Version: %i", version);

  if (version != MessageVersion) {
    multiplayerLogger.WriteLine(Info, L"Disconnecting the Client: Version Mismatch: (Client = %i  Server = %i)",
      version, MessageVersion);

    // TODO Disconnect Client
  }
}

// Server receives game started request from Client upon initial login.
void Server::HandleHasGameStarted(const SystemAddress clientAddress)
{
  NetworkMessages::GameHasStarted msgGameHasStarted;
  msgGameHasStarted.set_started(m_bGameStarted);

  SendMessage<NetworkMessages::GameHasStarted>(clientAddress, S_REPLY_HASGAMESTARTED, &msgGameHasStarted);
}

// Server receives game entrance from Client
void Server::HandleGameEnter(const SystemAddress clientAddress)
{
}

// Server receives game exit from client
void Server::HandleGameExited(const SystemAddress clientAddress)
{
}
