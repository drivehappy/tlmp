#include "Map.h"

void TLMP::_load_map_pre STDARG {
	log(" %p :: load_map_pre",e->_this);
}

void TLMP::_load_map_post STDARG {
	log("load_map_post");

  // MOVE ME TO A BETTER FUNCTION
	static bool UISetup = false;
	if (!UISetup) {
		log("Creating UI...");
    TLMP::UI::init();
		TLMP::UI::createWindow();

		UISetup = true;
	}
}

void TLMP::_load_area_pre STDARG {
	log(" %p :: load_area",e->_this);
	load_area_this = e->_this;
	wstring&s1 = *(wstring*)&Pz[0];
	int n1 = Pz[7];
	int n2 = Pz[8];
	int n3 = Pz[9];
	wstring&s2 = *(wstring*)&Pz[10];
	int n4 = Pz[17];

	log("load_area '%ls', %d, %d, %d, '%ls', %d",s1.c_str(),n1,n2,n3,s2.c_str(),n4);

  /* NETWORK STUFF
	// Clear out any objects we've made in our list, new ones will be initiated later
	// This _should_ sync interactable objects properly
	net_objects.clear();
	object_map.clear();
	// --

	if (host.is_active) {
		sendbuf.reset();
		sendbuf.pui(-1);
		sendbuf.pus(id_load_area);
		sendbuf.putwstring(s1.c_str());
		sendbuf.pui(n1);
		sendbuf.pui(n2);
		sendbuf.pui(n3);
		sendbuf.putwstring(s2.c_str());
		sendbuf.pui(n4);
		sendbuf.put<unsigned long long>(*random_seed);
		sendbuf.put<unsigned long long>(*random_seed2);
		send();
		log("host, sent load map");
	} else if (peer.is_active && !allow_load) {
		log("stopping peer load :o");
		e->calloriginal = false;
		e->retval = 0;
	}
  *random_seed = 0;
	*random_seed2 = 0;
  */
}

void TLMP::_on_load_area_pre STDARG {
  /* NETWORK STUFF
 	*random_seed = 0;
	*random_seed2 = 0;
	NL_ITERATE(i,c_entity,net_entities) {
		c_entity&en = *i;
		if (en.e == me) continue;
		c_inventory*inv = en.inventory();
		if (!inv) continue;
		log("inv->items.size() is %d",inv->items.size());
		stack<void*> t;
		for (size_t i=0;i<inv->items.size();i++) {
			c_inventory::c_slot&s = *inv->items[i];
			log("_on_load_area_pre, unequipping %p from slot %d",s.item,s.slot);
			t.push(s.item);
		}
		while (!t.empty()) {
			item_unequip(inv,t.top());
			t.pop();
		}
	}
  is_loading = true;
  */
}

void TLMP::_on_load_area_post STDARG {
	//unsigned long long guid = *((unsigned long long*)me + (0x168 / 8));
	//unsigned int level = *((unsigned int*)me + 0x3c);

	//log("~~~ _on_load_area_post: guid = %016I64X", guid);

  /* NETWORK STUFF
	if (peer.is_active) {
		allow_load = false;

		log("peer, sending join");
		c_entity t;
		t.e = me;
		t.init();
		t.level = level;
		t.guid = guid;

		sendbuf.reset();
		sendbuf.pui(-1);
		sendbuf.pus(id_join);
		sendbuf.put<unsigned long long>(t.guid);
		sendbuf.pui(t.level);
		sendbuf.put<XZY>(t.get_pos());
		send();
	} else if (host.is_active) {
		log("host, sending spawn entity for me");
// 		c_entity e;
// 		e.e = me;
// 		e.guid = 0x8D3EE5363F7611DE; // todo: read real info
// 		e.level = 1;
// 		e.noitems = true;
// 		e.init();
// 		index_t id = host_spawn_entity(e);
// 		set_alignment(me,1);
// 		sendbuf.reset();
// 		sendbuf.pui(id);
// 		sendbuf.pus(id_entity_initialize);
// 		sendbuf.put<XZY>(e.get_pos());
// 		send();
		if (peer_id!=-1) {
			log("host, sending spawn meee");
			c_entity&e = net_entities[peer_id];
			mk_spawn_entity(sendbuf,e);
			send();
			set_alignment(me,1);
			sendbuf.reset();
			sendbuf.pui(peer_id);
			sendbuf.pus(id_entity_initialize);
			sendbuf.put<XZY>(e.get_pos());
			send();

			c_inventory*inv = e.inventory();
			log("inv->items.size() is %d",inv->items.size());
			for (size_t i=0;i<inv->items.size();i++) {
				c_inventory::c_slot&s = *inv->items[i];
				log("%p (%ls) in slot %d",s.item,entity_offset<wchar_t*>(s.item,0x1b4),s.slot);
				index_t*id2;
				if (item_map.inuse_get(s.item,id2)) {
					sendbuf.reset();
					sendbuf.pui(e.id);
					sendbuf.pus(id_item_equip);
					sendbuf.pui(*id2);
					sendbuf.pui(s.slot);
					sendbuf.pui(1);
					send();
				}
			}
		}
// 		{
// 			log("host, sending equip stuff!");
// 			c_entity&e = net_entities[id];
// 		}
	}
  is_loading = false;
  */
}
