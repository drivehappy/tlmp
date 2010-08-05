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
      /*
      ServerAllowSpawn = false;
      otherPlayer = SpawnPlayer(message->guid(), message->level(), position);
      SendItemListToPlayer();
      ServerAllowSpawn = true;

      // Store the monster ptr in our shared network list
      NetworkEntity *networkItem = new NetworkEntity(otherPlayer, message->id());

      if (!NetworkSharedEntities)
        NetworkSharedEntities = new vector<NetworkEntity*>();
      NetworkSharedEntities->push_back(networkItem);
      */
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

      // TODO FIX ME
      /*
      if (otherPlayer) {
        // Convert otherPlayer into an entity
        c_entity otherPlayerEntity;
        if (otherPlayer) {
          otherPlayerEntity.e = otherPlayer;
          otherPlayerEntity.init();
        }

        //Vector3* otherDest = (Vector3*)GetDestination(otherPlayer);
        //log("  Retrieved destination");
        //log("  Dest ptr = %p", otherDest);
        //log("  Dest = %f %f %f", otherDest->x, otherDest->y, otherDest->z);
        //log("  Setting destination...");
        SetDestination(otherPlayer, EntityManager->pCLevel, destination->x(), destination->z());
        //log("  Done.");

        //log("Set Destination Info:");
        //log("  x: %f", destination->x());
        //log("  y: %f", destination->y());
        //log("  z: %f", destination->z());
      }
      */
    }
    break;

  case C_ITEM_CREATE:
    {
      NetworkMessages::Item *item = ParseMessage<NetworkMessages::Item>(m_pBitStream);

      log("[SERVER] Received item creation:");
      log("         guid = %016I64X", item->guid());
      log("         id = %i", item->id());

      // TODO FIX ME
      /*
      if (UnitManager) {
        ServerSendClientItemSpawn = false;
        PVOID itemCreated = ItemCreate(EntityManager, item->guid(), item->level(), item->unk0(), item->unk1());
        //ItemInitialize(itemCreated, ItemManager);
        log("Created and initialized: %p", itemCreated);
        ServerSendClientItemSpawn = true;

        NetworkEntity *networkItem = new NetworkEntity(itemCreated, item->id());

        if (!NetworkSharedItems)
          NetworkSharedItems = new vector<NetworkEntity *>();
        NetworkSharedItems->push_back(networkItem);
      } else {
        log("[ERROR] Could not create item: UnitManager is null!");
      }
      */
    }
    break;

  case C_ITEM_DROP:
    {
      NetworkMessages::ItemDrop *itemDropped = ParseMessage<NetworkMessages::ItemDrop>(m_pBitStream);
      Vector3 *itemPosition = new Vector3();
      itemPosition->x = itemDropped->position().Get(0).x();
      itemPosition->y = itemDropped->position().Get(0).y();
      itemPosition->z = itemDropped->position().Get(0).z();

      log("[SERVER] Received ItemDrop:");
      log("         id: %p", itemDropped->id());
      log("         pos: %f, %f, %f", itemPosition->x, itemPosition->y, itemPosition->z);
      log("         unk: %i", itemDropped->unk0());

      /*
      if (UnitManager) {
        CEquipment *item = NULL;
        vector<NetworkEntity *>::iterator itr;

        if (NetworkSharedItems) {
          // Ensure that the item has been created before we attempt to drop it
          for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
            if ((*itr)->getCommonId() == itemDropped->id()) {
              item = (CEquipment*)(*itr)->getInternalObject();
              log("[SERVER] Found item to drop (commonId = %i): %p", itemDropped->id(), item);

              // Drop the item
              // SUPPRESSED for now, it's not quite working right and causes crash
              if (Level) {
                // We don't know who's inventory this was dropped from
                //c_entity ne;
                //ne.e = me;
                //ne.init();
                //c_inventory *inv = ne.inventory();

                //ItemUnequip(inv, item);

                ServerSendClientItemSpawn = false;
                ItemDrop(Level, item, *itemPosition, 1);
                ServerSendClientItemSpawn = true;
              } else {
                log("[ERROR] drop_item_this is null (drivehappy - This is a ptr to a CLevel object)");
              }

              break;
            }
          }
        } else {
          log("[ERROR] NetworkSharedItems is null.");
        }
      } else {
        log("[ERROR] Could not drop item: UnitManager is null!");
      }
      */
    }
    break;

  case C_ITEM_PICKUP:
    {
      NetworkMessages::ItemPickup *itemPickup = ParseMessage<NetworkMessages::ItemPickup>(m_pBitStream);

      log("[SERVER] Received ItemPickup:");
      log("         id: %p", itemPickup->id());
      log("         owner: %i", itemPickup->ownerid());

      // TODO FIX ME
      /*
      if (UnitManager) {
        CEquipment *item = NULL;
        CPlayer *owner = NULL;
        vector<NetworkEntity *>::iterator itr;

        if (NetworkSharedItems) {
          // Ensure that the item has been created before we attempt to pick it up
          for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
            if ((*itr)->getCommonId() == itemPickup->id()) {
              item = (CEquipment*)(*itr)->getInternalObject();
              log("[SERVER] Found item to pickup (commonId = %x): %p", itemPickup->id(), item);
              break;
            }
          }

          // Ensure that the entity picking up the item has been created before we use it
          for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
            //log("[SERVER] Searching %p == %p  ?", (*itr)->getCommonId(), itemPickup->ownerid());
            if ((*itr)->getCommonId() == itemPickup->ownerid()) {
              owner = (CPlayer*)(*itr)->getInternalObject();
              log("[SERVER] Found owner of equip (commonId = %i): %p", itemPickup->ownerid(), owner);
            }
          }

          if (item && owner) {
            // Pickup the item
            ServerSendClientItemSpawn = false;
            ItemPickup(owner, item, Level);
            ServerSendClientItemSpawn = true;
          } else {
            log("[ERROR] owner or item is null (owner = %p, item = %p)", owner, item); // drop_item_this = CLevel object
          }
        } else {
          log("[ERROR] NetworkSharedItems is null.");
        }
      } else {
        log("[ERROR] Could not pickup item: UnitManager is null!");
      }
      */
    }
    break;

  case C_ITEM_EQUIP:
    {
      NetworkMessages::ItemEquip *itemEquipped = ParseMessage<NetworkMessages::ItemEquip>(m_pBitStream);
      CEquipment* item = NULL;
      CPlayer* owner = NULL;
      int slot = itemEquipped->slot();
      int unk = itemEquipped->unk();
      vector<NetworkEntity *>::iterator itr;

      logColor(B_GREEN, "[SERVER] Received item equip:");
      logColor(B_GREEN, "         id: %i", itemEquipped->id());
      logColor(B_GREEN, "         slot %i", itemEquipped->slot());
      logColor(B_GREEN, "         ownerid: %i", itemEquipped->ownerid());
      logColor(B_GREEN, "         unk: %i", itemEquipped->unk());

      if (NetworkSharedItems) {
        // Ensure that the item has been created before we attempt to unequip it
        for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
          if ((*itr)->getCommonId() == itemEquipped->id()) {
            item = (CEquipment*)(*itr)->getInternalObject();
            log("[SERVER] Found item to equip (commonId = %i): %p", itemEquipped->id(), item);
          }
        }

        // Ensure that the entity unequipping the item has been created before we use it
        for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
          //log("[SERVER] Searching %p == %p  ?", (*itr)->getCommonId(), itemEquipped->ownerid());
          if ((*itr)->getCommonId() == itemEquipped->ownerid()) {
            owner = (CPlayer*)(*itr)->getInternalObject();
            log("[SERVER] Found owner of equip (commonId = %i): %p", itemEquipped->ownerid(), owner);
          }
        }

        // If the owner and item are valid, unequip
        if (owner && item) {
          //log("[SUPPRESSED] Equipping item...");
          log("[SERVER] Equipping item (owner = %p, item = %p, slot = %i)", owner, item, slot);
          //log("[SERVER] me = %p, otherPlayer = %p", me, otherPlayer);

          //c_entity ce;
          //ce.e = owner;
          //ce.init();
          //PVOID pinv = ce.inventory();

          //ServerSendClientItemSpawn = false;
          //ItemEquip(pinv, item, slot, unk);
          //ServerSendClientItemSpawn = true;

          // TODO FIX ME
          /*
          ServerSendClientItemSpawn = false;
          ItemEquip(owner->pCInventory, item, slot, unk);
          ServerSendClientItemSpawn = true;
          */
        }
      }
    }
    break;

  case C_ITEM_UNEQUIP:
    {
      NetworkMessages::ItemUnequip *itemUnequipped = ParseMessage<NetworkMessages::ItemUnequip>(m_pBitStream);
      CEquipment* item = NULL;
      CPlayer* owner = NULL;
      vector<NetworkEntity *>::iterator itr;

      log("[SERVER] Received item unequip:");
      log("         id: %i", itemUnequipped->id());
      log("         ownerid: %i", itemUnequipped->ownerid());

      if (NetworkSharedItems) {
        // Ensure that the item has been created before we attempt to unequip it
        for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
          if ((*itr)->getCommonId() == itemUnequipped->id()) {
            item = (CEquipment*)(*itr)->getInternalObject();
            log("[SERVER] Found item to unequip (commonId = %i): %p", itemUnequipped->id(), item);
          }
        }

        // Ensure that the entity unequipping the item has been created before we use it
        for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
          //log("[SERVER] Searching %p == %p  ?", (*itr)->getCommonId(), itemUnequipped->ownerid());
          if ((*itr)->getCommonId() == itemUnequipped->ownerid()) {
            owner = (CPlayer*)(*itr)->getInternalObject();
            log("[SERVER] Found owner of unequip (commonId = %i): %p", itemUnequipped->ownerid(), owner);
            break;
          }
        }

        // If the owner and item are valid, unequip
        if (owner && item) {
          log("[SERVER] Received Unequip");
          //log("[SERVER] me = %p, otherPlayer = %p", me, otherPlayer);

          // ce;
          //ce.e = owner;
          //ce.init();
          //PVOID pinv = ce.inventory();

          //ServerSendClientItemSpawn = false;
          //ItemUnequip(pinv, item);
          //ServerSendClientItemSpawn = true;

          // TODO FIX ME
          /*
          ServerSendClientItemSpawn = false;
          ItemUnequip(owner->pCInventory, item);
          ServerSendClientItemSpawn = true;
          */
        }
      }

    }
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
