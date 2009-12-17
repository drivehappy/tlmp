#pragma once

#include "Common.h"

namespace TLMP {

  static PVOID drop_item_this = 0;
  static PVOID item_initialize_this = 0;

  struct c_item {
	  uint id;
	  unsigned long long guid;
	  int level;
	  int unk0, unk1;
	  union {
		  void*e;
		  char*ce;
	  };
	  //index_t equipped_entity;
	  //int equipped_slot;
	  c_item() {
		  //equipped_entity = -1;
	  }
  };


  void _item_initialize_pre STDARG;

  void _item_create_pre STDARG;
  void _item_create_post STDARG;

  void _item_drop_pre STDARG;

  void _item_pick_up_pre STDARG;
  void _item_pick_up_post STDARG;

  void _item_equip_pre STDARG;
  void _item_equip_post STDARG;

  void _item_unequip_pre STDARG;

};
