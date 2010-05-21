#pragma once

// Size: 40 bytes (at least, no more than)
// Inherits: CRunicCore
//
struct CEquipmentRef
{
  PVOID unk0;         // NULL
  PVOID pCEquipment;  // ptr to CEquipment

  u32 unk1[2];        // 67h, 67h
  u32 unk2[5];
};
