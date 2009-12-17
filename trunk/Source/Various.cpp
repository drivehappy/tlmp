#include "Various.h"

void TLMP::_set_destination_pre STDARG
{
  log("SetDestination");

  /* NETWORK STUFF
  dst_unk = (void*)Pz[0];
	if (e->_this==test_dog) {
	//	e->calloriginal = false;
	}
	if (e->_this==me) {
		my_dst[0] = *(float*)&Pz[1];
		my_dst[1] = *(float*)&Pz[2];
		if (peer.is_active && peer_id!=-1) {
			//log("me, send set destination");
			c_entity&ne = net_entities[peer_id];
			peer.sendbuf.reset();
			peer.sendbuf.pui(-1);
			peer.sendbuf.pus(id_set_destination);
			peer.sendbuf.put<float>(my_dst[0]);
			peer.sendbuf.put<float>(my_dst[1]);
			peer.sendbuf.put<bool>(ne.attack_midair());
			peer.send();
		}
	}
	if (host.is_active) {
		index_t*id;
		if (entity_map.inuse_get(e->_this,id)) {
			//log("host, sending set_destination for id %d",*id);
			c_entity&ne = net_entities[*id];
			host.sendbuf.reset();
			host.sendbuf.pui(*id);
			host.sendbuf.pus(id_set_destination);
			host.sendbuf.put<float>(*(float*)&Pz[1]);
			host.sendbuf.put<float>(*(float*)&Pz[2]);
			peer.sendbuf.put<bool>(ne.attack_midair());
			host.send_to_all();
		}
	}
  */
}

void TLMP::_on_strike_pre STDARG
{
	// (void*_this,void*world,void*target,void*,unsigned int flags,float one,float one2,int seven);
	log(" %p :: strike %p, %p, %p, %x, %f, %f, %d",e->_this,Pz[0],Pz[1],Pz[2],Pz[3],*(float*)&Pz[4],*(float*)&Pz[5],Pz[6]);
  /* NETWORK STUFF
	if (host.is_active) {
		index_t*id;
		if (entity_map.inuse_get(e->_this,id)) {
			index_t*id2;
			if (entity_map.inuse_get((void*)Pz[1],id2)) {
				log("host, sending strike for %d, %d",*id,*id2);
				host.sendbuf.reset(); 
				host.sendbuf.pui(*id);
				host.sendbuf.pus(id_strike);
				host.sendbuf.pui(*id2);
				host.sendbuf.pui(Pz[3]); // flags
				host.sendbuf.pui(Pz[4]); // float
				host.sendbuf.pui(Pz[5]); // float
				host.sendbuf.pui(Pz[6]); // seven
				host.sendbuf.put<unsigned long long>(*random_seed);
				host.sendbuf.put<unsigned long long>(*random_seed2);
				log("*random_seed is %016I64X",*random_seed);
				host.send_to_all();
				allow_strike = true;
			} else log("host, strike unknown entity :(");
		}
	} else if (peer.is_active) {
		if (e->_this==me && peer_id!=-1) {
			index_t*id2;
			if (entity_map.inuse_get((void*)Pz[1],id2)) {
				peer.sendbuf.reset(); 
				peer.sendbuf.pui(-1);
				peer.sendbuf.pus(id_strike);
				peer.sendbuf.pui(*id2);
				peer.sendbuf.pui(Pz[3]); // flags
				peer.sendbuf.pui(Pz[4]); // float
				peer.sendbuf.pui(Pz[5]); // float
				peer.sendbuf.pui(Pz[6]); // seven
				peer.sendbuf.put<unsigned long long>(*random_seed);
				peer.sendbuf.put<unsigned long long>(*random_seed2);
				log("*random_seed is %016I64X",*random_seed);
				peer.send();
			}
		} else if (!allow_strike) {
			e->calloriginal = false;
			e->retval = 0;
		}
	}
  */
}

void TLMP::_on_strike_post STDARG
{
  /* NETWORK STUFF
	if (host.is_active) {
		allow_strike = false;
	}
  */
}

void TLMP::_random_post STDARG
{
  /* NETWORK STUFF
	if (allow_strike) {
		log("random -> %08X",e->retval);
	}
  */
}

void TLMP::_destroy_pre STDARG
{
	log("Destroy unit: %p", e->_this);

// 	void*p = (void*)Pz[0];
// 	index_t*id;
// 	if (entity_map.inuse_get(p,id)) {
// 		log("net entity %d destroyed",*id);
// 		entity_map.remove(p);
// 		net_entities.remove(*id);
// 	}
}

