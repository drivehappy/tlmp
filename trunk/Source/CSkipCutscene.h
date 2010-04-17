#pragma once

// Size?: 30h
struct CSkipCutscene
{
  PVOID vtable;
  u32 unk0;
  u64 guid0;      // 92D33FB649DE11DFh
  u64 guid1;      // 92D33DE949DE11DFh
  u64 guid2;      // 0CB7579DEBB7F11DEh
  u64 guid3;      // 0

  PVOID unk1;
  PVOID pCSkipCutsceneDescriptor;
};
