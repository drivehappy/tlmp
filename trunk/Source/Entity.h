#pragma once

struct c_entity {
	uint id;
	unsigned long long guid;
	int level;
	void*e;
	char*ce;
	float dst[2];
	uint32_t last_pos_send;
	bool is_running;
	float hp, mana;
	void*last_target;
	bool last_moving;
	bool is_player;
	Vector3 test;
	Vector3 pos_update, pos_update_src;
	unsigned int pos_update_time;
	bool noitems;
	//slist<index_t> equipped_items;
	c_entity() : id(-1), guid(0), level(0), e(0), last_pos_send(0), is_running(false), hp(0), last_target(0), is_player(false) {
		dst[0] = 0.0f;
		dst[1] = 0.0f;
		pos_update_time = 0;
		noitems = false;
	}
	void init() {
		ce = (char*)e;
	}
	template<typename t> t&offset(int offset) const {if (!e || !ce) log(" !! offset with null this !!");return *(t*)&ce[offset];}
	static const int hp_offset = 0x394;
	static const int mana_offset = 0x3b8;
	static const int destroy_offset = 0x158;
	Vector3& get_pos() const {
		return offset<XZY>(0x70);
		//XZY r;
		//::get_pos(e,r,1);
		//return r;
	}
	void set_pos(const XZY&pos) {
		//::set_pos(e,pos);
		//get_pos() = pos;
	}
	float&get_hp() {
		return offset<float>(hp_offset);
	}
	float&get_mana() {
		return offset<float>(mana_offset);
	}
	void destroy() {
		offset<bool>(destroy_offset) = true;
	}
	void*&target() {
		return offset<void*>(0x290);
	}
	bool&attack_midair() {
		return offset<bool>(0x20a);
	}
	bool&running() {
		return offset<bool>(0x275);
	}
	bool&moving() {
		return offset<bool>(0x208);
	}
	c_inventory*&inventory() {
		return offset<c_inventory*>(0x404);
	}
};
