#include "Various.h"
#include "Map.h"

void TLMP::_set_destination_pre STDARG
{
  float dest[2] = {*(float *)&Pz[1], *(float *)&Pz[2]};

  //log("Set Destination: %f %f of %p", dest[0], dest[1], e->_this);

  // Only send my player movement to the server
  if (NetworkState::getSingleton().GetState() == CLIENT) {
    if (e->_this == me) {
      NetworkMessages::Position message;
      message.set_x(dest[0]);
      message.set_y(0);       // Ignored
      message.set_z(dest[1]);

      Client::getSingleton().SendMessage<NetworkMessages::Position>(C_PLAYER_SETDEST, &message);
    }
  }

  // Hosting, send any movement to the client
  else if (NetworkState::getSingleton().GetState() == SERVER) {
    // But for now just send the player movement
    if (e->_this == me) {
      NetworkMessages::Position message;
      message.set_x(dest[0]);
      message.set_y(0);       // Ignored
      message.set_z(dest[1]);

      Server::getSingleton().SendMessage<NetworkMessages::Position>(S_ENTITY_SETDEST, &message);
    }
  }
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

//   void*p = (void*)Pz[0];
//   index_t*id;
//   if (entity_map.inuse_get(p,id)) {
//     log("net entity %d destroyed",*id);
//     entity_map.remove(p);
//     net_entities.remove(*id);
//   }
}

void TLMP::_ogre_is_active STDARG
{
  e->calloriginal = false;
  e->retval = 1;
}

void TLMP::_process_objects_pre STDARG
{
  //log("Process Objects pre");

  if (NetworkState::getSingleton().GetState() == SERVER) {
    Network::Server::getSingleton().ReceiveMessages();
  } else if (NetworkState::getSingleton().GetState() == CLIENT) {
    Network::Client::getSingleton().ReceiveMessages();
  }

  // Testing structure out
  CGameClient *gameClient = (CGameClient*)e->_this;
  CPlayer *player = gameClient->pCPlayer;
  CLevel *level = gameClient->pCLevel;
  CLayout *layout = NULL;
  CMonster *pet = gameClient->pPet;

  if (level->CLayoutsList.size) {
    layout = level->CLayoutsList[0];
  }

  logColor(B_RED, "PROCESSOBJECTS: CGameClient = %p", gameClient);
  logColor(B_RED, "     CPlayer = %p", player);
  logColor(B_RED, "         GUID = %016I64X", player->GUIDUnk);
  logColor(B_RED, "     Pet = %p", pet);
  if (pet)
    logColor(B_RED, "         GUID = %016I64X", pet->GUIDUnk);
  logColor(B_RED, "     CLevel = %p", level);
  logColor(B_RED, "         Layout List: %p", &level->CLayoutsList);
  logColor(B_RED, "         Layout Count: %i", level->CLayoutsList.size);
  if (layout) {
    logColor(B_RED, "         Layout GUID0: %016I64X", layout->guid0);
    logColor(B_RED, "         Layout GUID1: %016I64X", layout->guid1);
    logColor(B_RED, "         Layout GUID2: %016I64X", layout->guid2);
    logColor(B_RED, "         Layout GUID3: %016I64X", layout->guid3);
  }
}

void TLMP::_process_objects_post STDARG
{
  //log("Process Objects post");
}

void TLMP::_wnd_proc_pre STDARG
{
  UINT msg = Pz[2];
  WPARAM wParam = Pz[3];

  if (msg == 0x100 || msg == 0x101 || msg == 0x102) {
    switch (msg) {
    case WM_KEYDOWN:
      break;
    case WM_KEYUP:
      switch (wParam)
      {
      case '1':
        {
          MemoryManager::getSingleton().reportMemory();
        }
        break;

      case '0':
        {
          c_entity em;

          em.e = me;
          em.init();

          int commonID = 4;

          Vector3 *itemPosition = new Vector3();
          itemPosition->x = em.GetPosition()->x;
          itemPosition->y = em.GetPosition()->y;
          itemPosition->z = em.GetPosition()->z;

          log("Forcing item drop commonID = %i", commonID);

          if (UnitManager) {
            PVOID item = NULL;
            vector<NetworkEntity *>::iterator itr;

            if (NetworkSharedItems) {
              // Ensure that the item has been created before we attempt to drop it
              for (itr = NetworkSharedItems->begin(); itr != NetworkSharedItems->end(); itr++) {
                if ((*itr)->getCommonId() == commonID) {
                  item = (*itr)->getInternalObject();
                  log("[CLIENT] Found item to drop (commonId = %i): %p", commonID, item);

                  // Drop the item
                  // SUPPRESSED for now, it's not quite working right and causes crash
                  if (drop_item_this) {
                    c_entity ne;
	                  ne.e = me;
	                  ne.init();
	                  c_inventory *inv = ne.inventory();

                    ItemUnequip(inv, item);
                    ItemDrop(drop_item_this, item, *itemPosition, 1);
                  } else {
                    log("[ERROR] drop_item_this is null (drivehappy - I think this is the world ptr, but we need a nice loc to set it up)");
                  }

                  break;
                }
              }
            } else {
              log("[ERROR] NetworkSharedItems is null.");
            }
          } else {
            log("[ERROR] Could not drop item: UnitManager is null!");
          }
        }
        break;

      case 'L':
        {
          LevelUp(me);
        }
        break;

      case 'T':
        {
          c_entity em;
          if (me) {
            em.e = me;
            em.init();
          }

          log("Creating Troll... EntityManager = %p", EntityManager);
          void *r = CreateUnitByName(EntityManager, L"MONSTERS", L"Troll", 1, 0);
          log("Troll Created: %p", r);

          Vector3* position = em.GetPosition();
          r = EntityInitialize(*(void**)(((char*)EntityManager)+0x0c), r, position, 0);
          log("Troll Created: %p (at: %f %f %f --- %p)", r, position->x, position->y, position->z, position);
        }
        break;

      case 'R':
        {
          c_entity em;
	        if (me) {
		        em.e = me;
		        em.init();
	        }

          SpawnPlayer(0xD3A8F9982FA111DE, 13, *em.GetPosition());
        }
        break;

      case 'F':
        {
          log("[DEBUG] Retrieving Player Destination");

          c_entity em;
          if (me) {
            em.e = me;
            em.init();
          }

          Vector3* destination = (Vector3*)GetDestination(me);
          Vector3* position = em.GetPosition();

          log("[DEBUG] Vector3* = %p", destination);
          log("[DEBUG] Destination = %f %f %f", destination->x, destination->y, destination->z);
          log("[DEBUG] Position = %f %f %f", position->x, position->y, position->z);
        }
        break;

      case 'W':
        {
          static int gameState = 0;
          log("[DEBUG] Force load main menu");

          if (gameState++ % 2) {
            ChangeGameStateToMainMenu();
          } else {
            ChangeGameStateToInGame();
          }
        }
        break;

      }
      break;
    case WM_CHAR:
      break;
    }
  }
}
