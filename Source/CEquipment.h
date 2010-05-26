#pragma once

#include "CItem.h"

// Size?: 3E0h
// Inherits: CItem
struct CEquipment : CItem
{
  u32 stackSize;    // 13h = 19 pots
  u32 stackSizeMax; // 14h = 20 max size pots

  u32   unk1000[16];
  PVOID unk1001[3];
  u32   unk1002[2];

  PVOID pCGenericModel;

  PVOID unk1003;
  PVOID unk1004;

  PVOID pCEGUIPropertySheet;


  void dumpEquipment() {
    logColor(B_GREEN, "Equipment Dump: %p", this);
    logColor(B_GREEN, "  Item Name Ptr: %p", name);
    //logColor(B_GREEN, "Item Name: %s", name.getString());
    logColor(B_GREEN, "  StackSize: %i", stackSize);
    logColor(B_GREEN, "  StackSize Max: %i", stackSizeMax);

    for (int i = 0; i < 16; i++) {
      logColor(B_GREEN, "  unk1000[%i] = %#x", i, unk1000[i]);
    }
    for (int i = 0; i < 3; i++) {
      logColor(B_BLUE, "  unk1001[%i] = %#x", i, unk1001[i]);
    }
    for (int i = 0; i < 2; i++) {
      logColor(B_GREEN, "  unk1002[%i] = %#x", i, unk1002[i]);
    }

  }
};
