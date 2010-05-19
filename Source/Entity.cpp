#include "Entity.h"
//#include "ServerState.h"
#include "SharedState.h"
using namespace TLMP;

bool                  TLMP::ClientAllowSpawn = false;
bool                  TLMP::ClientAllowEquip = true;
bool                  TLMP::ServerAllowSpawn = true;
PVOID                 TLMP::EntityManager = NULL;
//vector<c_entity *>   *TLMP::ServerEntities = NULL;
vector<NetworkEntity*>* TLMP::NetworkSharedEntities = NULL;

void TLMP::_entity_initialize_pre STDARG
{
  //log("Entity Initialize: %p %p %p %p", e->_this, Pz[0], Pz[1], Pz[2]);

  //u64 guid = *(u64*)&Pz[0];
  u32 level = Pz[2];
}

void TLMP::_entity_initialize_post STDARG
{
  log("Entity Initialize: %p %p %p %p", e->_this, Pz[0], Pz[1], Pz[2]);

  //u64 guid = *(u64*)&Pz[0];
  u32 level = Pz[2];
  Vector3* position = (Vector3*)Pz[1];

  if (NetworkState::getSingleton().GetState() == SERVER && ServerAllowSpawn) {
    c_entity o;
    o.e = (PVOID)e->retval;
    o.level = level;
    o.noitems = false;
    o.init();

    // This function gets the wrong offset for the GUID I think
    u64 guid = o.GetGUID();


    // Store this Monster in our shared entity's list
    NetworkEntity* entity = new NetworkEntity((PVOID)e->retval);

    if (!NetworkSharedEntities)
      NetworkSharedEntities = new vector<NetworkEntity*>();
    NetworkSharedEntities->push_back(entity);

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

    log("[SERVER] Sent Monster Spawn to Client at Position(%f, %f, %f)", position->x, position->y, position->z);
    log("[SERVER]   GUID = %016I64X    commonId = %i", guid, entity->getCommonId());

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

