#include "Item.h"
using namespace TLMP;

PVOID                    TLMP::drop_item_this = NULL;
PVOID                    TLMP::ItemManager = NULL;
bool                     TLMP::allowItemSpawn = true;
vector<NetworkEntity *> *TLMP::NetworkSharedItems    = NULL;

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

  /* NETWORK STUFF
  if (peer.is_active && !allow_spawn) {
    log("%p :: peer, stopping item spawn",e->_this);

    // Add the potential item to our list, we tell the host to spawn this 
    // for us if it's equipped on us in _item_equip
    peer_items_created.add(guid);
    log2("peer_items_created added buffer GUID: %016I64X", guid);

    //e->calloriginal = false;
    //e->retval = 0;
  } else if (host.is_active) {
    log2("Host _item_create_pre GUID: %016I64X", guid);
  }
  */

  log("Allow spawn: %i", ClientAllowSpawn);

  //
  if (Network::NetworkState::getSingleton().GetState() == Network::CLIENT && !ClientAllowSpawn) {
    log("[CLIENT] _item_create_pre stopping item spawn (%p)", e->_this);

    e->calloriginal = false;
    e->retval = false;
  } else if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    log("[HOST] _item_create_pre GUID: %016I64X", guid);
  }
}

void TLMP::_item_create_post STDARG
{
  u64 guid = *(u64 *)&Pz[0];
  UnitManager = e->_this;

  log("%p :: Item created: %p, GUID: %016I64X", e->retval, e->_this, guid);

  /* NETWORK STUFF
  if (host.is_active && e->retval && !no_netspawn) {
    CItem o;
    o.e = (void*)e->retval;
    o.guid = guid;
    o.level = Pz[2];
    o.unk0 = Pz[3];
    o.unk1 = Pz[4];

    o.id = net_items.add(o);
    net_items[o.id].id = o.id;
    item_map[o.e] = o.id;
    
    log("host, create item %016I64X (%p)", guid, o.e);

    host.sendbuf.reset();
    host.sendbuf.pui(-1);
    host.sendbuf.pus(id_item_create);
    host.sendbuf.pui(o.id);
    host.sendbuf.put<unsigned long long>(o.guid);
    host.sendbuf.pui(o.level);
    host.sendbuf.pui(o.unk0);
    host.sendbuf.pui(o.unk1);
    host.send_to_all();
  }
  */

  CItem* newItem = new CItem();
  newItem->e = (PVOID)e->retval;
  newItem->guid = guid;
  newItem->level = Pz[2];
  newItem->unk0 = Pz[3];
  newItem->unk1 = Pz[4];

  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    NetworkEntity *networkItem = new NetworkEntity((PVOID)e->retval);

    if (!NetworkSharedItems)
      NetworkSharedItems = new vector<NetworkEntity *>();
    NetworkSharedItems->push_back(networkItem);

    log("[HOST] Item created GUID: %016I64X (%p)", newItem->guid, newItem->e);

    NetworkMessages::Item message;
    message.set_id(networkItem->getCommonId());
    message.set_guid(newItem->guid);
    message.set_level(newItem->level);
    message.set_unk0(newItem->unk0);
    message.set_unk1(newItem->unk1);

    Network::Server::getSingleton().SendMessage<NetworkMessages::Item>(Network::S_ITEM_CREATE, &message);
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

  /* OLD NETWORK STUFF
  if (no_netsend) return;
  index_t*id;
  if (host.is_active) {
    if (item_map.inuse_get((void*)Pz[0],id)) {
      CItem&i = net_items[*id];

      sendbuf.reset();
      sendbuf.pui(-1);
      sendbuf.pus(id_item_drop);
      sendbuf.pui(*id);
      sendbuf.put<XZY>(*(XZY*)Pz[1]);
      sendbuf.put<bool>(*(bool*)&Pz[2]);
      send();
    } else {
      log("Couldn't find item: %p", Pz[0]);
    }
  }
  */

  if (Network::NetworkState::getSingleton().GetState() == Network::SERVER) {
    NetworkEntity* netItemDropped = NULL;
    bool isItemCreated = false;

    // Search the server's item list for the same ptr
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
}

void TLMP::_item_pick_up_pre STDARG
{
  //log(" %p :: pick up item %p",e->_this,Pz[0]);
}

void TLMP::_item_pick_up_post STDARG
{
  //log("pick up, retval is %d",e->retval);
  void *pitem = (void*)Pz[0];

  log("==== ItemPickup (this: %p, %p, %p) retVal = %i", e->_this, Pz[0], Pz[1], e->retval);

  c_entity ne;
	ne.e = e->_this;
	ne.init();
	c_inventory *inv = ne.inventory();
  log("---- inv = %p", inv);

  if (inv) {
    int slot = inv->get_item_slot(pitem);
    log("---- Slot = %i", slot);
  }

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

  if (!e->retval) {
    return;
  }

  void *pinv = e->_this;
  void *pitem = (void*)Pz[0];
  int slot = Pz[1];
  index_t entity_id = -1;

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
      sendbuf.pus(id_item_equip);
      sendbuf.pui(*id);
      sendbuf.pui(slot);
      send();
      log("sent item_equip for entity_id %d, item_id %d, slot %d",entity_id,*id,slot);
    }
  }
  */
}

void TLMP::_item_unequip_pre STDARG
{
  void *pinv = e->_this;
  void *pitem = (void*)Pz[0];
  index_t entity_id = -1;

  log("========= ItemUnequip (this: %p, item: %p) retVal = %i", e->_this, Pz[0], e->retval);

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
