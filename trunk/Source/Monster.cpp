#include "Monster.h"

void TLMP::_spider_some_create_pre STDARG
{
  log("!!!!! %p -- %p :: spider_some_create %08X %08X %d %d", e->retaddress, e->_this, Pz[0], Pz[1], Pz[2], Pz[3]);
  EntityManager = e->_this;

  u64 guid = *(u64 *)&Pz[0];

  /* NETWORK STUFF
  if (peer.is_active && !allow_spawn) {
//     if (guid==0xD3A8F9832FA111DE) {
//       log("stop da dog from spawning <.<");
//       e->calloriginal = false;
//       e->retval = 0;
//     }

    log("peer, stopping spawn");
    e->calloriginal = false;
    e->retval = 0;
  }
  */

  //* TESTING - MOVED TO TEST0_PRE (Parent function call)
  if (NetworkState::getSingleton().GetState() == CLIENT && !ClientAllowSpawn) {
    log("[CLIENT] Stopping spawn");
    e->calloriginal = false;
    e->retval = 0;
  } else if (NetworkState::getSingleton().GetState() == SERVER && !ServerAllowSpawn) {
    log("[SERVER] Suppressing spawn");
    e->calloriginal = false;
    e->retval = 0;
  }
}

void TLMP::_spider_some_create_post STDARG
{
  u64 guid      = *(u64 *)&Pz[0];
  u32 level     = Pz[2];
  bool noItems  = *(bool*)&Pz[3];

  log("!!!!!!! spider_some_create_post guid: %016I64X (level = %i)", guid, level);

  /* NETWORK STUFF
  if (host.is_active && e->retval && !no_netspawn) {
    c_entity o;
    o.e = (void*)e->retval;
    o.guid = guid;
    o.level = Pz[2];
    o.noitems = *(bool*)&Pz[3];
    o.init();
    host_spawn_entity(o);
    log("host, spawn %016I64X",guid);
  }
  */

  if (NetworkState::getSingleton().GetState() == SERVER && ServerAllowSpawn) {
    c_entity o;
    o.e = (void*)e->retval;
    o.guid = guid;
    o.level = level;
    o.noitems = noItems;
    o.init();

    NetworkMessages::Entity message;
    message.set_level(level);
    message.set_guid(guid);
    message.set_noitems(noItems);

    /*
    Server::getSingleton().SendMessage<NetworkMessages::Entity>(S_SPAWN_MONSTER, &message);

    log("[SERVER] Sent Monster Spawn to Client");
    */
    log("[SERVER] Todo Send Monster Spawn to Client");
  }
}

void TLMP::_spider_process_ai_pre STDARG
{
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    //e->calloriginal = false;
		//e->retval = 0;
  }

  /* NETWORK STUFF
  if (peer.is_active) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (host.is_active) {
    if (get_session_id_by_entity(e->_this)!=-1) {
      e->calloriginal = false;
      e->retval = 0;
    }
  }
  */
}

void TLMP::_spider_process_ai2_pre STDARG
{
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    e->calloriginal = false;
		e->retval = 0;
  }

  /* NETWORK STUFF
  if (peer.is_active) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (host.is_active) {
    if (get_session_id_by_entity(e->_this)!=-1) {
      e->calloriginal = false;
      e->retval = 0;
    }
  }
  */
}

void TLMP::_spider_process_ai3_pre STDARG
{
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    //e->calloriginal = false;
		//e->retval = 0;
  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    //e->calloriginal = false;
		//e->retval = 0;
  }
}

void TLMP::_spider_process_ai3_post STDARG
{
}

void TLMP::_spider_idle_pre STDARG
{
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    e->calloriginal = false;
		e->retval = 0;
  }

  /* NETWORK STUFF
  if (peer.is_active) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (host.is_active) {
    if (get_session_id_by_entity(e->_this)!=-1) {
      e->calloriginal = false;
      e->retval = 0;
    }
  }
  */
}

void TLMP::_spider_on_hit_pre STDARG
{
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    //e->calloriginal = false;
    //e->retval = 0;
  }

  /* NETWORK STUFF
  if (peer.is_active) {
    e->calloriginal = false;
    e->retval = 0;
  } else if (host.is_active) {
    if (get_session_id_by_entity(e->_this)!=-1) {
      e->calloriginal = false;
      e->retval = 0;
    }
  }
  */
}

void TLMP::_set_alignment_pre STDARG
{
  //log("Setting Alignment: %p %i", e->_this, Pz[0]);

  /* NETWORK STUFF
  if (host.is_active) {
    index_t*id;
    if (entity_map.inuse_get(e->_this,id)) {
      log("host, sending set_alignment for id %d",*id);
      char*p = (char*)e->_this;
      host.sendbuf.reset();
      host.sendbuf.pui(*id);
      host.sendbuf.pus(id_set_alignment);
      host.sendbuf.pui(Pz[0]);
      host.send_to_all();
    }
  }
  */
}
