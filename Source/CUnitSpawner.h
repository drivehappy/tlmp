#pragma once

// Size?: 1FCh
struct CUnitSpawner
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;
  u64 unk1;
  u64 guid1;
  u32 unk2[3];
  PVOID pCUnitSpawnerDescriptor;    // ptr to CUnitSpawnerDescriptor

};
