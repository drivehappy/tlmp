#include "Entity.h"
//#include "ServerState.h"
#include "SharedState.h"
using namespace TLMP;

bool                  TLMP::ClientAllowSpawn = false;
bool                  TLMP::ClientAllowEquip = true;
bool                  TLMP::ServerAllowSpawn = true;
CResourceManager*     TLMP::EntityManager = NULL;
//vector<c_entity *>   *TLMP::ServerEntities = NULL;
vector<NetworkEntity*>* TLMP::NetworkSharedEntities = NULL;

void TLMP::_entity_initialize_pre STDARG
{
  //log("Entity Initialize: %p %p %p %p", e->_this, Pz[0], Pz[1], Pz[2]);
  PVOID pCPlayer = (PVOID)Pz[0];    // Also a CMonster

  if (NetworkState::getSingleton().GetState() == CLIENT && !ClientAllowSpawn) {
    // Dirty hack, check if the base is CPlayer or CMonster
    const u32 CPLAYER_BASE = 0x00A80064;
    const u32 CMONSTER_BASE = 0x00A7F97C;

    u32 ptr_base = *((u32*)pCPlayer);

    if (ptr_base == CPLAYER_BASE) {
      log("[CLIENT] Detected CPlayer addition to CLevel, not suppressing load");
    } else if (ptr_base == CMONSTER_BASE) {
      log("[CLIENT] Suppressing Monster load into level");
      e->calloriginal = false;
      e->retval = 0;
    } else {
      log("[ERROR] Couldn't determine type of entity: %i", ptr_base);
    }
  }
}

void TLMP::_entity_initialize_post STDARG
{
  log("Entity Initialize: %p %p %p %p", e->_this, Pz[0], Pz[1], Pz[2]);

  PVOID pCLevel = (PVOID)e->_this;
  PVOID pCPlayer = (PVOID)Pz[0];    // Also a CMonster
  u32 level = Pz[2];
  Vector3* position = (Vector3*)Pz[1];

  if (NetworkState::getSingleton().GetState() == SERVER && ServerAllowSpawn) {
    const u32 CPLAYER_BASE = 0x00A80064;
    const u32 CMONSTER_BASE = 0x00A7F97C;

    u32 ptr_base = *((u32*)pCPlayer);

    if (ptr_base == CPLAYER_BASE) {
      log("[SERVER] Not sending redundant player data");
      return;
    }

    c_entity o;
    o.e = (PVOID)e->retval;
    o.level = level;
    o.noitems = false;
    o.init();

    // Get the GUID of the CPlayer/CMonster
    u64 guid = *(u64*)(((char *)pCPlayer) + 0x168);

    // Bad GUID? Suppress sending to client to check this
    if (guid == 0xFFFFFFFFFFFFFFFF)
      return;

    // Store this Monster in our shared entity's list
    // This function is called at least twice for entities,
    // initially they're set to position (0,0,0) then the position
    // is updated, and this is recalled
    if (!NetworkSharedEntities)
      NetworkSharedEntities = new vector<NetworkEntity*>();

    // So, search the list for the internalObject, if it already exists
    // set entity to it
    NetworkEntity* entity = NULL;
    vector<NetworkEntity *>::iterator itr;

    for (itr = NetworkSharedEntities->begin(); itr != NetworkSharedEntities->end(); itr++) {
      if ((*itr)->getInternalObject() == (PVOID)e->retval) {
        entity = (*itr);
        break;
      }
    }

    // If we didn't find it, just add it
    if (!entity) {
      entity = new NetworkEntity((PVOID)e->retval);
      NetworkSharedEntities->push_back(entity);
    }

    // Create the network message to the client
    NetworkMessages::Entity message;
    message.set_id(entity->getCommonId());
    message.set_level(level);
    message.set_guid(guid);
    message.set_noitems(false);

    NetworkMessages::Position *msgPosition = message.add_position();
    msgPosition->set_x(position->x);
    msgPosition->set_y(position->y);
    msgPosition->set_z(position->z);    

    ///*
    Server::getSingleton().SendMessage<NetworkMessages::Entity>(S_SPAWN_MONSTER, &message);

    logColor(B_RED, "[SERVER] Sent Monster Spawn to Client at Position(%f, %f, %f)", position->x, position->y, position->z);
    logColor(B_RED, "[SERVER]   GUID = %016I64X    commonId = %i", guid, entity->getCommonId());
    logColor(B_RED, "[SERVER]   base_ptr = %i", *((u32*)pCPlayer));

    //*/
    //log("[SERVER] Todo Send Monster Spawn to Client");
  }
}

c_entity::c_entity()
: id(-1), guid(0), level(0), e(0), last_pos_send(0), is_running(false), hp(0), last_target(0), is_player(false)
{
  dst[0] = 0.0f;
  dst[1] = 0.0f;
  pos_update_time = 0;
  noitems = false;
  m_bInitialized = false;
}
  
void c_entity::init()
{
  ce = (char*)e;
  m_bInitialized = true;
}
  
template<typename t>
t& c_entity::offset(int offset) const
{
  if (!e || !ce) {
    log(" !! offset with null this !!");
  }

  return *(t*)(&ce[offset]);
}

Vector3* c_entity::GetPosition() const
{
  //return offset<Vector3>(0x70);
  return (Vector3*)(&ce[0x70]);
}

void c_entity::SetPosition(const Vector3 & pos)
{
  *(Vector3*)(&ce[0x70]) = pos;

  //::set_pos(e,pos);
  //get_pos() = pos;
}
  
float& c_entity::get_hp()
{
  return offset<float>(hp_offset);
}
  
float& c_entity::get_mana()
{
  return offset<float>(mana_offset);
}
  
void c_entity::destroy()
{
  offset<bool>(destroy_offset) = true;
}
  
void*& c_entity::target()
{
  return offset<void*>(0x290);
}
  
bool& c_entity::attack_midair()
{
  return offset<bool>(0x20a);
}
  
bool& c_entity::running()
{
  return offset<bool>(0x275);
}
  
bool& c_entity::moving() {
  return offset<bool>(0x208);
}
  
c_inventory*& c_entity::inventory()
{
  return offset<c_inventory*>(0x404);
}

unsigned int& c_entity::GetLevel() const
{
  return offset<unsigned int>(0x3C);
}

u64& c_entity::GetGUID() const
{
  return offset<u64>(0x2d);
}

// get_dst
PVOID TLMP::GetDestination(PVOID player)
{
  //log("GetDestination: %p", player);

  char *c = *(char **)((char *)player + 0x60);
  
  //log("  c = %p", c);

  if (c) {
    c = *(char **)(c + 0xc);
    //log("  c = %p", c);
  }

  return c;
}

