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

  //ServerEntities = new vector<c_entity *>();
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
  case C_GAME_JOIN:
    {
      NetworkMessages::Entity *message = ParseMessage<NetworkMessages::Entity>(m_pBitStream);

      // Send the client our known entities here on the server
      log("[SERVER] Sending client entities...");
      // Just the player atm
      SendClientEntities();
      log("[SERVER] Send entities completed.");

      // Create player on the server instance
      log("Player Joined Game:");
      log("  guid: %016I64X", message->guid());
      log("  level: %i", message->level());
      log("  position: %f %f %f", message->position().Get(0).x(), message->position().Get(0).y(), message->position().Get(0).z());

      Vector3 position;
      position.x = message->position().Get(0).x();
      position.y = message->position().Get(0).y();
      position.z = message->position().Get(0).z();

      // Spawn the player on the server, but suppress sending the redundant msg to the client
      ServerAllowSpawn = false;
      otherPlayer = SpawnPlayer(message->guid(), message->level(), position);
      SendItemListToPlayer();
      ServerAllowSpawn = true;
    }
    break;
  case C_PLAYER_INFO:
    {
      NetworkMessages::Player *player = ParseMessage<NetworkMessages::Player>(m_pBitStream);

      log("Player Info Received:");
      log("  id = %016I64X", player->id());
      log("  name = %s", player->name().c_str());
      log("  type = %i", player->type());
    }

    break;

  case C_PLAYER_SETDEST:
    {
      NetworkMessages::Position *destination = ParseMessage<NetworkMessages::Position>(m_pBitStream);

      //log("[SERVER] SetDest received from client");

      if (otherPlayer) {
        //log("  Getting destination...");
        //PVOID otherDest = GetDestination(otherPlayer);

        // Convert otherPlayer into an entity
        c_entity otherPlayerEntity;
        if (otherPlayer) {
          otherPlayerEntity.e = otherPlayer;
          otherPlayerEntity.init();
        }

        Vector3* otherDest = (Vector3*)GetDestination(otherPlayer);
        //log("  Retrieved destination");
        //log("  Dest ptr = %p", otherDest);
        //log("  Dest = %f %f %f", otherDest->x, otherDest->y, otherDest->z);
        //log("  Setting destination...");
        SetDestination(otherPlayer, otherDest, destination->x(), destination->z());
        //log("  Done.");

        /*
        log("Set Destination Info:");
        log("  x: %f", destination->x());
        log("  y: %f", destination->y());
        log("  z: %f", destination->z());
        */
      }
    }

    break;
  }     
}

void Server::SendClientEntities()
{
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

  /* Don't send monsters for now
  for (itr = ServerEntities->begin(); itr != ServerEntities->end(); itr++) {
    log("[SERVER] Sending entity:");
    log("         level: %i", (*itr)->level);
    log("         guid:  %016I64X", (*itr)->guid);
    
    entity.set_level((*itr)->level);
    entity.set_guid((*itr)->guid);
    entity.set_noitems(true);
    SendMessage<NetworkMessages::Entity>(S_SPAWN_MONSTER, &entity);
  }
  */
}
