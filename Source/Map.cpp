#include "Map.h"

PVOID TLMP::load_map_this = NULL;
PVOID TLMP::load_area_this = NULL;

void TLMP::_load_map_pre STDARG
{
  log("LoadMap (pre) this = %p, unk0 = %i", e->_this, Pz[0]);

  load_map_this = e->_this;

  if (!NetworkSharedEntities)
    NetworkSharedEntities = new vector<NetworkEntity*>();
}

void TLMP::_load_map_post STDARG
{
  log("LoadMap (post)");

  // MOVE ME TO A BETTER FUNCTION
  static bool UISetup = false;
  if (!UISetup) {
    log("Creating UI...");
    TLMP::UI::init();
    TLMP::UI::createWindow();

    UISetup = true;
  }

  //
  // Testing character spawn code location
  //
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    unsigned long long guid = *((unsigned long long*)me + (0x168 / 8));
    unsigned int level = *((unsigned int*)me + 0x3c);
    log("~~~ guid = %016I64X, level = %i", guid, level);

    /*
    // Force this, not sure why it's not getting set in _initialize_player_post
    if (!NetworkSharedEntities)
      NetworkSharedEntities = new vector<NetworkEntity*>();
    NetworkEntity* entity = new NetworkEntity(me, CLIENT_COMMON_BASE_ID);
    NetworkSharedEntities->push_back(entity);
    */

    NetworkEntity* entity = NetworkSharedEntities->front();

    //
    ClientAllowSpawn = false;

    log("[CLIENT] Sending join...");
    c_entity t;
    t.e = me;
    t.init();
    t.level = level;
    t.guid = guid;

    NetworkMessages::Entity netEntity;

    netEntity.set_id(entity->getCommonId());
    netEntity.set_guid(guid);
    netEntity.set_level(level);
    netEntity.set_noitems(true);

    NetworkMessages::Position *position = netEntity.add_position();

    position->set_x(t.GetPosition()->x);
    position->set_y(t.GetPosition()->y);
    position->set_z(t.GetPosition()->z);

    Client::getSingleton().SendMessage<NetworkMessages::Entity>(C_GAME_JOIN, &netEntity);

  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    log("[SERVER] Sending spawn entity... TODO");
  }
}

void TLMP::_change_level_pre STDARG
{
  log(" %p :: load_area",e->_this);
  load_area_this = e->_this;
  wstring&s1 = *(wstring*)&Pz[0];
  int n1 = Pz[7];
  int n2 = Pz[8];
  int n3 = Pz[9];
  wstring&s2 = *(wstring*)&Pz[10];
  int n4 = Pz[17];

  log("ChangeLevel pre '%ls', %d, %d, %d, '%ls', %d",s1.c_str(),n1,n2,n3,s2.c_str(),n4);

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

  
  if (NetworkState::getSingleton().GetState() == CLIENT ||
      NetworkState::getSingleton().GetState() == SERVER)
  {
    log("Suppressing Load_Area");
    e->calloriginal = false;
    e->retval = NULL;
  }
  
}

void TLMP::_change_level_post STDARG
{
  log("ChangeLevel post");
}

void TLMP::_on_load_area_pre STDARG
{
  log("~~~ _on_load_area_pre");

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

void TLMP::_on_load_area_post STDARG
{
  unsigned long long guid = *((unsigned long long*)me + (0x168 / 8));
  unsigned int level = *((unsigned int*)me + 0x3c);

  log("~~~ _on_load_area_post: guid = %016I64X, level = %i", guid, level);

  /*
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    ClientAllowSpawn = false;

    log("[CLIENT] Sending join...");
    c_entity t;
    t.e = me;
    t.init();
    t.level = level;
    t.guid = guid;

    NetworkMessages::Entity entity;
    entity.set_guid(guid);
    entity.set_level(level);
    Client::getSingleton().SendMessage<NetworkMessages::Entity>(C_GAME_JOIN, &entity);

  } else if (NetworkState::getSingleton().GetState() == SERVER) {
    log("[SERVER] Sending spawn entity... TODO");


  }
  */

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
//     c_entity e;
//     e.e = me;
//     e.guid = 0x8D3EE5363F7611DE; // todo: read real info
//     e.level = 1;
//     e.noitems = true;
//     e.init();
//     index_t id = host_spawn_entity(e);
//     set_alignment(me,1);
//     sendbuf.reset();
//     sendbuf.pui(id);
//     sendbuf.pus(id_entity_initialize);
//     sendbuf.put<XZY>(e.get_pos());
//     send();
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
//     {
//       log("host, sending equip stuff!");
//       c_entity&e = net_entities[id];
//     }
  }
  is_loading = false;
  */
}

void TLMP::ChangeGameStateToMainMenu()
{
  LoadMap(load_map_this, 0);
}

void TLMP::ChangeGameStateToInGame()
{
  LoadMap(load_map_this, 2);
}

void TLMP::LoadAreaTown()
{
  // Crashes on first try, if a level change is done beforehand then this will reload the current level
  // -1 was the previous level player was at (not the level spatially above)
  //LoadArea(load_area_this, L"", 0, 0, 0, L"", 0);
}
