#pragma once

// Size?: 3E0h
// Inherits: CItem
struct CEquipment
{
  // CItem STRUCTURE HERE

  u32 unk1000[23];  //

  PVOID pCGenericModel;

  PVOID unk1001;
  PVOID unk1002;

  PVOID pCEGUIPropertySheet;
};
