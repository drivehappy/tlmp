#include "Entity.h"
#include "ServerState.h"
using namespace TLMP;

bool                  TLMP::ClientAllowSpawn = false;
bool                  TLMP::ServerAllowSpawn = true;
PVOID                 TLMP::EntityManager = NULL;
vector<c_entity *>   *TLMP::ServerEntities = NULL;

void TLMP::_entity_initialize_pre STDARG
{
  log("Entity Initialize: %p %p %p %p", e->_this, Pz[0], Pz[1], Pz[2]);

  u64 guid = *(u64*)&Pz[0];
  u32 level = Pz[2];
  
  c_entity *entity = new c_entity();
  entity->e = e->_this;
  entity->guid = guid;
  entity->level = level;
  entity->init();

  /*
  if (!ServerEntities)
    ServerEntities = new vector<c_entity *>();

  ServerEntities->push_back(entity);
  */
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
  //::set_pos(e,pos);
  //get_pos() = pos;
}

Vector3* c_entity::GetDestination() const
{
  Vector3* retVal = NULL;

  if (!m_bInitialized) {
    log("[ERROR] GetDestination: Entity not initialized!");
  } else {
    char* dest = (&ce[0x60]);
    retVal = (Vector3*)(*(char **)(dest + 0x4));
    log("[DEBUG] GetDestination: [0x60 -> 0x4] %p -> %f %f %f", retVal, retVal->x, retVal->y, retVal->z);

    /*
    retVal = (Vector3*)(&ce[0x64]);
    log("[DEBUG] GetDestination: [0x64] %p -> %f %f %f", retVal, retVal->x, retVal->y, retVal->z);
    retVal = (Vector3*)(&ce[0x68]);
    log("[DEBUG] GetDestination: [0x68] %p -> %f %f %f", retVal, retVal->x, retVal->y, retVal->z);
    retVal = (Vector3*)(&ce[0x6C]);
    log("[DEBUG] GetDestination: [0x6C] %p -> %f %f %f", retVal, retVal->x, retVal->y, retVal->z);
    */
  }

  return retVal;
}

void c_entity::SetDestination(const Vector3 & pos)
{
  // TODO
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

unsigned long long& c_entity::GetGUID() const
{
  return offset<unsigned long long>(0x2D);
}

/*
// get_dst
PVOID TLMP::GetDestination(PVOID player)
{
  log("GetDestination: %p", player);

  char *c = *(char **)((char *)player + 0x60);
  
  log("  c = %p", c);

  if (c) {
    c = *(char **)(c + 0xc);
    log("  c = %p", c);
  }

  return c;
}
*/
