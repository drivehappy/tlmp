#include "Various.h"

void TLMP::_set_destination_pre STDARG
{
  float dest[2] = {*(float *)&Pz[1], *(float *)&Pz[2]};

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
    NetworkMessages::Position message;
    message.set_x(dest[0]);
    message.set_y(0);       // Ignored
    message.set_z(dest[1]);

    Server::getSingleton().SendMessage<NetworkMessages::Position>(S_ENTITY_SETDEST, &message);
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
      switch (wParam) {
      case 'T':
        c_entity em;
        if (me) {
          em.e = me;
          em.init();
        }

        void *r = CreateUnitByName(EntityManager, L"MONSTERS", L"Troll", 1, 0);
        r = EntityInitialize(*(void**)(((char*)EntityManager)+0x0c), r, em.get_pos(), 0);
        log("Troll Created: %p (at: %f %f %f --- %p)", r, em.get_pos()->x, em.get_pos()->y, em.get_pos()->z, em.get_pos());
      }
      break;
    case WM_CHAR:
      break;
    }
  }
}
