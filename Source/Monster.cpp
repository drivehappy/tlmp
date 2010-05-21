#include "Monster.h"

void TLMP::_spider_some_create_pre STDARG
{
  u64 guid = *(u64 *)&Pz[0];
  log("!!!!! %p spider_some_create (%p %016I64X %d %d)", e->retaddress, e->_this, guid, Pz[2], Pz[3]);
  EntityManager = e->_this;

  //log("\n\n\n\n\nCResourceManager alloc addr: %p %i\n\n\n\n\n", 
  //  MemoryManager::getSingleton().getMemoryAllocationAddress(EntityManager),
  //  MemoryManager::getSingleton().getMemorySize(EntityManager)
  //  );

  PVOID pCResourceManager = e->_this;

  //* TESTING - MOVED TO TEST0_PRE (Parent function call)
  if (NetworkState::getSingleton().GetState() == CLIENT && !ClientAllowSpawn) {
    log("[CLIENT] Stopping spawn");
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

  //log("\n\n\n\n\n(retval = %p) CMonster alloc addr: %p %i\n\n\n\n\n", 
  //  (PVOID)e->retval,
  //  MemoryManager::getSingleton().getMemoryAllocationAddress((PVOID)e->retval),
  //  MemoryManager::getSingleton().getMemorySize((PVOID)e->retval)
  //  );
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
