#include "Entity.h"
using namespace TLMP;

c_entity::c_entity()
: id(-1), guid(0), level(0), e(0), last_pos_send(0), is_running(false), hp(0), last_target(0), is_player(false)
{
  dst[0] = 0.0f;
  dst[1] = 0.0f;
  pos_update_time = 0;
  noitems = false;
}
  
void c_entity::init() {
  ce = (char*)e;
}
  
template<typename t>
t& c_entity::offset(int offset) const
{
  if (!e || !ce) {
    log(" !! offset with null this !!");
    return *(t*)&ce[offset];
  }
}

Vector3& c_entity::get_pos() const {
  return offset<Vector3>(0x70);
}

void c_entity::set_pos(const Vector3 & pos) {
  //::set_pos(e,pos);
  //get_pos() = pos;
}
  
float& c_entity::get_hp() {
  return offset<float>(hp_offset);
}
  
float& c_entity::get_mana() {
  return offset<float>(mana_offset);
}
  
void c_entity::destroy() {
  offset<bool>(destroy_offset) = true;
}
  
void*& c_entity::target() {
  return offset<void*>(0x290);
}
  
bool& c_entity::attack_midair() {
  return offset<bool>(0x20a);
}
  
bool& c_entity::running() {
  return offset<bool>(0x275);
}
  
bool& c_entity::moving() {
  return offset<bool>(0x208);
}
  
c_inventory*& c_entity::inventory() {
  return offset<c_inventory*>(0x404);
}
