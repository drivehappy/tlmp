#include "Item.h"
using namespace TLMP;

PVOID                    TLMP::drop_item_this = NULL;
PVOID                    TLMP::ItemManager = NULL;
bool                     TLMP::ServerSendClientItemSpawn = true;
bool                     TLMP::ClientSendServerItemSpawn = true;
vector<NetworkEntity *> *TLMP::NetworkSharedItems    = NULL;
vector<EquippedItem>    *TLMP::EquippedItems = NULL;

void TLMP::NetItem_OnItemCreated(u64 guid, u32 level, u32 unk0, u32 unk1)
{
  log("NetItem_OnItemCreated");

  ClientAllowSpawn = true;
  void *r = ItemCreate(ItemManager, guid, level, unk0, unk1);
  ClientAllowSpawn = false;
}

void TLMP::_item_initialize_pre STDARG
{
  log(" %p :: item initialize %p", e->_this, Pz[0]);
  ItemManager = (PVOID)Pz[0];
}

void TLMP::_item_create_pre STDARG
{
  log("Item Create: this: %p  %#x %#x %#x %#x %#x", e->_this, Pz[0], Pz[1], Pz[2], Pz[3], Pz[4]);
  u64 guid = *(u64 *)&Pz[0];
  int level = Pz[2], unk0 = Pz[3], unk1 = Pz[4];

  // Item's utilize the same UnitManager as players do
  UnitManager = e->_this;

  /*
  //
  if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT && !ClientAllowSpawn) {
    log("[CLIENT] ItemCreate (%p), GUID: %016I64X, Level: %i", e->_this, guid, level);

    e->calloriginal = false;
    e->retval = false;
  } else if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    log("[HOST] _item_create_pre GUID: %016I64X", guid);
  }
  */
}

void TLMP::_item_create_post STDARG
{
  u64 guid = *(u64 *)&Pz[0];
  UnitManager = e->_this;

  log("%p :: Item created: %p, GUID: %016I64X", e->retval, e->_this, guid);

  CItem* newItem = new CItem();
  newItem->e = (PVOID)e->retval;
  newItem->guid = guid;
  newItem->level = Pz[2];
  newItem->unk0 = Pz[3];
  newItem->unk1 = Pz[4];

  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER && ServerSendClientItemSpawn) {
    NetworkEntity *networkItem = new NetworkEntity((PVOID)e->retval);

    if (!NetworkSharedItems)
      NetworkSharedItems = new vector<NetworkEntity *>();
    NetworkSharedItems->push_back(networkItem);

    log("[SERVER] Item created GUID: %016I64X (%p)", newItem->guid, newItem->e);

    NetworkMessages::Item message;
    message.set_id(networkItem->getCommonId());
    message.set_guid(newItem->guid);
    message.set_level(newItem->level);
    message.set_unk0(newItem->unk0);
    message.set_unk1(newItem->unk1);

    Network::Server::getSingleton().SendMessage<NetworkMessages::Item>(Network::S_ITEM_CREATE, &message);
  }
  else if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT && ClientSendServerItemSpawn) {
    NetworkEntity *networkItem = new NetworkEntity((PVOID)e->retval, CLIENT_COMMON_BASE_ID, NetworkSharedItems);

    if (!NetworkSharedItems)
      NetworkSharedItems = new vector<NetworkEntity *>();
    NetworkSharedItems->push_back(networkItem);

    log("[CLIENT] Item created GUID: %016I64X (%p)", newItem->guid, newItem->e);

    NetworkMessages::Item message;
    message.set_id(networkItem->getCommonId());
    message.set_guid(newItem->guid);
    message.set_level(newItem->level);
    message.set_unk0(newItem->unk0);
    message.set_unk1(newItem->unk1);

    Network::Client::getSingleton().SendMessage<NetworkMessages::Item>(Network::C_ITEM_CREATE, &message);
  }
}

void TLMP::SendItemListToPlayer()
{
  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    if (NetworkSharedItems) {
      log("[SERVER] Sending created items to client...");

      /* These GUIDs are bad... maybe they're trashed after item is created?
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        _CItem *citem = (_CItem*)((*itr)->getInternalObject());
        log("guid0 = %016I64X", citem->guid0);
        log("guid1 = %016I64X", citem->guid1);
        log("guid2 = %016I64X", citem->guid2);
        log("guid3 = %016I64X", citem->guid3);
      }
      */
    }
  }
}

void TLMP::_item_drop_pre STDARG
{
  drop_item_this = e->_this;
  // Update our world ptr
  //world = e->_this;

  CItem *itemDropped = (CItem*)Pz[0];
  Vector3 *position = (Vector3 *)Pz[1];
  bool unk0 = *(bool *)&Pz[2];

  log(" %p :: drop item %p %p %d",e->_this, Pz[0], Pz[1], Pz[2]);
  log("     pos: %f, %f, %f", position->x, position->y, position->z);
  log("     GUID: %016I64X", itemDropped->guid);
  log("     GUID Real: %016I64X", itemDropped->getCEquipmentGUID());

  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER && ServerSendClientItemSpawn) {
    NetworkEntity* netItemDropped = NULL;
    bool isItemCreated = false;

    // Search the server's item list for the same ptr
    if (NetworkSharedItems) {
      log("[SERVER] Searching for item in network list...");
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        log("[SERVER] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemDropped);
        if ((*itr)->getInternalId() == (int)itemDropped) {
          isItemCreated = true;
          netItemDropped = (*itr);
          break;
        }
      }
    }

    // Send off the common network id 
    if (isItemCreated) {
      NetworkMessages::ItemDrop message;
      message.set_id(netItemDropped->getCommonId());
      message.set_unk0(unk0);
      
      NetworkMessages::Position *messagePosition = message.add_position();
      messagePosition->set_x(position->x);
      messagePosition->set_y(position->y);
      messagePosition->set_z(position->z);

      Server::getSingleton().SendMessage<NetworkMessages::ItemDrop>(S_ITEM_DROP, &message);

      log("[SERVER] Sent ItemDrop to client:");
      log("         id: %p", message.id());
      log("         unk0: %i", message.unk0());
      log("         pos: %f, %f, %f", messagePosition->x(), messagePosition->y(),  messagePosition->z());
    } else {
      log("[ERROR] Could not find dropped item in network item list (id = %p)", itemDropped);
    }
  }

  else if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT && ClientSendServerItemSpawn) {
    NetworkEntity* netItemDropped = NULL;
    bool isItemCreated = false;

    // Search the server's item list for the same ptr
    if (NetworkSharedItems) {
      log("[SERVER] Searching for item in network list...");
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        log("[SERVER] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemDropped);
        if ((*itr)->getInternalId() == (int)itemDropped) {
          isItemCreated = true;
          netItemDropped = (*itr);
          break;
        }
      }
    }

    // Send off the common network id 
    if (isItemCreated) {
      NetworkMessages::ItemDrop message;
      message.set_id(netItemDropped->getCommonId());
      message.set_unk0(unk0);
      
      NetworkMessages::Position *messagePosition = message.add_position();
      messagePosition->set_x(position->x);
      messagePosition->set_y(position->y);
      messagePosition->set_z(position->z);

      Client::getSingleton().SendMessage<NetworkMessages::ItemDrop>(C_ITEM_DROP, &message);

      log("[CLIENT] Sent ItemDrop to server:");
      log("         id: %p", message.id());
      log("         unk0: %i", message.unk0());
      log("         pos: %f, %f, %f", messagePosition->x(), messagePosition->y(),  messagePosition->z());
    } else {
      log("[ERROR] Could not find dropped item in network item list (id = %p)", itemDropped);
    }
  }
}

void TLMP::_item_pick_up_pre STDARG
{
  //log(" %p :: pick up item %p",e->_this,Pz[0]);

  log("==== ItemPickup (this: %p, %p, %p) retVal = %i", e->_this, Pz[0], Pz[1], e->retval);

  PVOID itemPickup = (void*)Pz[0];
  PVOID owner = e->_this;
  int slot = 0;
  int ownerId = 0;

  c_entity ne;
	ne.e = owner;
	ne.init();
	c_inventory *inv = ne.inventory();
  log("---- inv = %p", inv);

  if (inv) {
    slot = inv->get_item_slot(itemPickup);
    log("---- Slot = %i", slot);
  }

  /*if (slot >= 0) */
  {
    // If we're the server send it off
    if (Network::NetworkState::getSingleton().GetState() == Network::SERVER && ServerSendClientItemSpawn) {
      NetworkEntity* netItemPickup = NULL;
      bool isItemPickup = false;
      bool ownerFound = false;

      // Search the server's item list for the same ptr
      log("[SERVER] Searching for item in network list...");
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        log("[SERVER] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemPickup);
        if ((*itr)->getInternalId() == (int)itemPickup) {
          isItemPickup = true;
          netItemPickup = (*itr);
          break;
        }
      }

      // Search the server's entity list for the owner id
      if (NetworkSharedEntities) {
        log("[SERVER] Searching for entity in network list...");
        for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
          log("[SERVER] Checking %p == %p ?", (*itr)->getInternalObject(), owner);

          if ((*itr)->getInternalObject() == owner) {
            ownerId = (*itr)->getCommonId();
            ownerFound = true;
            log("[SERVER] Found shared owner id: %x", ownerId);
            break;
          }
        }
      } else {
        log("[ERROR] NetworkSharedEntities is NULL!");
      }

      // Send off the common network id 
      if (isItemPickup && ownerFound) {
        NetworkMessages::ItemPickup message;
        message.set_id(netItemPickup->getCommonId());
        message.set_ownerid(ownerId);

        Server::getSingleton().SendMessage<NetworkMessages::ItemPickup>(S_ITEM_PICKUP, &message);

        log("[SERVER] Sent ItemPickup to client:");
        log("         id: %p", message.id());
        log("         ownerId: %i", message.ownerid());
      } else {
        log("[ERROR] Could not find pickup item in network item list (id = %p) (owner = %i) me = %p", itemPickup, ownerId, me);
      }
    }

    else if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT && ClientSendServerItemSpawn) {
      NetworkEntity* netItemPickup = NULL;
      bool isItemPickup = false;
      bool ownerFound = false;

      // Search the server's item list for the same ptr
      log("[CLIENT] Searching for item in network list...");
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        log("[CLIENT] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemPickup);
        if ((*itr)->getInternalId() == (int)itemPickup) {
          isItemPickup = true;
          netItemPickup = (*itr);
          break;
        }
      }

      // Search the server's entity list for the owner id
      if (NetworkSharedEntities) {
        log("[CLIENT] Searching for entity in network list...");
        for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
          log("[CLIENT] Checking %p == %p ?", (*itr)->getInternalObject(), owner);

          if ((*itr)->getInternalObject() == owner) {
            ownerId = (*itr)->getCommonId();
            ownerFound = true;
            log("[CLIENT] Found shared owner id: %x", ownerId);
            break;
          }
        }
      } else {
        log("[ERROR] NetworkSharedEntities is NULL!");
      }

      // Send off the common network id 
      if (isItemPickup && ownerFound) {
        NetworkMessages::ItemPickup message;
        message.set_id(netItemPickup->getCommonId());
        message.set_ownerid(ownerId);

        Client::getSingleton().SendMessage<NetworkMessages::ItemPickup>(C_ITEM_PICKUP, &message);

        log("[CLIENT] Sent ItemPickup to server:");
        log("         id: %p", message.id());
        log("         ownerId: %i", message.ownerid());
      } else {
        log("[ERROR] Could not find pickup item in network item list (id = %p) (owner = %i) me = %p", itemPickup, ownerId, me);
      }
    }
  }
}

void TLMP::_item_pick_up_post STDARG
{
  //log("pick up, retval is %d",e->retval);

  /*
  void *itemPickup = (void*)Pz[0];
  int slot = 0;

  log("==== ItemPickup (this: %p, %p, %p) retVal = %i", e->_this, Pz[0], Pz[1], e->retval);

  c_entity ne;
	ne.e = e->_this;
	ne.init();
	c_inventory *inv = ne.inventory();
  log("---- inv = %p", inv);

  if (inv) {
    slot = inv->get_item_slot(itemPickup);
    log("---- Slot = %i", slot);
  }

  if (slot >= 0) {
    // If we're the server send it off
    if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
      NetworkEntity* netItemPickup = NULL;
      bool isItemPickup = false;

      // Search the server's item list for the same ptr
      log("[SERVER] Searching for item in network list...");
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        log("[SERVER] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemPickup);
        if ((*itr)->getInternalId() == (int)itemPickup) {
          isItemPickup = true;
          netItemPickup = (*itr);
          break;
        }
      }

      // Send off the common network id 
      if (isItemPickup) {
        NetworkMessages::ItemPickup message;
        message.set_id(netItemPickup->getCommonId());
        message.set_slot(slot);

        Server::getSingleton().SendMessage<NetworkMessages::ItemPickup>(S_ITEM_PICKUP, &message);

        log("[SERVER] Sent ItemPickup to client:");
        log("         id: %p", message.id());
        log("         slot: %i", message.slot());
      } else {
        log("[ERROR] Could not find pickup item in network item list (id = %p)", itemPickup);
      }
    }
  }
  */
  
  /* NETWORK STUFF
  index_t*id;
  if (item_map.inuse_get(pitem,id)) {
    CItem&i = net_items[*id];

    c_entity ne;
    ne.e = e->_this;
    ne.init();
    c_inventory*inv = ne.inventory();
    if (inv) {
      int slot = inv->get_item_slot(pitem);
      log2("slot is %d",slot);
      log("slot is %d",slot);

      if (slot!=-1) {
        sendbuf.reset();
        sendbuf.pui(-1);
        sendbuf.pus(id_item_pick_up);
        sendbuf.pui(*id);
        send();

        log("sent item_pick_up");
        log("sent item_pick_up");

        // we do this since the pick up function reassigns the weapon slot without calling item_equip :(
        item_unequip(inv,pitem);
        item_equip(inv,pitem,slot,1);

        log("re-equipped");
        log("re-equipped");
      }
    }
  }
  */
}

void TLMP::_item_equip_pre STDARG
{
  //log(" %p :: item equip pre %p %d %d",e->_this,Pz[0],Pz[1],Pz[2]);
  CItem c = *(CItem*)Pz[0];

  u64 guid = c.guid;

  if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT) {
    if (!ClientAllowEquip) {
      log("[CLIENT] Suppressing item equip.");
      log("\n\n");
      e->calloriginal = false;
      e->retval = 0;
    }
  }

  /* NETWORK STUFF
  if (peer.is_active && !peer_allow_equip) {
    if (peer_items_created.getbyval(guid)) {
      peer_items_equipped.add(guid);
      log2("peer_items_equipped added buffer GUID = %016I64X", guid);
    } else {
      log2("Peer Error: Couldn't find equip requested item in our item list (guid = %016I64X)", guid);
      log2(" Available GUIDs:");
      NL_ITERATE(i, unsigned long long, peer_items_created) {
        log2("  %016I64X", (*i));
      }
    }

    e->calloriginal = false;
    e->retval = 0;
  }
  */
}

void TLMP::_item_equip_post STDARG
{
  log("===== ItemEquip (this: %p, %p, %p, %p) retVal = %i", e->_this, Pz[0], Pz[1], Pz[2], e->retval);

  //if (!e->retval) {
  //  return;
  //}

  void *pinv = e->_this;
  void *itemEquip = (void*)Pz[0];
  int slot = Pz[1];
  index_t entity_id = -1;

  // Retain equipped items for anyone that joins late
  if (!EquippedItems)
    EquippedItems = new vector<EquippedItem>();
  EquippedItem newEquippedItem;
  newEquippedItem.item = (PVOID)Pz[0];
  newEquippedItem.slot = Pz[1];
  EquippedItems->push_back(newEquippedItem);

  /*
  // If we're the server send it off
  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    NetworkEntity* netItemEquip = NULL;
    NetworkEntity* netEntityEquip = NULL;

    log("[SERVER] Searching for item in network list...");
    vector<NetworkEntity *>::iterator itr;
    for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
      log("[SERVER] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemEquip);
      if ((*itr)->getInternalId() == (int)itemEquip) {
        netItemEquip = (*itr);
        break;
      }
    }

    // Search the server's entity list for the owner id
    if (NetworkSharedEntities) {
      log("[SERVER] Searching for entity in network list... %p", me);
      for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
        log("[SERVER] Checking %p == %p ?", (*itr)->getInternalObject(), (int)pinv);

        if ((*itr)->getInternalObject() == me) {
          c_entity em;
          em.e = me;
          em.init();

          log("[SERVER] Found me");
          if ( (PVOID)(em.inventory()) == pinv ) {
            log("[SERVER] Found inventory of entity: %p", (*itr)->getInternalObject());
            netEntityEquip = (*itr);
            break;
          }
        }
      }
    } else {
      log("[ERROR] NetworkSharedEntities is NULL!");
    }

    // Send off the common network id 
    if (netItemEquip && netEntityEquip) {
      NetworkMessages::ItemEquip message;
      message.set_id(netItemEquip->getCommonId());
      message.set_slot(slot);
      message.set_ownerid(netEntityEquip->getCommonId());

      Server::getSingleton().SendMessage<NetworkMessages::ItemEquip>(S_ITEM_EQUIP, &message);

      log("[SERVER] Sent ItemEquip to client:");
      log("         id: %p", message.id());
      log("         slot: %i", message.slot());
      log("         ownerid: %i", message.ownerid());
    } else {
      log("[ERROR] Could not find equip item in network item list (id = %p)", itemEquip);
    }
  }
  else if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT) {
    log("[TODO] CLIENT Wants to send ItemEquip message!");
    log("\n\n");
  }
  */
}

void TLMP::_item_unequip_pre STDARG
{
  void *pinv = e->_this;
  void *itemUnequip = (void*)Pz[0];
  index_t entity_id = -1;

  log("========= ItemUnequip (this: %p, item: %p) retVal = %i", e->_this, Pz[0], e->retval);

  // Retain equipped items for anyone that joins late
  if (EquippedItems) {
    vector<EquippedItem>::iterator itr;
    bool found = false;

    for (itr = EquippedItems->begin(); itr != EquippedItems->end(); itr++) {
      if ((*itr).item == itemUnequip) {
        found = true;
        EquippedItems->erase(itr);
        break;
      }
    }

    if (!found) {
      log("!!! ERROR Unequipped item that wasn't in the equip list!");
    }
  }

  //
  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    NetworkEntity* netItemUnequip = NULL;
    NetworkEntity* netEntityUnequip = NULL;
    bool isItemUnequip = false;

    // Search the server's item list for the same ptr
    if (NetworkSharedItems) {
      log("[SERVER] Searching for item in network list...");
      vector<NetworkEntity *>::iterator itr;
      for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
        log("[SERVER] Checking %p == %p ?", (*itr)->getInternalId(), (int)itemUnequip);
        if ((*itr)->getInternalId() == (int)itemUnequip) {
          isItemUnequip = true;
          netItemUnequip = (*itr);
          break;
        }
      }

      // Search the server's entity list for the owner id
      if (NetworkSharedEntities) {
        log("[SERVER] Searching for entity in network list...");
        for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
          log("[SERVER] Checking %p == %p ?", (*itr)->getInternalObject(), (int)pinv);

          if ((*itr)->getInternalObject() == me) {
            c_entity em;
            em.e = me;
            em.init();

            log("[SERVER] Found me");
            if ( (PVOID)(em.inventory()) == pinv ) {
              log("[SERVER] Found inventory of entity: %p", (*itr)->getInternalObject());
              netEntityUnequip = (*itr);
              break;
            }
          }
        }
      } else {
        log("[ERROR] NetworkSharedEntities is NULL!");
      }
    }

    // Send off the common network id 
    if (isItemUnequip) {
      NetworkMessages::ItemUnequip message;
      message.set_id(netItemUnequip->getCommonId());
      message.set_ownerid(netEntityUnequip->getCommonId());

      log("");
      log("[DEBUG] netEntityUnequip->getCommonId() = %p", netEntityUnequip->getCommonId());
      log("[DEBUG] netEntityUnequip->getInternalId() = %p", netEntityUnequip->getInternalId());
      log("[DEBUG] netEntityUnequip->getInternalObject() = %p", netEntityUnequip->getInternalObject());
      log("[DEBUG] me = %p", me);
      log("");

      Server::getSingleton().SendMessage<NetworkMessages::ItemUnequip>(S_ITEM_UNEQUIP, &message);

      log("[SERVER] Sent ItemUnequip to client:");
      log("         id: %p", message.id());
      log("         ownerid: %p", message.ownerid());
    } else {
      log("[ERROR] Could not find unequip item in network item list (id = %p)", itemUnequip);
    }
  }

  /* NETWORK STUFF
  NL_ITERATE(i,c_entity,net_entities) {
    if ((*i).inventory()==pinv) {
      entity_id = (*i).id;
      break;
    }
  }
  if (entity_id!=-1 && (host.is_active || (peer.is_active && entity_id==peer_id ))) {
    //if (peer.is_active) return;
    index_t*id;
    if (item_map.inuse_get(pitem,id)) {
      sendbuf.reset();
      sendbuf.pui(entity_id);
      sendbuf.pus(id_item_unequip);
      sendbuf.pui(*id);
      send();
      log("sent item_unequip for entity_id %d, item_id %d",entity_id,*id);
    }
  }
  */
}

bool TLMP::CheckNetworkList_HasExisting(vector<NetworkEntity *> *list, int commonId)
{
  vector<NetworkEntity *>::iterator itr;
  bool commonIdFound = false;

  if (!list)
    return false;

  for (itr = list->begin(); itr != list->end(); itr++) {
    if ((*itr)->getCommonId() == commonId) {
      commonIdFound = true;
      break;
    }
  }

  //
  return commonIdFound;
}
