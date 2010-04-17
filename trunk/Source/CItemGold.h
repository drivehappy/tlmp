#pragma once

// Size?: 230h
struct CItemGold
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;      // 0B269C24649B211DFh
  u64 guid1;      // 0FFFFFFFFFFFFFFFFh
  u64 guid2;      // 0B269C24649B211DFh

  u64 unk1;
  float unk2[6];  // -0.70709229, 0, 1, 1, 1, 0

  PVOID unk3;     // ptr to unk struct
  u32 unk4[5];    // 0,7,0,0,4Ah

  PVOID pOctree0;
  u32 unk5;

  PVOID pCResourceManager;

  PVOID pOctree1;
  u32 unk6;

  float unk7[32];    // 79.81, -0.000015, 154.12, 0.0099, 0.0099, 0.0099, ..

  // ... more
};
