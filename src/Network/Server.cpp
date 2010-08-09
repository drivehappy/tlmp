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
      u32 msg;
      m_pBitStream->Reset();
      m_pBitStream->Write((const char *)packet->data, packet->length);
      m_pBitStream->IgnoreBits(8);
      m_pBitStream->Read<u32>(msg);

      WorkMessage((Message)msg, m_pBitStream);

      break;
    }

    m_pServer->DeallocatePacket(packet);
    packet = m_pServer->Receive();
  }
}

void Server::WorkMessage(Message msg, RakNet::BitStream *bitStream)
{
  //log("[NETWORK] Message Received: %x", msg);

  switch (msg) {
  case C_VERSION:
    break;

  case C_REQUEST_HASGAMESTARTED:
    break;

  case C_PUSH_GAMEENTER:
    break;

  case C_PUSH_GAMEEXITED:
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

void Server::SendClientEntities()
{
  // TODO FIX ME
  /*
  vector<c_entity *>::iterator itr;
  TLMP::NetworkMessages::Entity entity;

  // Just send the server's player info
  c_entity em;
  if (me) {
    unsigned long long guid = *((unsigned long long*)me + 0x2d); //(0x168 / 8));
    unsigned int level = *((unsigned int*)me + 0x3c);
    int commonId = 0;

    em.e = me;
    em.init();

    // Search for me in my network entities to find the common id
    if (NetworkSharedEntities) {
      vector<NetworkEntity*>::iterator itr;
      for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
        if ((*itr)->getInternalObject() == me) {
          commonId = (*itr)->getCommonId();
          break;
        }
      }
    } else {
      log("[ERROR] NetworkSharedEntities is NULL!");
    }

    entity.set_id(commonId);
    entity.set_level(level);
    entity.set_guid(guid);
    entity.set_noitems(true);

    NetworkMessages::Position *position = entity.add_position();

    position->set_x(em.GetPosition()->x);
    position->set_y(em.GetPosition()->y);
    position->set_z(em.GetPosition()->z);

    SendMessage<NetworkMessages::Entity>(S_GAME_JOIN, &entity);
    log("[SERVER] Sent player info: level: %i, guid: %16I64X", level, guid);
  } else {
    log("[ERROR] Server could not find it's player to send client!");
  }

  // Don't send monsters for now
  //for (itr = ServerEntities->begin(); itr != ServerEntities->end(); itr++) {
  //  log("[SERVER] Sending entity:");
  //  log("         level: %i", (*itr)->level);
  //  log("         guid:  %016I64X", (*itr)->guid);
  //  
  //  entity.set_level((*itr)->level);
  //  entity.set_guid((*itr)->guid);
  //  entity.set_noitems(true);
  //  SendMessage<NetworkMessages::Entity>(S_SPAWN_MONSTER, &entity);
  //}
  */
}
