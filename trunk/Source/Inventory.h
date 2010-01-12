#pragma once

#include "List.h"

//
// Developed by dengus
namespace TLMP {

  struct c_inventory {
    struct c_slot {
      virtual~c_slot() {};
      void*ogre_allocated_object;
      void*item;
      int slot;
      int also_slot;
      bool b, b2;
      static c_slot*get_new(void*item,int slot) {
        void*(*f)(int size) = (void*(*)(int))GetProcAddress(GetModuleHandle("OgreMain.dll"),"??2?$AllocatedObject@V?$CategorisedAllocPolicy@$0A@@Ogre@@@Ogre@@SAPAXI@Z");
        c_slot*r = (c_slot*)f(sizeof(c_slot));
        //void(__thiscall*ctor)(void*) = (void(__thiscall*)(void*))0x5F11D0;
        //ctor(r);
        ((void(__thiscall*)(void*,void*,int))0x4E2620)(r,item,slot);
        return r;
      }
    };

    virtual~c_inventory(){};
    void*unk[6];
    c_runic_list<c_slot*> items;
    int get_item_slot(void*item) {
      for (size_t i=0;i<items.size();i++) if (items[i]->item==item) return items[i]->slot;
      return -1;
    };
  };

};
