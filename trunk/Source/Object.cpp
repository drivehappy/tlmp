#include "Object.h"

void TLMP::_interact_with_object STDARG {
  void* object = e->_this;
  void* player = (void*)Pz[0];

  //if (!no_netspawn) {
  {
    log("Player (%p) interacting with object (%p)", player, object);
    //index_t *idPlayer, *idObject;
    bool objectFound = false;
    char itemMap = 0;

    /* NETWORK STUFF
    // Find the object
    // Check object map first
    objectFound = object_map.inuse_get(object, idObject);
    if (!objectFound) {
      objectFound = item_map.inuse_get(object, idObject);
      itemMap = 1;
    }
    
    if (!objectFound) {
      log("Couldn't find object in either object_map or item_map");
    } else {
    
      // Find the player, if Player ptr is null - probably a complex object interaction with logic
      if (!player) {
        if (host.is_active) {
          sendbuf.reset();
          sendbuf.pui(-1);
          sendbuf.pus(id_object_interaction);
          sendbuf.puc(itemMap);
          sendbuf.pui(*idObject);
          send();
        } else if (peer.is_active) {
          peer.sendbuf.reset();
          peer.sendbuf.pui(-1);
          peer.sendbuf.pus(id_object_interaction);
          peer.sendbuf.puc(itemMap);
          peer.sendbuf.pui(*idObject);
          peer.send();
        }

      } else if (entity_map.inuse_get(player, idPlayer)) {
        if (host.is_active) {
          sendbuf.reset();
          sendbuf.pui(*idPlayer);
          sendbuf.pus(id_object_interaction);
          sendbuf.puc(itemMap);
          sendbuf.pui(*idObject);
          send();
        } else if (peer.is_active) {
          peer.sendbuf.reset();
          peer.sendbuf.pui(-1);
          peer.sendbuf.pus(id_object_interaction);
          peer.sendbuf.puc(itemMap);
          peer.sendbuf.pui(*idObject);
          peer.send();
        }
      } else {
        log("Couldn't find player id");
      }

      log2("Interaction Success: Found IDs:");
      log2("(id = %#x) %p", *idPlayer, player);
      log2("(id = %#x) %p", *idObject, object);
    }
    */
  }
}

void TLMP::_object_create_pre STDARG {
  log("\n");
  log("Object pre-create: (this=%p, unk0=%#x) (GUID = %016I64X)", e->_this, Pz[0], ((CItemOld*)Pz[0])->guid);
  log("       Unk0: %#x, Unk1: %016I64X", ((CItemOld*)Pz[0])->unk0, ((CItemOld*)Pz[0])->unk1);

  /*
  // Suppress object creation, get this from the host
  if (peer.is_active && !allow_spawn) {
    log("~~~ Object suppressed on Peer");
    e->calloriginal = false;
    e->retval = 0;
  }
  */
}

void TLMP::_object_create_post STDARG {
  if (e->retval) {
    CItemOld o;
    o.e = (PVOID)e->retval;
    o.guid = ((CItemOld*)e->retval)->guid;

    /* NETWORK STUFF
    o.id = net_objects.add(o);
    net_objects[o.id].id = o.id;
    object_map[o.e] = o.id;

    log("Object post-create: (objPtr=%p, id=%#x) (GUID = %016I64X)", o.e, o.id, ((CItem*)e->retval)->guid);
    */

    /*
    // Host handles object creation, send the guid off to the peer
    if (host.is_active) {
      sendbuf.reset();
      sendbuf.pui(-1);
      sendbuf.pus(id_object_create);
      sendbuf.put<unsigned long long>(o.guid);
      send();
    }
    */
  }
}
