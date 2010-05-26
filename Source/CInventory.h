#pragma once

#include "Helper.h"

// Size?: 10h
// 
struct CInventory : CRunicCore
{
  u32   unk0[7];

  u32   equipmentCount;

  u32  *unk1[100];


  void dumpInventory() {
    logColor(B_RED, "Inventory Dump:");

    for (int i = 0; i < 7; i++) {
      logColor(B_RED, "  unk0[%i] = %9i %#9x", i, unk0[i], unk0[i]);
    }

    logColor(B_RED, "  EquipmentCount: %i", equipmentCount);

    for (int i = 0; i < 100; i++) {
      logColor(B_RED, "  unk1[%i] = %9i %#9x", i, unk1[i], unk1[i]);
    }
  }
};
