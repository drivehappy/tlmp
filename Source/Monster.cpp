#include "Monster.h"

void TLMP::_spider_some_create_pre STDARG
{
	log("%p -- %p :: spider_some_create %08X %08X %d %d",e->retaddress,e->_this,Pz[0],Pz[1],Pz[2],Pz[3]);
	EntityManager = e->_this;

	unsigned long long guid = *(unsigned long long*)&Pz[0];
  /* NETWORK STUFF
	if (peer.is_active && !allow_spawn) {
// 		if (guid==0xD3A8F9832FA111DE) {
// 			log("stop da dog from spawning <.<");
// 			e->calloriginal = false;
// 			e->retval = 0;
// 		}

		log("peer, stopping spawn");
		e->calloriginal = false;
		e->retval = 0;
	}
  */
}

void TLMP::_spider_some_create_post STDARG
{
	unsigned long long guid = *(unsigned long long*)&Pz[0];
	log("spider_some_create_post guid: %016IX64", guid);

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

	//if (Pz[1]==0x5C5BBC74) {
	//	cat = (void*)e->retval;
	//}
}

void TLMP::_spider_process_ai_pre STDARG {
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

void TLMP::_spider_process_ai2_pre STDARG {
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

void TLMP::_spider_idle_pre STDARG {
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

void TLMP::_spider_on_hit_pre STDARG {
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
