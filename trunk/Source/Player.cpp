#include "Player.h"

PVOID TLMP::me = 0;

void TLMP::_player_ctor_post STDARG
{
	me = (void*)e->retval;
}

void TLMP::_player_set_action_pre STDARG
{
	log("set action %d",Pz[0]);

  /* NETWORK STUFF
	if (e->_this==me) {
		if (peer.is_active) {
			log("me, send set action (%d)",Pz[0]);
			peer.sendbuf.reset();
			peer.sendbuf.pui(0);
			peer.sendbuf.pus(id_set_action);
			peer.sendbuf.pui(Pz[0]);
			peer.send();
		}
	}
	if (host.is_active) {
		index_t*id;
		if (entity_map.inuse_get(e->_this,id)) {
			char*p = (char*)e->_this;
			host.sendbuf.reset();
			host.sendbuf.pui(*id);
			host.sendbuf.pus(id_set_action);
			host.sendbuf.pui(Pz[0]);
			host.send_to_all();
		}
	}
  */
}

void TLMP::_add_goldtoplayer STDARG
{
  log("Added gold to player: %p (Amt: %i)", e->_this, Pz[0]);
}

void TLMP::_initialize_player_post STDARG
{
	unsigned long long guid = *((unsigned long long*)me + (0x168 / 8));
	unsigned int level = *((unsigned int*)me + 0x3c);

	log("~~~ Player initialized: guid = %016I64X", guid);

  /* NETWORK STUFF
	// Level isn't init'd by now, where is it?
	if (host.is_active) {
		c_entity e;
		e.e = me;
		e.guid = guid;
		e.level = level;
		e.noitems = true;
		e.init();
		peer_id = host_spawn_entity(e);
	}
  */
}


void TLMP::_player_died_pre STDARG
{
	log("Player died: %p", e->_this);
}

void TLMP::_player_resurrect_pre STDARG
{
	log("Player resurrected: %p %#x %#x %#x %#x %#x %#x %#x %#x", e->_this, Pz[0], Pz[1], Pz[2], Pz[3], Pz[4], Pz[5], Pz[6], Pz[7]);
	// 11f93b80 0xe 0 0x1a1accb0 0x11f93b80 0x74056083 0x1 0xa 0xf
}

void TLMP::_levelup_pre STDARG {
	log("Player leveled up! (%p)", e->_this);

  /* NETWORK STUFF
	index_t*id = 0;
	index_t minus_one = -1;
	if (entity_map.inuse_get(e->_this,id)) {
		sendbuf.reset();
		sendbuf.pui(*id);
		sendbuf.pus(id_levelup);
		send();
	}
  */
}

void TLMP::_levelup_silent_pre STDARG
{
	log("Playere leveled up silent! (%p)", e->_this);

}

void TLMP::_add_minion_pre STDARG
{
  log("Add Minion");

  /* NETWORK STUFF
	if (host.is_active) {
		index_t*id;
		if (entity_map.inuse_get(e->_this,id)) {
			index_t*id2;
			if (entity_map.inuse_get((void*)Pz[0],id2)) {
				log("host, sending add_minion for id %d, %d",*id,*id2);
				char*p = (char*)e->_this;
				host.sendbuf.reset();
				host.sendbuf.pui(*id);
				host.sendbuf.pus(id_add_minion);
				host.sendbuf.pui(*id2);
				host.send_to_all();
			} else log("host, added unknown minion :(");
		}
	}
  */
}
