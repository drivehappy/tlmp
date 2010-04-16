#pragma once

// Size?: 3E0h
struct CEquipment
{
  PVOID vtable;
  PVOID pUnknownStruct;     // ptr to unk struct
  u64 guid0;                // 9E29352249B011DFh
  u64 guid1;                // 9E29352249B011DFh
  u64 guid2;                // 9E29352249B011DFh

  u32 unk0[4];              // 0,0,0,0

  PVOID unk1;               // ptr to unknown struct

  // ... a bunch more
};
