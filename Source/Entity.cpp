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
  /* FIXME_!
  u64 guid = *(u64*)&Pz[0];
  u32 level = Pz[2];
  
  c_entity *entity = new c_entity();
  entity->e = e->_this;
  entity->guid = guid;
  entity->level = level;
  entity->init();
  */

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
}
  
void c_entity::init()
{
  ce = (char*)e;
}
  
template<typename t>
t& c_entity::offset(int offset) const
{
  if (!e || !ce) {
    log(" !! offset with null this !!");
    return *(t*)(&ce[offset]);
  }
}

Vector3* c_entity::get_pos() const
{
  //return offset<Vector3>(0x70);
  return (Vector3*)(&ce[0x70]);
}

void c_entity::set_pos(const Vector3 & pos)
{
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

// get_dst
void* TLMP::GetDestination(void *p)
{
  char *c = *(char **)((char *)p + 0x60);

  if (c) {
    c = *(char **)(c + 0xc);
  }

  return c;
}
