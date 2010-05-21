#pragma once

// Size?: 3E0h
// Inherits: CItem
struct CEquipment
{
  // CItem STRUCTURE HERE
  u32 stackSize;    // 13h = 19 pots
  u32 stackSizeMax; // 14h = 20 max size pots

  u32 unk1000[21];  //

  PVOID pCGenericModel;

  PVOID unk1001;
  PVOID unk1002;

  PVOID pCEGUIPropertySheet;
};
